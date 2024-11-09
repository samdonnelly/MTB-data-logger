/**
 * @file switch_debounce_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Switch debounce mock 
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "switch_debounce.h" 
#include "switch_debounce_mock.h" 

//=======================================================================================


//=======================================================================================
// Driver functions 

// Switch debounce initialization 
void debounce_init(uint8_t pull_mask)
{
    // 
}


// Switch debounce calculation 
void debounce(uint8_t button_status)
{
    // 
}


// Get pressed button status 
uint8_t debounce_pressed(uint8_t button_select)
{
    return TRUE; 
}


// Get released button status 
uint8_t debounce_released(uint8_t button_select)
{
    return TRUE; 
}

//=======================================================================================


//=======================================================================================
// Mock functions 
//=======================================================================================
