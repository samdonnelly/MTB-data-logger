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
#define MTBDL_NUM_STATES 18            // Number of system states 

//=======================================================================================


//=======================================================================================
// Enums 

/**
 * @brief MTB data logger controller states 
 */
typedef enum {
    MTBDL_INIT_STATE,            // State 0 : Startup 
    MTBDL_IDLE_STATE,            // State 1 : Idle 
    MTBDL_CALIBRATE_STATE,       // State 2 : Calibrate 
    MTBDL_PRERUN_STATE,          // State 3 : Pre-run 
    MTBDL_RUN_STATE,             // State 4 : Run 
    MTBDL_POSTRUN_STATE,         // State 5 : Post-run 
    MTBDL_PRERX_STATE,           // State 6 : Pre data receive 
    MTBDL_RX_STATE,              // State 7 : Data receive 
    MTBDL_POSTRX_STATE,          // State 8 : Post data receive 
    MTBDL_PRETX_STATE,           // State 9 : Pre data send 
    MTBDL_TX_STATE,              // State 10 : Data send 
    MTBDL_POSTTX_STATE,          // State 11 : Post data send 
    MTBDL_PRELOWPWR_STATE,       // State 12 : Pre low power mode 
    MTBDL_LOWPWR_STATE,          // State 13 : Low power mode 
    MTBDL_POSTLOWPWR_STATE,      // State 14 : Post low power mode 
    MTBDL_CHARGE_STATE,          // State 15 : Charge 
    MTBDL_FAULT_STATE,           // State 16 : Fault 
    MTBDL_RESET_STATE            // State 17 : Reset 
} mtbdl_states_t; 

//=======================================================================================


//=======================================================================================
// Structures 

// 
typedef struct mtbdl_trackers_s 
{
    // System information 
    mtbdl_states_t state;                       // State of the system controller 
    GPIO_TypeDef *user_btn_port;                // GPIO port for user buttons 
    uint8_t user_btn_1;                         // User button 1 pin number 
    uint8_t user_btn_2;                         // User button 2 pin number 
    uint8_t user_btn_3;                         // User button 3 pin number 
    uint8_t user_btn_4;                         // User button 4 pin number 

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
void mtbdl_app_init(
    GPIO_TypeDef *user_btn_gpio, 
    gpio_pin_num_t user_btn_1, 
    gpio_pin_num_t user_btn_2, 
    gpio_pin_num_t user_btn_3, 
    gpio_pin_num_t user_btn_4); 


/**
 * @brief MTB DL application 
 * 
 * @details 
 */
void mtbdl_app(void);

//=======================================================================================


#endif  // _USER_APP_H_
