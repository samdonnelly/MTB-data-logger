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
#include "string_config.h" 

//=======================================================================================


//=======================================================================================
// Enums 

// Log index change type 
typedef enum {
    PARAM_LOG_INDEX_DEC, 
    PARAM_LOG_INDEX_INC 
} param_log_index_change_t; 


// Bike setting index 
typedef enum {
    PARAM_BIKE_SET_FPSI,   // Fork PSI 
    PARAM_BIKE_SET_FC,     // Fork compression setting 
    PARAM_BIKE_SET_FR,     // Fork rebound setting 
    PARAM_BIKE_SET_SPSI,   // Shock SPI 
    PARAM_BIKE_SET_SL,     // Shock lockout setting 
    PARAM_BIKE_SET_SR,     // Shock rebound setting 
    PARAM_BIKE_SET_NONE    // No setting 
} param_bike_set_index_t; 


// System setting index 
typedef enum {
    PARAM_SYS_SET_AX_REST,      // Resting X-axis acceleration 
    PARAM_SYS_SET_AY_REST,      // Resting Y-axis acceleration 
    PARAM_SYS_SET_AZ_REST,      // Resting Z-axis acceleration 
    PARAM_SYS_SET_FORK_REST,    // Resting fork position 
    PARAM_SYS_SET_SHOCK_REST,   // Resting shock position 
    PARAM_SYS_SET_NUM           // Number of system settings 
} param_sys_set_index_t; 

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
    char param_buff[MTBDL_MAX_STR_LEN];         // Buffer for reading and writing 
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


/**
 * @brief Format and write the bike parameters 
 * 
 * @details Formats bike parameters (such as fork and shock settings) from the data 
 *          record into strings and writes the strings to the SD card. This function is 
 *          used when writing settings to both the bike parameters file and to newly 
 *          created log files. The bike parameters file is written upon creation and 
 *          when saving new settings. 
 */
void param_bike_format_write(void); 


/**
 * @brief Read and format the bike parameters 
 * 
 * @details Reads the bike settings/configuration from the SD card and saves the data 
 *          into the data record. These settings are stored in the bike parameters file 
 *          and this function is only called during startup if the file already exists. 
 */
void param_bike_read_format(void); 


/**
 * @brief Format and write the system parameters 
 * 
 * @details Formats system parameters (such as IMU and potentiometer calibration data) 
 *          from the data record into strings and writes the strings to the SD card. 
 *          This function is used when writing settings to both the system parameters 
 *          file and to newly created log files. The system parameters file is written 
 *          upon creation, when saving new settings and keeping track of the log file 
 *          number/index. 
 */
void param_sys_format_write(void); 


/**
 * @brief Read and format the system parameters 
 * 
 * @details Reads the system settings from the SD card and saves the data into the data 
 *          record. These settings are stored in the system parameters file and this 
 *          function is only called during startup if the file already exists. 
 */
void param_sys_read_format(void); 

//=======================================================================================


//=======================================================================================
// Setters 

/**
 * @brief Increment/decrement log file index 
 * 
 * @param log_index_change 
 */
void param_update_log_index(param_log_index_change_t log_index_change); 


/**
 * @brief Update bike setting 
 * 
 * @param setting_index 
 * @param setting 
 */
void param_update_bike_setting(
    param_bike_set_index_t setting_index, 
    uint8_t setting); 


/**
 * @brief Update system settings 
 * 
 * @param setting_index 
 * @param setting 
 */
void param_update_system_setting(
    param_sys_set_index_t setting_index, 
    void *setting); 

//=======================================================================================


//=======================================================================================
// Getters 

/**
 * @brief Get log file index 
 * 
 * @return uint8_t 
 */
uint8_t param_get_log_index(void); 


/**
 * @brief Get bike settings 
 * 
 * @param setting_index 
 * @return uint8_t 
 */
uint8_t param_get_bike_setting(param_bike_set_index_t setting_index); 

//=======================================================================================

#endif   // _SYSTEM_PARAMETERS_H_ 
