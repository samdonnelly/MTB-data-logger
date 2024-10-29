/**
 * @file mpu6050_driver_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MPU6050 driver mock 
 * 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "mpu6050_driver.h" 
#include "mpu6050_driver_mock.h" 

//=======================================================================================


//=======================================================================================
// Driver functions 

// MPU6050 Initialization 
void mpu6050_init(
    device_number_t device_num, 
    I2C_TypeDef *i2c, 
    mpu6050_i2c_addr_t mpu6050_addr,
    uint8_t standby_status, 
    mpu6050_dlpf_cfg_t dlpf_cfg,
    MPU6050_SMPLRT_DIV smplrt_div,
    mpu6050_afs_sel_set_t afs_sel,
    mpu6050_fs_sel_set_t fs_sel)
{
    // 
}


// MPU6050 INT pin initialization 
void mpu6050_int_pin_init(
    device_number_t device_num, 
    GPIO_TypeDef *gpio, 
    pin_selector_t pin)
{
    // 
}


// MPU6050 reference point set 
void mpu6050_calibrate(device_number_t device_num)
{
    // 
}


// MPU6050 low power mode config 
void mpu6050_low_pwr_config(
    device_number_t device_num, 
    mpu6050_sleep_mode_t sleep)
{
    // 
}


// MPU6050 Accelerometer Measurements registers read
void mpu6050_accel_read(device_number_t device_num)
{
    // 
}


// MPU6050 Gyroscope Measurements registers read
void mpu6050_gyro_read(device_number_t device_num)
{
    // 
}


// MPU6050 Temperature Measurements registers read 
void mpu6050_temp_read(device_number_t device_num)
{
    // 
}


// MPU6050 read all 
void mpu6050_read_all(device_number_t device_num)
{
    // 
}


// MPU6050 self-test
MPU6050_ST_RESULT mpu6050_self_test(device_number_t device_num)
{
    return TRUE; 
}


// Clear status flag 
void mpu6050_clear_status(device_number_t device_num)
{
    // 
}


// Get status flag 
uint8_t mpu6050_get_status(device_number_t device_num)
{
    return TRUE; 
}


// INT pin status 
MPU6050_INT_STATUS mpu6050_int_status(device_number_t device_num)
{
    return TRUE; 
}


// Accelerometer raw values 
void mpu6050_get_accel_raw(
    device_number_t device_num, 
    int16_t *accel_x_raw, 
    int16_t *accel_y_raw, 
    int16_t *accel_z_raw)
{
    // 
}


// Accelerometer calculation 
void mpu6050_get_accel(
    device_number_t device_num, 
    float *accel_x, 
    float *accel_y, 
    float *accel_z)
{
    // 
}


// Gyroscope raw values 
void mpu6050_get_gyro_raw(
    device_number_t device_num, 
    int16_t *gyro_x_raw, 
    int16_t *gyro_y_raw, 
    int16_t *gyro_z_raw)
{
    // 
}


// Gyroscopic value calculation 
void mpu6050_get_gyro(
    device_number_t device_num, 
    float *gyro_x, 
    float *gyro_y, 
    float *gyro_z)
{
    // 
}


// Temperature sensor raw value 
int16_t mpu6050_get_temp_raw(device_number_t device_num)
{
    return NONE; 
}


// Temperature sensor calculation 
float mpu6050_get_temp(device_number_t device_num)
{
    return NONE; 
}

//=======================================================================================


//=======================================================================================
// Mock functions 
//=======================================================================================
