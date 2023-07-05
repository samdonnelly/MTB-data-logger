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
// Includes 

#include "includes_app.h"
#include "includes_drivers.h"

//=======================================================================================


//=======================================================================================
// Directory declarations 

// Project root directory on SD card 
extern const char mtbdl_dir[]; 

// Sub-directories 
extern const char mtbdl_param_dir[];           // Parameters sub-directory 
extern const char mtbdl_data_dir[];            // Data logging sub-directory 

// File names 
extern const char mtbdl_bike_param_file[];     // Bike parameters file 
extern const char mtbdl_sys_param_file[];      // System parameters file 
extern const char mtbdl_log_file[];            // Log file name template 

//=======================================================================================


//=======================================================================================
// File information 

// Bike parameter information 
extern const char mtbdl_param_fork_info[];     // Fork setup parameters 
extern const char mtbdl_param_shock_info[];    // Shock setup parameters 

// System parameter information 
extern const char mtbdl_param_index[];         // Log file index 
extern const char mtbdl_param_accel_rest[];    // Resting accelerometer data 
extern const char mtbdl_param_pot_rest[];      // Resting potentiometer data 
extern const char mtbdl_param_time[];          // Time of file creation 
extern const char mtbdl_param_data[];          // Data logging info 

// Data log information 
extern const char mtbdl_data_log_start[];      // Signifies the start of the logging info 
extern const char mtbdl_data_log_end[];        // End of the log file 
extern const char mtbdl_data_log_1[];          // Data log message 1 - standard 
extern const char mtbdl_data_log_2[];          // Data log message 2 - includes wheel speed 
extern const char mtbdl_data_log_3[];          // Data log message 3 - includes accelerometer 
extern const char mtbdl_data_log_4[];          // Data log message 4 - includes GPS 

//=======================================================================================


//=======================================================================================
// User interface strings 

// RX info 
extern const char mtbdl_rx_prompt[];           // User prompt during RX state 
extern const char mtbdl_rx_input[];            // User input during the RX state 

//=======================================================================================

#endif   // _STRS_H_ 
