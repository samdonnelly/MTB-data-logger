/**
 * @file strs.h 
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL system strings header 
 * 
 * @version 0.1
 * @date 2023-05-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _STRS_H_ 
#define _STRS_H_ 

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
mtbdl_param_fork_info[],     // Fork setup parameters 
mtbdl_param_shock_info[],    // Shock setup parameters 
// System parameter information 
mtbdl_param_index[],         // Log file index 
mtbdl_param_accel_rest[],    // Resting accelerometer data 
mtbdl_param_pot_rest[],      // Resting potentiometer data 
mtbdl_param_time[],          // Time of file creation 
mtbdl_param_data[],          // Data logging info 
// Data log information 
mtbdl_data_log_start[],      // Signifies the start of the logging info 
mtbdl_data_log_end[],        // End of the log file 
mtbdl_data_log_1[],          // Data log message 1 - standard 
mtbdl_data_log_2[],          // Data log message 2 - includes wheel speed 
mtbdl_data_log_3[],          // Data log message 3 - includes accelerometer 
mtbdl_data_log_4[];          // Data log message 4 - includes GPS 

//=======================================================================================


//=======================================================================================
// User interface strings 

extern const char 
// RX info 
mtbdl_rx_prompt[],           // User prompt during RX state 
mtbdl_rx_input[];            // User input during the RX state 

//=======================================================================================

#endif   // _STRS_H_ 
