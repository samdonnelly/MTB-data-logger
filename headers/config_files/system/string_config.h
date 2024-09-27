/**
 * @file string_config.h 
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief String configuration interface 
 * 
 * @version 0.1
 * @date 2023-05-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _STRING_CONFIG_H_ 
#define _STRING_CONFIG_H_ 

//=======================================================================================
// Macros 

#define MTBDL_MAX_STR_LEN 65 

//=======================================================================================


//=======================================================================================
// Directories 

extern const char 
// Project root directory on SD card 
mtbdl_dir[], 
// Sub-directories 
mtbdl_param_dir[],           // Parameters sub-directory 
mtbdl_data_dir[],            // Data logging sub-directory 
// File names 
mtbdl_bike_param_file[],     // Bike parameters file 
mtbdl_sys_param_file[],      // System parameters file 
mtbdl_log_file[];            // Log file name template 

//=======================================================================================


//=======================================================================================
// File information 

extern const char 
// Bike parameter information 
mtbdl_param_fork_info[],     // Fork setings 
mtbdl_param_shock_info[],    // Shock setings 
mtbdl_param_bike_info[],     // Bike configuration 
// System parameter information 
mtbdl_param_index[],         // Log file index 
mtbdl_param_accel_rest[],    // Resting accelerometer data 
mtbdl_param_pot_rest[],      // Resting potentiometer data 
mtbdl_param_time[],          // Time of file creation 
mtbdl_param_data[],          // Data logging info 
// Data log information 
mtbdl_data_log_start[],      // Signifies the start of the logging info 
mtbdl_data_log_end[],        // End of the log file 
mtbdl_data_log_default[],    // Default data log message 
mtbdl_data_log_adc[],        // Default + ADC data log message 
mtbdl_data_log_gps[],        // Default + GPS data log message 
mtbdl_data_log_accel[],      // Default + Accelerometer data log message 
mtbdl_data_log_speed[];      // Default + Wheel speed data log message 

//=======================================================================================


//=======================================================================================
// User interface strings 

extern const char 
// RX info 
mtbdl_rx_prompt[],           // User prompt during RX state 
mtbdl_rx_input[],            // User input during the RX state 
mtbdl_rx_confirm[];          // Parameter update confirmation for the user 

//=======================================================================================

#endif   // _STRING_CONFIG_H_ 
