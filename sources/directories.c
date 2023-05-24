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
const char mtbdl_param_dir[] = "parameters";     // Parameters sub-directory 
const char mtbdl_data_dir[] = "data";            // Data logging sub-directory 

// File names 
const char mtbdl_param_file[] = "bike_params.txt"; 

// File information 
// TODO Try to use these in the screen messages 
const char mtbdl_param_fork_info[] = "Fork: PSI:%u C:%u R:%u\n"; 
const char mtbdl_param_shock_info[] = "Shock: PSI:%u L:%u R:%u\n"; 

//=======================================================================================
