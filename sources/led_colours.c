/**
 * @file led_colours.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL LED colour definition 
 * 
 * @version 0.1
 * @date 2023-06-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//=======================================================================================
// Includes 

#include "led_colours.h"

//=======================================================================================


//=======================================================================================
// LED colours 

// LED off 
uint32_t mtbdl_led_clear = 0x00000000; 

// LED 0 colours - Data loggng 
uint32_t mtbdl_led0_1 = 0x001E0000;   // Data logging in progress 

// LED 1 colours - GPS position lock status 
uint32_t mtbdl_led1_1 = 0x001E1E1E;   // Position lock 

// LED 2 colours - various states 
uint32_t mtbdl_led2_1 = 0x0000001E;   // Bluetooth connection in TX/RX states 
uint32_t mtbdl_led2_2 = 0x00001E1E;   // Calibration state 

// LED 3 colours - fault and low power states 
uint32_t mtbdl_led3_1 = 0x00001E00;   // Fault (solid) and low power (flashing) states 

// LED 4 colours - user button 4 status 
uint32_t mtbdl_led4_1 = 0x001E1E1E;   // Button pressed 

// LED 5 colours - user button 3 status 
uint32_t mtbdl_led5_1 = 0x001E1E1E;   // Button pressed 

// LED 6 colours - user button 2 status 
uint32_t mtbdl_led6_1 = 0x001E1E1E;   // Button pressed 

// LED 7 colours - user button 1 status 
uint32_t mtbdl_led7_1 = 0x001E1E1E;   // Button pressed 

//=======================================================================================
