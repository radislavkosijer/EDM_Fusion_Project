/*****************************************************************************
 * led.c
 *
 * Created on: February 13, 2025.
 * Author: Radislav Kosijer
 *
 * @brief Source file for LED control.
 *
 * This file implements functions to initialize and control LED states using
 * both flag pins and the System Routing Unit (SRU).
 *****************************************************************************/

#include "led.h"

/**
 * @brief Delay execution for a specified number of cycles.
 *
 * This function performs a busy-wait loop for the specified number of cycles.
 *
 * @param delayCount Number of cycles to delay.
 */
void Delay_Cycles(unsigned int delayCount)
{
    while(delayCount--);
}

/**
 * @brief Initialize the System Routing Unit (SRU) for LED configuration.
 *
 * This function configures the SRU for the following LED mappings:
 * - LED1, LED2, LED3 (DPI LEDs) are configured using flag pins and SRU routing.
 * - LED4, LED5, LED6, LED7, LED8 (DAI LEDs) are configured using SRU routing.
 *
 * Additionally, it sets the flag pins for LED1-LED3 as outputs and initializes
 * them to HIGH.
 */
void InitSRU(void)
{
	// Configuration for LED1, LED2, LED3 (DPI LEDs)
    SRU(HIGH, DPI_PBEN06_I);   // LED1
    SRU(FLAG4_O, DPI_PB06_I);

    SRU(HIGH, DPI_PBEN13_I);   // LED2
    SRU(FLAG5_O, DPI_PB13_I);

    SRU(HIGH, DPI_PBEN14_I);   // LED3
    SRU(FLAG6_O, DPI_PB14_I);

    // Configuration for LED4, LED5, LED6, LED7, LED8 (DAI LEDs)
    SRU(HIGH, DAI_PBEN03_I);   // LED4
    SRU(HIGH, DAI_PB03_I);

    SRU(HIGH, DAI_PBEN04_I);   // LED5
    SRU(HIGH, DAI_PB04_I);

    SRU(HIGH, DAI_PBEN15_I);   // LED6
    SRU(HIGH, DAI_PB15_I);

    SRU(HIGH, DAI_PBEN16_I);   // LED7
    SRU(HIGH, DAI_PB16_I);

    SRU(HIGH, DAI_PBEN17_I);   // LED8
    SRU(HIGH, DAI_PB17_I);

    // Set flag pins (for LED1-LED3) as outputs
    sysreg_bit_set(sysreg_FLAGS, (FLG4O | FLG5O | FLG6O));
    // Set HIGH on flag pins (initially turned on, later turned off)
    sysreg_bit_set(sysreg_FLAGS, (FLG4 | FLG5 | FLG6));
}

/**
 * @brief Initialize the LED system.
 *
 * This function initializes the hardware components required for LED control.
 * It calls the system initialization routines, configures the SRU, and turns
 * all LEDs off.
 */
void led_init(void)
{
    adi_initComponents();
    InitSRU();
    led_all_off();
}

/**
 * @brief Turn off all LEDs.
 *
 * This function turns off all LEDs by clearing the flag pins for the DPI LEDs
 * and setting the corresponding SRU outputs to LOW for the DAI LEDs.
 */
void led_all_off(void)
{
	// Turn off LED1, LED2, LED3 via flag pins
    sysreg_bit_clr(sysreg_FLAGS, FLG4);
    sysreg_bit_clr(sysreg_FLAGS, FLG5);
    sysreg_bit_clr(sysreg_FLAGS, FLG6);
    // Turn off LED4, LED5, LED6, LED7, LED8 via SRU
    SRU(LOW, DAI_PB03_I);
    SRU(LOW, DAI_PB04_I);
    SRU(LOW, DAI_PB15_I);
    SRU(LOW, DAI_PB16_I);
    SRU(LOW, DAI_PB17_I);
}

/**
 * @brief Turn on a specific LED.
 *
 * This function turns on an LED based on its index.
 *
 * @param led_index Index of the LED to be turned on (0 corresponds to LED1, 1 to LED2, etc.).
 */
void led_on(int led_index)
{
    switch (led_index) {
        case 0: // LED1 (DPI_P6)
            sysreg_bit_set(sysreg_FLAGS, FLG4);
            break;
        case 1: // LED2 (DPI_P13)
            sysreg_bit_set(sysreg_FLAGS, FLG5);
            break;
        case 2: // LED3 (DPI_P14)
            sysreg_bit_set(sysreg_FLAGS, FLG6);
            break;
        case 3: // LED4 (DAI_P3)
            SRU(HIGH, DAI_PB03_I);
            break;
        case 4: // LED5 (DAI_P4)
            SRU(HIGH, DAI_PB04_I);
            break;
        case 5: // LED6 (DAI_P15)
            SRU(HIGH, DAI_PB15_I);
            break;
        case 6: // LED7 (DAI_P16)
            SRU(HIGH, DAI_PB16_I);
            break;
        case 7: // LED8 (DAI_P17)
            SRU(HIGH, DAI_PB17_I);
            break;
        default:
            break;
    }
}

/**
 * @brief Turn off a specific LED.
 *
 * This function turns off an LED based on its index.
 *
 * @param led_index Index of the LED to be turned off (0 corresponds to LED1, 1 to LED2, etc.).
 */
void led_off(int led_index)
{
    switch (led_index) {
        case 0: // LED1
            sysreg_bit_clr(sysreg_FLAGS, FLG4);
            break;
        case 1: // LED2
            sysreg_bit_clr(sysreg_FLAGS, FLG5);
            break;
        case 2: // LED3
            sysreg_bit_clr(sysreg_FLAGS, FLG6);
            break;
        case 3: // LED4
            SRU(LOW, DAI_PB03_I);
            break;
        case 4: // LED5
            SRU(LOW, DAI_PB04_I);
            break;
        case 5: // LED6
            SRU(LOW, DAI_PB15_I);
            break;
        case 6: // LED7
            SRU(LOW, DAI_PB16_I);
            break;
        case 7: // LED8
            SRU(LOW, DAI_PB17_I);
            break;
        default:
            break;
    }
}
