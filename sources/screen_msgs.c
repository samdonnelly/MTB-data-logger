/**
 * @file screen_msgs.c
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

//=======================================================================================
// Includes 

#include "screen_msgs.h" 

//=======================================================================================


//=======================================================================================
// MTB DL screen messages 

// Init state message 
mtbdl_msgs_t mtbdl_welcome_msg[MTBDL_MSG_LEN_1_LINE] = 
{
    {HD44780U_L2, "MTB DL", 7} 
}; 

// Idle state message 
mtbdl_msgs_t mtbdl_idle_msg[MTBDL_MSG_LEN_4_LINE] = 
{
    {HD44780U_L1, "F: PSI:    R:   D:", 0}, 
    {HD44780U_L2, "S: PSI:    R:   L:", 0}, 
    {HD44780U_L3, "SOC:    %", 0}, 
    {HD44780U_L4, "GPS: ", 0} 
}; 

// Pre run state message 
// mtbdl_msgs_t mtbdl_prerun_msg[]; 

// Run state message 
// mtbdl_msgs_t mtbdl_run_msg[]; 

// Post run state message 
// mtbdl_msgs_t mtbdl_postrun_msg[]; 

// Data transfer selection state message 
mtbdl_msgs_t mtbdl_data_select_msg[MTBDL_MSG_LEN_3_LINE] = 
{
    {HD44780U_L1, "1: TX", 0}, 
    {HD44780U_L2, "2: RX", 0}, 
    {HD44780U_L3, "3: Return", 0} 
}; 

// Device search state message 
mtbdl_msgs_t mtbdl_dev_search_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "Searching...", 0}, 
    {HD44780U_L2, "1: Cancel", 0} 
}; 

// Pre data receive (RX) state message 
mtbdl_msgs_t mtbdl_prerx_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "1: Get Data", 0}, 
    {HD44780U_L2, "2: Cancel", 0} 
}; 

// Data receive (RX) state message 
mtbdl_msgs_t mtbdl_rx_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "Reading...", 0}, 
    {HD44780U_L2, "1: Stop", 0} 
}; 

// Post data receive (RX) state message 
mtbdl_msgs_t mtbdl_postrx_msg[MTBDL_MSG_LEN_1_LINE] = 
{
    {HD44780U_L1, "Read Complete.", 0} 
}; 

// Pre data send (TX) state message 
mtbdl_msgs_t mtbdl_pretx_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "1: Send Data", 0}, 
    {HD44780U_L2, "2: Cancel", 0} 
}; 

// Data send (TX) state message 
mtbdl_msgs_t mtbdl_tx_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "Sending...", 0}, 
    {HD44780U_L2, "1: Stop", 0} 
}; 

// Post data send (TX) state message 
mtbdl_msgs_t mtbdl_posttx_msg[MTBDL_MSG_LEN_1_LINE] = 
{
    {HD44780U_L1, "Send Complete.", 0} 
}; 

// Pre calibration state message 
mtbdl_msgs_t mtbdl_precal_msg[MTBDL_MSG_LEN_4_LINE] = 
{
    {HD44780U_L1, "Place bike upright", 0}, 
    {HD44780U_L2, "on flat ground", 0}, 
    {HD44780U_L3, "1: Ready", 0}, 
    {HD44780U_L4, "2: Cancel", 0} 
}; 

// Calibration state message 
mtbdl_msgs_t mtbdl_cal_msg[MTBDL_MSG_LEN_1_LINE] = 
{
    {HD44780U_L1, "Calibrating...", 0} 
}; 

// Fault state state message 
mtbdl_msgs_t mtbdl_low_pwr_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "SOC low. Shut down", 0}, 
    {HD44780U_L2, "and charge battery.", 0} 
}; 

// Fault state state message 
mtbdl_msgs_t mtbdl_fault_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "System Fault.", 0}, 
    {HD44780U_L2, "4. Reset", 0} 
}; 

// Fault state state message 
mtbdl_msgs_t mtbdl_reset_msg[MTBDL_MSG_LEN_1_LINE] = 
{
    {HD44780U_L1, "Resetting...", 0}
}; 

//=======================================================================================


//=======================================================================================
// 

// 
void mtbdl_screen_msg_format(
    mtbdl_msgs_t *msg, 
    uint8_t msg_len)
{
    for (uint8_t i = 0; i < msg_len; i++)
    {
        hd44780u_line_set(msg->line, msg->msg, msg->offset); 
        msg++; 
    }
}


// 
void mtbdl_screen_line_clear(
    mtbdl_msgs_t *msg, 
    uint8_t msg_len)
{
    for (uint8_t i = 0; i < msg_len; i++)
    {
        hd44780u_line_clear(msg->line); 
        msg++; 
    }
}

//=======================================================================================
