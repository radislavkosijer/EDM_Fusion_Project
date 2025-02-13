/*****************************************************************************
 * led.h
 *
 * Created on: February 13, 2025.
 * Author: Radislav Kosijer
 *
 * @brief Header file for LED control.
 *
 * This file contains the LED mapping definitions and function prototypes for
 * initializing, controlling, and managing the LED system.
 *****************************************************************************/

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <sys/platform.h>
#include <def21489.h>
#include <sru21489.h>
#include <SYSREG.h>
#include <stdio.h>
#include "adi_initialize.h"

/**
 * @brief LED Mapping (From datasheet):
 *
 * - LED1: DPI_P6    (Flag pin FLG4)
 * - LED2: DPI_P13   (Flag pin FLG5)
 * - LED3: DPI_P14   (Flag pin FLG6)
 * - LED4: DAI_P3    (SRU: DAI_PB03_I)
 * - LED5: DAI_P4    (SRU: DAI_PB04_I)
 * - LED6: DAI_P15   (SRU: DAI_PB15_I)
 * - LED7: DAI_P16   (SRU: DAI_PB16_I)
 * - LED8: DAI_P17   (SRU: DAI_PB17_I)
 */

/** @brief Total number of LEDs. */
#define NUM_LEDS 8

/**
 * @brief Initialize the LED system.
 *
 * This function configures the necessary hardware registers and settings
 * required for LED control.
 */
void led_init(void);

/**
 * @brief Turn all LEDs off.
 *
 * This function disables all LEDs by turning them off.
 */
void led_all_off(void);

/**
 * @brief Turn on a specific LED.
 *
 * @param led_index Index of the LED to turn on (0 to NUM_LEDS-1).
 */
void led_on(int led_index);

/**
 * @brief Turn off a specific LED.
 *
 * @param led_index Index of the LED to turn off (0 to NUM_LEDS-1).
 */
void led_off(int led_index);

/**
 * @brief Create a delay for a specified number of cycles.
 *
 * @param delayCount Number of cycles for which the execution will be delayed.
 */
void Delay_Cycles(unsigned int delayCount);

/**
 * @brief Initialize the System Routing Unit (SRU).
 *
 * This function configures the SRU for proper peripheral routing, including
 * the LED signal mapping.
 */
void InitSRU(void);

#endif /* LED_CONTROL_H */
