/****************************************************************************
 *
 *   Authors: Ilya Sopkin <ivsopkin@yandex.ru>
 *
 ****************************************************************************/

/**
 * @file board_config.h
 *
 * PRIBORBOTTLE internal definitions
 */

#pragma once

/****************************************************************************************************
 * Included Files
 ****************************************************************************************************/

#include <px4_platform_common/px4_config.h>
#include <nuttx/compiler.h>
#include <stdint.h>

#include <stm32_gpio.h>

#if !defined(CONFIG_BUILD_FLAT)
#include <sys/boardctl.h>
#include <px4_platform/board_ctrl.h>
#endif

/****************************************************************************************************
 * Definitions
 ****************************************************************************************************/

/* Configuration ************************************************************************************/


/* PRIBORBOTTLE GPIOs ***********************************************************************************/

/* LEDs are driven with push open drain to support Anode to 5V or 3.3V */

#define GPIO_nLED_RED        /* PA2 */  (GPIO_OUTPUT|GPIO_OPENDRAIN|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTA|GPIO_PIN2)
#define GPIO_nLED_GREEN      /* PA3 */  (GPIO_OUTPUT|GPIO_OPENDRAIN|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTA|GPIO_PIN3)
#define GPIO_nLED_BLUE       /* PA15 */ (GPIO_OUTPUT|GPIO_OPENDRAIN|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTA|GPIO_PIN15)

//#define BOARD_HAS_CONTROL_STATUS_LEDS      1
#define BOARD_OVERLOAD_LED     LED_RED
#define BOARD_ARMED_STATE_LED  LED_BLUE

/*
 * ADC channels
 *
 * These are the channel numbers of the ADCs of the microcontroller that
 * can be used by the Px4 Firmware in the adc driver
 */

/* ADC defines to be used in sensors.cpp to read from a particular channel */

#define ADC1_CH(n)                  (n)
#define ADC1_GPIO(n)                GPIO_ADC1_IN##n

/* Define GPIO pins used as ADC N.B. Channel numbers must match below */

#define PX4_ADC_GPIO  \
	/* PB0 */  ADC1_GPIO(8),  \
	/* PB1 */  ADC1_GPIO(9),  \
	/* PC0 */  ADC1_GPIO(10), \
	/* PC1 */  ADC1_GPIO(11), \
	/* PC3 */  ADC1_GPIO(13), \
	/* PC4 */  ADC1_GPIO(14)

/* Define Channel numbers must match above GPIO pin IN(n)*/

#define ADC_HW_VER_SENSE_CHANNEL        	/* PB0 */  ADC1_CH(8)
#define ADC_HW_REV_SENSE_CHANNEL        	/* PB1 */  ADC1_CH(9)
#define ADC_SCALED_V5_CHANNEL        		/* PC0 */  ADC1_CH(10)
#define ADC_SCALED_VDD_3V3_MAIN_CHANNEL        	/* PC1 */  ADC1_CH(11)
#define ADC_SCALED_VDD_3V3_SENSORS_CHANNEL      /* PC3 */  ADC1_CH(13)
#define ADC1_SPARE_1_CHANNEL                 	/* PC4 */  ADC1_CH(14)

#define ADC_CHANNELS \
	((1 << ADC_HW_VER_SENSE_CHANNEL)       		| \
	 (1 << ADC_HW_REV_SENSE_CHANNEL)       		| \
	 (1 << ADC_SCALED_V5_CHANNEL)       		| \
	 (1 << ADC_SCALED_VDD_3V3_MAIN_CHANNEL)       	| \
	 (1 << ADC_SCALED_VDD_3V3_SENSORS_CHANNEL)      | \
	 (1 << ADC1_SPARE_1_CHANNEL))

/* HW has to large of R termination on ADC todo:change when HW value is chosen */
#define BOARD_ADC_OPEN_CIRCUIT_V     (5.6f)

/* HW Version and Revision drive signals Default to 1 to detect */
#define BOARD_HAS_HW_VERSIONING

#define GPIO_HW_REV_DRIVE    /* PE7  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTE|GPIO_PIN7)
#define GPIO_HW_REV_SENSE    /* PB1   */ ADC1_GPIO(9)
#define GPIO_HW_VER_DRIVE    /* PB2   */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTB|GPIO_PIN2)
#define GPIO_HW_VER_SENSE    /* PB0   */ ADC1_GPIO(8)
#define HW_INFO_INIT         {'V','1','x', 'x',0}
#define HW_INFO_INIT_VER     1
#define HW_INFO_INIT_REV     1

