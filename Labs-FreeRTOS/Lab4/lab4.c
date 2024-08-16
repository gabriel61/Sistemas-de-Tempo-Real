#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#define BUTTON_PIN 12
#define ADC_PIN 4

SemaphoreHandle_t semaphore;

void task1(void *parameter) {
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    while (true) {
      
        if (digitalRead(BUTTON_PIN) == LOW) {
            // Sinaliza o semáforo
            xSemaphoreGive(semaphore);
            vTaskDelay(pdMS_TO_TICKS(200));  // Debounce
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // Evitar ler rapido demais
    }
}

void task2(void *parameter) {
    while (true) {
        // Aguarda a sinalização do semáforo
        if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE) {
            // Lê o ADC
            int adcValue = analogRead(ADC_PIN);
            Serial.println("Valor do ADC: " + String(adcValue));
        }
    }
}

void setup() {
    Serial.begin(115200);

    semaphore = xSemaphoreCreateBinary();

    xTaskCreate(task1, "Monitor Button", 1024, NULL, 1, NULL);
    xTaskCreate(task2, "Read ADC", 1024, NULL, 1, NULL);
}

void loop() {

}

// Link do projeto no Wokwi: https://wokwi.com/projects/406337968407684097