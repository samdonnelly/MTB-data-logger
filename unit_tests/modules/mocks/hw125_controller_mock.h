/**
 * @file hw125_controller_mock.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief HW125 controller mock interface 
 * 
 * @version 0.1
 * @date 2024-10-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _HW125_CONTROLLER_MOCK_H_ 
#define _HW125_CONTROLLER_MOCK_H_ 

//=======================================================================================
// Includes 
//=======================================================================================


//=======================================================================================
// Macros 

#define HW125_MOCK_STR_SIZE 100 

//=======================================================================================


//=======================================================================================
// Mock functions 

// HW125 Controller Mock Init 
void hw125_controller_mock_init(void); 


// HW125 Controller Mock: Get String 
void hw125_controller_mock_get_str(
    char *buff, 
    uint8_t buff_len); 

//=======================================================================================

#endif   // _HW125_CONTROLLER_MOCK_H_ 