#define HW_INFO_INIT_PREFIX         "V1"
//#define BOARD_NUM_SPI_CFG_HW_VERSIONS 3
#define V101   HW_VER_REV(0x1,0x1) // FMUV5,                    Rev 0
// #define V515   HW_VER_REV(0x1,0x5) // CUAV V5,                  Rev 5
// #define V540   HW_VER_REV(0x4,0x0) // HolyBro mini no can 2,3,  Rev 0
// #define V550   HW_VER_REV(0x5,0x0) // CUAV V5+,                 Rev 0
// #define V552   HW_VER_REV(0x5,0x2) // CUAV V5+ ICM42688P,       Rev 2
// #define V560   HW_VER_REV(0x6,0x0) // CUAV V5nano with can 2,   Rev 0
// #define V562   HW_VER_REV(0x6,0x2) // CUAV V5nano ICM42688P,    Rev 2

/* CAN Silence
 *
 * Silent mode control \ ESC Mux select
 */

#define GPIO_CAN1_SILENT  /* PC13  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTC|GPIO_PIN13)

#define UAVCAN_NUM_IFACES_RUNTIME 1

/* PWM
 */
#define DIRECT_PWM_OUTPUT_CHANNELS  10

#define BOARD_HAS_LED_PWM              1
#define BOARD_LED_PWM_DRIVE_ACTIVE_LOW 1

// #define BOARD_HAS_UI_LED_PWM            1

// #define BOARD_UI_LED_PWM_DRIVE_ACTIVE_LOW 1


/* Power supply control and monitoring GPIOs */

#define GPIO_VDD_5V_RADIO_EN          	/* PA10 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTA|GPIO_PIN10)
#define GPIO_VDD_5V_PERIPH_EN          	/* PC10 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTC|GPIO_PIN10)
#define GPIO_VDD_3V3_SD_CARD_EN         /* PE1  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTE|GPIO_PIN1)
#define GPIO_VDD_3V3_SENSORS_EN         /* PE8  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTE|GPIO_PIN8)

#define GPIO_nVDD_5V_RADIO_OC          /* PE15 */ (GPIO_INPUT |GPIO_FLOAT|GPIO_PORTE|GPIO_PIN15)
#define GPIO_nVDD_5V_PERIPH_OC         /* PC11 */ (GPIO_INPUT |GPIO_FLOAT|GPIO_PORTC|GPIO_PIN11)
#define GPIO_nVDD_3V3_SD_CARD_OC       /* PE0 */  (GPIO_INPUT |GPIO_FLOAT|GPIO_PORTE|GPIO_PIN0)

/* Define True logic Power Control in arch agnostic form */

#define VDD_5V_RADIO_EN(on_true)           px4_arch_gpiowrite(GPIO_VDD_5V_RADIO_EN, (on_true))
#define VDD_5V_PERIPH_EN(on_true)          px4_arch_gpiowrite(GPIO_VDD_5V_PERIPH_EN, (on_true))
#define VDD_3V3_SD_CARD_EN(on_true)        px4_arch_gpiowrite(GPIO_VDD_3V3_SD_CARD_EN, (on_true))
#define VDD_3V3_SENSORS_EN(on_true)        px4_arch_gpiowrite(GPIO_VDD_3V3_SENSORS_EN, (on_true))

/* Tone alarm output */

#define TONE_ALARM_TIMER        11  /* timer 11 */
#define TONE_ALARM_CHANNEL      1  /* PB9 TIM11_CH1 */

#define GPIO_BUZZER_1           /* PB9 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTB|GPIO_PIN9)

#define GPIO_TONE_ALARM_IDLE    GPIO_BUZZER_1
#define GPIO_TONE_ALARM         GPIO_TIM11_CH1OUT_1

/* USB OTG FS
 *
 * PA9  OTG_FS_VBUS VBUS sensing
 */
#define GPIO_OTGFS_VBUS         /* PA9 */ (GPIO_INPUT|GPIO_PULLDOWN|GPIO_SPEED_100MHz|GPIO_PORTA|GPIO_PIN9)

/* High-resolution timer */
#define HRT_TIMER               8  /* use timer8 for the HRT */
#define HRT_TIMER_CHANNEL       3  /* use capture/compare channel 3 */

/* RC Serial port */

#define RC_SERIAL_PORT                     "/dev/ttyS1"
//#define RC_SERIAL_SINGLEWIRE
#define BOARD_SUPPORTS_RC_SERIAL_PORT_OUTPUT

/* Safety Switch is HW version dependent on having an PX4IO
 * So we init to a benign state with the _INIT definition
 * and provide the the non _INIT one for the driver to make a run time
 * decision to use it.
 */
