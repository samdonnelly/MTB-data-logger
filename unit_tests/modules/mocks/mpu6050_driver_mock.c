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
// Global data 

typedef struct mpu6050_mock_data_s 
{
    int16_t ax_raw; 
    int16_t ay_raw; 
    int16_t az_raw; 
}
mpu6050_mock_data_t; 

mpu6050_mock_data_t mpu6050_mock_data; 

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
    if (i2c == NULL)
    {
        return; 
    }
}


// MPU6050 INT pin initialization 
void mpu6050_int_pin_init(
    device_number_t device_num, 
    GPIO_TypeDef *gpio, 
    pin_selector_t pin)
{
    if (gpio == NULL)
    {
        return; 
    }
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
void mpu6050_get_accel_axis(
    device_number_t device_num, 
    int16_t *accel_x_raw, 
    int16_t *accel_y_raw, 
    int16_t *accel_z_raw)
{
    if ((accel_x_raw == NULL) || (accel_y_raw == NULL) || (accel_z_raw == NULL))
    {
        return; 
    }

    *accel_x_raw = mpu6050_mock_data.ax_raw; 
    *accel_y_raw = mpu6050_mock_data.ay_raw; 
    *accel_z_raw = mpu6050_mock_data.az_raw; 
}


// Accelerometer calculation 
void mpu6050_get_accel_axis_gs(
    device_number_t device_num, 
    float *accel_axis_gs)
{
    if (accel_axis_gs == NULL)
    {
        return; 
    }
}


// Gyroscope raw values 
void mpu6050_get_gyro_axis(
    device_number_t device_num, 
    int16_t *gyro_axis)
{
    if (gyro_axis == NULL)
    {
        return; 
    }
}


// Gyroscopic value calculation 
void mpu6050_get_gyro_axis_rate(
    device_number_t device_num, 
    float *gyro_axis_rate)
{
    if (gyro_axis_rate == NULL)
    {
        return; 
    }
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

// MPU-6050 driver mock init 
void mpu6050_mock_init(void)
{
    mpu6050_mock_data.ax_raw = CLEAR; 
    mpu6050_mock_data.ax_raw = CLEAR; 
    mpu6050_mock_data.ax_raw = CLEAR; 
}


// Set axis acceleration 
void mpu6050_mock_set_accel(
    int16_t ax, 
    int16_t ay, 
    int16_t az)
{
    mpu6050_mock_data.ax_raw = ax; 
    mpu6050_mock_data.ay_raw = ay; 
    mpu6050_mock_data.az_raw = az; 
}

//=======================================================================================
