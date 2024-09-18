/**
 * @file system_parameters.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief System parameters module 
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
#include "stm32f4xx_it.h" 

//=======================================================================================


//=======================================================================================
// Macros 

#define PARAM_MAX_SUS_SETTING 20         // Max compression and rebound setting 

//=======================================================================================


//=======================================================================================
// Variables 

static mtbdl_param_t mtbdl_param; 

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
    mtbdl_param.log_index = CLEAR; 
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
    param_bike_read_format(); 
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
    param_sys_format_write(); 
    hw125_close(); 
}


// Read system parameters on file 
void param_read_sys_params(uint8_t mode)
{
    // Move to the parameters directory, open the system parameters file for reading, 
    // read the parameters and store them in the data record, then close the file. 
    hw125_set_dir(mtbdl_param_dir); 
    hw125_open(mtbdl_sys_param_file, mode); 
    param_sys_read_format(); 
    hw125_close(); 
}


// Format and write the bike parameters 
void param_bike_format_write(void)
{
    // Write fork settings 
    snprintf(mtbdl_param.param_buff, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_param_fork_info, 
             mtbdl_param.fork_psi, 
             mtbdl_param.fork_comp, 
             mtbdl_param.fork_reb); 
    hw125_puts(mtbdl_param.param_buff); 

    // Write shock settings 
    snprintf(mtbdl_param.param_buff, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_param_shock_info, 
             mtbdl_param.shock_psi, 
             mtbdl_param.shock_lock, 
             mtbdl_param.shock_reb); 
    hw125_puts(mtbdl_param.param_buff); 
}


// Read and format the bike parameters 
void param_bike_read_format(void)
{
    // Read fork settings 
    hw125_gets(mtbdl_param.param_buff, MTBDL_MAX_STR_LEN); 
    sscanf(mtbdl_param.param_buff, 
           mtbdl_param_fork_info, 
           &mtbdl_param.fork_psi, 
           &mtbdl_param.fork_comp, 
           &mtbdl_param.fork_reb); 

    // Read shock settings 
    hw125_gets(mtbdl_param.param_buff, MTBDL_MAX_STR_LEN); 
    sscanf(mtbdl_param.param_buff, 
           mtbdl_param_shock_info, 
           &mtbdl_param.shock_psi, 
           &mtbdl_param.shock_lock, 
           &mtbdl_param.shock_reb); 
}


// Format and write the system parameters 
void param_sys_format_write(void)
{
    // Write logging parameters 
    snprintf(mtbdl_param.param_buff, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_param_index, 
             mtbdl_param.log_index); 
    hw125_puts(mtbdl_param.param_buff); 
    
    // Write accelerometer calibration data 
    snprintf(mtbdl_param.param_buff, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_param_accel_rest, 
             mtbdl_param.accel_x_rest, 
             mtbdl_param.accel_y_rest, 
             mtbdl_param.accel_z_rest); 
    hw125_puts(mtbdl_param.param_buff); 

    // Write potentiometer calibrated starting points 
    snprintf(mtbdl_param.param_buff, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_param_pot_rest, 
             mtbdl_param.pot_fork_rest, 
             mtbdl_param.pot_shock_rest); 
    hw125_puts(mtbdl_param.param_buff); 
}


// Read and format the system parameters 
void param_sys_read_format(void)
{
    // Read logging parameters 
    hw125_gets(mtbdl_param.param_buff, MTBDL_MAX_STR_LEN); 
    sscanf(mtbdl_param.param_buff, 
           mtbdl_param_index, 
           &mtbdl_param.log_index); 

    // Read accelerometer calibration data 
    hw125_gets(mtbdl_param.param_buff, MTBDL_MAX_STR_LEN); 
    sscanf(mtbdl_param.param_buff, 
           mtbdl_param_accel_rest, 
           &mtbdl_param.accel_x_rest, 
           &mtbdl_param.accel_y_rest, 
           &mtbdl_param.accel_z_rest); 

    // Read potentiometer starting points 
    hw125_gets(mtbdl_param.param_buff, MTBDL_MAX_STR_LEN); 
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


// Update bike setting 
void param_update_bike_setting(
    param_bike_set_index_t setting_index, 
    uint8_t setting)
{
    switch (setting_index)
    {
        case PARAM_BIKE_SET_FPSI:
            mtbdl_param.fork_psi = setting; 
            break;

        case PARAM_BIKE_SET_FC:
            if (setting <= PARAM_MAX_SUS_SETTING)
            {
                mtbdl_param.fork_comp = setting; 
            }
            break;

        case PARAM_BIKE_SET_FR:
            if (setting <= PARAM_MAX_SUS_SETTING)
            {
                mtbdl_param.fork_reb = setting; 
            }
            break;

        case PARAM_BIKE_SET_SPSI:
            mtbdl_param.shock_psi = setting; 
            break;

        case PARAM_BIKE_SET_SL:
            if (setting <= PARAM_MAX_SUS_SETTING)
            {
                mtbdl_param.shock_lock = setting; 
            }
            break;

        case PARAM_BIKE_SET_SR:
            if (setting <= PARAM_MAX_SUS_SETTING)
            {
                mtbdl_param.shock_reb = setting; 
            }
            break;
        
        default: 
            break;
    }
}


// Update system settings 
void param_update_system_setting(
    param_sys_set_index_t setting_index, 
    void *setting)
{
    switch (setting_index)
    {
        case PARAM_SYS_SET_AX_REST:
            mtbdl_param.accel_x_rest = *(int16_t *)setting; 
            break;

        case PARAM_SYS_SET_AY_REST:
            mtbdl_param.accel_y_rest = *(int16_t *)setting; 
            break;

        case PARAM_SYS_SET_AZ_REST:
            mtbdl_param.accel_z_rest = *(int16_t *)setting; 
            break;

        case PARAM_SYS_SET_FORK_REST:
            mtbdl_param.pot_fork_rest = *(uint16_t *)setting; 
            break;

        case PARAM_SYS_SET_SHOCK_REST:
            mtbdl_param.pot_shock_rest = *(uint16_t *)setting; 
            break;
        
        default: 
            break;
    }
}

//=======================================================================================


//=======================================================================================
// Getters 

// Get log file index 
uint8_t param_get_log_index(void)
{
    return mtbdl_param.log_index; 
}


// Get bike settings 
uint8_t param_get_bike_setting(param_bike_set_index_t setting_index)
{
    uint8_t setting = ~CLEAR; 

    switch (setting_index)
    {
        case PARAM_BIKE_SET_FPSI: 
            setting = mtbdl_param.fork_psi; 
            break;

        case PARAM_BIKE_SET_FC:
            setting = mtbdl_param.fork_comp; 
            break;

        case PARAM_BIKE_SET_FR:
            setting = mtbdl_param.fork_reb; 
            break;

        case PARAM_BIKE_SET_SPSI:
            setting = mtbdl_param.shock_psi; 
            break;

        case PARAM_BIKE_SET_SL:
            setting = mtbdl_param.shock_lock; 
            break;

        case PARAM_BIKE_SET_SR:
            setting = mtbdl_param.shock_reb; 
            break;
        
        default: 
            break;
    }

    return setting; 
}

//=======================================================================================
