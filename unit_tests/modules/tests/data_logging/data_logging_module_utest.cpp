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

// Test list: 
// - log_data 
//   - Stream schedule & table works as expected 
//   - Wheel speed stream works as expected 
//   - Interrupt callback execution 
// - log_data_adc_handler 
//   - ADC buffer populated but also capped 
// - log_data_end 
//   - File closed and index decremented if file is open 
// - log_calibration 
//   - Data read sequence is correct 
// - log_calibration_calculation 
//   - Calculation performed correctly 
// - log_get_batt_voltage 
//   - Update ADC only if data logging interrupts are disabled 

//=======================================================================================


//=======================================================================================
// Includes 

#include "CppUTest/TestHarness.h"

extern "C"
{
	// Add your C-only include files here 
    #include "data_logging.h" 
}

//=======================================================================================


//=======================================================================================
// Macros 

#define LOG_TEST_NUM_INTERVALS 100 

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


// Log Data: data log output  
TEST(data_logging_test, log_data_log_output)
{
    log_data_prep(); 
    log_data(); 
}

//=======================================================================================
