/**
 * @file data_handling.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL data handling 
 * 
 * @version 0.1
 * @date 2023-05-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//=======================================================================================
// Includes 

#include "data_handling.h" 

//=======================================================================================


//=======================================================================================
// Function prototypes 

/**
 * @brief Format and write the bike parameters 
 * 
 * @details 
 */
void mtbdl_format_write_bike_params(void); 


/**
 * @brief Format and write the system parameters 
 * 
 * @details 
 */
void mtbdl_format_write_sys_params(void); 

//=======================================================================================


//=======================================================================================
// Variables 

// Data record instance 
static mtbdl_data_t mtbdl_data; 

//=======================================================================================


//=======================================================================================
// Initialization 

// Initialize data record 
void mtbdl_data_init(void)
{
    // Bike parameters 
    mtbdl_data.fork_psi = CLEAR; 
    mtbdl_data.fork_comp = CLEAR; 
    mtbdl_data.fork_reb = CLEAR; 
    mtbdl_data.shock_psi = CLEAR; 
    mtbdl_data.shock_lock = CLEAR; 
    mtbdl_data.shock_reb = CLEAR; 

    // System parameters 
    mtbdl_data.log_index = CLEAR; 
    mtbdl_data.accel_x_rest = CLEAR; 
    mtbdl_data.accel_y_rest = CLEAR; 
    mtbdl_data.accel_x_rest = CLEAR; 
    mtbdl_data.pot_fork_rest = CLEAR; 
    mtbdl_data.pot_shock_rest = CLEAR; 

    // SD card 
    memset((void *)mtbdl_data.data_buff, CLEAR, sizeof(mtbdl_data.data_buff)); 

    // System data 
    mtbdl_data.soc = CLEAR; 
    mtbdl_data.navstat = CLEAR; 
    mtbdl_data.accel_x = CLEAR; 
    mtbdl_data.accel_y = CLEAR; 
    mtbdl_data.accel_z = CLEAR; 
    mtbdl_data.pot_fork = CLEAR; 
    mtbdl_data.pot_shock = CLEAR; 
}

//=======================================================================================


//=======================================================================================
// Parameters 

// Parameter setup 
void mtbdl_parm_setup(void)
{
    // Create a "parameters" directory if it does not already exist 
    hw125_mkdir(mtbdl_param_dir); 

    // Check for the existance of the bike parameters file 
    if (hw125_get_exists(mtbdl_bike_param_file) == FR_NO_FILE)
    {
        // No file - create one and write default parameter data to it 
        mtbdl_write_bike_params(HW125_MODE_WW); 
    }
    else 
    {
        // File already exists - open the file for reading 
        mtbdl_read_bike_params(HW125_MODE_OEWR); 
    }

    // Check for the existance of the system parameters file 
    if (hw125_get_exists(mtbdl_sys_param_file) == FR_NO_FILE)
    {
        // No file - create one and write default parameter data to it 
        mtbdl_write_sys_params(HW125_MODE_WW); 
    }
    else 
    {
        // File already exists - open the file for reading 
        mtbdl_read_sys_params(HW125_MODE_OEWR); 
    }
}


// Read bike parameter on file 
void mtbdl_read_bike_params(
    uint8_t mode)
{
    // Move to the parameters directory 
    hw125_set_dir(mtbdl_param_dir); 

    // Open the file for reading 
    hw125_open(mtbdl_bike_param_file, mode); 

    // Read fork parameters 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    sscanf(mtbdl_data.data_buff, 
           mtbdl_param_fork_info, 
           &mtbdl_data.fork_psi, 
           &mtbdl_data.fork_comp, 
           &mtbdl_data.fork_reb); 

    // Read shock parameters 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    sscanf(mtbdl_data.data_buff, 
           mtbdl_param_shock_info, 
           &mtbdl_data.shock_psi, 
           &mtbdl_data.shock_lock, 
           &mtbdl_data.shock_reb); 

    // Close the file 
    hw125_close(); 
}


// Write bike parameters to file 
void mtbdl_write_bike_params(
    uint8_t mode)
{
    // Move to the parameters directory 
    hw125_set_dir(mtbdl_param_dir); 

    // Open the file for writing 
    hw125_open(mtbdl_bike_param_file, mode); 

    // Format and write the bike parameters 
    mtbdl_format_write_bike_params(); 

    // Close the file 
    hw125_close(); 
}


// Read system parameters on file 
void mtbdl_read_sys_params(
    uint8_t mode)
{
    // Move to the parameters directory 
    hw125_set_dir(mtbdl_param_dir); 

    // Open the file for reading 
    hw125_open(mtbdl_sys_param_file, mode); 

    // Read logging parameters 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    sscanf(mtbdl_data.data_buff, 
           mtbdl_param_log, 
           &mtbdl_data.log_index); 

    // Read accelerometer calibration data 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    sscanf(mtbdl_data.data_buff, 
           mtbdl_param_accel_rest, 
           &mtbdl_data.accel_x_rest, 
           &mtbdl_data.accel_y_rest, 
           &mtbdl_data.accel_z_rest); 

    // Read potentiometer starting points 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    sscanf(mtbdl_data.data_buff, 
           mtbdl_param_pot_rest, 
           &mtbdl_data.pot_fork_rest, 
           &mtbdl_data.pot_shock_rest); 

    // Close the file 
    hw125_close(); 
}


