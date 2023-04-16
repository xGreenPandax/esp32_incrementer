#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_log.h"

#define INCREMENT_PERIOD_MS 5000

static QueueHandle_t counter_queue = NULL; // create a queue handle for counter

/**
 * @brief
 * task for incrementing counter each INCREMENT_PERIOD_MS ms
*/
static void increment_counter_task(void *arg)
{
    uint32_t cntr = 0;
    while (1)
    {
        cntr++;
        xQueueSend(counter_queue, &cntr, 10 / portTICK_PERIOD_MS); // update counter tick in queue
        vTaskDelay(INCREMENT_PERIOD_MS / portTICK_PERIOD_MS);
    }
}

/**
 * @brief
 * task for make logs
*/
static void log_task(void *arg)
{
    uint32_t cntr_lnk;
    uint64_t ticks = 0;
    uint64_t old_ticks = 0;
    while (1)
    {
        if (xQueueReceive(counter_queue, &cntr_lnk, 50)) // if we received counter ticks then update logs
        {
            ticks = esp_timer_get_time();
            ESP_LOGI("COUNTER", "counter is %ld", cntr_lnk);
            ESP_LOGI("COUNTER", "period is %lld us", ticks - old_ticks);
            old_ticks = ticks;
        }
    }
}

void app_main(void)
{
    vTaskDelay(pdMS_TO_TICKS(100)); // delay for finish initialization

    counter_queue = xQueueCreate(1, sizeof(uint32_t)); // create queue for transmit counter variable
    xTaskCreate(increment_counter_task, "incrementer", 4096, NULL, 2, NULL);
    xTaskCreate(log_task, "logger", 4096, NULL, 3, NULL);
}
