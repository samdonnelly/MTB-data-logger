/**
 * @file m8q_controller_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief M8Q controller mock 
 * 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "m8q_controller.h" 
#include "m8q_controller_mock.h" 

//=======================================================================================


//=======================================================================================
// Driver functions 

// Initialization 
void m8q_controller_init(TIM_TypeDef *timer)
{
    // 
}


// Controller 
void m8q_controller(void)
{
    // 
}


// Set the read flag 
void m8q_set_read_flag(void)
{
    // 
}


// Set the idle flag 
void m8q_set_idle_flag(void)
{
    // 
}


// Enter the low power state 
void m8q_set_low_pwr_flag(void)
{
    // 
}


// Exit the low power state 
void m8q_clear_low_pwr_flag(void)
{
    // 
}


// Set the reset flag 
void m8q_set_reset_flag(void)
{
    // 
}


// Get current controller state 
M8Q_STATE m8q_get_state(void)
{
    return M8Q_INIT_STATE; 
}


// Get low power flag state 
uint8_t m8q_get_lp_flag(void)
{
    return FALSE; 
}


// Get fault code 
M8Q_FAULT_CODE m8q_get_fault_code(void)
{
    return NONE; 
}

//=======================================================================================


//=======================================================================================
// Mock functions 
//=======================================================================================
