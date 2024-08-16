#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#define ADC_PIN 4
#define QUEUE_SIZE 100
#define SAMPLE_RATE 100 // 100 Hz (10 ms por amostra)

// Fila para ADC
QueueHandle_t adcQueue;

int adcValues[QUEUE_SIZE];
int cont = 0;

void collectData(void* parameter) {
    int adcValue;
    TickType_t lastWakeTime = xTaskGetTickCount();

    while (true) {
        // Ler valor do ADC
        adcValue = analogRead(ADC_PIN);

        // Envia o valor para a fila
        xQueueSend(adcQueue, &adcValue, portMAX_DELAY);

        // Aguardar até o prox ciclo para manter a taxa de amostragem
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000 / SAMPLE_RATE)); // 100 Hz
    }
}

void processData(void* parameter) {
    int adcValue;
    int sum = 0;

    while (true) {
        // Receber valor da fila
        if (xQueueReceive(adcQueue, &adcValue, portMAX_DELAY) == pdPASS) {
            // Armazenar valor no vetor
            adcValues[cont] = adcValue;
            sum += adcValue;
            cont++;

            // Cálculo da média
            if (cont == QUEUE_SIZE) {
                float average = sum / (float)QUEUE_SIZE;
                Serial.print("Average ADC Value: ");
                Serial.println(average);

                cont = 0;
                sum = 0;
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(ADC_PIN, INPUT);

    adcQueue = xQueueCreate(QUEUE_SIZE, sizeof(int));

    xTaskCreate(collectData, "Collect Data", 2048, NULL, 1, NULL);
    xTaskCreate(processData, "Process Data", 2048, NULL, 1, NULL);
}

void loop() {

}

// Link do projeto no Wokwi: https://wokwi.com/projects/406367443314941953