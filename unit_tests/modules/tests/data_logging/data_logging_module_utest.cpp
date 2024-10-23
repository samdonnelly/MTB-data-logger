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

#include "CppUTest/TestHarness.h"

extern "C"
{
	// Add your C-only include files here 
}

//=======================================================================================


//=======================================================================================
// Macros 

// Possible mocks needed: 
// - HW125 driver 
// - M8Q driver 
// - ADC driver 
// - NVIC 

// Test list: 
// - log_data_name_prep 
//   - Log name creation (index within bounds) 
//   - Log index out of bounds 
// - log_data_file_prep 
//   - String formatting? 
// - log_data 
//   - Stream schedule & table works as expected 
//   - Standard stream works as expected 
//   - GPS stream works as expected 
//   - IMU stream works as expected 
//   - Wheel speed stream works as expected 
//   - Overrun bit does not set 
//   - Overrun bit force set 
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
// Test group 

TEST_GROUP(data_logging_test)
{
    // Global test group variables 

    // Constructor 
    void setup()
    {
        // 
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

// Test 0 
TEST(data_logging_test, test0)
{
    // 
}

//=======================================================================================
