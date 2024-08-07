/**
 * @file system_parameters.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief System parameters interface 
 * 
 * @version 0.1
 * @date 2024-07-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _SYSTEM_PARAMETERS_H_ 
#define _SYSTEM_PARAMETERS_H_ 

//=======================================================================================
// Includes 

#include "includes_drivers.h" 

//=======================================================================================


//=======================================================================================
// Structures 

// Parameters data record 
typedef struct mtbdl_params_s 
{
    uint8_t test; 
}
mtbdl_params_t; 

//=======================================================================================

#endif   // _SYSTEM_PARAMETERS_H_ 
