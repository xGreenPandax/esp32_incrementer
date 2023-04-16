#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"

static QueueHandle_t counter_queue = NULL;

static void increment_counter_task(void *arg)
{
    uint32_t cntr = 0;
    while (1)
    {
        cntr++;
        xQueueSend(counter_queue, &cntr, 10 / portTICK_PERIOD_MS);
        printf("CNTR++");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

static void log_task(void *arg)
{
    uint32_t cntr_lnk;
    while (1)
    {

        if (xQueueReceive(counter_queue, &cntr_lnk, 5))
            printf("Counter is: %ld\n", cntr_lnk);
    }
}

void app_main(void)
{
    vTaskDelay(pdMS_TO_TICKS(100));

    counter_queue = xQueueCreate(1, sizeof(uint32_t));
    xTaskCreate(increment_counter_task, "incrementer", 4096, NULL, 2, NULL);
    xTaskCreate(log_task, "logger", 4096, NULL, 3, NULL);
}
