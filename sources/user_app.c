/**
 * @file user_app.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Code that gets called once per loop
 * 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

//=======================================================================================
// Includes 

#include "user_app.h"

//=======================================================================================


//=======================================================================================
// Function prototypes 

// Initialization state 
void mtbdl_init_state(
    mtbdl_trackers_t *mtbdl); 


// Idle state 
void mtbdl_idle_state(
    mtbdl_trackers_t *mtbdl); 


// Calibrate state 
void mtbdl_calibrate_state(
    mtbdl_trackers_t *mtbdl); 


// Pre run state 
void mtbdl_prerun_state(
    mtbdl_trackers_t *mtbdl); 


// Run state 
void mtbdl_run_state(
    mtbdl_trackers_t *mtbdl); 


// Post run state 
void mtbdl_postrun_state(
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


// Pre low power state 
void mtbdl_prelowpwr_state(
    mtbdl_trackers_t *mtbdl); 


// Low power state 
void mtbdl_lowpwr_state(
    mtbdl_trackers_t *mtbdl); 


// Post low power state 
void mtbdl_postlowpwr_state(
    mtbdl_trackers_t *mtbdl); 


// Charge state 
void mtbdl_charge_state(
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
    &mtbdl_calibrate_state, 
    &mtbdl_prerun_state, 
    &mtbdl_run_state, 
    &mtbdl_postrun_state, 
    &mtbdl_prerx_state, 
    &mtbdl_rx_state, 
    &mtbdl_postrx_state, 
    &mtbdl_pretx_state, 
    &mtbdl_tx_state, 
    &mtbdl_posttx_state, 
    &mtbdl_prelowpwr_state, 
    &mtbdl_lowpwr_state, 
    &mtbdl_postlowpwr_state, 
    &mtbdl_charge_state, 
    &mtbdl_fault_state, 
    &mtbdl_reset_state
}; 

//=======================================================================================


//=======================================================================================
// System controller 

// MTB DL controller init 
void mtbdl_app_init(
    GPIO_TypeDef *user_btn_gpio, 
    gpio_pin_num_t user_btn_1, 
    gpio_pin_num_t user_btn_2, 
    gpio_pin_num_t user_btn_3, 
    gpio_pin_num_t user_btn_4)
{
    // System information 
    mtbdl_trackers.state = MTBDL_INIT_STATE; 
    mtbdl_trackers.user_btn_port = user_btn_gpio; 
    mtbdl_trackers.user_btn_1 = (uint8_t)user_btn_1; 
    mtbdl_trackers.user_btn_2 = (uint8_t)user_btn_2; 
    mtbdl_trackers.user_btn_3 = (uint8_t)user_btn_3; 
    mtbdl_trackers.user_btn_4 = (uint8_t)user_btn_4; 

    // State flags 
    mtbdl_trackers.init = SET_BIT; 
}


// MTB DL application 
void mtbdl_app(void)
{
    // Local variables 
    mtbdl_states_t next_state = mtbdl_trackers.state; 

    // Check device statuses 

    // Update user input button status 
    if (handler_flags.tim1_up_tim10_glbl_flag)
    {
        handler_flags.tim1_up_tim10_glbl_flag = CLEAR; 
        debounce((uint8_t)gpio_port_read(mtbdl_trackers.user_btn_port)); 
    }

    //===================================================
    // System state machine 

    switch (next_state)
    {
        case MTBDL_INIT_STATE: 
            if (!mtbdl_trackers.init)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_IDLE_STATE: 
            break; 

        case MTBDL_CALIBRATE_STATE: 
            break; 

        case MTBDL_PRERUN_STATE: 
            break; 

        case MTBDL_RUN_STATE: 
            break; 

        case MTBDL_POSTRUN_STATE: 
            break; 

        case MTBDL_PRERX_STATE: 
            break; 

        case MTBDL_RX_STATE: 
            break; 

        case MTBDL_POSTRX_STATE: 
            break; 

        case MTBDL_PRETX_STATE: 
            break; 

        case MTBDL_TX_STATE: 
            break; 

        case MTBDL_POSTTX_STATE: 
            break; 

        case MTBDL_PRELOWPWR_STATE: 
            break; 

        case MTBDL_LOWPWR_STATE: 
            break; 

        case MTBDL_POSTLOWPWR_STATE: 
            break; 

        case MTBDL_CHARGE_STATE: 
            break; 

        case MTBDL_FAULT_STATE: 
            break; 

        case MTBDL_RESET_STATE: 
            next_state = MTBDL_INIT_STATE; 
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
}

//=======================================================================================


//=======================================================================================
// State functions 

// Init state 
void mtbdl_init_state(
    mtbdl_trackers_t *mtbdl)
{
    // Clear the init flag so we can go to the idle state 
    mtbdl->init = CLEAR_BIT; 
}


// Idle state 
void mtbdl_idle_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Calibrate state 
void mtbdl_calibrate_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Pre run state 
void mtbdl_prerun_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Run state 
void mtbdl_run_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Post run state 
void mtbdl_postrun_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Pre RX state 
void mtbdl_prerx_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// RX state 
void mtbdl_rx_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Post RX state 
void mtbdl_postrx_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// PRE TX state 
void mtbdl_pretx_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// TX state 
void mtbdl_tx_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Post TX state 
void mtbdl_posttx_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Pre low power state 
void mtbdl_prelowpwr_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Low power state 
void mtbdl_lowpwr_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Post low power state 
void mtbdl_postlowpwr_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Charge state 
void mtbdl_charge_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// Fault state 
void mtbdl_fault_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}


// reset state 
void mtbdl_reset_state(
    mtbdl_trackers_t *mtbdl)
{
    // 
}

//=======================================================================================
