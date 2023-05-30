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
void mtbdl_run_countdown_state(
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


// Low power state 
void mtbdl_lowpwr_state(
    mtbdl_trackers_t *mtbdl); 


// Non-critical fault state 
void mtbdl_noncrit_fault_state(
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
    &mtbdl_init_state,                  // State 0 : Startup 
    &mtbdl_idle_state,                  // State 1 : Idle 
    &mtbdl_run_prep_state,              // State 2 : Run prep 
    &mtbdl_run_countdown_state,         // State 3 : Run countdown 
    &mtbdl_run_state,                   // State 4 : Run 
    &mtbdl_postrun_state,               // State 5 : Post run 
    &mtbdl_data_select_state,           // State 6 : Data transfer selection 
    &mtbdl_dev_search_state,            // State 7 : Search for Bluetooth connection 
    &mtbdl_prerx_state,                 // State 8 : Pre data receive 
    &mtbdl_rx_state,                    // State 9 : Data receive 
    &mtbdl_postrx_state,                // State 10 : Post data receive 
    &mtbdl_pretx_state,                 // State 11 : Pre data send 
    &mtbdl_tx_state,                    // State 12 : Data send 
    &mtbdl_posttx_state,                // State 13 : Post data send 
    &mtbdl_precalibrate_state,          // State 14 : Pre calibration 
    &mtbdl_calibrate_state,             // State 15 : Calibration 
    &mtbdl_lowpwr_state,                // State 16 : Low power mode 
    &mtbdl_noncrit_fault_state,         // State 17 : Non-critical fault 
    &mtbdl_fault_state,                 // State 18 : Fault 
    &mtbdl_reset_state                  // State 19 : Reset 
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
    mtbdl_trackers.delay_timer.clk_freq = tim_get_pclk_freq(timer_nonblocking); 
    mtbdl_trackers.delay_timer.time_cnt_total = CLEAR; 
    mtbdl_trackers.delay_timer.time_cnt = CLEAR; 
    mtbdl_trackers.delay_timer.time_start = SET_BIT; 

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
    mtbdl_trackers.noncrit_fault = CLEAR_BIT; 
    mtbdl_trackers.fault = CLEAR_BIT; 
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

            // Non-critical fault state flag set 
            else if (mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_NONCRIT_FAULT_STATE; 
            }

            // Idle state flag set 
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            // Run state flag set 
            else if (mtbdl_trackers.run)
            {
                next_state = MTBDL_RUN_COUNTDOWN_STATE; 
            }

            break; 

        case MTBDL_RUN_COUNTDOWN_STATE: 
            if (mtbdl_trackers.run)
            {
                next_state = MTBDL_RUN_STATE; 
            }

            break; 

        case MTBDL_RUN_STATE: 
            // Fault code set 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }

            // Run state flag set 
            else if (mtbdl_trackers.run)
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

            // Idle state flag set 
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            // Data select state flag set 
            else if (mtbdl_trackers.data_select)
            {
                next_state = MTBDL_DEV_SEARCH_STATE; 
            }

            break; 

        case MTBDL_DEV_SEARCH_STATE: 
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

            // Data select and RX state flags set 
            else if (mtbdl_trackers.data_select && mtbdl_trackers.rx)
            {
                next_state = MTBDL_PRERX_STATE; 
            }

            // Data select and TX state flags set 
            else if (mtbdl_trackers.data_select && mtbdl_trackers.tx)
            {
                next_state = MTBDL_PRETX_STATE; 
            }

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

            // RX state flag set 
            else if (mtbdl_trackers.rx)
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

            // Non-critical fault state flag set 
            else if (mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_NONCRIT_FAULT_STATE; 
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

            // TX state flag set 
            else if (mtbdl_trackers.tx)
            {
                next_state = MTBDL_POSTTX_STATE; 
            }

            break; 

        case MTBDL_POSTTX_STATE: 
            // TX flag set 
            if (mtbdl_trackers.tx)
            {
                next_state = MTBDL_PRETX_STATE; 
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

        case MTBDL_LOWPWR_STATE: 
            // Idle state flag set 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_NONCRIT_FAULT_STATE: 
            // Idle state flag set 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
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
    hw125_controller(); 
}

//=======================================================================================


//=======================================================================================
// State functions 

// Init state 
void mtbdl_init_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->init)
    {
        // Clear the line data 

        // Display the startup message 
        hd44780u_set_msg(mtbdl_welcome_msg, MTBDL_MSG_LEN_1_LINE); 

        // HC-05 
        // - Put into low power mode 

        // MPU-6050 
        // - Put into low power mode 
    }
    
    mtbdl->init = CLEAR_BIT; 
    
    //==================================================

    //==================================================
    // Checks 

    // Wait for the SD card to be mounted then access the file system 
    if (hw125_get_state() == HW125_ACCESS_STATE)
    {
        // Set the check flag 
        hw125_set_check_flag(); 

        // Set up the file structure and system info 
        mtbdl_file_sys_setup(); 
    }

    //==================================================

    //==================================================
    // State exit 

    // Wait for a short period of time before leaving the init state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_WAIT, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Clear the screen startup message 
        hd44780u_set_clear_flag(); 

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
    // State entry 

    if (mtbdl->idle)
    {
        // Display the idle state message 
        mtbdl_set_idle_msg(); 

        // Set the screen to power save mode 
        hd44780u_set_pwr_save_flag(); 
        hd44780u_set_sleep_time(MTBDL_LCD_SLEEP); 
    }

    mtbdl->idle = CLEAR_BIT; 

    //==================================================

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
        hd44780u_wake_up(); 
    }
    
    //==================================================

    //==================================================
    // Checks 

    // If GPS position lock is found then put the module in LP mode - can hot start 

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->run || 
        mtbdl->data_select || 
        mtbdl->calibrate)
    {
        // Clear the idle state message 
        hd44780u_set_clear_flag(); 

        // Take the screen out of power save mode 
        hd44780u_clear_pwr_save_flag(); 
    }

    //==================================================
}


