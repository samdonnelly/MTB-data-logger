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
// Macros 

#define PARAM_MAX_STR_LEN 60 

//=======================================================================================


//=======================================================================================
// Structures 

// Parameters data record 
typedef struct mtbdl_param_s 
{
    // Bike parameters 
    uint8_t fork_psi;                           // Fork pressure (psi) 
    uint8_t fork_comp;                          // Fork compression setting 
    uint8_t fork_reb;                           // Fork rebound setting 
    uint8_t shock_psi;                          // Shock pressure (psi) 
    uint8_t shock_lock;                         // Shock lockout setting 
    uint8_t shock_reb;                          // Shock rebound setting 

    // System parameters 
    uint8_t log_index;                          // Data log index 
    int16_t accel_x_rest;                       // Resting x-axis acceleration offset 
    int16_t accel_y_rest;                       // Resting y-axis acceleration offset 
    int16_t accel_z_rest;                       // Resting z-axis acceleration offset 
    uint16_t pot_fork_rest;                     // Resting potentiometer reading for fork 
    uint16_t pot_shock_rest;                    // Resting potentiometer reading for shock 

    // SD card 
    char param_buff[PARAM_MAX_STR_LEN];         // Buffer for reading and writing 
}
mtbdl_param_t; 

//=======================================================================================


//=======================================================================================
// Dev 

/**
 * @brief Write bike parameters to file 
 * 
 * @param mode 
 */
void param_write_bike_params(uint8_t mode); 


/**
 * @brief Read bike parameter on file 
 * 
 * @param mode 
 */
void param_read_bike_params(uint8_t mode); 


/**
 * @brief Write system parameters to file 
 * 
 * @param mode 
 */
void param_write_sys_params(uint8_t mode); 


/**
 * @brief Read system parameters on file 
 * 
 * @param mode 
 */
void param_read_sys_params(uint8_t mode); 

//=======================================================================================

#endif   // _SYSTEM_PARAMETERS_H_ 
