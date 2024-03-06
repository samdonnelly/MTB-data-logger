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

// Run prep state message 
extern hd44780u_msgs_t mtbdl_run_prep_msg[MTBDL_MSG_LEN_3_LINE]; 

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
extern hd44780u_msgs_t mtbdl_pretx_msg[MTBDL_MSG_LEN_4_LINE]; 

// Data send (TX) state message 
extern hd44780u_msgs_t mtbdl_tx_msg[MTBDL_MSG_LEN_2_LINE]; 

// Post data send (TX) state message 
extern hd44780u_msgs_t mtbdl_posttx_msg[MTBDL_MSG_LEN_1_LINE]; 

// Pre calibration state message 
extern hd44780u_msgs_t mtbdl_precal_msg[MTBDL_MSG_LEN_4_LINE]; 

// Calibration state message 
extern hd44780u_msgs_t mtbdl_cal_msg[MTBDL_MSG_LEN_1_LINE]; 

// Post calibration state message 
extern hd44780u_msgs_t mtbdl_postcal_msg[MTBDL_MSG_LEN_1_LINE]; 

// Low power state state message 
extern hd44780u_msgs_t mtbdl_low_pwr_msg[MTBDL_MSG_LEN_2_LINE]; 

// Fault state state message 
extern hd44780u_msgs_t mtbdl_fault_msg[MTBDL_MSG_LEN_2_LINE]; 

//==================================================
// Non-critical fault messages 

// Non-critical fault (NCF) message: no files to send 
extern hd44780u_msgs_t mtbdl_ncf_no_files_msg[MTBDL_MSG_LEN_1_LINE]; 

// Non-critical fault (NCF) message: too many files on drive 
extern hd44780u_msgs_t mtbdl_ncf_excess_files_msg[MTBDL_MSG_LEN_1_LINE]; 

// Non-critical fault (NCF) message: Bluetooth connection lost 
extern hd44780u_msgs_t mtbdl_ncf_bt_con_lost[MTBDL_MSG_LEN_1_LINE]; 

//==================================================

//=======================================================================================

#endif   // _SCREEN_MSGS_H_ 
