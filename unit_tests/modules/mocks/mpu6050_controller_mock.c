/**
 * @file mpu6050_controller.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief 
 * 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "mpu6050_controller.h" 
#include "mpu6050_controller_mock.h" 

//=======================================================================================


//=======================================================================================
// Driver functions 

// MPU6050 controller initialization 
void mpu6050_controller_init(
    device_number_t device_num, 
    TIM_TypeDef *timer, 
    uint32_t sample_period)
{
    // 
}


// MPU6050 controller 
void mpu6050_controller(device_number_t device_num)
{
    // 
}


// Set low power flag 
void mpu6050_set_low_power(device_number_t device_num)
{
    // 
}


// Clear low power flag 
void mpu6050_clear_low_power(device_number_t device_num)
{
    // 
}


// Set the data sample type 
void mpu6050_set_smpl_type(
    device_number_t device_num, 
    mpu6050_sample_type_t sample_type)
{
    // 
}


// Set the read state 
void mpu6050_set_read_state(
    device_number_t device_num, 
    mpu6050_read_state_t read_type)
{
    // 
}


// Set the read flag 
void mpu6050_set_read_flag(device_number_t device_num)
{
    // 
}


// Set reset flag 
void mpu6050_set_reset_flag(device_number_t device_num)
{
    // 
}


// Get the controller state 
MPU6050_STATE mpu6050_get_state(device_number_t device_num)
{
    return MPU6050_INIT_STATE; 
}


// Get the controller fault code 
MPU6050_FAULT_CODE mpu6050_get_fault_code(device_number_t device_num)
{
    return NONE; 
}

//=======================================================================================


//=======================================================================================
// Mock functions 
//=======================================================================================
