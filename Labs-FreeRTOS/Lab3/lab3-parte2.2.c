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
    int gpio = (intptr_t)parameter;  // Corrige a conversão de tipo

    TickType_t delayTime;
    if (gpio == LED1) {
        delayTime = pdMS_TO_TICKS(1000);
    } else if (gpio == LED2) {
        delayTime = pdMS_TO_TICKS(500);
    } else {
        delayTime = pdMS_TO_TICKS(250);
    }

    pinMode(gpio, OUTPUT);

    // Liga o LED, aguarda o delay e desliga o LED
    while (true) {
        digitalWrite(gpio, HIGH);
        vTaskDelay(delayTime);
        digitalWrite(gpio, LOW);
        vTaskDelay(delayTime);
    }
}

void control_task(void* parameter) {
    int buttonPin = (intptr_t)parameter;

    pinMode(buttonPin, INPUT_PULLUP);

    while (true) {
        if (digitalRead(buttonPin) == LOW) {  // Botão pressionado
            if (buttonPin == BUTTON1) {
                // Alterna a execução da task 2
                if (eTaskGetState(taskHandle2) == eSuspended) {
                    vTaskResume(taskHandle2);
                } else {
                    vTaskSuspend(taskHandle2);
                }
            } else if (buttonPin == BUTTON2) {
                // Alterna a execução da task 3
                if (eTaskGetState(taskHandle3) == eSuspended) {
                    vTaskResume(taskHandle3);
                } else {
                    vTaskSuspend(taskHandle3);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(200));  // Debounce do botão
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup() {
    Serial.begin(115200);
    
    xTaskCreate(blink_led, "Blink LED 1", 1024, (void*)LED1, 1, NULL);
    xTaskCreate(blink_led, "Blink LED 2", 1024, (void*)LED2, 1, &taskHandle2);
    xTaskCreate(blink_led, "Blink LED 3", 1024, (void*)LED3, 1, &taskHandle3);

    // Tasks para controlar a execução das outras tasks com base nos botões
    xTaskCreate(control_task, "Control Task", 1024, (void*)BUTTON1, 2, NULL);
    xTaskCreate(control_task, "Control Task 2", 1024, (void*)BUTTON2, 2, NULL);
}

void loop() {
    // O loop está vazio pq as tasks FreeRTOS estão em execução
}

// Link do projeto no Wokwi: https://wokwi.com/projects/406261536799763457