// Pre run state 
void mtbdl_run_prep_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->run)
    {
        // Check the log file name 
        if (mtbdl_log_name_prep()) 
        {
            // New file name created - display the run prep state message 
            mtbdl_set_run_prep_msg(); 
        }
        else 
        {
            // Too many log files saved - abort 
            hd44780u_set_msg(mtbdl_ncf_excess_files_msg, MTBDL_MSG_LEN_1_LINE); 
            mtbdl->noncrit_fault = SET_BIT; 
        }
    }

    mtbdl->run = CLEAR_BIT; 

    //==================================================

    //==================================================
    // Check user button input 

    // Button 1 - triggers the run state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->run = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 

        // Prepare the log file 
        mtbdl_log_file_prep(); 
    }
    
    // Button 2 - cancels the run state --> triggers idle state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
    }
    
    //==================================================

    //==================================================
    // Checks 

    // Check for GPS lock - update the display message as needed 

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->run || mtbdl->idle)
    {
        // Clear the run prep state message 
        hd44780u_set_clear_flag(); 
    }

    //==================================================
}


// Run countdown state 
void mtbdl_run_countdown_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->run)
    {
        // Display the run countdown state message 
        hd44780u_set_msg(mtbdl_run_countdown_msg, MTBDL_MSG_LEN_1_LINE); 

        // SD card will be written to constantly so no need for the check state 
        hw125_clear_check_flag(); 
    }

    mtbdl->run = CLEAR_BIT; 

    //==================================================

    //==================================================
    // State exit 

    // Wait for a short period of time before leaving the init state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_WAIT, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Put the screen in low power mode 
        hd44780u_set_low_pwr_flag(); 

        // Set the run state flag when ready 
        mtbdl->run = SET_BIT; 
    }

    //==================================================
}


