/**
 * @file ws2812_config.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief WS2812 LED config interface 
 * 
 * @version 0.1
 * @date 2023-06-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _WS2812_CONFIG_H_ 
#define _WS2812_CONFIG_H_ 

//=======================================================================================
// Includes 

#include "stm32f411xe.h" 

//=======================================================================================


//=======================================================================================
// LED colours 

// LED off 
extern uint32_t mtbdl_led_clear; 

// LED 0 colours - Data loggng 
extern uint32_t mtbdl_led0_1;   // Data logging in progress 

// LED 1 colours - GPS position lock status 
extern uint32_t mtbdl_led1_1;   // Position lock 

// LED 2 colours - various states 
extern uint32_t mtbdl_led2_1;   // Bluetooth connection in TX/RX states 
extern uint32_t mtbdl_led2_2;   // Calibration state 
extern uint32_t mtbdl_led2_3;   // Initialization state 

// LED 3 colours - fault and low power states 
extern uint32_t mtbdl_led3_1;   // Fault (solid) and low power (flashing) states 

// LED 4 colours - user button 4 status 
extern uint32_t mtbdl_led4_1;   // Button pressed 

// LED 5 colours - user button 3 status 
extern uint32_t mtbdl_led5_1;   // Button pressed 

// LED 6 colours - user button 2 status 
extern uint32_t mtbdl_led6_1;   // Button pressed 

// LED 7 colours - user button 1 status 
extern uint32_t mtbdl_led7_1;   // Button pressed 

//=======================================================================================

#endif   // _WS2812_CONFIG_H_ 
