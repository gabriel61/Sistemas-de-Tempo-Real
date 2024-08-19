#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/timers.h>

#define LED_PIN 2
#define BUTTON_PIN 12

TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;

volatile uint32_t last_interrupt_time = 0; // Tempo da última interrupção

// Função de interrupção do botão com debounce
void IRAM_ATTR button_isr_handler(void* arg) {
    uint32_t current_time = xTaskGetTickCountFromISR();
    
    // Debounce: ignora interrupções se ocorrerem em menos de 200ms
    if (current_time - last_interrupt_time > pdMS_TO_TICKS(200)) {
        last_interrupt_time = current_time;

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // Notifica as tarefas
        vTaskNotifyGiveFromISR(task1Handle, &xHigherPriorityTaskWoken);
        vTaskNotifyGiveFromISR(task2Handle, &xHigherPriorityTaskWoken);

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

// Tarefa 1: Altera o estado do LED quando notificada
void vTask1(void* pvParameters) {
    bool ledState = false;
    while (1) {
        // Espera por uma notificação
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Inverte o estado do LED
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
    }
}

// Tarefa 2: Envia uma mensagem ao terminal quando notificada
void vTask2(void* pvParameters) {
    while (1) {
        // Espera por uma notificação
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        Serial.println("Botão pressionado!");
    }
}

void setup() {
    Serial.begin(115200);

    // Configura o pino do LED como saída
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Configura o pino do botão como entrada com pull-up e interrupção
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), (void (*)(void))button_isr_handler, FALLING);

    xTaskCreate(vTask1, "Task 1", 2048, NULL, 1, &task1Handle);
    xTaskCreate(vTask2, "Task 2", 2048, NULL, 1, &task2Handle);
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}

// Link do projeto no Wokwi: https://wokwi.com/projects/406633816034379777