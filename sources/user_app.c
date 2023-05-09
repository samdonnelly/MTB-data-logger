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

/**
 * @brief 
 * 
 * @details 
 * 
 * @param mtbdl 
 */
void mtbdl_init_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief 
 * 
 * @details 
 * 
 * @param mtbdl 
 */
void mtbdl_idle_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief 
 * 
 * @details 
 * 
 * @param mtbdl 
 */
void mtbdl_fault_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief 
 * 
 * @details 
 * 
 * @param mtbdl 
 */
void mtbdl_reset_state(
    mtbdl_trackers_t *mtbdl); 

//=======================================================================================


//=======================================================================================
// Global variables 

// Instance of the system data trackers 
static mtbdl_trackers_t mtbdl_system_trackers; 


// Function pointers to system controller states 
static mtbdl_func_ptr_t mtbdl_state_table[MTBDL_NUM_STATES] = 
{
    &mtbdl_init_state, 
    &mtbdl_idle_state, 
    &mtbdl_fault_state, 
    &mtbdl_reset_state
}; 

//=======================================================================================


//=======================================================================================
// System controller 

// MTB DL controller init 
void mtbdl_controller_init(void)
{
    // Device and controller information 
    mtbdl_system_trackers.state = MTBDL_INIT_STATE; 

    // State flags 
    mtbdl_system_trackers.init = SET_BIT; 
}


// MTB DL application 
void mtbdl_app()
{
    // Local variables 
    mtbdl_states_t next_state = mtbdl_system_trackers.state; 

    // Check user inputs 

    //===================================================
    // System state machine 

    switch (next_state)
    {
        case MTBDL_INIT_STATE: 
            if (!mtbdl_system_trackers.init)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_IDLE_STATE: 
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
    mtbdl_state_table[next_state](&mtbdl_system_trackers); 

    // Update the state 
    mtbdl_system_trackers.state = next_state; 
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
