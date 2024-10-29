/**
 * @file stm32f4xx_it_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief STM32F4XX interrupts mock 
 * 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "stm32f4xx_it.h" 
#include "stm32f4xx_it_mock.h" 

//=======================================================================================


//=======================================================================================
// Globals 

// Instance of interrupt flag data record 
int_handle_flags_t handler_flags; 

//=======================================================================================
