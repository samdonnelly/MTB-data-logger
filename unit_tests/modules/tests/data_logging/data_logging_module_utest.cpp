/**
 * @file data_logging_module_utest.cpp
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Data logging module unit tests 
 * 
 * @version 0.1
 * @date 2024-10-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Notes 
//=======================================================================================


//=======================================================================================
// Includes 

#include <iostream>

#include "CppUTest/TestHarness.h"

extern "C"
{
	// Add your C-only include files here 
    #include "data_logging.h" 
    #include "stm32f4xx_it.h" 
    #include "m8q_driver_mock.h" 
    #include "mpu6050_driver_mock.h" 
    #include "fatfs_controller_mock.h" 
}

//=======================================================================================


//=======================================================================================
// Macros 

#define LOG_TEST_NUM_INTERVALS 100 
#define LOG_TEST_NUM_REVS 4 

//=======================================================================================


//=======================================================================================
// Test group 

TEST_GROUP(data_logging_test)
{
    // Global test group variables 

    // Constructor 
    void setup()
    {
        log_init(EXTI4_IRQn, TIM1_TRG_COM_TIM11_IRQn, ADC1, DMA2, DMA2_Stream0); 

        // Mock init 
        m8q_mock_init(); 
        mpu6050_mock_init(); 
        fatfs_controller_mock_init(); 
    }

    // Destructor 
    void teardown()
    {
        // 
    }
}; 

//=======================================================================================


//=======================================================================================
// Helper functions 

// Wheel rev stream isolation 
void wheel_rev_iso(uint8_t& index, uint8_t rev_num)
{
    while (++index < LOG_SPEED_PERIOD)
    {
        for (uint8_t j = CLEAR; j < LOG_PERIOD_DIVIDER; j++)
        {
            log_data_adc_handler(); 
            log_data(); 
        }
    }

    index = CLEAR; 

    for (uint8_t i = CLEAR; i < rev_num; i++)
    {
        EXTI4_IRQHandler(); 
        log_data(); 
    }

    fatfs_controller_mock_init(); 

    for (uint8_t i = CLEAR; i < LOG_PERIOD_DIVIDER; i++)
    {
        log_data_adc_handler(); 
        log_data(); 
    }
}


// Wheel rev log read 
void wheel_rev_log_read(unsigned int& rev_count)
{
    char log_line[FATFS_MOCK_STR_SIZE]; 
    memset((void *)log_line, CLEAR, sizeof(log_line)); 
    unsigned int dummy1 = CLEAR, dummy2 = CLEAR, dummy3 = CLEAR; 

    for (uint8_t i = CLEAR; i < LOG_PERIOD_DIVIDER; i++)
    {
        fatfs_controller_mock_get_str(log_line, FATFS_MOCK_STR_SIZE); 
    }

    sscanf(log_line, "%u, %u, %u, %u", &dummy1, &dummy2, &dummy3, &rev_count); 
}

//=======================================================================================


//=======================================================================================
// Tests 

// Log Data: schedule 
TEST(data_logging_test, log_data_schedule)
{
    // This test makes sure no two streams get scheduled to run at the same time. This 
    // is done using the starting offsets and counter periods of each stream. 

    uint16_t 
    test_time = LOG_PERIOD * LOG_PERIOD_DIVIDER * LOG_TEST_NUM_INTERVALS, 
    overlap_count = CLEAR;   // Number of times more than one stream was called 

    uint8_t 
    period_count = CLEAR,             // Keeps track of number of streams called at once 
    gps_count = LOG_GPS_OFFSET,       // GPS log stream counter 
    accel_count = LOG_ACCEL_OFFSET,   // Accelerometer log stream counter 
    speed_count = LOG_SPEED_OFFSET;   // Wheel speed log stream counter 

    for (uint16_t i = CLEAR; i < test_time; i++)
    {
        if (++gps_count >= LOG_GPS_PERIOD)
        {
            gps_count = CLEAR; 
            period_count++; 
        }
        if (++accel_count >= LOG_ACCEL_PERIOD)
        {
            accel_count = CLEAR; 
            period_count++; 
        }
        if (++speed_count >= LOG_SPEED_PERIOD)
        {
            speed_count = CLEAR; 
            period_count++; 
        }

        if (period_count > 1)
        {
            overlap_count++; 
        }

        period_count = CLEAR; 
    }

    UNSIGNED_LONGS_EQUAL(TRUE, overlap_count == 0); 
}


// Log Data: data log header output 
TEST(data_logging_test, log_data_log_header_output)
{
    // Header data 
    uint16_t 
    fork_psi = 231, 
    fork_compression = 10, 
    fork_rebound = 8, 
    fork_travel = 160, 
    shock_psi = 229, 
    shock_lockout = 1, 
    shock_rebound = 4, 
    shock_travel = 150, 
    wheel_size = 29, 
    fork_rest = 305, 
    shock_rest = 298, 
    file_index = 0; 
    int16_t 
    ax_rest = 500, 
    ay_rest = -450, 
    az_rest = -60; 
    char 
    utc_time[] = "073731.00", 
    utc_date[] = "091202"; 

    // Data buffer 
    char header_line[FATFS_MOCK_STR_SIZE]; 
    char line_buff[FATFS_MOCK_STR_SIZE]; 
    memset((void *)header_line, CLEAR, sizeof(header_line)); 
    memset((void *)line_buff, CLEAR, sizeof(line_buff)); 

    // Set mock data 
    m8q_mock_set_time_utc_time(utc_time, sizeof(utc_time)); 
    m8q_mock_set_time_utc_date(utc_date, sizeof(utc_date)); 

    // Set system data 
    param_update_bike_setting(PARAM_BIKE_SET_FPSI, fork_psi); 
    param_update_bike_setting(PARAM_BIKE_SET_FC, fork_compression); 
    param_update_bike_setting(PARAM_BIKE_SET_FR, fork_rebound); 
    param_update_bike_setting(PARAM_BIKE_SET_FT, fork_travel); 
    param_update_bike_setting(PARAM_BIKE_SET_SPSI, shock_psi); 
    param_update_bike_setting(PARAM_BIKE_SET_SL, shock_lockout); 
    param_update_bike_setting(PARAM_BIKE_SET_SR, shock_rebound); 
    param_update_bike_setting(PARAM_BIKE_SET_ST, shock_travel); 
    param_update_bike_setting(PARAM_BIKE_SET_WS, wheel_size); 
    param_update_system_setting(PARAM_SYS_SET_AX_REST, &ax_rest); 
    param_update_system_setting(PARAM_SYS_SET_AY_REST, &ay_rest); 
    param_update_system_setting(PARAM_SYS_SET_AZ_REST, &az_rest); 
    param_update_system_setting(PARAM_SYS_SET_FORK_REST, &fork_rest); 
    param_update_system_setting(PARAM_SYS_SET_SHOCK_REST, &shock_rest); 

    // Generate the log file header 
    log_data_file_prep(); 

    // Check each line of the header 
    fatfs_controller_mock_get_str(header_line, FATFS_MOCK_STR_SIZE); 
    snprintf(line_buff, FATFS_MOCK_STR_SIZE, mtbdl_param_fork_info, 
             fork_psi, fork_compression, fork_rebound); 
    STRCMP_EQUAL(line_buff, header_line); 

    fatfs_controller_mock_get_str(header_line, FATFS_MOCK_STR_SIZE); 
    snprintf(line_buff, FATFS_MOCK_STR_SIZE, mtbdl_param_shock_info, 
             shock_psi, shock_lockout, shock_rebound); 
    STRCMP_EQUAL(line_buff, header_line); 

    fatfs_controller_mock_get_str(header_line, FATFS_MOCK_STR_SIZE); 
    snprintf(line_buff, FATFS_MOCK_STR_SIZE, mtbdl_param_bike_info, 
             fork_travel, shock_travel, wheel_size); 
    STRCMP_EQUAL(line_buff, header_line); 

    fatfs_controller_mock_get_str(header_line, FATFS_MOCK_STR_SIZE); 
    snprintf(line_buff, FATFS_MOCK_STR_SIZE, mtbdl_param_index, file_index); 
    STRCMP_EQUAL(line_buff, header_line); 

    fatfs_controller_mock_get_str(header_line, FATFS_MOCK_STR_SIZE); 
    snprintf(line_buff, FATFS_MOCK_STR_SIZE, mtbdl_param_accel_rest, 
             ax_rest, ay_rest, az_rest); 
    STRCMP_EQUAL(line_buff, header_line); 

    fatfs_controller_mock_get_str(header_line, FATFS_MOCK_STR_SIZE); 
    snprintf(line_buff, FATFS_MOCK_STR_SIZE, mtbdl_param_pot_rest, 
             fork_rest, shock_rest); 
    STRCMP_EQUAL(line_buff, header_line); 

    fatfs_controller_mock_get_str(header_line, FATFS_MOCK_STR_SIZE); 
    snprintf(line_buff, FATFS_MOCK_STR_SIZE, mtbdl_param_time, 
             utc_time, utc_date); 
    STRCMP_EQUAL(line_buff, header_line); 

    fatfs_controller_mock_get_str(header_line, FATFS_MOCK_STR_SIZE); 
    snprintf(line_buff, FATFS_MOCK_STR_SIZE, mtbdl_param_data, 
             LOG_PERIOD, LOG_PERIOD * LOG_PERIOD_DIVIDER * LOG_SPEED_PERIOD, LOG_REV_SAMPLE_SIZE); 
    STRCMP_EQUAL(line_buff, header_line); 

    fatfs_controller_mock_get_str(header_line, FATFS_MOCK_STR_SIZE); 
    STRCMP_EQUAL(mtbdl_data_log_start, header_line); 
}


// Log Data: data log output 
TEST(data_logging_test, log_data_log_output)
{
    // This test counts the number of times certain log messages are seen while logging 
    // data and compares the count to the expected sum. Expected sums are determined 
    // using the number of logging intervals divided by each logging streams period. The 
    // standard or default stream sum is determined by taking the total intervals and 
    // subtracting the sum of other streams as this stream runs when no other streams 
    // are scheduled. A stream is counted when the read log line matches one of the 
    // pre-defined log lines. 

    //==================================================
    // Create sample data 

    uint16_t 
    trail_mark = CLEAR, 
    fork_adc = CLEAR, 
    shock_adc = CLEAR, 
    wheel_speed = CLEAR; 

    int16_t 
    ax = 500, 
    ay = -450, 
    az = -60; 

    char 
    sog[] = "0.007", 
    lat[] = "4717.11321", 
    ns[] = "N", 
    lon[] = "00833.91518", 
    ew[] = "E"; 

    char 
    log_line[FATFS_MOCK_STR_SIZE], 
    log_default[FATFS_MOCK_STR_SIZE], 
    log_gps[FATFS_MOCK_STR_SIZE], 
    log_accel[FATFS_MOCK_STR_SIZE], 
    log_rev[FATFS_MOCK_STR_SIZE]; 

    memset((void *)log_line, CLEAR, sizeof(log_line)); 
    snprintf(log_default, FATFS_MOCK_STR_SIZE, mtbdl_data_log_default, 
             trail_mark, fork_adc, shock_adc); 
    snprintf(log_gps, FATFS_MOCK_STR_SIZE, mtbdl_data_log_gps, 
             "", "", "", "", trail_mark, fork_adc, shock_adc, sog, lat, ns[0], lon, ew[0]); 
    snprintf(log_accel, FATFS_MOCK_STR_SIZE, mtbdl_data_log_accel, 
             "", "", "", "", trail_mark, fork_adc, shock_adc, ax, ay, az); 
    snprintf(log_rev, FATFS_MOCK_STR_SIZE, mtbdl_data_log_speed, 
             "", "", "", "", trail_mark, fork_adc, shock_adc, wheel_speed); 
    
    //==================================================

    //==================================================
    // Set mock data 

    // M8Q GPS 
    m8q_mock_set_position_lat(lat, sizeof(lat)); 
    m8q_mock_set_position_ns(ns, sizeof(ns)); 
    m8q_mock_set_position_lon(lon, sizeof(lon)); 
    m8q_mock_set_position_ew(ew, sizeof(ew)); 
    m8q_mock_set_position_sog(sog, sizeof(sog)); 

    // MPU-6050 Accelerometer 
    mpu6050_mock_set_accel(ax, ay, az); 

    //==================================================

    //==================================================
    // Test log output 

    uint16_t 
    gps_count = CLEAR, 
    accel_count = CLEAR, 
    rev_count = CLEAR, 
    standard_count = CLEAR, 
    gps_expected = LOG_TEST_NUM_INTERVALS / LOG_GPS_PERIOD, 
    accel_expected = LOG_TEST_NUM_INTERVALS / LOG_ACCEL_PERIOD, 
    rev_expected = LOG_TEST_NUM_INTERVALS / LOG_SPEED_PERIOD, 
    standard_expected = LOG_PERIOD_DIVIDER * LOG_TEST_NUM_INTERVALS - 
                        (gps_expected + accel_expected + rev_expected); 

    log_data_prep(); 

    for (uint8_t i = CLEAR; i < LOG_TEST_NUM_INTERVALS; i++)
    {
        for (uint8_t j = CLEAR; j < LOG_PERIOD_DIVIDER; j++)
        {
            log_data_adc_handler(); 
            log_data(); 
        }

        for (uint8_t j = CLEAR; j < LOG_PERIOD_DIVIDER; j++)
        {
            fatfs_controller_mock_get_str(log_line, FATFS_MOCK_STR_SIZE); 

            if (!strcmp(log_line, log_default))
            {
                standard_count++; 
            }
            else if (!strcmp(log_line, log_gps))
            {
                gps_count++; 
            }
            else if (!strcmp(log_line, log_accel))
            {
                accel_count++; 
            }
            else if (!strcmp(log_line, log_rev))
            {
                rev_count++; 
            }
        }

        fatfs_controller_mock_init(); 
    }

    UNSIGNED_LONGS_EQUAL(standard_expected, standard_count); 
    UNSIGNED_LONGS_EQUAL(gps_expected, gps_count); 
    UNSIGNED_LONGS_EQUAL(accel_expected, accel_count); 
    UNSIGNED_LONGS_EQUAL(rev_expected, rev_count); 
    
    //==================================================
}


// Log Data: wheel revolution calculation 
TEST(data_logging_test, log_data_wheel_revs)
{
    // The wheel revolution log stream records the number of detected wheel revolutions 
    // since the last time the stream was called. The number of times gets recorded in a 
    // circular buffer by taking the place of the oldest data. All interval numbers get 
    // summed so the total rev count over X stream calls (known time delta) is know and 
    // that sum gets logged. This test fills the circular buffer with each spot having 
    // the same value so the expected sum is easy to determine. The stream is then run 
    // once more but with a different number of detected revolutions and it's checked 
    // that the oldest piece of data is replaced by the new piece of data. 

    uint8_t 
    index = LOG_SPEED_OFFSET, 
    rev_num = LOG_TEST_NUM_REVS, 
    rev_sum = rev_num * LOG_REV_SAMPLE_SIZE; 
    unsigned int rev_count = CLEAR; 

    log_data_prep(); 

    // Run the wheel rev stream until it's buffer fills up and check that the rev count 
    // is as expected by reading the data logged. 
    for (uint8_t i = CLEAR; i < LOG_REV_SAMPLE_SIZE; i++)
    {
        wheel_rev_iso(index, rev_num); 
    }

    wheel_rev_log_read(rev_count); 
    UNSIGNED_LONGS_EQUAL(rev_sum, rev_count); 

    // Change the number of revs that occur between log samples. To show that we've 
    // reached the rev sum buffer limit and the new value will replace an old value, 
    // the expected sum gets updated. Run the log sequence until the next wheel rev 
    // stream is reached and re-check the results. 
    rev_sum -= rev_num; 
    rev_num *= rev_num; 
    rev_sum += rev_num; 

    wheel_rev_iso(index, rev_num); 
    wheel_rev_log_read(rev_count); 
    UNSIGNED_LONGS_EQUAL(rev_sum, rev_count); 
}


// Calibration: calibration calculation 
TEST(data_logging_test, calibration_calculation)
{
    // System calibration performs data collection very similar to data logging. In this 
    // test a calibration sequence is simulated by calling the calibration function X 
    // number of times. The accelerometer data is updated at specific points in the 
    // calibration sequence so that the data averaging of the whole calibration process 
    // can be tested. Once the calibration sequence is over, the average accelerometer 
    // values recorded from calibration are found through the calibration calculation 
    // function. This average value is what is checked. 

    char sys_param_line[FATFS_MOCK_STR_SIZE]; 

    int16_t 
    ax[BYTE_2] = { 500, 1000 }, 
    ay[BYTE_2] = { -450, 100 }, 
    az[BYTE_2] = { -60, -540 }; 

    int 
    ax_calc = CLEAR, 
    ay_calc = CLEAR, 
    az_calc = CLEAR; 

    uint8_t 
    toggle = CLEAR_BIT, 
    index = LOG_ACCEL_OFFSET, 
    intervals = LOG_TEST_NUM_INTERVALS / LOG_ACCEL_PERIOD; 

    log_calibration_prep(); 

    // Simulate a system calibration 
    for (uint8_t i = CLEAR; i < intervals; i++)
    {
        mpu6050_mock_set_accel(ax[toggle], ay[toggle], az[toggle]); 
        toggle = SET_BIT - toggle; 

        while (++index <= LOG_ACCEL_PERIOD)
        {
            for (uint8_t j = CLEAR; j < LOG_PERIOD_DIVIDER; j++)
            {
                log_data_adc_handler(); 
                log_calibration(); 
            }
        }

        index = CLEAR; 
    }

    // Once calibration is done then calculate the average values of the samples. 
    log_calibration_calculation(); 

    // The data must be read in the order that it was written to the SD card. 
    // Logging params 
    fatfs_controller_mock_get_str(sys_param_line, FATFS_MOCK_STR_SIZE); 
    // Accelerometer calibration 
    fatfs_controller_mock_get_str(sys_param_line, FATFS_MOCK_STR_SIZE); 
    sscanf(sys_param_line, mtbdl_param_accel_rest, &ax_calc, &ay_calc, &az_calc); 
    // Voltage/potentiometer calibration 
    fatfs_controller_mock_get_str(sys_param_line, FATFS_MOCK_STR_SIZE); 

    LONGS_EQUAL((ax[BYTE_0] + ax[BYTE_1]) / BYTE_2, ax_calc); 
    LONGS_EQUAL((ay[BYTE_0] + ay[BYTE_1]) / BYTE_2, ay_calc); 
    LONGS_EQUAL((az[BYTE_0] + az[BYTE_1]) / BYTE_2, az_calc); 
}

//=======================================================================================
