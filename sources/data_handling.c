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
// Variables 

// Data record instance 
static mtbdl_data_t mtbdl_data; 

//=======================================================================================


//=======================================================================================
// Initialization 

// Initialize data record 
void mtbdl_data_init(void)
{
    // Fork data 
    mtbdl_data.fork_psi = CLEAR; 
    mtbdl_data.fork_comp = CLEAR; 
    mtbdl_data.fork_reb = CLEAR; 

    // Shock data 
    mtbdl_data.shock_psi = CLEAR; 
    mtbdl_data.shock_lock = CLEAR; 
    mtbdl_data.shock_reb = CLEAR; 

    // SD card 
    memset((void *)mtbdl_data.data_buff, CLEAR, sizeof(mtbdl_data.data_buff)); 
}

//=======================================================================================


//=======================================================================================
// Parameter data 

// Bike parameter setup 
void mtbdl_parm_setup(void)
{
    // Create a "parameters" and "data" directories if they do not already exist 
    hw125_mkdir(mtbdl_param_dir); 
    hw125_mkdir(mtbdl_data_dir); 

    // Move to the 'parameters' sub-directory 
    hw125_set_dir(mtbdl_param_dir); 

    // Check for the existance of the parameter file that contains bike data 
    if (hw125_get_exists(mtbdl_bike_param_file) == FR_NO_FILE)
    {
        // No file - Create one and write default parameter data to the file 
        mtbdl_write_bike_params(HW125_MODE_WW); 
    }
    else 
    {
        // File already exists - open the file for reading 
        mtbdl_read_bike_params(HW125_MODE_OEWR); 
    }
}


// Read bike parameter on file 
void mtbdl_read_bike_params(
    uint8_t mode)
{
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
    // Open the file for writing 
    hw125_open(mtbdl_bike_param_file, mode); 
    
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

    // Close the file 
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
