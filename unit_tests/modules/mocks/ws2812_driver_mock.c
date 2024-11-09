/**
 * @file ws2812_driver_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief WS2812 driver mock 
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "ws2812_driver.h" 
#include "ws2812_driver_mock.h" 

//=======================================================================================


//=======================================================================================
// Driver functions 

// WS2812 initialization 
void ws2812_init(
    device_number_t device_num, 
    TIM_TypeDef *timer, 
    tim_channel_t tim_channel, 
    GPIO_TypeDef *gpio, 
    pin_selector_t pin)
{
    if ((timer == NULL) || (gpio == NULL))
    {
        return; 
    }
}


// Write data to the device 
void ws2812_send(
    device_number_t device_num, 
    const uint32_t *colour_data)
{
    if (colour_data == NULL)
    {
        return; 
    }
}

//=======================================================================================


//=======================================================================================
// Mock functions 
//=======================================================================================
