/**
 * @file screen_msgs.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief 
 * 
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _SCREEN_MSGS_H_ 
#define _SCREEN_MSGS_H_ 

//=======================================================================================
// Includes 

#include "includes_app.h"
#include "includes_drivers.h"

//=======================================================================================


//=======================================================================================
// Macros 

// Message lengths 
#define MTBDL_MSG_LEN_1_LINE 1        // Screen message is 1 line long 
#define MTBDL_MSG_LEN_2_LINE 2        // Screen message is 2 line long 
#define MTBDL_MSG_LEN_3_LINE 3        // Screen message is 3 line long 
#define MTBDL_MSG_LEN_4_LINE 4        // Screen message is 4 line long 

//=======================================================================================


//=======================================================================================
// Message declarations 

// Init state message 
extern hd44780u_msgs_t mtbdl_welcome_msg[MTBDL_MSG_LEN_1_LINE]; 

// Idle state message and data items per message line 
extern hd44780u_msgs_t mtbdl_idle_msg[MTBDL_MSG_LEN_4_LINE]; 
extern uint8_t mtbdl_idle_data_nums[MTBDL_MSG_LEN_4_LINE]; 

// Run prep state message 
extern hd44780u_msgs_t mtbdl_run_prep_msg[MTBDL_MSG_LEN_3_LINE]; 
extern uint8_t mtbdl_run_prep_data_nums[MTBDL_MSG_LEN_3_LINE]; 

// Run countdown state message 
extern hd44780u_msgs_t mtbdl_run_countdown_msg[MTBDL_MSG_LEN_1_LINE]; 

// Post run state message 
extern hd44780u_msgs_t mtbdl_postrun_msg[MTBDL_MSG_LEN_2_LINE]; 

// Data transfer selection state message 
extern hd44780u_msgs_t mtbdl_data_select_msg[MTBDL_MSG_LEN_3_LINE]; 

// Device search state message 
extern hd44780u_msgs_t mtbdl_dev_search_msg[MTBDL_MSG_LEN_2_LINE]; 

// Pre data receive (RX) state message 
extern hd44780u_msgs_t mtbdl_prerx_msg[MTBDL_MSG_LEN_3_LINE]; 

// Data receive (RX) state message 
extern hd44780u_msgs_t mtbdl_rx_msg[MTBDL_MSG_LEN_2_LINE]; 

// Post data receive (RX) state message 
extern hd44780u_msgs_t mtbdl_postrx_msg[MTBDL_MSG_LEN_1_LINE]; 

// Pre data send (TX) state message 
extern hd44780u_msgs_t mtbdl_pretx_msg[MTBDL_MSG_LEN_3_LINE]; 

// Data send (TX) state message 
extern hd44780u_msgs_t mtbdl_tx_msg[MTBDL_MSG_LEN_2_LINE]; 

// Post data send (TX) state message 
extern hd44780u_msgs_t mtbdl_posttx_msg[MTBDL_MSG_LEN_1_LINE]; 

// Pre calibration state message 
extern hd44780u_msgs_t mtbdl_precal_msg[MTBDL_MSG_LEN_4_LINE]; 

// Calibration state message 
extern hd44780u_msgs_t mtbdl_cal_msg[MTBDL_MSG_LEN_1_LINE]; 

// Low power state state message 
extern hd44780u_msgs_t mtbdl_low_pwr_msg[MTBDL_MSG_LEN_2_LINE]; 

// Fault state state message 
extern hd44780u_msgs_t mtbdl_fault_msg[MTBDL_MSG_LEN_2_LINE]; 

//=======================================================================================


//=======================================================================================
// Message formatting 

/**
 * @brief Format the idle state message 
 * 
 * @details The idle state message contains system values that are relevant to the user 
 *          and these values can change, This function updates the values of the idle 
 *          state message and triggers a write of this message to the screen. A list of 
 *          the values that the message contains are listed in the parameters below. 
 * 
 * @param fpsi : fork pressure (psi) - set by user, read from memory 
 * @param spsi : shock pressure (psi) - set by user, read from memory 
 * @param fc : fork compression setting - set by user, read from memory 
 * @param sl : shock lockout setting - set by user, read from memory 
 * @param fr : fork rebound setting - set by user, read from memory 
 * @param sr : shock rebound setting - set by user, read from memory 
 * @param soc : battery SOC - read from system 
 */
void mtbdl_set_idle_msg(
    uint8_t fpsi, 
    uint8_t spsi, 
    uint8_t fc, 
    uint8_t sl, 
    uint8_t fr, 
    uint8_t sr, 
    uint8_t soc); 


/**
 * @brief Format the run prep state message 
 * 
 * @details The run prep state message contains the GPS position lock status. This 
 *          information is displayed to the user before entering the run mode and allows 
 *          the user to know if they have GPS lock before beginning to record data. This 
 *          function updates GPS status information and triggers a write of this message 
 *          to the screen. 
 * 
 * @param navstat : GPS navigation status - read from the system 
 */
void mtbdl_set_run_prep_msg(
    uint8_t navstat); 

//=======================================================================================

#endif   // _SCREEN_MSGS_H_ 
