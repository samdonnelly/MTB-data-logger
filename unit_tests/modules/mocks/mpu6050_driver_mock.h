/**
 * @file mpu6050_driver_mock.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MPU6050 driver mock interface 
 * 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _MPU6050_DRIVER_MOCK_H_ 
#define _MPU6050_DRIVER_MOCK_H_ 

//=======================================================================================
// Includes 
//=======================================================================================


//=======================================================================================
// Macros 
//=======================================================================================


//=======================================================================================
// Mock functions 

// MPU-6050 driver mock init 
void mpu6050_mock_init(void); 


// Set axis acceleration 
void mpu6050_mock_set_accel(
    int16_t ax, 
    int16_t ay, 
    int16_t az); 

//=======================================================================================

#endif   // _MPU6050_DRIVER_MOCK_H_ 
