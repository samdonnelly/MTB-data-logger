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
#define MTBDL_NUM_STATES 19            // Number of system states 

//=======================================================================================


//=======================================================================================
// Enums 

/**
 * @brief MTB data logger controller states 
 */
typedef enum {
    MTBDL_INIT_STATE,            // State 0 : Startup 
    MTBDL_IDLE_STATE,            // State 1 : Idle 
    MTBDL_PRECALIBRATE_STATE,    // State 2 : Calibrate 
    MTBDL_CALIBRATE_STATE,       // State 3 : Calibrate 
    MTBDL_PRERUN_STATE,          // State 4 : Pre-run 
    MTBDL_RUN_STATE,             // State 5 : Run 
    MTBDL_POSTRUN_STATE,         // State 6 : Post-run 
    MTBDL_PRERX_STATE,           // State 7 : Pre data receive 
    MTBDL_RX_STATE,              // State 8 : Data receive 
    MTBDL_POSTRX_STATE,          // State 9 : Post data receive 
    MTBDL_PRETX_STATE,           // State 10 : Pre data send 
    MTBDL_TX_STATE,              // State 11 : Data send 
    MTBDL_POSTTX_STATE,          // State 12 : Post data send 
    MTBDL_PRELOWPWR_STATE,       // State 13 : Pre low power mode 
    MTBDL_LOWPWR_STATE,          // State 14 : Low power mode 
    MTBDL_POSTLOWPWR_STATE,      // State 15 : Post low power mode 
    MTBDL_CHARGE_STATE,          // State 16 : Charge 
    MTBDL_FAULT_STATE,           // State 17 : Fault 
    MTBDL_RESET_STATE            // State 18 : Reset 
} mtbdl_states_t; 

//=======================================================================================


//=======================================================================================
// Structures 

// 
typedef struct mtbdl_trackers_s 
{
    // System information 
    mtbdl_states_t state;                       // State of the system controller 
    uint16_t fault_code;                        // System fault code 
    GPIO_TypeDef *user_btn_port;                // GPIO port for user buttons 

    // User buttons 
    uint8_t user_btn_1;                         // User button 1 pin number 
    uint8_t user_btn_2;                         // User button 2 pin number 
    uint8_t user_btn_3;                         // User button 3 pin number 
    uint8_t user_btn_4;                         // User button 4 pin number 
    uint8_t user_btn_1_block : 1;               // Stops repeated actions on btn 1 press 
    uint8_t user_btn_2_block : 1;               // Stops repeated actions on btn 2 press 
    uint8_t user_btn_3_block : 1;               // Stops repeated actions on btn 3 press 
    uint8_t user_btn_4_block : 1;               // Stops repeated actions on btn 4 press 

    // State flags 
    uint8_t init      : 1;                      // Ensures the init state is run 
    uint8_t idle      : 1;                      // Idle state flag 
    uint8_t calibrate : 1;                      // Calibration state flag 
    uint8_t run       : 1;                      // Run state flag 
    uint8_t tx        : 1;                      // Send/transmit data state flag 
    uint8_t rx        : 1;                      // Read/receive data state flag 
    uint8_t low_pwr   : 1;                      // Low power state flag 
    uint8_t reset     : 1;                      // Reset state flag 
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
