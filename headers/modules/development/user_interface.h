/**
 * @file user_interface.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief User interface header 
 * 
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _USER_INTERFACE_H_ 
#define _USER_INTERFACE_H_ 

//=======================================================================================
// Includes 

#include "includes_drivers.h" 

//=======================================================================================


//=======================================================================================
// Structures 

// User interface data record 
typedef struct mtbdl_ui_s 
{
    uint8_t test; 

    // User buttons 
    uint8_t user_btn_1;                         // User button 1 pin number 
    uint8_t user_btn_2;                         // User button 2 pin number 
    uint8_t user_btn_3;                         // User button 3 pin number 
    uint8_t user_btn_4;                         // User button 4 pin number 
    uint8_t user_btn_1_block : 1;               // Stops repeated actions on btn 1 press 
    uint8_t user_btn_2_block : 1;               // Stops repeated actions on btn 2 press 
    uint8_t user_btn_3_block : 1;               // Stops repeated actions on btn 3 press 
    uint8_t user_btn_4_block : 1;               // Stops repeated actions on btn 4 press 
}
mtbdl_ui_t; 

//=======================================================================================


//=======================================================================================
// Input functions 

// Button press check 
void ui_button_press(void); 


// Button release check 
void ui_button_release(void); 

//=======================================================================================

#endif   // _USER_INTERFACE_H_ 
