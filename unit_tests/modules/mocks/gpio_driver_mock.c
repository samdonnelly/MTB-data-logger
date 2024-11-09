/**
 * @file gpio_driver_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief 
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "gpio_driver.h" 
#include "gpio_driver_mock.h" 

//=======================================================================================


//=======================================================================================
// Driver functions 

// GPIO communication initialization 
void gpio_port_init(void)
{    
    // 
}


// GPIO pin initialization 
void gpio_pin_init(
    GPIO_TypeDef  *gpio, 
    pin_selector_t pin_num,
    gpio_moder_t   moder,
    gpio_otyper_t  otyper,
    gpio_ospeedr_t ospeedr,
    gpio_pupdr_t   pupdr)
{
    if (gpio == NULL)
    {
        return; 
    }
}


// GPIO Alternate Function Register (AFR) configuration 
void gpio_afr(
    GPIO_TypeDef *gpio, 
    pin_selector_t pin, 
    bit_setter_t setpoint)
{
    if (gpio == NULL)
    {
        return; 
    }
}


// GPIOA write 
void gpio_write(
    GPIO_TypeDef *gpio, 
    gpio_pin_num_t pin_num,
    gpio_pin_state_t pin_state)
{
    if (gpio == NULL)
    {
        return; 
    }
}


// GPIO read 
GPIO_STATE gpio_read(
    GPIO_TypeDef *gpio, 
    gpio_pin_num_t pin_num)
{
    if (gpio == NULL)
    {
        return GPIO_LOW; 
    }

    return GPIO_HIGH; 
}


// GPIO port read 
GPIOX_DR gpio_port_read(GPIO_TypeDef *gpio)
{
    if (gpio == NULL)
    {
        return CLEAR; 
    }

    return ~CLEAR; 
}

//=======================================================================================


//=======================================================================================
// Mock functions 
//=======================================================================================
