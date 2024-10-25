/**
 * @file core_cm4.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Copied & modified code from 'core_cm4.h' located in: 
 *        --> ./../../../STM32F4-driver-library/stmcode/Drivers/CMSIS/Include 
 * 
 * @details This file/code is copied to satisfy the unit test compiler. Only the needed 
 *          code was copied over. The unit test makefile specifies this file over the 
 *          actual file. The actual file is used for production code. The code here is 
 *          slightly modified to emulate the functionality of the actual file. 
 * 
 * @version 0.1
 * @date 2024-10-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _CORE_CM4_H_
#define _CORE_CM4_H_

//=======================================================================================
// Includes 

#include <stdint.h>
#include "stm32f411xe.h" 

#include "core_cm4_mock.h" 

//=======================================================================================


//=======================================================================================
// Macros 

/**
 * IO definitions (access restrictions to peripheral registers) 
 * 
 * CMSIS_glob_defs CMSIS Global Defines
 * 
 * <strong>IO Type Qualifiers</strong> are used
 * to specify the access to peripheral variables.
 * for automatic generation of peripheral register debug information.
 */
#define     __IO    volatile             // !< Defines 'read / write' permissions 

//=======================================================================================


//=======================================================================================
// NVIC functions - Functions that manage interrupts and exceptions via the NVIC 

#define NVIC_EnableIRQ              __NVIC_EnableIRQ
#define NVIC_GetEnableIRQ           __NVIC_GetEnableIRQ
#define NVIC_DisableIRQ             __NVIC_DisableIRQ


/**
 * @brief Enable Interrupt 
 * 
 * @details Enables a device specific interrupt in the NVIC interrupt controller. 
 * 
 * @param IRQn : Device specific interrupt number 
 * 
 * @note IRQn must not be negative 
 */
static inline void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
    NVIC_SetEnableStatusIQR(NVIC_IQR_ENABLE); 
}


/**
 * @brief Get Interrupt Enable status 
 * 
 * @details Returns a device specific interrupt enable status from the NVIC interrupt 
 *          controller. 
 * 
 * @param IRQn : Device specific interrupt number 
 * @return uint32_t : 0 --> Interrupt is not enabled, 1 --> Interupt is enabled 
 * 
 * @note IRQn must not be negative 
 */
static inline uint32_t __NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
    return NVIC_GetEnableStatusIQR(); 
}


/**
 * @brief Disable Interrupt 
 * 
 * @details Disables a device specific interrupt in the NVIC interrupt controller. 
 * 
 * @param IRQn : Device specific interrupt number 
 * 
 * @note IRQn must not be negative.
 */
static inline void __NVIC_DisableIRQ(IRQn_Type IRQn)
{
    NVIC_SetEnableStatusIQR(NVIC_IQR_DISABLE); 
}

//=======================================================================================

#endif   // _CORE_CM4_H_ 
