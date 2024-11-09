/**
 * @file hc05_driver_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief HC-05 driver mock 
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "hc05_driver.h" 
#include "hc05_driver_mock.h" 
#include "string_config.h" 

//=======================================================================================


//=======================================================================================
// Global data 

typedef struct hc05_mock_data_s 
{
    char data_buff[MTBDL_MAX_STR_LEN]; 
    uint8_t send_count; 
}
hc05_mock_data_t; 

hc05_mock_data_t hc05_mock_data; 

//=======================================================================================


//=======================================================================================
// Driver functions 

// HC05 initialization 
void hc05_init(
    USART_TypeDef *uart, 
    TIM_TypeDef *timer, 
    GPIO_TypeDef *gpio_at, 
    pin_selector_t at, 
    GPIO_TypeDef *gpio_en, 
    pin_selector_t en, 
    GPIO_TypeDef *gpio_state, 
    pin_selector_t state) 
{
    if ((uart == NULL) || 
        (timer == NULL) || 
        (gpio_at == NULL) || 
        (gpio_en == NULL) || 
        (gpio_state == NULL))
    {
        return; 
    }
}


// Set EN pin high to turn the device on 
void hc05_on(void)
{
    // 
}


// Set EN pin low to turn the device off 
void hc05_off(void)
{
    // 
}


// Send a string of data 
void hc05_send(const char *send_data)
{
    if (send_data == NULL)
    {
        return; 
    }

    hc05_mock_data.send_count++; 
}


// Check for available data 
HC05_DATA_STATUS hc05_data_status(void)
{
    return TRUE; 
}


// Read a string of data 
void hc05_read(
    char *receive_data, 
    uint8_t data_len)
{
    if ((receive_data == NULL) || (MTBDL_MAX_STR_LEN <= data_len))
    {
        return; 
    }

    memcpy((void *)receive_data, hc05_mock_data.data_buff, MTBDL_MAX_STR_LEN); 
}


// Read the connection status (STATE pin) 
HC05_CONNECT_STATUS hc05_status(void)
{
    return HC05_CONNECTED; 
}


// Clear the UART data register 
void hc05_clear(void)
{
    // 
}


// Get status flag 
uint8_t hc05_get_status(void)
{
    return FALSE; 
}


// Clear status flag 
void hc05_clear_status(void)
{
    // 
}

//=======================================================================================


//=======================================================================================
// Mock functions 

// Initialization 
void hc05_mock_init(void)
{
    memset((void *)hc05_mock_data.data_buff, CLEAR, sizeof(hc05_mock_data.data_buff)); 
    hc05_mock_data.send_count = CLEAR; 
}


// Set read data 
void hc05_mock_set_read_data(
    const char *read_data, 
    uint8_t len)
{
    if ((read_data == NULL) || (len <= MTBDL_MAX_STR_LEN))
    {
        return; 
    }

    memcpy((void *)hc05_mock_data.data_buff, read_data, len); 
}


// Get send count 
uint8_t hc05_mock_get_send_count(void)
{
    return hc05_mock_data.send_count; 
}

//=======================================================================================
