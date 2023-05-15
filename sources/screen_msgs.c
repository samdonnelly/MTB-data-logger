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

mtbdl_msgs_t mtbdl_welcome[MTBDL_WELCOME_MSG_LEN] = 
{
    {HD44780U_L2, "MTB DL", 7} 
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
