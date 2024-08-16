#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define LED1 2
#define LED2 4
#define LED3 5

#define BUTTON1 12
#define BUTTON2 13

TaskHandle_t taskHandle2 = NULL;
TaskHandle_t taskHandle3 = NULL;

void blink_led(void* parameter) {
    int gpio = (intptr_t)parameter;
    TickType_t delayTime;

    if (gpio == LED1) {
        delayTime = pdMS_TO_TICKS(1000);
    } else if (gpio == LED2) {
        delayTime = pdMS_TO_TICKS(500);
    } else {
        delayTime = pdMS_TO_TICKS(250);
    }

    pinMode(gpio, OUTPUT);

    while (true) {
        digitalWrite(gpio, HIGH);
        vTaskDelay(delayTime);
        digitalWrite(gpio, LOW);
        vTaskDelay(delayTime);

        // Se o LED1 está piscando, controla a task do LED2
        if (gpio == LED1 && digitalRead(BUTTON1) == LOW) {
            vTaskDelay(pdMS_TO_TICKS(50)); // Debounce

            if (digitalRead(BUTTON1) == LOW) {
                if (eTaskGetState(taskHandle2) == eSuspended) {
                    vTaskResume(taskHandle2);
                } else {
                    vTaskSuspend(taskHandle2);
                }

                // Aguarda até que o botão seja liberado
                while (digitalRead(BUTTON1) == LOW) {
                    vTaskDelay(pdMS_TO_TICKS(30));
                }
            }
        }

        // Se o LED2 está piscando, controla a task do LED3
        if (gpio == LED2 && digitalRead(BUTTON2) == LOW) {
            vTaskDelay(pdMS_TO_TICKS(50)); // Debounce

            if (digitalRead(BUTTON2) == LOW) {
                if (eTaskGetState(taskHandle3) == eSuspended) {
                    vTaskResume(taskHandle3);
                } else {
                    vTaskSuspend(taskHandle3);
                }

                // Aguarda até que o botão seja liberado
                while (digitalRead(BUTTON2) == LOW) {
                    vTaskDelay(pdMS_TO_TICKS(30));
                }
            }
        }
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);

    xTaskCreate(blink_led, "Blink LED 1", 1024, (void*)LED1, 1, NULL);
    xTaskCreate(blink_led, "Blink LED 2", 1024, (void*)LED2, 1, &taskHandle2);
    xTaskCreate(blink_led, "Blink LED 3", 1024, (void*)LED3, 1, &taskHandle3);
}

void loop() {
    // O loop está vazio porque as tarefas FreeRTOS estão em execução
}

// Link do projeto no Wokwi: https://wokwi.com/projects/406334924460491777