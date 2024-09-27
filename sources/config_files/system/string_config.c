/**
 * @file string_config.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief String configuration 
 * 
 * @version 0.1
 * @date 2023-05-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//=======================================================================================
// Includes 

#include "string_config.h" 

//=======================================================================================


//=======================================================================================
// Directory definitions 

const char 
// Project root directory on SD card 
mtbdl_dir[] = "mtbdl", 
// Sub-directories 
mtbdl_param_dir[] = "parameters", 
mtbdl_data_dir[] = "data", 
// File names 
mtbdl_bike_param_file[] = "bike_params.txt", 
mtbdl_sys_param_file[] = "sys_params.txt", 
mtbdl_log_file[] = "log_%u.txt"; 

//=======================================================================================


//=======================================================================================
// File information 

const char 
// Bike parameter information 
mtbdl_param_fork_info[] = "Fork: P: %upsi, Comp: %u, Reb: %u\r\n", 
mtbdl_param_shock_info[] = "Shock: P: %upsi, Lock: %u, Reb: %u\r\n", 
mtbdl_param_bike_info[] = "Bike: FT: %umm, ST: %umm, Wheel: %uin\r\n", 
// System parameter information 
mtbdl_param_index[] = "File Index: %u\r\n", 
mtbdl_param_accel_rest[] = "IMU Offset: X:%d Y:%d Z:%d\r\n", 
mtbdl_param_pot_rest[] = "Pot Offset: F:%u S:%u\r\n", 
mtbdl_param_time[] = "UTC: %s %s\r\n", 
mtbdl_param_data[] = "Data: T:%ums RPM_f:%uHz RPM_size:%u\r\n", 
// Data log information 
mtbdl_data_log_start[] = "Data log:\r\n", 
mtbdl_data_log_end[] = "Overrun: %u\r\nEnd\r\n\n", 
// Data order: <trail marker>, <fork pot>, <shock pot>, <wheel speed>, <accelerometer>, <GPS> 
mtbdl_data_log_default[] = "%u, %u, %u, -, -, -, -, -, -, -\r\n", 
mtbdl_data_log_adc[] = "%s%s%s%s%u, %u, %u, -, -, -, -, -, -, -\r\n", 
mtbdl_data_log_gps[] = "%s%s%s%s%u, %u, %u, -, -, -, -, %s, %s%c, %s%c\r\n", 
mtbdl_data_log_accel[] = "%s%s%s%s%u, %u, %u, -, %d, %d, %d, -, -, -\r\n", 
mtbdl_data_log_speed[] = "%s%s%s%s%u, %u, %u, %u, -, -, -, -, -, -\r\n"; 

//=======================================================================================


//=======================================================================================
// User interface strings 

const char 
// RX info 
mtbdl_rx_prompt[] = "\r\n>>> ", 
mtbdl_rx_input[] = "%u %u", 
mtbdl_rx_confirm[] = "\r\nconfirm\r\n"; 

//=======================================================================================
