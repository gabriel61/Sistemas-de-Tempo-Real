#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "esp_log.h"
#include "esp_dsp.h"

// Definições gerais
#define I2S_NUM         (0)
#define SAMPLE_RATE     (16000)
#define BUFFER_SIZE     (1024)
#define WINDOW_SIZE     (256)
#define BLINK_GPIO      (2)
#define TIMER_GROUP     TIMER_GROUP_0
#define TIMER_INDEX     TIMER_0
#define FREQ_AMOSTRAGEM 16000
#define PI 3.14159265358979323846

static const char *TAG = "example";

// Variáveis para controle de tarefa
volatile bool task = false;
static int fft_counter = 0;

typedef struct {
    int16_t buffer1[BUFFER_SIZE];
    int16_t buffer2[BUFFER_SIZE];
    int16_t *read;
    int16_t *write;
    int index;
    bool buffer_full;
} PingPongBuffer;

PingPongBuffer ppb;
float circular_buffer[WINDOW_SIZE] = {0}; // Buffer circular para a média deslizante
int circular_index = 0; // Índice buffer circular
float media_buffer_anterior = 0.0; // Média do ruído anterior
float fator_multiplicador = 5; // Fator multiplicador para detecção de som
int cont = 0;

#define N_SAMPLES 1024
int N = N_SAMPLES;
// Input test array
// __attribute__((aligned(16)))
// float x1[N_SAMPLES];
// __attribute__((aligned(16)))
// float x2[N_SAMPLES];
// Window coefficients
__attribute__((aligned(16)))
float wind[N_SAMPLES];
// working complex array
__attribute__((aligned(16)))
float y_cf[N_SAMPLES * 2];
// Pointers to result arrays
float *y1_cf = &y_cf[0];
float *y2_cf = &y_cf[N_SAMPLES];

// Sum of y1 and y2
__attribute__((aligned(16)))
float sum_y[N_SAMPLES / 2];

// Interrupção do timer
static void IRAM_ATTR timer_group0_isr(void *para) {
    uint32_t intr_status = TIMERG0.int_st_timers.val;

    if ((intr_status & BIT(TIMER_INDEX))) {
        TIMERG0.int_clr_timers.t0 = 1;
        TIMERG0.hw_timer[TIMER_INDEX].update = 1;

        // Resetar o contador de interrupções
        TIMERG0.hw_timer[TIMER_INDEX].config.alarm_en = TIMER_ALARM_EN;
        task = true;
    }
}

// Configuração do timer
static void init_timer() {
    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_START,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .divider = 80   // 1 us por tick (80 MHz / 80)
    };
    timer_init(TIMER_GROUP, TIMER_INDEX, &config);
    timer_set_counter_value(TIMER_GROUP, TIMER_INDEX, 0x00000000ULL);
    timer_set_alarm_value(TIMER_GROUP, TIMER_INDEX, 64000);  // 64 ms
    timer_enable_intr(TIMER_GROUP, TIMER_INDEX);
    timer_isr_register(TIMER_GROUP, TIMER_INDEX, timer_group0_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TIMER_GROUP, TIMER_INDEX);
}

void wait_for_interrupt() {
    while(task == false) {}
    task = false;
}

// Inicialização do buffer ping-pong
void init_buffer() {
    ppb.write = ppb.buffer1;
    ppb.read = ppb.buffer2;
    ppb.index = 0;
    ppb.buffer_full = false;
}

// Adiciona elemento ao buffer ping-pong
void add(int16_t element, PingPongBuffer *buffer) {
    if (buffer->index < BUFFER_SIZE) {
        buffer->write[buffer->index] = element;
        buffer->index++;
    } else {
        int16_t *temp = buffer->read;
        buffer->read = buffer->write;
        buffer->write = temp;
        buffer->index = 0;
        buffer->write[buffer->index] = element;
        buffer->index++;
        buffer->buffer_full = true;
    }
}

// Configuração do I2S
void i2s_init() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX), // Modo de recebimento
        .sample_rate = FREQ_AMOSTRAGEM, // Frequência de amostragem em Hz
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // 16 bits por amostra
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // Somente canal esquerdo
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB, // Formato I2S
        .intr_alloc_flags = 0, // Flag de alocação de interrupção
        .dma_buf_count = 8, // Contagem de buffers DMA
        .dma_buf_len = BUFFER_SIZE, // Tamanho do buffer DMA
        .use_apll = false // Não usar PLL auxiliar
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = 26, // GPIO26 para bit clock
        .ws_io_num = 25, // GPIO25 para word select
        .data_out_num = -1, // Não estamos transmitindo
        .data_in_num = 34   // GPIO34 para data input
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

// Calcula a potência média do ruído
float calcular_potencia_media(int16_t *amostras, int tamanho) {
    float soma = 0.0;
    for (int i = 0; i < tamanho; i++) {
        soma += (float)(amostras[i] * amostras[i]); // Potência das amostras
    }
    return soma / tamanho;
}

