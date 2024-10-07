/**
 * @file core_cm4_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Core CM4 mock 
 * 
 * @version 0.1
 * @date 2024-10-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "core_cm4_mock.h" 

//=======================================================================================


//=======================================================================================
// Global variables 

// Mock data record 
typedef struct core_cm4_mock_data_s 
{
    nvic_iqr_status_t iqr_status; 
}
core_cm4_mock_data_t; 

static core_cm4_mock_data_t mock_data; 

//=======================================================================================


//=======================================================================================
// Mock functions 

// Initialize the Core CM4 mock 
void CoreCM4MockInit(void)
{
    mock_data.iqr_status = NVIC_IQR_DISABLE; 
}


// Set the interrupt enable status 
void NVIC_SetEnableStatusIQR(nvic_iqr_status_t enable_status)
{
    mock_data.iqr_status = enable_status; 
}


// Get the interrupt enable status 
uint32_t NVIC_GetEnableStatusIQR(void)
{
    return (uint32_t)mock_data.iqr_status; 
}

//=======================================================================================
