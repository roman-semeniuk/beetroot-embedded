#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"

#define SERVO_GPIO          GPIO_NUM_4
#define POT_ADC_CHANNEL     ADC_CHANNEL_4 // GPIO_NUM_5

#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL        LEDC_CHANNEL_0
#define LEDC_DUTY_RES       LEDC_TIMER_14_BIT
#define LEDC_DUTY_MAX       ((1U << LEDC_DUTY_RES) - 1)

#define SERVO_PWM_FREQ      50
#define SERVO_PERIOD_US     20000
#define SERVO_MIN_US        500
#define SERVO_MAX_US        2400
#define SERVO_NEUTRAL_US    ((SERVO_MIN_US + SERVO_MAX_US) / 2)
#define SERVO_NEUTRAL_DUTY  ((SERVO_NEUTRAL_US * LEDC_DUTY_MAX) / SERVO_PERIOD_US)

#define SERVO_MAX_ANGLE     180.0f

#define ADC_BITWIDTH        ADC_BITWIDTH_12
#define ADC_MAX_RAW         ((float)((1U << ADC_BITWIDTH) - 1))

#define POT_MAX_ANGLE       270.0f
#define POT_DEADZONE_DEG    ((POT_MAX_ANGLE - SERVO_MAX_ANGLE) / 2.0f)

#define LOG_INTERVAL_MS     1000

static void servo_init(void){
    ledc_timer_config_t timer_config = {
        .speed_mode      = LEDC_MODE,
        .timer_num       = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz         = SERVO_PWM_FREQ,
        .clk_cfg         = LEDC_AUTO_CLK
    }; 
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    ledc_channel_config_t channel_config = {
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL,
        .timer_sel  = LEDC_TIMER,
        .intr_type  = LEDC_INTR_DISABLE,
        .gpio_num   = SERVO_GPIO,
        .duty       = SERVO_NEUTRAL_DUTY,
        .hpoint     = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));
}

static adc_oneshot_unit_handle_t adc_handle;

static void potentio_init (void) {    

    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id    = ADC_UNIT_1,
        .ulp_mode   = ADC_ULP_MODE_DISABLE
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config, &adc_handle));

    adc_oneshot_chan_cfg_t chan_config = {
        .atten      = ADC_ATTEN_DB_12,
        .bitwidth   = ADC_BITWIDTH
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, POT_ADC_CHANNEL, &chan_config));
}

static float get_potentio_angle(adc_channel_t channel) {
    int raw_val = 0;
    adc_oneshot_read(adc_handle, channel, &raw_val); // ESP32 ADC: 0 - 4095

    // Convert raw read value into an 0 - 270° angle
    float potentio_angle = ((float)raw_val / ADC_MAX_RAW) * POT_MAX_ANGLE;

    // Shift by the deadzone: pot range 45°–225° maps to 0°– 180°.
    float centered_angle = potentio_angle - POT_DEADZONE_DEG;

    if (centered_angle < 0) return 0;
    if (centered_angle > SERVO_MAX_ANGLE) return SERVO_MAX_ANGLE;
    return centered_angle;
}

static inline uint32_t angle_to_duty(float angle) {
    uint32_t pulse_us = SERVO_MIN_US + (uint32_t)((angle / SERVO_MAX_ANGLE) *(SERVO_MAX_US - SERVO_MIN_US));
    return (pulse_us * LEDC_DUTY_MAX) / SERVO_PERIOD_US;
}

static void print_servo_angle(float current_servo_angle) {
    static uint32_t last_print_ms = 0;

    uint32_t current_ms = (uint32_t)(esp_timer_get_time() / 1000ULL);
    if (current_ms - last_print_ms >= LOG_INTERVAL_MS) {
        printf("Servo angle: %.1f\n", current_servo_angle);
        last_print_ms = current_ms;
    }
}

static void servo_set_angle(float target_angle) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, angle_to_duty(target_angle));
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void app_main(void) {
    servo_init();
    potentio_init();
    vTaskDelay(pdMS_TO_TICKS(200));

    while(1) {
        float target_angle = get_potentio_angle(POT_ADC_CHANNEL);

        servo_set_angle(target_angle);
        print_servo_angle(target_angle);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}