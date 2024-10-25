/**
 * @file core_cm4_mock.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Core CM4 mock interface 
 * 
 * @version 0.1
 * @date 2024-10-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _CORE_CM4_MOCK_H_ 
#define _CORE_CM4_MOCK_H_ 

//=======================================================================================
// Includes 

#include <stdint.h>

//=======================================================================================


//=======================================================================================
// Enums 

// NVIC IQR status 
typedef enum {
    NVIC_IQR_DISABLE, 
    NVIC_IQR_ENABLE 
} nvic_iqr_status_t; 

//=======================================================================================


//=======================================================================================
// Mock functions 

/**
 * @brief Initialize the Core CM4 mock 
 */
void CoreCM4MockInit(void); 


/**
 * @brief Set the interrupt enable status 
 * 
 * @param enable_status : 
 */
void NVIC_SetEnableStatusIQR(nvic_iqr_status_t enable_status); 


/**
 * @brief Get the interrupt enable status 
 * 
 * @return uint32_t 
 */
uint32_t NVIC_GetEnableStatusIQR(void); 

//=======================================================================================

#endif   // _CORE_CM4_MOCK_H_ 