// Calcula a potência média do buffer circular
float calcular_potencia_circular(float *amostras, int tamanho) {
    float soma = 0.0;
    for (int i = 0; i < tamanho; i++) {
        soma += amostras[i]; // Média das potências
    }
    return soma / tamanho;
}

// Função para atualizar o buffer circular (leitura dos dados I2S)
void read_i2s_samples() {
    size_t bytes_read;
    int16_t samples[BUFFER_SIZE];
    i2s_read(I2S_NUM_0, samples, sizeof(samples), &bytes_read, portMAX_DELAY);

    for (int i = 0; i < BUFFER_SIZE; i++) {
        add(samples[i], &ppb);
    }
}

// Função de detecção sonora
void deteccaoSonora(float potencia_media_atual, float fator_multiplicador, float potencia_media_ruido) {
    if (ppb.buffer_full) {
        if (cont > 50) {
            cont = 0;
        } else {
            cont++;
        }

        circular_buffer[circular_index] = potencia_media_atual;
        circular_index = (circular_index + 1) % WINDOW_SIZE;

        if (potencia_media_atual > fator_multiplicador * potencia_media_ruido) {
            gpio_set_level(BLINK_GPIO, 1);
            ESP_LOGI("DETECCAO", "Som detectado: %.2f", potencia_media_atual);
        } else {
            gpio_set_level(BLINK_GPIO, 0);
        }

        media_buffer_anterior = potencia_media_atual;
    }
}

// Função FFT
float fftFuction(int16_t *buffer){
    
    esp_err_t ret;
    //ESP_LOGI(TAG, "Start Example.");
    ret = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    if (ret  != ESP_OK) {
        //ESP_LOGE(TAG, "Not possible to initialize FFT. Error = %i", ret);
        return 0;
    }

    // Generate hann window
    dsps_wind_hann_f32(wind, N);
   
    // Convert two input vectors to one complex vector
    for (int i = 0 ; i < N ; i++) {
        y_cf[i * 2 + 0] = buffer[i] * wind[i];
        y_cf[i * 2 + 1] = 0; // Parte imaginária é zero
    }
    // FFT
    unsigned int start_b = dsp_get_cpu_cycle_count();
    dsps_fft2r_fc32(y_cf, N);
    unsigned int end_b = dsp_get_cpu_cycle_count();
    // Bit reverse
    dsps_bit_rev_fc32(y_cf, N);
    // Convert one complex vector to two complex vectors
    dsps_cplx2reC_fc32(y_cf, N);

    for (int i = 0 ; i < N / 2 ; i++) {
        y1_cf[i] = 10 * log10f((y1_cf[i * 2 + 0] * y1_cf[i * 2 + 0] + y1_cf[i * 2 + 1] * y1_cf[i * 2 + 1]) / N);

    }
    double max = 0;
    int imax = -1;
    for (int j = 0 ; j < N / 2 ; j++) {
        if (y1_cf[j] > max)
        {
            max = y1_cf[j];
            imax = j;
        }
    }

    //ESP_LOGI(TAG, "maior valor: %f", max);
    //ESP_LOGI(TAG, "posicao: %i", imax);
    
    float freqMax;
    freqMax = ((float)imax/1024)*FREQ_AMOSTRAGEM;

    return freqMax;
}

void app_main(void) {
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    init_buffer();
    i2s_init();
    init_timer();

    float freq=0.0;
    int cont = 0;
    
    while (1) {
        // Ciclo 1
        wait_for_interrupt();
        read_i2s_samples();

        // Ciclo 2
        wait_for_interrupt();
        read_i2s_samples();
        float potencia_media_atual = calcular_potencia_media(ppb.read, BUFFER_SIZE);
        deteccaoSonora(potencia_media_atual, fator_multiplicador, media_buffer_anterior);

        // Ciclo 3
        wait_for_interrupt();
        read_i2s_samples();

        // Ciclo 4
        wait_for_interrupt();
        read_i2s_samples();
        freq = fftFuction(ppb.read);

        if(cont%4 == 0)
        ESP_LOGE("FFT", "freqMax: %.2f", freq);
        cont++;

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// task read_i2s_samples -> 0 ms -- 64 ms        -> U = 0%
// task deteccaoSonora -> 0,098 ms -- 256 ms     -> U = 0,4%
// task fftFuction -> 6 ms -- 256 ms             -> U = 2,34%
//                                               -> Ut= 2,74%


// ----> test time fftFuction()
// uint64_t start = esp_timer_get_time();
// for(int i=0; i<2000; i++){
//     fftFuction(ppb.buffer1);
// }
// uint64_t end = esp_timer_get_time();
// ESP_LOGI("TIMER", "%llu ms por task\n", (end - start) / 2000 / 1000);