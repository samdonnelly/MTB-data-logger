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
hd44780u_msgs_t mtbdl_welcome_msg[MTBDL_MSG_LEN_1_LINE] = 
{
    {HD44780U_L2, "MTB DL", 7} 
}; 

// Idle state message 
hd44780u_msgs_t mtbdl_idle_msg[MTBDL_MSG_LEN_4_LINE] = 
{
    {HD44780U_L1, "F: PSI:    R:   D:", 0}, 
    {HD44780U_L2, "S: PSI:    R:   L:", 0}, 
    {HD44780U_L3, "SOC:    %", 0}, 
    {HD44780U_L4, "1:RUN 2:TX/RX 3:CAL", 0} 
}; 

// Pre run state message 
hd44780u_msgs_t mtbdl_run_prep_msg[MTBDL_MSG_LEN_4_LINE] = 
{
    {HD44780U_L1, "Preparing for run...", 0}, 
    {HD44780U_L2, "NAVSTAT: ", 0}, 
    {HD44780U_L3, "1: Proceed", 0}, 
    {HD44780U_L4, "2: Cancel", 0} 
}; 

// Run countdown state message 
hd44780u_msgs_t mtbdl_run_countdown_msg[MTBDL_MSG_LEN_1_LINE] = 
{
    {HD44780U_L1, "Time to shred!", 0}
}; 

// Post run state message 
hd44780u_msgs_t mtbdl_postrun_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "Rad!", 0}, 
    {HD44780U_L2, "Saving data...", 0} 
}; 

// Data transfer selection state message 
hd44780u_msgs_t mtbdl_data_select_msg[MTBDL_MSG_LEN_3_LINE] = 
{
    {HD44780U_L1, "1: TX", 0}, 
    {HD44780U_L2, "2: RX", 0}, 
    {HD44780U_L3, "3: Return", 0} 
}; 

// Device search state message 
hd44780u_msgs_t mtbdl_dev_search_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "Searching...", 0}, 
    {HD44780U_L2, "1: Cancel", 0} 
}; 

// Pre data receive (RX) state message 
hd44780u_msgs_t mtbdl_prerx_msg[MTBDL_MSG_LEN_3_LINE] = 
{
    {HD44780U_L1, "Connected", 0}, 
    {HD44780U_L2, "1: Get Data", 0}, 
    {HD44780U_L3, "2: Cancel", 0} 
}; 

// Data receive (RX) state message 
hd44780u_msgs_t mtbdl_rx_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "Reading...", 0}, 
    {HD44780U_L2, "1: Stop", 0} 
}; 

// Post data receive (RX) state message 
hd44780u_msgs_t mtbdl_postrx_msg[MTBDL_MSG_LEN_1_LINE] = 
{
    {HD44780U_L1, "Read Complete", 0} 
}; 

// Pre data send (TX) state message 
hd44780u_msgs_t mtbdl_pretx_msg[MTBDL_MSG_LEN_3_LINE] = 
{
    {HD44780U_L1, "Connected", 0}, 
    {HD44780U_L2, "1: Send Data", 0}, 
    {HD44780U_L3, "2: Cancel", 0} 
}; 

// Data send (TX) state message 
hd44780u_msgs_t mtbdl_tx_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "Sending...", 0}, 
    {HD44780U_L2, "1: Stop", 0} 
}; 

// Post data send (TX) state message 
hd44780u_msgs_t mtbdl_posttx_msg[MTBDL_MSG_LEN_1_LINE] = 
{
    {HD44780U_L1, "Send Complete", 0} 
}; 

// Pre calibration state message 
hd44780u_msgs_t mtbdl_precal_msg[MTBDL_MSG_LEN_4_LINE] = 
{
    {HD44780U_L1, "Place bike upright", 0}, 
    {HD44780U_L2, "on flat ground", 0}, 
    {HD44780U_L3, "1: Ready", 0}, 
    {HD44780U_L4, "2: Cancel", 0} 
}; 

// Calibration state message 
hd44780u_msgs_t mtbdl_cal_msg[MTBDL_MSG_LEN_1_LINE] = 
{
    {HD44780U_L1, "Calibrating...", 0} 
}; 

// Low power state state message 
hd44780u_msgs_t mtbdl_low_pwr_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "SOC low. Shut down", 0}, 
    {HD44780U_L2, "and charge battery.", 0} 
}; 

// Fault state state message 
hd44780u_msgs_t mtbdl_fault_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L1, "System Fault.", 0}, 
    {HD44780U_L2, "4. Reset", 0} 
}; 

//=======================================================================================


//=======================================================================================
// 

// Format messages 

// // 
// void mtbdl_screen_msg_format(
//     mtbdl_msgs_t *msg, 
//     uint8_t msg_len)
// {
//     for (uint8_t i = 0; i < msg_len; i++)
//     {
//         hd44780u_line_set(msg->line, msg->msg, msg->offset); 
//         msg++; 
//     }
// }


// // 
// void mtbdl_screen_line_clear(
//     mtbdl_msgs_t *msg, 
//     uint8_t msg_len)
// {
//     for (uint8_t i = 0; i < msg_len; i++)
//     {
//         hd44780u_line_clear(msg->line); 
//         msg++; 
//     }
// }

//=======================================================================================
