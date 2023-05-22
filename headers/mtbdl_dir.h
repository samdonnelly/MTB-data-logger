/**
 * @file mtbdl_dir.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL filesystem directories header 
 * 
 * @version 0.1
 * @date 2023-05-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _MTBDL_DIRS_H_ 
#define _MTBDL_DIRS_H_ 

//=======================================================================================
// Includes 

#include "includes_app.h"
#include "includes_drivers.h"

//=======================================================================================


//=======================================================================================
// Directory declarations 

// Project root directory on SD card 
extern const char mtbdl_dir[]; 

// Parameters sub-directory 
extern const char mtbdl_param_dir[]; 

// Data logging sub-directory 
extern const char mtbdl_data_dir[]; 

//=======================================================================================

#endif   // _MTBDL_DIRS_H_ 