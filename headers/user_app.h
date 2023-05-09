/**
 * @file user_app.h
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

#ifndef _USER_APP_H_
#define _USER_APP_H_

//=======================================================================================
// Includes 

#include "includes_app.h"
#include "includes_drivers.h"

//=======================================================================================


//=======================================================================================
// Macros 

// System info 
#define MTBDL_NUM_STATES 4            // Number of system states 

//=======================================================================================


//=======================================================================================
// Enums 

/**
 * @brief MTB data logger controller states 
 */
typedef enum {
    MTBDL_INIT_STATE, 
    MTBDL_IDLE_STATE, 
    MTBDL_FAULT_STATE, 
    MTBDL_RESET_STATE 
} mtbdl_states_t; 

//=======================================================================================


//=======================================================================================
// Structures 

// 
typedef struct mtbdl_trackers_s 
{
    // Device and controller information 
    mtbdl_states_t state;                       // State of the system controller 

    // State flags 
    uint8_t init : 1;                           // Ensures the init state is run 
}
mtbdl_trackers_t; 

//=======================================================================================


//=======================================================================================
// Function pointers 

/**
 * @brief System state machine function pointer 
 */
typedef void (*mtbdl_func_ptr_t)(
    mtbdl_trackers_t *mtbdl); 

//=======================================================================================


//=======================================================================================
// Function Prototypes

/**
 * @brief MTB DL controller init 
 * 
 * @details 
 */
void mtbdl_controller_init(void); 


/**
 * @brief MTB DL application 
 * 
 * @details 
 */
void mtbdl_app(void);

//=======================================================================================


#endif  // _USER_APP_H_
