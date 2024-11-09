/**
 * @file hd44780u_controller_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief HD44780U controller mock 
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "hd44780u_controller.h" 
#include "hd44780u_controller_mock.h" 

//=======================================================================================


//=======================================================================================
// Controller functions 

// Initialization 
void hd44780u_controller_init(TIM_TypeDef *timer)
{
    if (timer == NULL)
    {
        return; 
    }
}


// Controller 
void hd44780u_controller(void)
{
    // 
}


// Set power save mode 
void hd44780u_set_pwr_save_flag(void)
{
    // 
}


// Clear power save mode 
void hd44780u_clear_pwr_save_flag(void)
{
    // 
}


// Reset the power save state 
void hd44780u_wake_up(void)
{
    // 
}


// Set screen sleep time 
void hd44780u_set_sleep_time(uint32_t sleep_time)
{
    // 
}


// Message set 
void hd44780u_set_msg(
    hd44780u_msgs_t *msg, 
    uint8_t msg_len)
{
    if (msg == NULL)
    {
        return; 
    }
}


// Set write flag 
void hd44780u_set_write_flag(void)
{
    // 
}


// Set the clear screen flag 
void hd44780u_set_clear_flag(void)
{
    // 
}


// Set low power mode flag 
void hd44780u_set_low_pwr_flag(void)
{
    // 
}


// Clear low power mode flag 
void hd44780u_clear_low_pwr_flag(void)
{
    // 
}


// Set reset flag 
void hd44780u_set_reset_flag(void)
{
    // 
}


// Get state 
HD44780U_STATE hd44780u_get_state(void)
{
    return HD44780U_INIT_STATE; 
}


// Get fault code 
HD44780U_FAULT_CODE hd44780u_get_fault_code(void)
{
    return FALSE; 
}

//=======================================================================================


//=======================================================================================
// Mock functions 
//=======================================================================================
