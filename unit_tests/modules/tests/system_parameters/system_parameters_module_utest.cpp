/**
 * @file system_parameters_module_utest.cpp
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief System parameters module unit tests 
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
    #include "system_parameters.h" 
    #include "fatfs_controller_mock.h" 
}

//=======================================================================================


//=======================================================================================
// Macros 
//=======================================================================================


//=======================================================================================
// Test group 

TEST_GROUP(system_parameters_test)
{
    // Global test group variables 

    // Constructor 
    void setup()
    {
        param_init(); 
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

// Write then get system parameters 
void write_get_sys_params(
    int16_t& accel_x_rest, 
    int16_t& accel_y_rest, 
    int16_t& accel_z_rest, 
    uint16_t& pot_fork_rest, 
    uint16_t& pot_shock_rest)
{
    char param_buff[FATFS_MOCK_STR_SIZE]; 

    // Temporary variables were added so that sscanf would work for the unit tests 
    // across all platforms. 
    int accel_x = CLEAR, accel_y = CLEAR, accel_z = CLEAR; 
    unsigned int pot_fork = CLEAR, pot_shock = CLEAR; 

    // We use the fatfs controller mock to first "write" the parameters to an SD card 
    // before fetching them. 
    param_sys_format_write(); 

    // The below sequence is copied from the param_sys_read_format function. 

    // Read logging parameters - we don't check the data from this read but we read the 
    // data so we can increment our read index in the fatfs controller mock. 
    fatfs_controller_mock_get_str(param_buff, MTBDL_MAX_STR_LEN); 

    // Read accelerometer calibration data 
    fatfs_controller_mock_get_str(param_buff, MTBDL_MAX_STR_LEN); 
    sscanf(param_buff, 
           mtbdl_param_accel_rest, 
           &accel_x, 
           &accel_y, 
           &accel_z); 

    // Read potentiometer starting points 
    fatfs_controller_mock_get_str(param_buff, MTBDL_MAX_STR_LEN); 
    sscanf(param_buff, 
           mtbdl_param_pot_rest, 
           &pot_fork, 
           &pot_shock); 

    accel_x_rest = accel_x; 
    accel_y_rest = accel_y; 
    accel_z_rest = accel_z; 
    pot_fork_rest = pot_fork; 
    pot_shock_rest = pot_shock; 
}

//=======================================================================================


//=======================================================================================
// Tests 

// Bike parameter get: invalid setting 
TEST(system_parameters_test, bike_param_get_invalid_setting)
{
    // Call the bike parameter getter with an invalid setting index. 
    // Check that the return aligns with no setting being returned. When an invalid 
    // setting is requested, the maximum value of a unsigned 16-bit number is returned 
    // as this is larger than any bike parameter can be set to. 
    UNSIGNED_LONGS_EQUAL(0xFFFF, param_get_bike_setting(PARAM_BIKE_SET_NONE)); 
}


// Bike parameter set: invalid update 
TEST(system_parameters_test, bike_param_set_invalid_update)
{
    uint8_t bike_param_status = SET_BIT; 
    uint16_t bike_param_setting = CLEAR; 

    // Check for no update with an invalid setting index 
    bike_param_status = param_update_bike_setting(PARAM_BIKE_SET_NONE, bike_param_setting); 
    UNSIGNED_LONGS_EQUAL(CLEAR_BIT, bike_param_status); 

    // Check for no update when the setting is outside of the acceptable range 

    bike_param_setting = PARAM_MAX_SUS_PSI + 1; 
    bike_param_status = param_update_bike_setting(PARAM_BIKE_SET_FPSI, bike_param_setting); 
    UNSIGNED_LONGS_EQUAL(CLEAR_BIT, bike_param_status); 

    bike_param_setting = PARAM_MAX_SUS_SETTING + 1; 
    bike_param_status = param_update_bike_setting(PARAM_BIKE_SET_FC, bike_param_setting); 
    UNSIGNED_LONGS_EQUAL(CLEAR_BIT, bike_param_status); 

    bike_param_setting = PARAM_MAX_SUS_SETTING + 1; 
    bike_param_status = param_update_bike_setting(PARAM_BIKE_SET_FR, bike_param_setting); 
    UNSIGNED_LONGS_EQUAL(CLEAR_BIT, bike_param_status); 

    bike_param_setting = PARAM_MAX_SUS_TRAVEL + 1; 
    bike_param_status = param_update_bike_setting(PARAM_BIKE_SET_FT, bike_param_setting); 
    UNSIGNED_LONGS_EQUAL(CLEAR_BIT, bike_param_status); 

    bike_param_setting = PARAM_MAX_SUS_PSI + 1; 
    bike_param_status = param_update_bike_setting(PARAM_BIKE_SET_SPSI, bike_param_setting); 
    UNSIGNED_LONGS_EQUAL(CLEAR_BIT, bike_param_status); 

    bike_param_setting = PARAM_MAX_SUS_SETTING + 1; 
    bike_param_status = param_update_bike_setting(PARAM_BIKE_SET_SL, bike_param_setting); 
    UNSIGNED_LONGS_EQUAL(CLEAR_BIT, bike_param_status); 

    bike_param_setting = PARAM_MAX_SUS_SETTING + 1; 
    bike_param_status = param_update_bike_setting(PARAM_BIKE_SET_SR, bike_param_setting); 
    UNSIGNED_LONGS_EQUAL(CLEAR_BIT, bike_param_status); 

    bike_param_setting = PARAM_MAX_SUS_TRAVEL + 1; 
    bike_param_status = param_update_bike_setting(PARAM_BIKE_SET_ST, bike_param_setting); 
    UNSIGNED_LONGS_EQUAL(CLEAR_BIT, bike_param_status); 

    bike_param_setting = PARAM_MAX_WHEEL_SIZE + 1; 
    bike_param_status = param_update_bike_setting(PARAM_BIKE_SET_WS, bike_param_setting); 
    UNSIGNED_LONGS_EQUAL(CLEAR_BIT, bike_param_status); 
}


// Bike parameter update: set and get parameters 
TEST(system_parameters_test, bike_param_update_set_get)
{
    // Values are references from their max value so they (should) always be valid. 
    // Different offsets are applied for the sake of making values different. 
    const uint16_t bike_params[PARAM_BIKE_SET_NONE] = 
    {
        PARAM_MAX_SUS_PSI - 1,       // Fork PSI 
        PARAM_MAX_SUS_SETTING - 1,   // Fork compression setting 
        PARAM_MAX_SUS_SETTING - 2,   // Fork rebound setting 
        PARAM_MAX_SUS_TRAVEL - 1,    // Fork travel distance 
        PARAM_MAX_SUS_PSI - 2,       // Shock PSI 
        PARAM_MAX_SUS_SETTING - 3,   // Shock lockout setting 
        PARAM_MAX_SUS_SETTING - 4,   // Shock rebound setting 
        PARAM_MAX_SUS_TRAVEL - 2,    // Shock travel distance 
        PARAM_MAX_WHEEL_SIZE - 1     // Wheel size/diameter 
    }; 

    // Get and check the initial settings 
    UNSIGNED_LONGS_EQUAL(CLEAR, param_get_bike_setting(PARAM_BIKE_SET_FPSI)); 
    UNSIGNED_LONGS_EQUAL(CLEAR, param_get_bike_setting(PARAM_BIKE_SET_FC)); 
    UNSIGNED_LONGS_EQUAL(CLEAR, param_get_bike_setting(PARAM_BIKE_SET_FR)); 
    UNSIGNED_LONGS_EQUAL(CLEAR, param_get_bike_setting(PARAM_BIKE_SET_FT)); 
    UNSIGNED_LONGS_EQUAL(CLEAR, param_get_bike_setting(PARAM_BIKE_SET_SPSI)); 
    UNSIGNED_LONGS_EQUAL(CLEAR, param_get_bike_setting(PARAM_BIKE_SET_SL)); 
    UNSIGNED_LONGS_EQUAL(CLEAR, param_get_bike_setting(PARAM_BIKE_SET_SR)); 
    UNSIGNED_LONGS_EQUAL(CLEAR, param_get_bike_setting(PARAM_BIKE_SET_ST)); 
    UNSIGNED_LONGS_EQUAL(CLEAR, param_get_bike_setting(PARAM_BIKE_SET_WS)); 

    // Set new settings and check for a confirmation from the setter 
    UNSIGNED_LONGS_EQUAL(
        SET_BIT, 
        param_update_bike_setting(PARAM_BIKE_SET_FPSI, bike_params[PARAM_BIKE_SET_FPSI])); 
    UNSIGNED_LONGS_EQUAL(
        SET_BIT, 
        param_update_bike_setting(PARAM_BIKE_SET_FC, bike_params[PARAM_BIKE_SET_FC])); 
    UNSIGNED_LONGS_EQUAL(
        SET_BIT, 
        param_update_bike_setting(PARAM_BIKE_SET_FR, bike_params[PARAM_BIKE_SET_FR])); 
    UNSIGNED_LONGS_EQUAL(
        SET_BIT, 
        param_update_bike_setting(PARAM_BIKE_SET_FT, bike_params[PARAM_BIKE_SET_FT])); 
    UNSIGNED_LONGS_EQUAL(
        SET_BIT, 
        param_update_bike_setting(PARAM_BIKE_SET_SPSI, bike_params[PARAM_BIKE_SET_SPSI])); 
    UNSIGNED_LONGS_EQUAL(
        SET_BIT, 
        param_update_bike_setting(PARAM_BIKE_SET_SL, bike_params[PARAM_BIKE_SET_SL])); 
    UNSIGNED_LONGS_EQUAL(
        SET_BIT, 
        param_update_bike_setting(PARAM_BIKE_SET_SR, bike_params[PARAM_BIKE_SET_SR])); 
    UNSIGNED_LONGS_EQUAL(
        SET_BIT, 
        param_update_bike_setting(PARAM_BIKE_SET_ST, bike_params[PARAM_BIKE_SET_ST])); 
    UNSIGNED_LONGS_EQUAL(
        SET_BIT, 
        param_update_bike_setting(PARAM_BIKE_SET_WS, bike_params[PARAM_BIKE_SET_WS])); 

    // Get new settings and check that they're updated 
    UNSIGNED_LONGS_EQUAL(
        bike_params[PARAM_BIKE_SET_FPSI], 
        param_get_bike_setting(PARAM_BIKE_SET_FPSI)); 
    UNSIGNED_LONGS_EQUAL(
        bike_params[PARAM_BIKE_SET_FC], 
        param_get_bike_setting(PARAM_BIKE_SET_FC)); 
    UNSIGNED_LONGS_EQUAL(
        bike_params[PARAM_BIKE_SET_FR], 
        param_get_bike_setting(PARAM_BIKE_SET_FR)); 
    UNSIGNED_LONGS_EQUAL(
        bike_params[PARAM_BIKE_SET_FT], 
        param_get_bike_setting(PARAM_BIKE_SET_FT)); 
    UNSIGNED_LONGS_EQUAL(
        bike_params[PARAM_BIKE_SET_SPSI], 
        param_get_bike_setting(PARAM_BIKE_SET_SPSI)); 
    UNSIGNED_LONGS_EQUAL(
        bike_params[PARAM_BIKE_SET_SL], 
        param_get_bike_setting(PARAM_BIKE_SET_SL)); 
    UNSIGNED_LONGS_EQUAL(
        bike_params[PARAM_BIKE_SET_SR], 
        param_get_bike_setting(PARAM_BIKE_SET_SR)); 
    UNSIGNED_LONGS_EQUAL(
        bike_params[PARAM_BIKE_SET_ST], 
        param_get_bike_setting(PARAM_BIKE_SET_ST)); 
    UNSIGNED_LONGS_EQUAL(
        bike_params[PARAM_BIKE_SET_WS], 
        param_get_bike_setting(PARAM_BIKE_SET_WS)); 
}


// System parameter set: invalid setting 
TEST(system_parameters_test, sys_param_set_invalid_setting)
{
    int16_t accel_x_rest = ~CLEAR, accel_y_rest = ~CLEAR, accel_z_rest = ~CLEAR; 
    const int16_t accel_x_rest_set = 3124, accel_y_rest_set = -5698, accel_z_rest_set = 11974; 

    uint16_t pot_fork_rest = ~CLEAR, pot_shock_rest = ~CLEAR; 
    const uint16_t pot_fork_rest_set = 345, pot_shock_rest_set = 876; 

    // Check initial value of system parameters. These values are initialized to zero 
    // in the module when we call the module init function (see constructor). 
    write_get_sys_params(
        accel_x_rest, 
        accel_y_rest, 
        accel_z_rest, 
        pot_fork_rest, 
        pot_shock_rest); 

    LONGS_EQUAL(CLEAR, accel_x_rest); 
    LONGS_EQUAL(CLEAR, accel_y_rest); 
    LONGS_EQUAL(CLEAR, accel_z_rest); 
    UNSIGNED_LONGS_EQUAL(CLEAR, pot_fork_rest); 
    UNSIGNED_LONGS_EQUAL(CLEAR, pot_shock_rest); 

    // Call setter with an invalid parameter index 
    param_update_system_setting(PARAM_SYS_SET_NUM, (void *)&accel_x_rest_set); 
    param_update_system_setting(PARAM_SYS_SET_NUM, (void *)&accel_y_rest_set); 
    param_update_system_setting(PARAM_SYS_SET_NUM, (void *)&accel_z_rest_set); 
    param_update_system_setting(PARAM_SYS_SET_NUM, (void *)&pot_fork_rest_set); 
    param_update_system_setting(PARAM_SYS_SET_NUM, (void *)&pot_shock_rest_set); 

    // Reset the actual values to show that they get written to zero again. 
    accel_x_rest = ~CLEAR, accel_y_rest = ~CLEAR, accel_z_rest = ~CLEAR; 
    pot_fork_rest = ~CLEAR, pot_shock_rest = ~CLEAR; 

    // Check that no parameter value has changed 
    write_get_sys_params(
        accel_x_rest, 
        accel_y_rest, 
        accel_z_rest, 
        pot_fork_rest, 
        pot_shock_rest); 

    LONGS_EQUAL(CLEAR, accel_x_rest); 
    LONGS_EQUAL(CLEAR, accel_y_rest); 
    LONGS_EQUAL(CLEAR, accel_z_rest); 
    UNSIGNED_LONGS_EQUAL(CLEAR, pot_fork_rest); 
    UNSIGNED_LONGS_EQUAL(CLEAR, pot_shock_rest); 
}


// System parameter set: set parameters 
TEST(system_parameters_test, sys_param_set)
{
    int16_t accel_x_rest = ~CLEAR, accel_y_rest = ~CLEAR, accel_z_rest = ~CLEAR; 
    const int16_t accel_x_rest_set = 3124, accel_y_rest_set = -5698, accel_z_rest_set = 11974; 

    uint16_t pot_fork_rest = ~CLEAR, pot_shock_rest = ~CLEAR; 
    const uint16_t pot_fork_rest_set = 345, pot_shock_rest_set = 876; 

    // The initial value of each system parameter is checked in the previous test so 
    // it is not checked again here. 

    // Call setter to update each parameter 
    param_update_system_setting(PARAM_SYS_SET_AX_REST, (void *)&accel_x_rest_set); 
    param_update_system_setting(PARAM_SYS_SET_AY_REST, (void *)&accel_y_rest_set); 
    param_update_system_setting(PARAM_SYS_SET_AZ_REST, (void *)&accel_z_rest_set); 
    param_update_system_setting(PARAM_SYS_SET_FORK_REST, (void *)&pot_fork_rest_set); 
    param_update_system_setting(PARAM_SYS_SET_SHOCK_REST, (void *)&pot_shock_rest_set); 

    // Check that the parameters are updated 
    write_get_sys_params(
        accel_x_rest, 
        accel_y_rest, 
        accel_z_rest, 
        pot_fork_rest, 
        pot_shock_rest); 

    LONGS_EQUAL(accel_x_rest_set, accel_x_rest); 
    LONGS_EQUAL(accel_y_rest_set, accel_y_rest); 
    LONGS_EQUAL(accel_z_rest_set, accel_z_rest); 
    UNSIGNED_LONGS_EQUAL(pot_fork_rest_set, pot_fork_rest); 
    UNSIGNED_LONGS_EQUAL(pot_shock_rest_set, pot_shock_rest); 
}

//=======================================================================================