// Write system parameters to file 
void mtbdl_write_sys_params(
    uint8_t mode)
{
    // Move to the parameters directory 
    hw125_set_dir(mtbdl_param_dir); 
    
    // Open the file for writing 
    hw125_open(mtbdl_sys_param_file, mode); 

    // Format and write the system parameters 
    mtbdl_format_write_sys_params(); 

    // Close the file 
    hw125_close(); 
}


// Format and write the bike parameters 
void mtbdl_format_write_bike_params(void)
{
    // Write fork parameters 
    snprintf(mtbdl_data.data_buff, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_param_fork_info, 
             mtbdl_data.fork_psi, 
             mtbdl_data.fork_comp, 
             mtbdl_data.fork_reb); 
    
    hw125_puts(mtbdl_data.data_buff); 

    // Write shock parameters 
    snprintf(mtbdl_data.data_buff, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_param_shock_info, 
             mtbdl_data.shock_psi, 
             mtbdl_data.shock_lock, 
             mtbdl_data.shock_reb); 
    
    hw125_puts(mtbdl_data.data_buff); 
}


// Format and write the system parameters 
void mtbdl_format_write_sys_params(void)
{
    // Write logging parameters 
    snprintf(mtbdl_data.data_buff, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_param_log, 
             mtbdl_data.log_index); 

    hw125_puts(mtbdl_data.data_buff); 
    
    // Write accelerometer calibration data 
    snprintf(mtbdl_data.data_buff, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_param_accel_rest, 
             mtbdl_data.accel_x_rest, 
             mtbdl_data.accel_y_rest, 
             mtbdl_data.accel_z_rest); 
    
    hw125_puts(mtbdl_data.data_buff); 

    // Write potentiometer calibrated starting points 
    snprintf(mtbdl_data.data_buff, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_param_pot_rest, 
             mtbdl_data.pot_fork_rest, 
             mtbdl_data.pot_shock_rest); 
    
    hw125_puts(mtbdl_data.data_buff); 
}

//=======================================================================================


//=======================================================================================
// Data logging 

// Data loggin setup 
void mtbdl_data_setup(void)
{
    // Create "data" directory if it does not already exist 
    hw125_mkdir(mtbdl_data_dir); 
}


// Log name preparation 
uint8_t mtbdl_log_name_prep(void)
{
    // Check the data log index is within bounds 
    if (mtbdl_data.log_index > MTBDL_LOG_NUM_MAX)
    {
        // Too many log files on drive - don't create a new file name 
        return FALSE; 
    }

    // Number of log files is within the limit - generate a new log file name 
    snprintf(mtbdl_data.data_buff, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_log_file, 
             mtbdl_data.log_index); 

    return TRUE; 
}


// Log file prep 
void mtbdl_log_file_prep(void)
{
    // Move to the data directory 
    hw125_set_dir(mtbdl_data_dir); 

    if (hw125_open(mtbdl_data.data_buff, HW125_MODE_WWX) == FR_OK)
    {
        // File successfully created - write parameters to it and update the file index 
        mtbdl_format_write_bike_params(); 
        mtbdl_format_write_sys_params(); 
        hw125_puts(mtbdl_data_log_start); 
        mtbdl_data.log_index++; 
    }
}


// Log file close 
void mtbdl_log_file_close(void)
{
    hw125_close(); 
}

//=======================================================================================


//=======================================================================================
// Screen message formatting 

// Format the idle state message 
void mtbdl_set_idle_msg(void)
{
    // Local variables 
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_4_LINE]; 

    // Create an editable copy of the message 
    for (uint8_t i = 0; i < MTBDL_MSG_LEN_4_LINE; i++) msg[i] = mtbdl_idle_msg[i]; 

    // Format the message with data 
    snprintf(msg[0].msg, HD44780U_LINE_LEN, mtbdl_idle_msg[0].msg, 
             mtbdl_data.fork_psi, mtbdl_data.fork_comp, mtbdl_data.fork_reb); 
    snprintf(msg[1].msg, HD44780U_LINE_LEN, mtbdl_idle_msg[1].msg, 
             mtbdl_data.shock_psi, mtbdl_data.shock_lock, mtbdl_data.shock_reb); 
    snprintf(msg[2].msg, HD44780U_LINE_LEN, mtbdl_idle_msg[2].msg, 
             mtbdl_data.soc); 

    // Set the screen message 
    hd44780u_set_msg(msg, MTBDL_MSG_LEN_4_LINE); 
}


// Format the run prep state message 
void mtbdl_set_run_prep_msg(void)
{
    // Local variables 
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_3_LINE]; 

    // Create an editable copy of the message 
    for (uint8_t i = 0; i < MTBDL_MSG_LEN_3_LINE; i++) msg[i] = mtbdl_run_prep_msg[i]; 

    // Convert the NAVSTAT code to an easily readable value 

    // Format the message with data 
    snprintf(msg[0].msg, HD44780U_LINE_LEN, mtbdl_run_prep_msg[0].msg, 
             mtbdl_data.navstat); 

    // Set the screen message 
    hd44780u_set_msg(msg, MTBDL_MSG_LEN_3_LINE); 
}

//=======================================================================================
