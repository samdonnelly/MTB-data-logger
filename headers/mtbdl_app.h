/**
 * @file user_app.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL main application code header 
 * 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _MTBDL_APP_H_
#define _MTBDL_APP_H_

//=======================================================================================
// Includes 

#include "includes_app.h"
#include "includes_drivers.h"

//=======================================================================================


//=======================================================================================
// Macros 

// System info 
#define MTBDL_NUM_STATES 20            // Number of system states 
#define MTBDL_LCD_SLEEP 15000000       // (us) inactive time before screen backlight off 
#define MTBDL_INIT_WAIT 3000000        // (us) init state wait time 
#define MTBDL_RUN_WAIT 3000000         // (us) run states timer 

//=======================================================================================


//=======================================================================================
// Enums 

/**
 * @brief MTB data logger controller states 
 */
typedef enum {
    MTBDL_INIT_STATE,            // State 0 : Startup 
    MTBDL_IDLE_STATE,            // State 1 : Idle 
    MTBDL_RUN_PREP_STATE,        // State 2 : Run prep 
    MTBDL_RUN_COUNTDOWN_STATE,   // State 3 : Run countdown 
    MTBDL_RUN_STATE,             // State 4 : Run 
    MTBDL_POSTRUN_STATE,         // State 5 : Post-run 
    MTBDL_DATA_SELECT_STATE,     // State 6 : Data transfer selection 
    MTBDL_DEV_SEARCH_STATE,      // State 7 : Search for Bluetooth connection 
    MTBDL_PRERX_STATE,           // State 8 : Pre data receive 
    MTBDL_RX_STATE,              // State 9 : Data receive 
    MTBDL_POSTRX_STATE,          // State 10 : Post data receive 
    MTBDL_PRETX_STATE,           // State 11 : Pre data send 
    MTBDL_TX_STATE,              // State 12 : Data send 
    MTBDL_POSTTX_STATE,          // State 13 : Post data send 
    MTBDL_PRECALIBRATE_STATE,    // State 14 : Pre calibration 
    MTBDL_CALIBRATE_STATE,       // State 15 : Calibration 
    MTBDL_PRELOWPWR_STATE,       // State 16 : Pre low power mode 
    MTBDL_LOWPWR_STATE,          // State 17 : Low power mode 
    MTBDL_FAULT_STATE,           // State 18 : Fault 
    MTBDL_RESET_STATE            // State 19 : Reset 
} mtbdl_states_t; 

//=======================================================================================


//=======================================================================================
// Structures 

// MTB DL system trackers 
typedef struct mtbdl_trackers_s 
{
    // System information 
    mtbdl_states_t state;                       // State of the system controller 
    uint16_t fault_code;                        // System fault code 
    GPIO_TypeDef *user_btn_port;                // GPIO port for user buttons 

    // Timing information 
    TIM_TypeDef *timer_nonblocking;             // Timer used for non-blocking delays 
    tim_compare_t screen_timer;                 // Screen sleep timing info 

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
    uint8_t init        : 1;                    // Ensures the init state is run 
    uint8_t idle        : 1;                    // Idle state flag 
    uint8_t run         : 1;                    // Run state flag 
    uint8_t data_select : 1;                    // Data transfer select state flag 
    uint8_t tx          : 1;                    // Send/transmit data state flag 
    uint8_t rx          : 1;                    // Read/receive data state flag 
    uint8_t calibrate   : 1;                    // Calibration state flag 
    uint8_t low_pwr     : 1;                    // Low power state flag 
    uint8_t reset       : 1;                    // Reset state flag 
}
mtbdl_trackers_t; 

//=======================================================================================


//=======================================================================================
// Screen messages 
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
 * 
 * @param timer_nonblocking 
 * @param user_btn_gpio 
 * @param user_btn_1 
 * @param user_btn_2 
 * @param user_btn_3 
 * @param user_btn_4 
 */
void mtbdl_app_init(
    TIM_TypeDef *timer_nonblocking, 
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


#endif  // _MTBDL_APP_H_