// Run state 
void mtbdl_run_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    mtbdl->run = CLEAR_BIT; 

    //==================================================
    
    //==================================================
    // Check user button input 

    // Button 1 - stops the run state --> triggers the post run state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->run = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    // Button 2 - Sets a marker 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->user_btn_2_block = SET_BIT; 
    }
    
    //==================================================

    //==================================================
    // Record data 

    // Log the system data 
    mtbdl_logging(); 

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->run)
    {
        // Take the screen out of low power mode 
        hd44780u_clear_low_pwr_flag(); 
    }

    //==================================================
}


// Post run state 
void mtbdl_postrun_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->run)
    {
        // Display the post run state message 
        hd44780u_set_msg(mtbdl_postrun_msg, MTBDL_MSG_LEN_2_LINE); 

        // Close the open data log file 
        mtbdl_log_end(); 
    }

    mtbdl->run = CLEAR_BIT; 

    //==================================================
    
    //==================================================
    // State exit 

    // Wait for a short period of time before leaving the init state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_WAIT, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Clear the post run state message 
        hd44780u_set_clear_flag(); 

        // Set the SD card controller check flag 
        hw125_set_check_flag(); 

        // Set the idle state flag when ready 
        mtbdl->idle = SET_BIT; 
    }
    
    //==================================================
}


// Data transfer selection 
void mtbdl_data_select_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->data_select)
    {
        // Display the data select state message 
        hd44780u_set_msg(mtbdl_data_select_msg, MTBDL_MSG_LEN_3_LINE); 
    }

    mtbdl->data_select = CLEAR_BIT; 
    
    //==================================================

    //==================================================
    // Check user button input 

    // Button 1 - triggers the pre receive (RX) state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->rx = SET_BIT; 
        mtbdl->data_select = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    // Button 2 - triggers the pre send (TX) state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->tx = SET_BIT; 
        mtbdl->data_select = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
    }
    
    // Button 3 - triggers the idle state - cancels data selection 
    else if (debounce_pressed(mtbdl->user_btn_3) && !(mtbdl->user_btn_3_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_3_block = SET_BIT; 
    }
    
    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->data_select || mtbdl->idle)
    {
        // Clear the data select state message 
        hd44780u_set_clear_flag(); 
    }

    //==================================================
}


// Search for Bluetooth connection 
void mtbdl_dev_search_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->data_select)
    {
        // Display the device connection search state message 
        hd44780u_set_msg(mtbdl_dev_search_msg, MTBDL_MSG_LEN_2_LINE); 
    }

    mtbdl->data_select = CLEAR_BIT; 

    //==================================================

    //==================================================
    // Check user button input 

    // Button 1 - triggers the idle state - cancels device search 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    //==================================================
    
    //==================================================
    // Check for device connection 

    // TODO replace this with a check to HC-05 connection status once HC-05 control is added 
    if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->data_select = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
    }

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->idle || mtbdl->data_select)
    {
        // Clear the device connection search state message 
        hd44780u_set_clear_flag(); 
    }

    //==================================================
}


// Pre RX state 
void mtbdl_prerx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->rx)
    {
        // Display the pre rx state message 
        hd44780u_set_msg(mtbdl_prerx_msg, MTBDL_MSG_LEN_3_LINE); 
    }
    
    mtbdl->rx = CLEAR_BIT; 
    mtbdl->data_select = CLEAR_BIT; 
    
    //==================================================

    //==================================================
    // Check user button input 

    // Button 1 - triggers the rx state 
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

    //==================================================
    // State exit 

    if (mtbdl->rx || mtbdl->idle)
    {
        // Clear the pre rx state message 
        hd44780u_set_clear_flag(); 
    }

    //==================================================
}


// RX state 
void mtbdl_rx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->rx)
    {
        // Display the rx state message 
        hd44780u_set_msg(mtbdl_rx_msg, MTBDL_MSG_LEN_2_LINE); 
    }
    
    mtbdl->rx = CLEAR_BIT; 

    //==================================================

    //==================================================
    // Check user button input 

    // Button 1 - stops the rx state --> triggers the post rx state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->rx = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    //==================================================

    //==================================================
    // Read data 

    mtbdl_rx(); 

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->rx)
    {
        // Clear the rx state message 
        hd44780u_set_clear_flag(); 
    }

    //==================================================
}


