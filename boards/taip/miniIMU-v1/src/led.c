#include "led.h"

static uint32_t g_ledmap[] = {
    GPIO_nLED_BLUE,
    GPIO_nLED_RED,
    GPIO_nLED_GREEN,
};

/**
 * @brief Configure all LED GPIOs for output
*/
void all_led_init(void)
{
    for (size_t l = 0; l < (sizeof(g_ledmap) / sizeof(g_ledmap[0])); l++) {
        stm32_configgpio(g_ledmap[l]);
    }
}

/**
 * @brief Configure LED GPIO for output
 * @param led configured gpio like (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTA | GPIO_PIN0)
*/
void led_init(int led)
{
    stm32_configgpio(led);
    stm32_gpiowrite(led, false);
}

/**
 * @brief Uncodfigure LED GPIO
 * @param led configured gpio like (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTA | GPIO_PIN0)
*/
void led_deinit(int led)
{
    stm32_gpiowrite(led, false);
    stm32_unconfiggpio(led);
}

/**
 * @brief Get state of gpio
 * @param led configured gpio like (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTA | GPIO_PIN0)
*/
bool led_get(int led)
{
    return stm32_gpioread(led);
}

/**
 * @brief Set the output on the pin
 * @param led configured gpio like (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTA | GPIO_PIN0)
*/
void led_on(int led)
{
    stm32_gpiowrite(led, true);
}

/**
 * @brief Clear the output on the pin
 * @param led configured gpio like (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTA | GPIO_PIN0)
*/
void led_off(int led)
{
    stm32_gpiowrite(led, false);
}

/**
 * @brief Set or Clear the output on the pin
 * @param led configured gpio like (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTA | GPIO_PIN0)
 * @param on true - led_on; false - led_off
*/
void led_set(int led, bool on)
{
    switch (on) {
    case true:
        led_on(led);
        break;

    case false:
        led_off(led);
        break;
    }
}

/**
 * @brief Toggle the current output on the pin
 * @param led configured gpio like (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTA | GPIO_PIN0)
*/
void led_toggle(int led)
{
    led_set(led, led_get(led));
}
