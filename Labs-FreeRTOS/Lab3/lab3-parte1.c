#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define LED1 2
#define LED2 4
#define LED3 5

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

void setup() {
    xTaskCreate(blink_led, "Blink LED 1", 1024, (void*)LED1, 1, NULL);
    xTaskCreate(blink_led, "Blink LED 2", 1024, (void*)LED2, 1, NULL);
    xTaskCreate(blink_led, "Blink LED 3", 1024, (void*)LED3, 1, NULL);
}

void loop() {
    // O loop está vazio pq as tasks FreeRTOS estão em execução
}
