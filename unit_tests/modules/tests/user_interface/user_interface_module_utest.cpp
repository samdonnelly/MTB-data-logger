/**
 * @file user_interface_module_utest.cpp
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief User interface module unit tests 
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
    #include "user_interface.h" 
    #include "stm32f4xx_it.h" 
    #include "battery_config.h" 
    #include "hc05_driver_mock.h" 
}

//=======================================================================================


//=======================================================================================
// Macros 

#define SOC_OFFSET 10 

//=======================================================================================


//=======================================================================================
// Test group 

TEST_GROUP(user_interface_test)
{
    // Global test group variables 

    // Constructor 
    void setup()
    {
        ui_init(GPIOC, PIN_0, PIN_1, PIN_2, PIN_3); 

        // Mocks 
        hc05_mock_init(); 
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

// UI update: battery SOC calculation 
TEST(user_interface_test, test0)
{
    // The battery SOC is assumed to follow a linear discharge curve. This is not the 
    // most accurate representation but more battery data is needed to create a better 
    // model. The current ADC reading for the battery is used between two min and max 
    // values to linearly interpolate and find a battery charge percentage. Voltage 
    // values supplied to the soc calculation function that are above or below the 
    // voltage limits will be adjusted to max and min SOC. 

    uint8_t soc = ~CLEAR; 

    // Capped at 0% SOC 
    soc = battery_soc_calc(adc_volt_min - SOC_OFFSET); 
    UNSIGNED_LONGS_EQUAL(0, soc); 

    // Capped at 100% SOC 
    soc = battery_soc_calc(adc_volt_max + SOC_OFFSET); 
    UNSIGNED_LONGS_EQUAL(100, soc); 

    // ~50% SOC --> This only works under the assumption the calculation is linear. 
    // If the max and min voltage readings add to an odd number then the SOC won't 
    // read exactly 50% so it's checked if it's within 1%. 
    soc = battery_soc_calc((adc_volt_max + adc_volt_min) / 2); 
    UNSIGNED_LONGS_EQUAL(TRUE, (50 - soc) <= 1); 
}


// RX mode 
TEST(user_interface_test, ui_rx_mode)
{
    const char 
    param_msg_1[] = "0 355",   // Valid: "PARAM_BIKE_SET_FPSI 355" 
    param_msg_2[] = "7 150",   // Valid: "PARAM_BIKE_SET_ST 150" 
    param_msg_3[] = "9 10",    // Invalid parameter: "PARAM_BIKE_SET_NONE 10"
    param_msg_4[] = "2 600";   // Invalid value: "PARAM_BIKE_SET_FR 600" 

    ui_rx_prep(); 

    hc05_mock_set_read_data(param_msg_1, sizeof(param_msg_1)); 
    ui_rx(); 

    hc05_mock_set_read_data(param_msg_2, sizeof(param_msg_2)); 
    ui_rx(); 

    hc05_mock_set_read_data(param_msg_3, sizeof(param_msg_3)); 
    ui_rx(); 

    hc05_mock_set_read_data(param_msg_4, sizeof(param_msg_4)); 
    ui_rx(); 

    // Number of valid parameter messages. The send counter will increment with each 
    // 'ui_rx' call because regardless of the message validity, a user prompt will always 
    // be sent after an input via 'ui_rx_prep'. An additional send occurs if a message is 
    // valid and a confirmation is sent to the user. There are 4 messages being tested, 
    // 2 are valid and 'ui_rx_prep' gets called once at the start so the total we look for 
    // is 7. 
    UNSIGNED_LONGS_EQUAL(7, hc05_mock_get_send_count()); 
}

//=======================================================================================
