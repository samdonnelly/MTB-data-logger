/**
 * @file fatfs_controller_mock.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief FATFS controller mock interface 
 * 
 * @version 0.1
 * @date 2024-10-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _FATFS_CONTROLLER_MOCK_H_ 
#define _FATFS_CONTROLLER_MOCK_H_ 

//=======================================================================================
// Includes 
//=======================================================================================


//=======================================================================================
// Macros 

#define FATFS_MOCK_STR_SIZE 100 

//=======================================================================================


//=======================================================================================
// Mock functions 

// FATFS Controller Mock Init 
void fatfs_controller_mock_init(void); 


// FATFS Controller Mock: Get String 
void fatfs_controller_mock_get_str(
    char *buff, 
    uint8_t buff_len); 

//=======================================================================================

#endif   // _FATFS_CONTROLLER_MOCK_H_ 
