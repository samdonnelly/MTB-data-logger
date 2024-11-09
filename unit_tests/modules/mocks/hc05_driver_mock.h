/**
 * @file hc05_driver_mock.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief HC-05 driver mock inteface 
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _HC05_DRIVER_MOCK_H_ 
#define _HC05_DRIVER_MOCK_H_ 

#ifdef __cplusplus
extern "C" {
#endif

//=======================================================================================
// Includes 
//=======================================================================================


//=======================================================================================
// Macros 
//=======================================================================================


//=======================================================================================
// Mock functions 

// Initialization 
void hc05_mock_init(void); 


// Set read data 
void hc05_mock_set_read_data(
    const char *read_data, 
    uint8_t len); 


// Get send count 
uint8_t hc05_mock_get_send_count(void); 

//=======================================================================================

#ifdef __cplusplus
}
#endif

#endif   // _HC05_DRIVER_MOCK_H_ 
