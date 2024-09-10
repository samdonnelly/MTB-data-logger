/**
 * @file system_parameters.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief System parameters 
 * 
 * @version 0.1
 * @date 2024-07-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "system_parameters.h" 
#include "data_logging_config.h" 

//=======================================================================================


//=======================================================================================
// Variables 

static mtbdl_param_t mtbdl_param; 

//=======================================================================================


//=======================================================================================
// Prototypes 

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
void param_bike_read_formats(void); 


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
void param_sys_format_writes(void); 


/**
 * @brief Read and format the system parameters 
 * 
 * @details Reads the system settings from the SD card and saves the data into the data 
 *          record. These settings are stored in the system parameters file and this 
 *          function is only called during startup if the file already exists. 
 */
void param_sys_read_formats(void); 

//=======================================================================================


//=======================================================================================
// Initialization 

// System parameters init 
void param_init(void)
{
    // Bike Settings 
    mtbdl_param.fork_psi = CLEAR; 
    mtbdl_param.fork_comp = CLEAR; 
    mtbdl_param.fork_reb = CLEAR; 
    mtbdl_param.shock_psi = CLEAR; 
    mtbdl_param.shock_lock = CLEAR; 
    mtbdl_param.shock_reb = CLEAR; 

    // System Settings 
    mtbdl_param.accel_x_rest = CLEAR; 
    mtbdl_param.accel_y_rest = CLEAR; 
    mtbdl_param.accel_z_rest = CLEAR; 
    mtbdl_param.pot_fork_rest = CLEAR; 
    mtbdl_param.pot_shock_rest = CLEAR; 

    // SD Card 
    memset((void *)mtbdl_param.param_buff, CLEAR, sizeof(mtbdl_param.param_buff)); 
}


// File system setup 
void param_file_sys_setup(void)
{
    // Create "parameters" and "data" directories if they do not already exist 
    hw125_mkdir(mtbdl_data_dir); 
    hw125_mkdir(mtbdl_param_dir); 

    // Check for the existance of the bike parameters file 
    if (hw125_get_exists(mtbdl_bike_param_file) == FR_NO_FILE)
    {
        // No file - create one and write default parameter data to it 
        param_write_bike_params(HW125_MODE_WW); 
    }
    else 
    {
        // File already exists - open the file for reading 
        param_read_bike_params(HW125_MODE_OEWR); 
    }

    // Check for the existance of the system parameters file 
    if (hw125_get_exists(mtbdl_sys_param_file) == FR_NO_FILE)
    {
        // No file - create one and write default parameter data to it 
        param_write_sys_params(HW125_MODE_WW); 
    }
    else 
    {
        // File already exists - open the file for reading 
        param_read_sys_params(HW125_MODE_OEWR); 
    }
}

//=======================================================================================


//=======================================================================================
// Parameter read and write 

// Write bike parameters to file 
void param_write_bike_params(uint8_t mode)
{
    // Move to the parameters directory, open the bike parameters file for writing, 
    // format and write the bike parameters from the data record to the file, then close 
    // the file. 
    hw125_set_dir(mtbdl_param_dir); 
    hw125_open(mtbdl_bike_param_file, mode); 
    param_bike_format_write(); 
    hw125_close(); 
}


// Read bike parameter on file 
void param_read_bike_params(uint8_t mode)
{
    // Move to the parameters directory, open the bike parameters file for reading, 
    // read the parameters and store them in the data record, then close the file. 
    hw125_set_dir(mtbdl_param_dir); 
    hw125_open(mtbdl_bike_param_file, mode); 
    param_bike_read_formats(); 
    hw125_close(); 
}


// Write system parameters to file 
void param_write_sys_params(uint8_t mode)
{
    // Move to the parameters directory, open the system parameters file for writing, 
    // format and write the system parameters from the data record to the file, then 
    // close the file. 
    hw125_set_dir(mtbdl_param_dir); 
    hw125_open(mtbdl_sys_param_file, mode); 
    param_sys_format_writes(); 
    hw125_close(); 
}


// Read system parameters on file 
void param_read_sys_params(uint8_t mode)
{
    // Move to the parameters directory, open the system parameters file for reading, 
    // read the parameters and store them in the data record, then close the file. 
    hw125_set_dir(mtbdl_param_dir); 
    hw125_open(mtbdl_sys_param_file, mode); 
    param_sys_read_formats(); 
    hw125_close(); 
}


