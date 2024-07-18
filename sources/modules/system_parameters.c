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
void params_bike_format_write(void); 


/**
 * @brief Read and format the bike parameters 
 * 
 * @details Reads the bike settings/configuration from the SD card and saves the data 
 *          into the data record. These settings are stored in the bike parameters file 
 *          and this function is only called during startup if the file already exists. 
 */
void params_bike_read_formats(void); 


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
void params_sys_format_writes(void); 


/**
 * @brief Read and format the system parameters 
 * 
 * @details Reads the system settings from the SD card and saves the data into the data 
 *          record. These settings are stored in the system parameters file and this 
 *          function is only called during startup if the file already exists. 
 */
void params_sys_read_formats(void); 

//=======================================================================================


//=======================================================================================
// Dev 

// // Write bike parameters to file 
// void mtbdl_write_bike_params(uint8_t mode)
// {
//     // Move to the parameters directory, open the bike parameters file for writing, 
//     // format and write the bike parameters from the data record to the file, then close 
//     // the file. 
//     hw125_set_dir(mtbdl_param_dir); 
//     hw125_open(mtbdl_bike_param_file, mode); 
//     params_bike_format_write(); 
//     hw125_close(); 
// }


// // Read bike parameter on file 
// void mtbdl_read_bike_params(uint8_t mode)
// {
//     // Move to the parameters directory, open the bike parameters file for reading, 
//     // read the parameters and store them in the data record, then close the file. 
//     hw125_set_dir(mtbdl_param_dir); 
//     hw125_open(mtbdl_bike_param_file, mode); 
//     params_bike_read_formats(); 
//     hw125_close(); 
// }


// // Write system parameters to file 
// void mtbdl_write_sys_params(uint8_t mode)
// {
//     // Move to the parameters directory, open the system parameters file for writing, 
//     // format and write the system parameters from the data record to the file, then 
//     // close the file. 
//     hw125_set_dir(mtbdl_param_dir); 
//     hw125_open(mtbdl_sys_param_file, mode); 
//     params_sys_format_writes(); 
//     hw125_close(); 
// }


// // Read system parameters on file 
// void mtbdl_read_sys_params(uint8_t mode)
// {
//     // Move to the parameters directory, open the system parameters file for reading, 
//     // read the parameters and store them in the data record, then close the file. 
//     hw125_set_dir(mtbdl_param_dir); 
//     hw125_open(mtbdl_sys_param_file, mode); 
//     params_sys_read_formats(); 
//     hw125_close(); 
// }


// // Format and write the bike parameters 
// void params_bike_format_write(void)
// {
//     // Write fork parameters 
//     snprintf(
//         mtbdl_data.data_buff, 
//         MTBDL_MAX_DATA_STR_LEN, 
//         mtbdl_param_fork_info, 
//         mtbdl_data.fork_psi, 
//         mtbdl_data.fork_comp, 
//         mtbdl_data.fork_reb); 
    
//     hw125_puts(mtbdl_data.data_buff); 

//     // Write shock parameters 
//     snprintf(
//         mtbdl_data.data_buff, 
//         MTBDL_MAX_DATA_STR_LEN, 
//         mtbdl_param_shock_info, 
//         mtbdl_data.shock_psi, 
//         mtbdl_data.shock_lock, 
//         mtbdl_data.shock_reb); 
    
//     hw125_puts(mtbdl_data.data_buff); 
// }


// // Read and format the bike parameters 
// void params_bike_read_formats(void)
// {
//     // Read fork parameters 
//     hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

//     sscanf(
//         mtbdl_data.data_buff, 
//         mtbdl_param_fork_info, 
//         &mtbdl_data.fork_psi, 
//         &mtbdl_data.fork_comp, 
//         &mtbdl_data.fork_reb); 

//     // Read shock parameters 
//     hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

//     sscanf(
//         mtbdl_data.data_buff, 
//         mtbdl_param_shock_info, 
//         &mtbdl_data.shock_psi, 
//         &mtbdl_data.shock_lock, 
//         &mtbdl_data.shock_reb); 
// }


// // Format and write the system parameters 
// void params_sys_format_writes(void)
// {
//     // Write logging parameters 
//     snprintf(
//         mtbdl_data.data_buff, 
//         MTBDL_MAX_DATA_STR_LEN, 
//         mtbdl_param_index, 
//         mtbdl_data.log_index); 

//     hw125_puts(mtbdl_data.data_buff); 
    
//     // Write accelerometer calibration data 
//     snprintf(
//         mtbdl_data.data_buff, 
//         MTBDL_MAX_DATA_STR_LEN, 
//         mtbdl_param_accel_rest, 
//         mtbdl_data.accel_x_rest, 
//         mtbdl_data.accel_y_rest, 
//         mtbdl_data.accel_z_rest); 
    
//     hw125_puts(mtbdl_data.data_buff); 

//     // Write potentiometer calibrated starting points 
//     snprintf(
//         mtbdl_data.data_buff, 
//         MTBDL_MAX_DATA_STR_LEN, 
//         mtbdl_param_pot_rest, 
//         mtbdl_data.pot_fork_rest, 
//         mtbdl_data.pot_shock_rest); 
    
//     hw125_puts(mtbdl_data.data_buff); 
// }


// // Read and format the system parameters 
// void params_sys_read_formats(void)
// {
//     // Read logging parameters 
//     hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

//     sscanf(
//         mtbdl_data.data_buff, 
//         mtbdl_param_index, 
//         &mtbdl_data.log_index); 

//     // Read accelerometer calibration data 
//     hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

//     sscanf(
//         mtbdl_data.data_buff, 
//         mtbdl_param_accel_rest, 
//         &mtbdl_data.accel_x_rest, 
//         &mtbdl_data.accel_y_rest, 
//         &mtbdl_data.accel_z_rest); 

//     // Read potentiometer starting points 
//     hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

//     sscanf(
//         mtbdl_data.data_buff, 
//         mtbdl_param_pot_rest, 
//         &mtbdl_data.pot_fork_rest, 
//         &mtbdl_data.pot_shock_rest); 
// }

//=======================================================================================