#define GPIO_nSAFETY_SWITCH_LED_OUT_INIT   /* PE2 */ (GPIO_INPUT|GPIO_FLOAT|GPIO_PORTE|GPIO_PIN2)
#define GPIO_nSAFETY_SWITCH_LED_OUT        /* PE2 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTE|GPIO_PIN2)

/* Enable the FMU to control it if there is no px4io fixme:This should be BOARD_SAFETY_LED(__ontrue) */
#define GPIO_LED_SAFETY GPIO_nSAFETY_SWITCH_LED_OUT
#define GPIO_SAFETY_SWITCH_IN              /* PE3 */ (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTE|GPIO_PIN3)
/* Enable the FMU to use the switch it if there is no px4io fixme:This should be BOARD_SAFTY_BUTTON() */
#define GPIO_BTN_SAFETY GPIO_SAFETY_SWITCH_IN /* Enable the FMU to control it if there is no px4io */

#define SDIO_SLOTNO                    0  /* Only one slot */
#define SDIO_MINOR                     0

/* SD card bringup does not work if performed on the IDLE thread because it
 * will cause waiting.  Use either:
 *
 *  CONFIG_LIB_BOARDCTL=y, OR
 *  CONFIG_BOARD_INITIALIZE=y && CONFIG_BOARD_INITTHREAD=y
 */

#if defined(CONFIG_BOARD_INITIALIZE) && !defined(CONFIG_LIB_BOARDCTL) && \
   !defined(CONFIG_BOARD_INITTHREAD)
#  warning SDIO initialization cannot be perfomed on the IDLE thread
#endif

/* By Providing BOARD_ADC_USB_CONNECTED (using the px4_arch abstraction)
 * this board support the ADC system_power interface, and therefore
 * provides the true logic GPIO BOARD_ADC_xxxx macros.
 */
#define BOARD_ADC_USB_CONNECTED (px4_arch_gpioread(GPIO_OTGFS_VBUS))

#define BOARD_ADC_USB_VALID     BOARD_ADC_USB_CONNECTED

#define BOARD_ADC_PERIPH_5V_OC  (!px4_arch_gpioread(GPIO_nVDD_5V_PERIPH_OC))
#define BOARD_ADC_RADIO_5V_OC (!px4_arch_gpioread(GPIO_nVDD_5V_RADIO_OC))


/* This board provides a DMA pool and APIs */
#define BOARD_DMA_ALLOC_POOL_SIZE 5120

/* This board provides the board_on_reset interface */

#define BOARD_HAS_ON_RESET 1

#define PX4_GPIO_INIT_LIST { \
		PX4_ADC_GPIO,                     \
		GPIO_HW_REV_DRIVE,                \
		GPIO_HW_VER_DRIVE,                \
		GPIO_CAN1_TX,                     \
		GPIO_CAN1_RX,                     \
		GPIO_CAN1_SILENT,              	  \
		GPIO_VDD_5V_PERIPH_EN,           \
		GPIO_nVDD_5V_PERIPH_OC,           \
		GPIO_VDD_5V_RADIO_EN,          \
		GPIO_nVDD_5V_RADIO_OC,          \
		GPIO_VDD_3V3_SENSORS_EN,   \
		GPIO_VDD_3V3_SD_CARD_EN,          \
		GPIO_TONE_ALARM_IDLE,             \
		GPIO_nSAFETY_SWITCH_LED_OUT_INIT, \
		GPIO_SAFETY_SWITCH_IN,            \
	}

#define BOARD_ENABLE_CONSOLE_BUFFER

#define BOARD_NUM_IO_TIMERS 4

#define PX4_I2C_BUS_MTD      3


__BEGIN_DECLS

/****************************************************************************************************
 * Public Types
 ****************************************************************************************************/

/****************************************************************************************************
 * Public data
 ****************************************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************************************
 * Public Functions
 ****************************************************************************************************/

/****************************************************************************
 * Name: stm32_sdio_initialize
 *
 * Description:
 *   Initialize SDIO-based MMC/SD card support
 *
 ****************************************************************************/

int stm32_sdio_initialize(void);

/****************************************************************************************************
 * Name: stm32_spiinitialize
 *
 * Description:
 *   Called to configure SPI chip select GPIO pins for the PX4FMU board.
 *
 ****************************************************************************************************/

extern void stm32_spiinitialize(void);

extern void stm32_usbinitialize(void);

extern void board_peripheral_reset(int ms);

#include <px4_platform_common/board_common.h>

#endif /* __ASSEMBLY__ */

__END_DECLS
