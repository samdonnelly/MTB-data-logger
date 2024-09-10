/**
 * @file mtbdl.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB data logger interface 
 * 
 * @version 0.1
 * @date 2024-03-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _MTBDL_H_ 
#define _MTBDL_H_ 

//=======================================================================================
// Includes 

#include "includes_drivers.h" 
#include "includes_app.h" 

//=======================================================================================


//=======================================================================================
// Enums 

// MTB data logger controller states 
typedef enum {
    MTBDL_INIT_STATE,            // State 0  : Startup 
    MTBDL_IDLE_STATE,            // State 1  : Idle 
    MTBDL_RUN_PREP_STATE,        // State 2  : Run prep 
    MTBDL_RUN_COUNTDOWN_STATE,   // State 3  : Run countdown 
    MTBDL_RUN_STATE,             // State 4  : Run 
    MTBDL_POSTRUN_STATE,         // State 5  : Post-run 
    MTBDL_DATA_SELECT_STATE,     // State 6  : Data transfer selection 
    MTBDL_DEV_SEARCH_STATE,      // State 7  : Search for Bluetooth connection 
    MTBDL_PRERX_STATE,           // State 8  : Pre data receive 
    MTBDL_RX_STATE,              // State 9  : Data receive 
    MTBDL_POSTRX_STATE,          // State 10 : Post data receive 
    MTBDL_PRETX_STATE,           // State 11 : Pre data send 
    MTBDL_TX_STATE,              // State 12 : Data send 
    MTBDL_POSTTX_STATE,          // State 13 : Post data send 
    MTBDL_PRECALIBRATE_STATE,    // State 14 : Pre calibration 
    MTBDL_CALIBRATE_STATE,       // State 15 : Calibration 
    MTBDL_POSTCALIBRATE_STATE,   // State 16 : Post calibration 
    MTBDL_LOWPWR_STATE,          // State 17 : Low power mode 
    MTBDL_FAULT_STATE,           // State 18 : Fault 
    MTBDL_RESET_STATE,           // State 19 : Reset 
    MTBDL_NUM_STATES             // Not a state - number of system states 
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
    tim_compare_t delay_timer;                  // Delay timing info 
    uint8_t led_state;                          // LED counter used with non-blocking timer 

    // Screen message 
    hd44780u_msgs_t *msg;                       // Message content 
    uint8_t msg_len;                            // Message length (number of lines) 

    // Modules 
    // mtbdl_data_t data_logging; 
    // mtbdl_params_t sys_params; 

    // User buttons 
    ui_btn_num_t btn_press;                     // Button press status 

    // State flags 
    uint8_t init          : 1;                  // Ensures the init state is run 
    uint8_t idle          : 1;                  // Idle state flag 
    uint8_t run           : 1;                  // Run state flag 
    uint8_t data_select   : 1;                  // Data transfer select state flag 
    uint8_t tx            : 1;                  // Send/transmit data state flag 
    uint8_t rx            : 1;                  // Read/receive data state flag 
    uint8_t calibrate     : 1;                  // Calibration state flag 
    uint8_t low_pwr       : 1;                  // Low power state flag 
    uint8_t noncrit_fault : 1;                  // Non-critical fault state flag 
    uint8_t fault         : 1;                  // Fault state flag 
    uint8_t reset         : 1;                  // Reset state flag 
}
mtbdl_trackers_t; 


// Instance of the system data trackers 
extern mtbdl_trackers_t mtbdl_trackers; 

//=======================================================================================


//=======================================================================================
// Functions 

/**
 * @brief Function that gets called once. Initialization function calls go here.
 */
void mtbdl_init(void); 


/**
 * @brief MTB DL application 
 * 
 * @details Checks for system faults, updates the status of user buttons, executes the 
 *          systems state machine and calls devices controllers. 
 */
void mtbdl_app(void); 

//=======================================================================================

#endif   // _MTBDL_H_ 
