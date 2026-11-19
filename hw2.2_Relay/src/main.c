#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_attr.h"

#define PIN_RELAY_IN    GPIO_NUM_4 // Via NPN transistor to relay IN
#define PIN_RELAY_NO    GPIO_NUM_5
#define SETTLE_MS       500

static volatile bool first_intr_occurred = false;
static volatile int64_t first_intr_us = 0;
static volatile int64_t last_intr_us = 0;

typedef enum {
    RELAY_ON =  1,
    RELAY_OFF = 0
} relay_switch_t;

static void IRAM_ATTR relay_isr_handler(void *arg) {
    int64_t interrupt_us = esp_timer_get_time();
    if (!first_intr_occurred) {
        first_intr_us = interrupt_us;
        first_intr_occurred = true;
    }
    last_intr_us = interrupt_us;
}

static void relay_init(void) {
    ESP_ERROR_CHECK(gpio_set_level(PIN_RELAY_IN, RELAY_OFF));

    gpio_config_t relay_in_config = {
        .pin_bit_mask   = (1ULL << PIN_RELAY_IN),
        .mode           = GPIO_MODE_OUTPUT
    };
    ESP_ERROR_CHECK(gpio_config(&relay_in_config));

    gpio_config_t relay_no_config = {
        .pin_bit_mask   = (1ULL << PIN_RELAY_NO),
        .mode           = GPIO_MODE_INPUT,
        .pull_up_en     = GPIO_PULLUP_ENABLE,
        .intr_type      = GPIO_INTR_ANYEDGE
    };
    ESP_ERROR_CHECK(gpio_config(&relay_no_config));

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(PIN_RELAY_NO, relay_isr_handler, NULL));

    vTaskDelay(pdMS_TO_TICKS(SETTLE_MS));
}

static void measure_switch_delay(relay_switch_t target_state, int64_t *delay_us, int64_t *bounce_us) {
    first_intr_occurred = false;
    first_intr_us = 0;
    last_intr_us = 0;
    int64_t signal_send_us = esp_timer_get_time();
    gpio_set_level(PIN_RELAY_IN, target_state);
    vTaskDelay(pdMS_TO_TICKS(SETTLE_MS));

    if (!first_intr_occurred) {
        *delay_us = 0;
        *bounce_us = 0;
        return;
    }
    *delay_us = first_intr_us - signal_send_us;
    *bounce_us = last_intr_us - first_intr_us;
}

void app_main(void) {
    relay_init();

    const int measure_count = 10;

    int64_t mechanical_delay_us = 0;
    int64_t contacts_bounce_us = 0;

    int64_t total_on_delay_us = 0;
    int64_t total_on_bounce_us = 0;
    int64_t total_off_delay_us = 0;
    int64_t total_off_bounce_us = 0;
    
    printf("\nStarting mechanical delay measurements\n");
    for (int i = 0; i < measure_count; i++) {
        measure_switch_delay(RELAY_ON, &mechanical_delay_us, &contacts_bounce_us);
        total_on_delay_us += mechanical_delay_us;
        total_on_bounce_us += contacts_bounce_us;

        printf("Measurement %2d: switch-on delay = %.2f ms, bounced for %.2f ms\n",
            i + 1, mechanical_delay_us / 1000.0f, contacts_bounce_us / 1000.0f);

        measure_switch_delay(RELAY_OFF, &mechanical_delay_us, &contacts_bounce_us);
        total_off_delay_us += mechanical_delay_us;
        total_off_bounce_us += contacts_bounce_us;

        printf("Measurement %2d: switch-off delay = %.2f ms, bounced for %.2f ms\n",
            i + 1, mechanical_delay_us / 1000.0f, contacts_bounce_us / 1000.0f);
    }
    printf("\nRelay mechanical delay measurements done.\n");
    printf("Average switch-on time: %.2f ms\n", total_on_delay_us / 1000.0f / measure_count);
    printf("Average switch-on contacts bounce time: %.2f ms\n", total_on_bounce_us / 1000.0f / measure_count);

    printf("\nAverage switch-off time: %.2f ms\n", total_off_delay_us / 1000.0f / measure_count);
    printf("Average switch-off contacts bounce time: %.2f ms\n", total_off_bounce_us / 1000.0f / measure_count);
}