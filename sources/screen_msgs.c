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


// Idle state message and number of data items for each line 
hd44780u_msgs_t mtbdl_idle_msg[MTBDL_MSG_LEN_4_LINE] = 
{
    {HD44780U_L1, "F: PSI:%u C:%u R:%u", 0}, 
    {HD44780U_L2, "S: PSI:%u L:%u R:%u", 0}, 
    {HD44780U_L3, "SOC: %u", 0}, 
    {HD44780U_L4, "1:RUN 2:TX/RX 3:CAL", 0} 
}; 

uint8_t mtbdl_idle_data_nums[MTBDL_MSG_LEN_4_LINE] = {3, 3, 1, 0}; 


// Pre run state message and number of data items for each line 
hd44780u_msgs_t mtbdl_run_prep_msg[MTBDL_MSG_LEN_3_LINE] = 
{
    {HD44780U_L1, "NAVSTAT: %s", 0}, 
    {HD44780U_L2, "1: Proceed to run", 0}, 
    {HD44780U_L3, "2: Cancel", 0} 
}; 

uint8_t mtbdl_run_prep_data_nums[MTBDL_MSG_LEN_3_LINE] = {1, 0, 0}; 


// Run countdown state message 
hd44780u_msgs_t mtbdl_run_countdown_msg[MTBDL_MSG_LEN_1_LINE] = 
{
    {HD44780U_L2, "Time to shred!", 3}
}; 


// Post run state message 
hd44780u_msgs_t mtbdl_postrun_msg[MTBDL_MSG_LEN_2_LINE] = 
{
    {HD44780U_L2, "Rad!", 8}, 
    {HD44780U_L3, "Saving data...", 3} 
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
// Message formatting 

// Format the idle state message 
void mtbdl_set_idle_msg(
    uint8_t fpsi, 
    uint8_t spsi, 
    uint8_t fc, 
    uint8_t sl, 
    uint8_t fr, 
    uint8_t sr, 
    uint8_t soc)
{
    // Local variables 
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_4_LINE]; 

    // Create an editable copy of the message 
    for (uint8_t i = 0; i < MTBDL_MSG_LEN_4_LINE; i++) msg[i] = mtbdl_idle_msg[i]; 

    // Format the message with data 
    snprintf(msg[0].msg, HD44780U_LINE_LEN, mtbdl_idle_msg[0].msg, fpsi, fc, fr); 
    snprintf(msg[1].msg, HD44780U_LINE_LEN, mtbdl_idle_msg[1].msg, spsi, sl, sr); 
    snprintf(msg[2].msg, HD44780U_LINE_LEN, mtbdl_idle_msg[2].msg, soc); 

    // Set the screen message 
    hd44780u_set_msg(msg, MTBDL_MSG_LEN_4_LINE); 
}


// Format the run prep state message 
void mtbdl_set_run_prep_msg(
    uint8_t navstat)
{
    // Local variables 
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_3_LINE]; 

    // Create an editable copy of the message 
    for (uint8_t i = 0; i < MTBDL_MSG_LEN_3_LINE; i++) msg[i] = mtbdl_run_prep_msg[i]; 

    // Format the message with data 
    snprintf(msg[0].msg, HD44780U_LINE_LEN, mtbdl_run_prep_msg[0].msg, navstat); 

    // Set the screen message 
    hd44780u_set_msg(msg, MTBDL_MSG_LEN_3_LINE); 
}

//=======================================================================================
