/**
 * @file mtbdl_dir.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL filesystem directories 
 * 
 * @version 0.1
 * @date 2023-05-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//=======================================================================================
// Includes 

#include "directories.h" 

//=======================================================================================


//=======================================================================================
// Directory definitions 

// Project root directory on SD card 
const char mtbdl_dir[] = "mtbdl"; 

// Sub-directories 
const char mtbdl_param_dir[] = "parameters"; 
const char mtbdl_data_dir[] = "data"; 

// File names 
const char mtbdl_bike_param_file[] = "bike_params.txt"; 
const char mtbdl_sys_param_file[] = "sys_params.txt"; 
const char mtbdl_log_file[] = "log_%u.txt"; 

// Bike parameter information 
const char mtbdl_param_fork_info[] = "Fork: PSI:%u C:%u R:%u\n"; 
const char mtbdl_param_shock_info[] = "Shock: PSI:%u L:%u R:%u\n"; 

// System parameter information 
const char mtbdl_param_log[] = "Index: %u\n"; 
const char mtbdl_param_accel_rest[] = "Accel: X:%u Y:%u Z:%u\n"; 
const char mtbdl_param_pot_rest[] = "Pots: F:%u S:%u\n"; 

// Data log information 
const char mtbdl_data_log_start[] = "Data log:\n"; 
// Data order: 
// <time>, <trail marker>, <fork pot>, <shock pot>, <wheel speed>, <accelerometer>, <GPS> 
const char mtbdl_data_log_1[] = "%u, %u, %u, %u, -, -, -, -, -\n";     // Standard 
const char mtbdl_data_log_2[] = "%u, %u, %u, %u, %u, -, -, -, -\n";    // Includes wheel speed 
const char mtbdl_data_log_3[] = "%u, %u, %u, %u, -, %u, %u, %u, -\n";  // Includes accelerometer 
const char mtbdl_data_log_4[] = "%u, %u, %u, %u, -, -, -, -, %u\n";    // Includes GPS 

// RX info 
const char mtbdl_rx_prompt[] = "\r\n>>> "; 
const char mtbdl_rx_input[] = "%u %u"; 

//=======================================================================================
