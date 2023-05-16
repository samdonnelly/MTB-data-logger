/**
 * @file user_app.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL main application code 
 * 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

//=======================================================================================
// Includes 

#include "mtbdl_app.h"

//=======================================================================================


//=======================================================================================
// Function prototypes 

// Initialization state 
void mtbdl_init_state(
    mtbdl_trackers_t *mtbdl); 


// Idle state 
void mtbdl_idle_state(
    mtbdl_trackers_t *mtbdl); 


// Run prep state 
void mtbdl_run_prep_state(
    mtbdl_trackers_t *mtbdl); 


// Run countdown state 
void mtbdl_run_prep_state(
    mtbdl_trackers_t *mtbdl); 


// Run state 
void mtbdl_run_state(
    mtbdl_trackers_t *mtbdl); 


// Post run state 
void mtbdl_postrun_state(
    mtbdl_trackers_t *mtbdl); 


// Data transfer selection state 
void mtbdl_data_select_state(
    mtbdl_trackers_t *mtbdl); 


// Search for Bluetooth connection state 
void mtbdl_dev_search_state(
    mtbdl_trackers_t *mtbdl); 


// Pre RX state 
void mtbdl_prerx_state(
    mtbdl_trackers_t *mtbdl); 


// RX state 
void mtbdl_rx_state(
    mtbdl_trackers_t *mtbdl); 


// Post RX state 
void mtbdl_postrx_state(
    mtbdl_trackers_t *mtbdl); 


// PRE TX state 
void mtbdl_pretx_state(
    mtbdl_trackers_t *mtbdl); 


// TX state 
void mtbdl_tx_state(
    mtbdl_trackers_t *mtbdl); 


// Post TX state 
void mtbdl_posttx_state(
    mtbdl_trackers_t *mtbdl); 


// Pre calibration state 
void mtbdl_precalibrate_state(
    mtbdl_trackers_t *mtbdl); 


// Calibrate state 
void mtbdl_calibrate_state(
    mtbdl_trackers_t *mtbdl); 


// Pre low power state 
void mtbdl_prelowpwr_state(
    mtbdl_trackers_t *mtbdl); 


// Low power state 
void mtbdl_lowpwr_state(
    mtbdl_trackers_t *mtbdl); 


// Fault state 
void mtbdl_fault_state(
    mtbdl_trackers_t *mtbdl); 


// Reset state 
void mtbdl_reset_state(
    mtbdl_trackers_t *mtbdl); 

//=======================================================================================


//=======================================================================================
// Global variables 

// Instance of the system data trackers 
static mtbdl_trackers_t mtbdl_trackers; 


// Function pointers to system controller states 
static mtbdl_func_ptr_t mtbdl_state_table[MTBDL_NUM_STATES] = 
{
    &mtbdl_init_state, 
    &mtbdl_idle_state, 
    &mtbdl_run_prep_state, 
    &mtbdl_run_state, 
    &mtbdl_postrun_state, 
    &mtbdl_data_select_state, 
    &mtbdl_dev_search_state, 
    &mtbdl_prerx_state, 
    &mtbdl_rx_state, 
    &mtbdl_postrx_state, 
    &mtbdl_pretx_state, 
    &mtbdl_tx_state, 
    &mtbdl_posttx_state, 
    &mtbdl_precalibrate_state, 
    &mtbdl_calibrate_state, 
    &mtbdl_prelowpwr_state, 
    &mtbdl_lowpwr_state, 
    &mtbdl_fault_state, 
    &mtbdl_reset_state
}; 

//=======================================================================================


//=======================================================================================
// System controller 

// MTB DL controller init 
void mtbdl_app_init(
    TIM_TypeDef *timer_nonblocking, 
    GPIO_TypeDef *user_btn_gpio, 
    gpio_pin_num_t user_btn_1, 
    gpio_pin_num_t user_btn_2, 
    gpio_pin_num_t user_btn_3, 
    gpio_pin_num_t user_btn_4)
{
    // System information 
    mtbdl_trackers.state = MTBDL_INIT_STATE; 
    mtbdl_trackers.user_btn_port = user_btn_gpio; 

    // Timing information 
    mtbdl_trackers.timer_nonblocking = timer_nonblocking; 
    // Screen timer 
    mtbdl_trackers.screen_timer.clk_freq = tim_get_pclk_freq(timer_nonblocking); 
    mtbdl_trackers.screen_timer.time_cnt_total = CLEAR; 
    mtbdl_trackers.screen_timer.time_cnt = CLEAR; 
    mtbdl_trackers.screen_timer.time_start = SET_BIT; 

    // User buttons 
    mtbdl_trackers.user_btn_1 = (uint8_t)user_btn_1; 
    mtbdl_trackers.user_btn_2 = (uint8_t)user_btn_2; 
    mtbdl_trackers.user_btn_3 = (uint8_t)user_btn_3; 
    mtbdl_trackers.user_btn_4 = (uint8_t)user_btn_4; 
    mtbdl_trackers.user_btn_1_block = CLEAR; 
    mtbdl_trackers.user_btn_2_block = CLEAR; 
    mtbdl_trackers.user_btn_3_block = CLEAR; 
    mtbdl_trackers.user_btn_4_block = CLEAR; 

    // State flags 
    mtbdl_trackers.init = SET_BIT; 
    mtbdl_trackers.idle = CLEAR_BIT; 
    mtbdl_trackers.run = CLEAR_BIT; 
    mtbdl_trackers.data_select = CLEAR_BIT; 
    mtbdl_trackers.tx = CLEAR_BIT; 
    mtbdl_trackers.rx = CLEAR_BIT; 
    mtbdl_trackers.calibrate = CLEAR_BIT; 
    mtbdl_trackers.low_pwr = CLEAR_BIT; 
    mtbdl_trackers.reset = CLEAR_BIT; 
}


// MTB DL application 
void mtbdl_app(void)
{
    // Local variables 
    mtbdl_states_t next_state = mtbdl_trackers.state; 

    // Check device statuses 

    //===================================================
    // User buttons 

    // Update user input button status 
    if (handler_flags.tim1_up_tim10_glbl_flag)
    {
        handler_flags.tim1_up_tim10_glbl_flag = CLEAR; 
        debounce((uint8_t)gpio_port_read(mtbdl_trackers.user_btn_port)); 
    }

    // Free the button pressed status as soon as possible 
    if (debounce_released(mtbdl_trackers.user_btn_1) && mtbdl_trackers.user_btn_1_block)
    {
        mtbdl_trackers.user_btn_1_block = CLEAR; 
    }
    if (debounce_released(mtbdl_trackers.user_btn_2) && mtbdl_trackers.user_btn_2_block)
    {
        mtbdl_trackers.user_btn_2_block = CLEAR; 
    }
    if (debounce_released(mtbdl_trackers.user_btn_3) && mtbdl_trackers.user_btn_3_block)
    {
        mtbdl_trackers.user_btn_3_block = CLEAR; 
    }
    if (debounce_released(mtbdl_trackers.user_btn_4) && mtbdl_trackers.user_btn_4_block)
    {
        mtbdl_trackers.user_btn_4_block = CLEAR; 
    }
    
    //===================================================

    //===================================================
    // System state machine 

    switch (next_state)
    {
        case MTBDL_INIT_STATE: 
            // Idle state flag set 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_IDLE_STATE: 
            // Fault code set 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }

            // Run state flag set 
            else if (mtbdl_trackers.run)
            {
                next_state = MTBDL_RUN_PREP_STATE; 
            }

            // Data transfer select state flag set 
            else if (mtbdl_trackers.data_select)
            {
                next_state = MTBDL_DATA_SELECT_STATE; 
            }

            // Calibration state flag set 
            else if (mtbdl_trackers.calibrate)
            {
                next_state = MTBDL_PRECALIBRATE_STATE;  
            }

            break; 

        case MTBDL_RUN_PREP_STATE: 
            // Fault code set 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }

            // Idle state flag set 
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            // Run state flag set 
            else if (mtbdl_trackers.run)
            {
                next_state = MTBDL_RUN_STATE; 
            }

            break; 

        case MTBDL_RUN_COUNTDOWN_STATE: 
            break; 

        case MTBDL_RUN_STATE: 
            // Fault code set 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }

            // Run state flag cleared 
            else if (!mtbdl_trackers.run)
            {
                next_state = MTBDL_POSTRUN_STATE; 
            }

            break; 

        case MTBDL_POSTRUN_STATE: 
            // Idle state flag set 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_DATA_SELECT_STATE: 
            // Fault code set 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }

            // TX state flag set 
            else if (mtbdl_trackers.tx)
            {
                next_state = MTBDL_PRETX_STATE; 
            }

            // RX state flag set 
            else if (mtbdl_trackers.rx)
            {
                next_state = MTBDL_PRERX_STATE; 
            }

            // Idle state flag set 
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_DEV_SEARCH_STATE: 
            break; 

        case MTBDL_PRERX_STATE: 
            // Fault code set 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }

            // Idle state flag set 
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            // RX state flag set 
            else if (mtbdl_trackers.rx)
            {
                next_state = MTBDL_RX_STATE; 
            }

            break; 

        case MTBDL_RX_STATE: 
            // Fault code set 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }

            // RX state flag cleared 
            else if (!mtbdl_trackers.rx)
            {
                next_state = MTBDL_POSTRX_STATE; 
            }

            break; 

        case MTBDL_POSTRX_STATE: 
            // Idle state flag set 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_PRETX_STATE: 
            // Fault code set 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }

            // Idle state flag set 
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            // TX state flag set 
            else if (mtbdl_trackers.tx)
            {
                next_state = MTBDL_TX_STATE; 
            }

            break; 

        case MTBDL_TX_STATE: 
            // Fault code set 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }

            // TX state flag cleared 
            else if (!mtbdl_trackers.tx)
            {
                next_state = MTBDL_POSTTX_STATE; 
            }

            break; 

        case MTBDL_POSTTX_STATE: 
            // Idle state flag set 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_PRECALIBRATE_STATE: 
            // Fault code set 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }

            // Idle state flag set 
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            // Calibration state flag set 
            else if (mtbdl_trackers.calibrate)
            {
                next_state = MTBDL_CALIBRATE_STATE;  
            }

            break; 
        
        case MTBDL_CALIBRATE_STATE: 
            // Idle state flag set 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_PRELOWPWR_STATE: 
            break; 

        case MTBDL_LOWPWR_STATE: 
            break; 

        case MTBDL_FAULT_STATE: 
            // Reset flag set 
            if (mtbdl_trackers.reset)
            {
                next_state = MTBDL_RESET_STATE; 
            }
            
            break; 

        case MTBDL_RESET_STATE: 
            // Init state flag set 
            if (mtbdl_trackers.init)
            {
                next_state = MTBDL_INIT_STATE; 
            }
            break; 

        default: 
            next_state = MTBDL_INIT_STATE; 
            break; 
    }

    //===================================================

    // Execute the state 
    mtbdl_state_table[next_state](&mtbdl_trackers); 

    // Update the state 
    mtbdl_trackers.state = next_state; 

    // Call device controllers 
    hd44780u_controller(); 
}

//=======================================================================================


//=======================================================================================
// State functions 

// Init state 
void mtbdl_init_state(
    mtbdl_trackers_t *mtbdl)
{
    if (mtbdl->init)
    {
        // Clear the line data 

        // Display the startup message 
        mtbdl_screen_msg_format(mtbdl_welcome_msg, MTBDL_MSG_LEN_1_LINE); 
        hd44780u_set_write_flag(); 

        // Put the HC-05 in low power mode 

        // Put the MPU-6050 in low power mode 

        // Check for existing bike / logging data in the filesystem 
    }
    
    mtbdl->init = CLEAR_BIT; 

    //==================================================
    // Init timer 

    // Wait for a short period of time before leaving the init state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->screen_timer.clk_freq, 
                    MTBDL_INIT_WAIT, 
                    &mtbdl->screen_timer.time_cnt_total, 
                    &mtbdl->screen_timer.time_cnt, 
                    &mtbdl->screen_timer.time_start))
    {
        mtbdl->screen_timer.time_start = SET_BIT; 

        // Clear the screen startup message 
        hd44780u_clear(); 
        mtbdl_screen_line_clear(mtbdl_welcome_msg, MTBDL_MSG_LEN_1_LINE); 

        // Set the idle state flag when ready 
        mtbdl->idle = SET_BIT; 
    }

    //==================================================
}


// Idle state 
void mtbdl_idle_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // Idle state screen message 

    if (mtbdl->idle)
    {
        // Format the display message with data 

        // Display the idle state message 
        mtbdl_screen_msg_format(mtbdl_idle_msg, MTBDL_MSG_LEN_4_LINE); 
        hd44780u_set_write_flag(); 
    }

    //==================================================
    
    mtbdl->idle = CLEAR_BIT; 

    //==================================================
    // Check user button input 

    // Button 1 - triggers the pre run state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->run = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    // Button 2 - triggers the data transfer selection state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->data_select = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
    }
    
    // Button 3 - triggers the alternate functions state 
    else if (debounce_pressed(mtbdl->user_btn_3) && !(mtbdl->user_btn_3_block))
    {
        mtbdl->calibrate = SET_BIT; 
        mtbdl->user_btn_3_block = SET_BIT; 
    }
    
    // Button 4 - Turns the screen backlight on 
    else if (debounce_pressed(mtbdl->user_btn_4) && !(mtbdl->user_btn_4_block))
    {
        mtbdl->user_btn_4_block = SET_BIT; 
        hd44780u_backlight_on(); 
        mtbdl->screen_timer.time_start = SET_BIT; 
    }
    
    //==================================================

    //==================================================
    // Screen settings 
    
    // Turn the screen backlight on and clear the screen when leaving the state 
    if (mtbdl->run | 
        mtbdl->data_select | 
        mtbdl->calibrate)
    {
        hd44780u_backlight_on(); 
        mtbdl->screen_timer.time_start = SET_BIT; 

        // Clear the idle state message 
        hd44780u_clear(); 
        mtbdl_screen_line_clear(mtbdl_idle_msg, MTBDL_MSG_LEN_4_LINE); 
    }

    // If the system has been inactive for long enough then turn the screen backlight off 
    else if (tim_compare(mtbdl->timer_nonblocking, 
                         mtbdl->screen_timer.clk_freq, 
                         MTBDL_LCD_SLEEP, 
                         &mtbdl->screen_timer.time_cnt_total, 
                         &mtbdl->screen_timer.time_cnt, 
                         &mtbdl->screen_timer.time_start))
    {
        hd44780u_backlight_off(); 
        mtbdl->screen_timer.time_start = SET_BIT; 
    }

    //==================================================
}


// Pre run state 
void mtbdl_run_prep_state(
    mtbdl_trackers_t *mtbdl)
{
    mtbdl->run = CLEAR_BIT; 

    //==================================================
    // Check user button input 

    // Button 1 - triggers the run state 
    // Add another condition to prevent button 1 press until the system is ready 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->run = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 

        // Turn the screen backlight off 
    }
    
    // Button 2 - cancels the run state --> triggers idle state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
    }
    
    //==================================================
}


// Run countdown state 
void mtbdl_run_prep_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Run state 
void mtbdl_run_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // Check user button input 

    // Button 1 - stops the run state --> triggers the post run state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->run = CLEAR_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 

        // Turn the screen backlight back on 
    }
    
    // Button 2 - Sets a marker 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->user_btn_2_block = SET_BIT; 
    }
    
    //==================================================

    // Record data 
}


// Post run state 
void mtbdl_postrun_state(
    mtbdl_trackers_t *mtbdl)
{
    // Set the idle state flag when ready 
    mtbdl->idle = SET_BIT; 
}


// Data transfer selection 
void mtbdl_data_select_state(
    mtbdl_trackers_t *mtbdl)
{
    mtbdl->data_select = CLEAR_BIT; 

    //==================================================
    // Check user button input 

    // Button 1 - triggers the pre receive (RX) state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->rx = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    // Button 2 - triggers the pre send (TX) state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->tx = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
    }
    
    // Button 3 - triggers the idle state - cancels data selection 
    else if (debounce_pressed(mtbdl->user_btn_3) && !(mtbdl->user_btn_3_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_3_block = SET_BIT; 
    }
    
    //==================================================
}


// Search for Bluetooth connection 
void mtbdl_dev_search_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Pre RX state 
void mtbdl_prerx_state(
    mtbdl_trackers_t *mtbdl)
{
    mtbdl->rx = CLEAR_BIT; 

    //==================================================
    // Check user button input 

    // Button 1 - triggers the rx state (only available when connected to a device) 
    // Add another condition to prevent button 1 press until the system is ready 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->rx = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    // Button 2 - cancels the rx state --> triggers idle state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
    }
    
    //==================================================
}


// RX state 
void mtbdl_rx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // Check user button input 

    // Button 1 - stops the rx state --> triggers the post rx state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->rx = CLEAR_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    //==================================================
}


// Post RX state 
void mtbdl_postrx_state(
    mtbdl_trackers_t *mtbdl)
{
    // Set the idle state flag when ready 
    mtbdl->idle = SET_BIT; 
}


// PRE TX state 
void mtbdl_pretx_state(
    mtbdl_trackers_t *mtbdl)
{
    mtbdl->tx = CLEAR_BIT; 

    //==================================================
    // Check user button input 

    // Button 1 - triggers the tx state (only available when connected to a device) 
    // Add another condition to prevent button 1 press until the system is ready 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->tx = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    // Button 2 - cancels the rx state --> triggers idle state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
    }
    
    //==================================================
}


// TX state 
void mtbdl_tx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // Check user button input 

    // Button 1 - stops the tx state --> triggers the post tx state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->tx = CLEAR_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    //==================================================
}


// Post TX state 
void mtbdl_posttx_state(
    mtbdl_trackers_t *mtbdl)
{
    // Set the idle state flag when ready 
    mtbdl->idle = SET_BIT; 
}


// Pre calibration state 
void mtbdl_precalibrate_state(
    mtbdl_trackers_t *mtbdl)
{
    mtbdl->calibrate = CLEAR_BIT; 

    //==================================================
    // Check user button input 

    // Button 1 - triggers the calibration state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->calibrate = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    // Button 2 - cancels the calibration state --> triggers idle state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
    }
    
    //==================================================
}


// Calibration state 
void mtbdl_calibrate_state(
    mtbdl_trackers_t *mtbdl)
{
    mtbdl->calibrate = CLEAR_BIT; 

    // Set the idle state flag once ready 
    mtbdl->idle = SET_BIT; 
}


// Pre low power state 
void mtbdl_prelowpwr_state(
    mtbdl_trackers_t *mtbdl)
{
    mtbdl->low_pwr = CLEAR_BIT; 

    // 
}


// Low power state 
void mtbdl_lowpwr_state(
    mtbdl_trackers_t *mtbdl)
{
    mtbdl->low_pwr = CLEAR_BIT; 

    // 
}


// Fault state 
void mtbdl_fault_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // Check user button input 

    // Button 4 - triggers a system reset (reset state) 
    // Add another condition to prevent premature reset 
    if (debounce_pressed(mtbdl->user_btn_4) && !(mtbdl->user_btn_4_block))
    {
        mtbdl->reset = SET_BIT; 
        mtbdl->user_btn_4_block = SET_BIT; 
    }
    
    //==================================================
}


// reset state 
void mtbdl_reset_state(
    mtbdl_trackers_t *mtbdl)
{
    mtbdl->reset = CLEAR_BIT; 

    // Set the init state flag once ready 
    mtbdl->init = SET_BIT; 
}

//=======================================================================================
