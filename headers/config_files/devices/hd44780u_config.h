/**
 * @file hd44789u_config.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief HD44780U screen config interface 
 * 
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _HD44780U_CONFIG_H_ 
#define _HD44780U_CONFIG_H_ 

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

extern hd44780u_msgs_t 
mtbdl_welcome_msg[MTBDL_MSG_LEN_1_LINE],         // Init state message 
mtbdl_idle_msg[MTBDL_MSG_LEN_4_LINE],            // Idle state message and data items per message line 
mtbdl_run_prep_msg[MTBDL_MSG_LEN_3_LINE],        // Run prep state message 
mtbdl_run_countdown_msg[MTBDL_MSG_LEN_1_LINE],   // Run countdown state message 
mtbdl_postrun_msg[MTBDL_MSG_LEN_2_LINE],         // Post run state message 
mtbdl_data_select_msg[MTBDL_MSG_LEN_3_LINE],     // Data transfer selection state message 
mtbdl_dev_search_msg[MTBDL_MSG_LEN_2_LINE],      // Device search state message 
mtbdl_prerx_msg[MTBDL_MSG_LEN_3_LINE],           // Pre data receive (RX) state message 
mtbdl_rx_msg[MTBDL_MSG_LEN_2_LINE],              // Data receive (RX) state message 
mtbdl_postrx_msg[MTBDL_MSG_LEN_1_LINE],          // Post data receive (RX) state message 
mtbdl_pretx_msg[MTBDL_MSG_LEN_4_LINE],           // Pre data send (TX) state message 
mtbdl_tx_msg[MTBDL_MSG_LEN_2_LINE],              // Data send (TX) state message 
mtbdl_posttx_msg[MTBDL_MSG_LEN_1_LINE],          // Post data send (TX) state message 
mtbdl_precal_msg[MTBDL_MSG_LEN_4_LINE],          // Pre calibration state message 
mtbdl_cal_msg[MTBDL_MSG_LEN_1_LINE],             // Calibration state message 
mtbdl_postcal_msg[MTBDL_MSG_LEN_1_LINE],         // Post calibration state message 
mtbdl_low_pwr_msg[MTBDL_MSG_LEN_2_LINE],         // Low power state state message 
mtbdl_fault_msg[MTBDL_MSG_LEN_2_LINE];           // Fault state state message 


// Non-critical fault (NCF) messages 
extern hd44780u_msgs_t 
mtbdl_ncf_no_files_msg[MTBDL_MSG_LEN_1_LINE],       // No files to send 
mtbdl_ncf_excess_files_msg[MTBDL_MSG_LEN_1_LINE],   // Too many files on drive 
mtbdl_ncf_bt_con_lost[MTBDL_MSG_LEN_1_LINE];        // Bluetooth connection lost 

//=======================================================================================

#endif   // _HD44780U_CONFIG_H_ 
