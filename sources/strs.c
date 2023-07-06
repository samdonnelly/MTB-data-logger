/**
 * @file strs.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL system strings 
 * 
 * @version 0.1
 * @date 2023-05-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//=======================================================================================
// Includes 

#include "strs.h" 

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

//=======================================================================================


//=======================================================================================
// File information 

// Bike parameter information 
const char mtbdl_param_fork_info[] = "Fork: PSI:%u C:%u R:%u\r\n"; 
const char mtbdl_param_shock_info[] = "Shock: PSI:%u L:%u R:%u\r\n"; 

// System parameter information 
const char mtbdl_param_index[] = "File Index: %u\r\n"; 
const char mtbdl_param_accel_rest[] = "IMU Offset: X:%d Y:%d Z:%d\r\n"; 
const char mtbdl_param_pot_rest[] = "Pot Offset: F:%u S:%u\r\n"; 
const char mtbdl_param_time[] = "UTC: %s %s\r\n"; 
const char mtbdl_param_data[] = "Data: T:%ums RPM_f:%uHz RPM_size:%u\r\n"; 

// Data log information 
const char mtbdl_data_log_start[] = "Data log:\r\n"; 
const char mtbdl_data_log_end[] = "End\r\n\n"; 
// Data order: <trail marker>, <fork pot>, <shock pot>, <wheel speed>, <accelerometer>, <GPS> 
const char mtbdl_data_log_1[] = "%u, %u, %u, -, -, -, -, -, -\r\n"; 
const char mtbdl_data_log_2[] = "%u, %u, %u, %u, -, -, -, -, -\r\n"; 
const char mtbdl_data_log_3[] = "%u, %u, %u, -, %d, %d, %d, -, -\r\n"; 
const char mtbdl_data_log_4[] = "%u, %u, %u, -, -, -, -, %s.%s%c, %s.%s%c\r\n"; 

//=======================================================================================


//=======================================================================================
// User interface strings 

// RX info 
const char mtbdl_rx_prompt[] = "\r\n>>> "; 
const char mtbdl_rx_input[] = "%u %u"; 

//=======================================================================================
