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
// - M8Q driver 
// - ADC driver 

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

// Things that don't need testing: 
// - The GPS and acclerometer data logging streams. These streams just call device 
//   controllers and driver getters which is not within the scope of this module 
//   unit test to check the functionality of. 

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