// Format and write the bike parameters 
void param_bike_format_write(void)
{
    // Write fork settings 
    snprintf(mtbdl_param.param_buff, 
             PARAM_MAX_STR_LEN, 
             mtbdl_param_fork_info, 
             mtbdl_param.fork_psi, 
             mtbdl_param.fork_comp, 
             mtbdl_param.fork_reb); 
    hw125_puts(mtbdl_param.param_buff); 

    // Write shock settings 
    snprintf(mtbdl_param.param_buff, 
             PARAM_MAX_STR_LEN, 
             mtbdl_param_shock_info, 
             mtbdl_param.shock_psi, 
             mtbdl_param.shock_lock, 
             mtbdl_param.shock_reb); 
    hw125_puts(mtbdl_param.param_buff); 
}


// Read and format the bike parameters 
void param_bike_read_formats(void)
{
    // Read fork settings 
    hw125_gets(mtbdl_param.param_buff, PARAM_MAX_STR_LEN); 
    sscanf(mtbdl_param.param_buff, 
           mtbdl_param_fork_info, 
           &mtbdl_param.fork_psi, 
           &mtbdl_param.fork_comp, 
           &mtbdl_param.fork_reb); 

    // Read shock settings 
    hw125_gets(mtbdl_param.param_buff, PARAM_MAX_STR_LEN); 
    sscanf(mtbdl_param.param_buff, 
           mtbdl_param_shock_info, 
           &mtbdl_param.shock_psi, 
           &mtbdl_param.shock_lock, 
           &mtbdl_param.shock_reb); 
}


// Format and write the system parameters 
void param_sys_format_writes(void)
{
    // Write logging parameters 
    snprintf(mtbdl_param.param_buff, 
             PARAM_MAX_STR_LEN, 
             mtbdl_param_index, 
             mtbdl_param.log_index); 
    hw125_puts(mtbdl_param.param_buff); 
    
    // Write accelerometer calibration data 
    snprintf(mtbdl_param.param_buff, 
             PARAM_MAX_STR_LEN, 
             mtbdl_param_accel_rest, 
             mtbdl_param.accel_x_rest, 
             mtbdl_param.accel_y_rest, 
             mtbdl_param.accel_z_rest); 
    hw125_puts(mtbdl_param.param_buff); 

    // Write potentiometer calibrated starting points 
    snprintf(mtbdl_param.param_buff, 
             PARAM_MAX_STR_LEN, 
             mtbdl_param_pot_rest, 
             mtbdl_param.pot_fork_rest, 
             mtbdl_param.pot_shock_rest); 
    hw125_puts(mtbdl_param.param_buff); 
}


// Read and format the system parameters 
void param_sys_read_formats(void)
{
    // Read logging parameters 
    hw125_gets(mtbdl_param.param_buff, PARAM_MAX_STR_LEN); 
    sscanf(mtbdl_param.param_buff, 
           mtbdl_param_index, 
           &mtbdl_param.log_index); 

    // Read accelerometer calibration data 
    hw125_gets(mtbdl_param.param_buff, PARAM_MAX_STR_LEN); 
    sscanf(mtbdl_param.param_buff, 
           mtbdl_param_accel_rest, 
           &mtbdl_param.accel_x_rest, 
           &mtbdl_param.accel_y_rest, 
           &mtbdl_param.accel_z_rest); 

    // Read potentiometer starting points 
    hw125_gets(mtbdl_param.param_buff, PARAM_MAX_STR_LEN); 
    sscanf(mtbdl_param.param_buff, 
           mtbdl_param_pot_rest, 
           &mtbdl_param.pot_fork_rest, 
           &mtbdl_param.pot_shock_rest); 
}

//=======================================================================================


//=======================================================================================
// Setters 

// Increment/decrement log file index 
void param_update_log_index(param_log_index_change_t log_index_change)
{
    switch (log_index_change)
    {
        case PARAM_LOG_INDEX_DEC: 
            mtbdl_param.log_index--; 
            break; 

        case PARAM_LOG_INDEX_INC: 
            mtbdl_param.log_index++; 
            break; 

        default: 
            break; 
    }

    param_write_sys_params(HW125_MODE_OAWR); 
}

//=======================================================================================


//=======================================================================================
// Getters 

// Get log file index 
uint8_t param_get_log_index(void)
{
    return mtbdl_param.log_index; 
}

//=======================================================================================
