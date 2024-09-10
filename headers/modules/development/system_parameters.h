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
// Enums 

// 
typedef enum {
    PARAM_LOG_INDEX_DEC, 
    PARAM_LOG_INDEX_INC 
} param_log_index_change_t; 

//=======================================================================================   


//=======================================================================================
// Structures 

// Parameters data record 
typedef struct mtbdl_param_s 
{
    // Bike settings 
    uint8_t fork_psi;                           // Fork pressure (psi) 
    uint8_t fork_comp;                          // Fork compression setting 
    uint8_t fork_reb;                           // Fork rebound setting 
    uint8_t shock_psi;                          // Shock pressure (psi) 
    uint8_t shock_lock;                         // Shock lockout setting 
    uint8_t shock_reb;                          // Shock rebound setting 

    // System settings 
    int16_t accel_x_rest;                       // Resting x-axis acceleration offset 
    int16_t accel_y_rest;                       // Resting y-axis acceleration offset 
    int16_t accel_z_rest;                       // Resting z-axis acceleration offset 
    uint16_t pot_fork_rest;                     // Resting potentiometer reading for fork 
    uint16_t pot_shock_rest;                    // Resting potentiometer reading for shock 

    // SD card 
    char param_buff[PARAM_MAX_STR_LEN];         // Buffer for reading and writing 
    uint8_t log_index;                          // Data log index 
}
mtbdl_param_t; 

//=======================================================================================


//=======================================================================================
// Initialization 

/**
 * @brief System parameters init 
 */
void param_init(void); 


/**
 * @brief File system setup 
 * 
 * @details Creates directories on the SD card for storing system and bike parameters 
 *          as well data logs if the directories do not already exist. After establishing 
 *          directories, checks for existance of system and bike parameter files. If they 
 *          exist then they will be read and stored into the data handling record. If not 
 *          then they will be created and intialized to default values. 
 *          
 *          This function should only be called after the SD card has been mounted. 
 */
void param_file_sys_setup(void); 

//=======================================================================================


//=======================================================================================
// Parameter read and write 

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


//=======================================================================================
// Setters 

/**
 * @brief Increment/decrement log file index 
 * 
 * @param log_index_change 
 */
void param_update_log_index(param_log_index_change_t log_index_change); 

//=======================================================================================


//=======================================================================================
// Getters 

/**
 * @brief Get log file index 
 * 
 * @return uint8_t 
 */
uint8_t param_get_log_index(void); 

//=======================================================================================

#endif   // _SYSTEM_PARAMETERS_H_ 