// Post RX state 
void mtbdl_postrx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->rx)
    {
        // Display the post rx state message 
        hd44780u_set_msg(mtbdl_postrx_msg, MTBDL_MSG_LEN_1_LINE); 

        // Save the bike parameters 
        mtbdl_write_bike_params(HW125_MODE_OEW); 
    }
    
    mtbdl->rx = CLEAR_BIT; 

    //==================================================

    //==================================================
    // State exit 

    // Wait for a short period of time before leaving the post rx state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_WAIT, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Clear the post rx state message 
        hd44780u_set_clear_flag(); 

        // Set the idle state flag when ready 
        mtbdl->idle = SET_BIT; 
    }
    
    //==================================================
}


// Pre TX state 
void mtbdl_pretx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->tx)
    {
        // Prepare the log file to send 
        if (mtbdl_tx_prep())
        {
            // File ready - display the pre tx state message 
            mtbdl_set_pretx_msg(); 
        }
        else 
        {
            // File does not exist - display a non-critical fault message 
            hd44780u_set_msg(mtbdl_ncf_no_files_msg, MTBDL_MSG_LEN_1_LINE); 
            mtbdl->noncrit_fault = SET_BIT; 
        }
    }
    
    mtbdl->tx = CLEAR_BIT; 
    mtbdl->data_select = CLEAR_BIT; 
    
    //==================================================

    //==================================================
    // Check user button input 

    // Button 1 - triggers the tx state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->tx = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    // Button 2 - cancels the tx state --> triggers idle state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 

        // End the data transfer 
        mtbdl_tx_end(); 
    }
    
    //==================================================

    //==================================================
    // Checks 

    // Check that there is still a Bluetooth connection 
    // If connection lost then revert back to the idle state 

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->tx || mtbdl->idle)
    {
        // Clear the pre rx state message 
        hd44780u_set_clear_flag(); 
    }

    //==================================================
}


// TX state 
void mtbdl_tx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->tx)
    {
        // Display the tx state message 
        hd44780u_set_msg(mtbdl_tx_msg, MTBDL_MSG_LEN_2_LINE); 
    }
    
    mtbdl->tx = CLEAR_BIT; 

    //==================================================

    //==================================================
    // Check user button input 

    // Button 1 - stops the tx state --> triggers the post tx state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->tx = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
    }
    
    //==================================================

    //==================================================
    // Send data 

    // Transfer data log contents and set the tx bit if the transfer finishes 
    if (mtbdl_tx())
    {
        mtbdl->tx = SET_BIT; 
    }

    //==================================================

    //==================================================
    // Checks 

    // Check that there is still a Bluetooth connection 
    // If connection lost then stop the transfer and exit the state 
    
    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->tx)
    {
        // Clear the tx state message 
        hd44780u_set_clear_flag(); 
    }

    //==================================================
}


// Post TX state 
void mtbdl_posttx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->tx)
    {
        // Display the post tx state message 
        hd44780u_set_msg(mtbdl_posttx_msg, MTBDL_MSG_LEN_1_LINE); 

        // End the data transfer 
        mtbdl_tx_end(); 
    }
    
    mtbdl->tx = CLEAR_BIT; 

    //==================================================

    //==================================================
    // State exit 

    // Wait for a short period of time before leaving the post tx state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_WAIT, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Clear the post tx state message 
        hd44780u_set_clear_flag(); 

        // Go back to the pre-tx state to send more log data 
        mtbdl->tx = SET_BIT; 
    }
    
    //==================================================
}


// Pre calibration state 
void mtbdl_precalibrate_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->calibrate)
    {
        // Display the pre calibration state message 
        hd44780u_set_msg(mtbdl_precal_msg, MTBDL_MSG_LEN_4_LINE); 
    }

    mtbdl->calibrate = CLEAR_BIT; 
    
    //==================================================

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

    //==================================================
    // State exit 

    if (mtbdl->calibrate || mtbdl->idle)
    {
        // Clear the pre calibration state message 
        hd44780u_set_clear_flag(); 
    }

    //==================================================
}


