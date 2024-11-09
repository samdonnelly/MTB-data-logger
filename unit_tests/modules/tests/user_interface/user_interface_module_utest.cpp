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

    // ~50% SOC --> This only works under the assumption the calculation is linear 
    soc = battery_soc_calc((adc_volt_max + adc_volt_min) / 2); 
    UNSIGNED_LONGS_EQUAL(TRUE, (50 - soc) <= 1); 
}


// RX mode 
TEST(user_interface_test, ui_rx_mode)
{
    const char 
    param_msg_1[] = "",   // Valid parameter message 
    param_msg_2[] = "",   // Valid parameter message 
    param_msg_3[] = "",   // Invalid parameter message 
    param_msg_4[] = "";   // Invalid parameter message 

    ui_rx_prep(); 

    hc05_mock_set_read_data(param_msg_1, sizeof(param_msg_1)); 
    ui_rx(); 

    hc05_mock_set_read_data(param_msg_2, sizeof(param_msg_2)); 
    ui_rx(); 

    hc05_mock_set_read_data(param_msg_3, sizeof(param_msg_3)); 
    ui_rx(); 

    hc05_mock_set_read_data(param_msg_4, sizeof(param_msg_4)); 
    ui_rx(); 

    // Number of valid parameter messages 
    // UNSIGNED_LONGS_EQUAL(2, hc05_mock_get_send_count()); 
}


// TX mode 
TEST(user_interface_test, ui_tx_mode)
{
    ui_tx_prep(); 
    ui_tx(); 
    ui_tx_end(); 
}

//=======================================================================================
