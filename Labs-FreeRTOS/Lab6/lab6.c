#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>

#define LED2 2
#define POTENTIOMETER_PIN 34

#define TASK1_EVENT_BIT (1 << 0)
#define TASK2_EVENT_BIT (1 << 1)

#define ADC_READ_INTERVAL pdMS_TO_TICKS(3000)
#define AVERAGE_INTERVAL pdMS_TO_TICKS(15000)
#define NUM_SAMPLES 5  // num de amostras para média

EventGroupHandle_t eventGroup;
TimerHandle_t timer;
QueueHandle_t adcQueue;

void vTask1(void *pvParameters) {
  static int adcValues[NUM_SAMPLES];
  static int sampleIndex = 0;
  
  while (1) {
    xEventGroupWaitBits(eventGroup, TASK1_EVENT_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

    // Ler o valor do ADC e enviar para a fila
    int adcValue = analogRead(POTENTIOMETER_PIN);
    xQueueSend(adcQueue, &adcValue, portMAX_DELAY);
    Serial.print("Valor do ADC enviado: ");
    Serial.println(adcValue);

    // Atualizar o índice da amostra
    adcValues[sampleIndex] = adcValue;
    sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void vTask2(void *pvParameters) {
  int adcValues[NUM_SAMPLES];
  int count = 0;
  const int THRESHOLD = 512;  // Limiar do LED
  
  while (1) {
    xEventGroupWaitBits(eventGroup, TASK2_EVENT_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

    // Processar os dados da fila
    int adcValue;
    while (xQueueReceive(adcQueue, &adcValue, 0)) {
      adcValues[count] = adcValue;
      count = (count + 1) % NUM_SAMPLES;
    }

    // Calcular a média
    long sum = 0;
    int numValues = (count > 0) ? count : NUM_SAMPLES; // Corrigir se count for zero
    for (int i = 0; i < numValues; i++) {
      sum += adcValues[i];
    }
    int average = (numValues > 0) ? (sum / numValues) : 0;
    Serial.print("Média do ADC: ");
    Serial.println(average);

    // Ligar ou apagar o LED com base no limiar
    if (average > THRESHOLD) {
      digitalWrite(LED2, HIGH);
    } else {
      digitalWrite(LED2, LOW);
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// Função de callback do timer
void onTimerISR(TimerHandle_t xTimer) {
  static unsigned long lastTask1Time = 0;
  static unsigned long lastTask2Time = 0;

  unsigned long currentTime = millis();
  
  // A cada 3 segundos, liberar a task 1
  if ((currentTime - lastTask1Time) >= 3000) {
    xEventGroupSetBits(eventGroup, TASK1_EVENT_BIT);
    lastTask1Time = currentTime;
  }

  // A cada 15 segundos, liberar a task 2
  if ((currentTime - lastTask2Time) >= 15000) {
    xEventGroupSetBits(eventGroup, TASK2_EVENT_BIT);
    lastTask2Time = currentTime;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED2, OUTPUT);
  pinMode(POTENTIOMETER_PIN, INPUT);

  // Cria o grupo de eventos
  eventGroup = xEventGroupCreate();
  
  // Fila para enviar os dados do ADC
  adcQueue = xQueueCreate(NUM_SAMPLES, sizeof(int));

  xTaskCreate(vTask1, "Task1", 2048, NULL, 1, NULL);
  xTaskCreate(vTask2, "Task2", 2048, NULL, 1, NULL);

  // Criar e iniciar o timer (1s)
  timer = xTimerCreate("Timer", pdMS_TO_TICKS(1000), pdTRUE, 0, onTimerISR);
  if (timer != NULL) {
    xTimerStart(timer, 0);
  }
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}

// Link do projeto no Wokwi: https://wokwi.com/projects/406369442197766145