// Calibration state 
void mtbdl_calibrate_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->calibrate)
    {
        // Display the calibration state message 
        hd44780u_set_msg(mtbdl_cal_msg, MTBDL_MSG_LEN_1_LINE); 
    }

    mtbdl->calibrate = CLEAR_BIT; 
    
    //==================================================

    //==================================================
    // Sample data 

    // Record the data as needed in the data handling code 

    //==================================================

    //==================================================
    // State exit 

    // Wait until calibration is done before leaving the state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_CAL_SAMPLE_TIME, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Record the calibration data 
        mtbdl_write_sys_params(HW125_MODE_OEW); 

        // Clear the calibration state message 
        hd44780u_set_clear_flag(); 

        // Set the idle state flag when ready 
        mtbdl->idle = SET_BIT; 
    }
    
    //==================================================
}


// Low power state 
void mtbdl_lowpwr_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->low_pwr)
    {
        // Display the low power state message 
        hd44780u_set_msg(mtbdl_low_pwr_msg, MTBDL_MSG_LEN_2_LINE); 

        // Put the screen into power save mode 
        hd44780u_set_pwr_save_flag(); 
        hd44780u_set_sleep_time(MTBDL_LCD_LP_SLEEP); 

        // Put devices into low power mode 
    }

    mtbdl->low_pwr = CLEAR_BIT; 
    
    //==================================================

    //==================================================
    // Check user button input 
    
    // Button 4 - Turns the screen backlight on 
    if (debounce_pressed(mtbdl->user_btn_4) && !(mtbdl->user_btn_4_block))
    {
        mtbdl->user_btn_4_block = SET_BIT; 
        hd44780u_wake_up(); 
    }
    
    //==================================================

    //==================================================
    // Check SOC 

    // If SOC is above the minimum threshold then we can exit low power state 
    // TODO replace this with a check on the SOC 
    // Button 3 - triggers the idle state 
    if (debounce_pressed(mtbdl->user_btn_3) && !(mtbdl->user_btn_3_block))
    {
        mtbdl->user_btn_3_block = SET_BIT; 
        mtbdl->low_pwr = SET_BIT; 
    }

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->low_pwr)
    {
        mtbdl->low_pwr = CLEAR_BIT; 

        // Clear the idle state message 
        hd44780u_set_clear_flag(); 

        // Take the screen out of power save mode 
        hd44780u_clear_pwr_save_flag(); 

        // Set the idle state flag 
        mtbdl->idle = SET_BIT; 
    }

    //==================================================
}


// Non-critical fault state 
void mtbdl_noncrit_fault_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    // Message gets displayed from the state that triggered this state 

    mtbdl->noncrit_fault = CLEAR_BIT; 

    //==================================================

    //==================================================
    // State exit 

    // Allow the non-critical fault message to be displayed before returning to idle 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_WAIT, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Clear non-critical fault message 
        hd44780u_set_clear_flag(); 

        // Set the idle state flag when ready 
        mtbdl->idle = SET_BIT; 
    }

    //==================================================
}


// Fault state 
void mtbdl_fault_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (!mtbdl->fault)
    {
        // Display the fault state message 
        hd44780u_set_msg(mtbdl_fault_msg, MTBDL_MSG_LEN_2_LINE); 

        // Record the fault code in a log 
    }

    mtbdl->fault = SET_BIT; 

    //==================================================

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

    //==================================================
    // State exit 

    if (mtbdl->reset)
    {
        mtbdl->fault_code = CLEAR; 
        mtbdl->fault = CLEAR_BIT; 

        // Clear the fault state message 
        hd44780u_set_clear_flag(); 
    }

    //==================================================
}


// reset state 
void mtbdl_reset_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    mtbdl->reset = CLEAR_BIT; 
    
    //==================================================

    //==================================================
    // State exit 

    // Set the init state flag once ready 
    mtbdl->init = SET_BIT; 
    
    //==================================================
}

//=======================================================================================
