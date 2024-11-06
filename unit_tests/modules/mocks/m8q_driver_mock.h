/**
 * @file m8q_driver_mock.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief M8Q driver mock interface 
 * 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _M8Q_DRIVER_MOCK_H_ 
#define _M8Q_DRIVER_MOCK_H_ 

//=======================================================================================
// Includes 
//=======================================================================================


//=======================================================================================
// Macros 
//=======================================================================================


//=======================================================================================
// Mock functions 

// M8Q driver mock init 
void m8q_mock_init(void); 


// Set UTC time 
void m8q_mock_set_time_utc_time(
    char *time_str, 
    uint8_t time_str_len); 


// Set UTC date 
void m8q_mock_set_time_utc_date(
    char *date_str, 
    uint8_t date_str_len); 

//=======================================================================================

#endif   // _M8Q_DRIVER_MOCK_H_ 
