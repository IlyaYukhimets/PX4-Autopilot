#pragma once

#include <stdbool.h>

#include "stm32_gpio.h"
#include "board_config.h"

void all_led_init(void);
void led_init(int led);
void led_deinit(int led);
void led_set(int led, bool on);
void led_toggle(int led);
