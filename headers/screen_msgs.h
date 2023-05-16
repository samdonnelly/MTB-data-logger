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
#define MTBDL_WELCOME_MSG_LEN 1            // Number of lines in welcome message 
#define MTBDL_IDLE_MSG_LEN 4               // 

//=======================================================================================


//=======================================================================================
// Enums 
//=======================================================================================


//=======================================================================================
// Structures 

// Screen information 
typedef struct mtbdl_msgs_s 
{
    hd44780u_lines_t line; 
    char msg[HD44780U_LINE_LEN]; 
    uint8_t offset; 
}
mtbdl_msgs_t; 

//=======================================================================================


//=======================================================================================
// Message declarations 

// Init state message 
extern mtbdl_msgs_t mtbdl_welcome_msg[MTBDL_WELCOME_MSG_LEN]; 

// Idle state message 
extern mtbdl_msgs_t mtbdl_idle_msg[MTBDL_IDLE_MSG_LEN]; 

//=======================================================================================


//=======================================================================================
// 

/**
 * @brief 
 * 
 * @param msg 
 * @param msg_len 
 */
void mtbdl_screen_msg_format(
    mtbdl_msgs_t *msg, 
    uint8_t msg_len); 


/**
 * @brief 
 * 
 * @param msg 
 * @param msg_len 
 */
void mtbdl_screen_line_clear(
    mtbdl_msgs_t *msg, 
    uint8_t msg_len); 

//=======================================================================================

#endif   // _SCREEN_MSGS_H_ 
