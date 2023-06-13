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
 * @brief Read and format the bike parameters 
 * 
 * @details 
 */
void mtbdl_format_read_bike_params(void); 


/**
 * @brief Format and write the system parameters 
 * 
 * @details 
 */
void mtbdl_format_write_sys_params(void); 


/**
 * @brief Read and format the system parameters 
 * 
 * @details 
 */
void mtbdl_format_read_sys_params(void); 

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
    mtbdl_data.tx_status = CLEAR_BIT; 

    // LEDs 
    memset((void *)mtbdl_data.led_colour_data, CLEAR, sizeof(mtbdl_data.led_colour_data)); 

    // System data 
    mtbdl_data.soc = CLEAR; 
    mtbdl_data.navstat = CLEAR; 
    mtbdl_data.accel_x = CLEAR; 
    mtbdl_data.accel_y = CLEAR; 
    mtbdl_data.accel_z = CLEAR; 
    mtbdl_data.pot_fork = CLEAR; 
    mtbdl_data.pot_shock = CLEAR; 
}


// File system setup 
void mtbdl_file_sys_setup(void)
{
    // Create "parameters" and "data" directories if they do not already exist 
    hw125_mkdir(mtbdl_data_dir); 
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

//=======================================================================================


//=======================================================================================
// Parameters 

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


// Read bike parameter on file 
void mtbdl_read_bike_params(
    uint8_t mode)
{
    // Move to the parameters directory 
    hw125_set_dir(mtbdl_param_dir); 

    // Open the file for reading 
    hw125_open(mtbdl_bike_param_file, mode); 

    // Read and format the bikr parameters 
    mtbdl_format_read_bike_params(); 

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


// Read system parameters on file 
void mtbdl_read_sys_params(
    uint8_t mode)
{
    // Move to the parameters directory 
    hw125_set_dir(mtbdl_param_dir); 

    // Open the file for reading 
    hw125_open(mtbdl_sys_param_file, mode); 

    // Read and format the system parameters 
    mtbdl_format_read_sys_params(); 

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


// Read and format the bike parameters 
void mtbdl_format_read_bike_params(void)
{
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


// Read and format the system parameters 
void mtbdl_format_read_sys_params(void)
{
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
}

//=======================================================================================


//=======================================================================================
// Data logging 

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
    snprintf(mtbdl_data.filename, 
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

    if (hw125_open(mtbdl_data.filename, HW125_MODE_WWX) == FR_OK)
    {
        // File successfully created - write parameters to it and update the file index 
        mtbdl_format_write_bike_params(); 
        mtbdl_format_write_sys_params(); 
        hw125_puts(mtbdl_data_log_start); 
        mtbdl_data.log_index++; 
    }
}


// Record data 
void mtbdl_logging(void)
{
    // 
}


// Log file close 
void mtbdl_log_end(void)
{
    hw125_puts(mtbdl_data_log_end); 
    hw125_close(); 

    // Update the log index 
    mtbdl_write_sys_params(HW125_MODE_OAWR); 
}

//=======================================================================================


//=======================================================================================
// RX state functions 

// RX user interface start 
void mtbdl_rx_prep(void)
{
    hc05_send(mtbdl_rx_prompt); 
    hc05_clear(); 
}


// Read and assign the user input 
void mtbdl_rx(void)
{
    // Local variables 
    unsigned int param_index; 
    unsigned int temp_data; 

    // Read Bluetooth data if available 
    if (hc05_data_status())
    {
        // Read and parse the data from the HC-05 
        hc05_read(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 
        sscanf(mtbdl_data.data_buff, 
               mtbdl_rx_input, 
               &param_index, 
               &temp_data); 

        // Check for a data match 
        switch (param_index)
        {
            case MTBDL_PARM_FPSI:
                mtbdl_data.fork_psi = temp_data; 

                break;

            case MTBDL_PARM_FC:
                if (temp_data <= MTBDL_MAX_SUS_SETTING)
                {
                    mtbdl_data.fork_comp = temp_data; 
                }

                break;

            case MTBDL_PARM_FR:
                if (temp_data <= MTBDL_MAX_SUS_SETTING)
                {
                    mtbdl_data.fork_reb = temp_data; 
                }

                break;

            case MTBDL_PARM_SPSI:
                mtbdl_data.shock_psi = temp_data; 

                break;

            case MTBDL_PARM_SL:
                if (temp_data <= MTBDL_MAX_SUS_SETTING)
                {
                    mtbdl_data.shock_lock = temp_data; 
                }

                break;

            case MTBDL_PARM_SR:
                if (temp_data <= MTBDL_MAX_SUS_SETTING)
                {
                    mtbdl_data.shock_reb = temp_data; 
                }

                break;
            
            default:
                break;
        }

        // Provide a user prompt 
        mtbdl_rx_prep(); 
    }
}

//=======================================================================================


//=======================================================================================
// TX state functions 

// Prepare to send a data log file 
uint8_t mtbdl_tx_prep(void)
{
    // Check if there are no log files 
    if (mtbdl_data.log_index == MTBDL_LOG_NUM_MIN)
    {
        return FALSE; 
    }

    // Log files exist - generate a new log file name 
    // The log index is adjusted because it will be one ahead of the most recent log 
    // file number after the most recent log has been created 
    snprintf(mtbdl_data.filename, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_log_file, 
             (mtbdl_data.log_index - MTBDL_DATA_INDEX_OFFSET)); 

    // Move to the data directory 
    hw125_set_dir(mtbdl_data_dir); 

    // Check for the existance of the specified file number 
    if (hw125_get_exists(mtbdl_data.filename) == FR_NO_FILE)
    {
        return FALSE; 
    }

    // Open the file 
    hw125_open(mtbdl_data.filename, HW125_MODE_OAWR); 

    return TRUE; 
}


// Transfer data log contents 
uint8_t mtbdl_tx(void)
{
    // Read a line from the data log 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    // Send the data over Bluetooth 
    hc05_send(mtbdl_data.data_buff); 

    // Check for end of file - if true we can stop the transaction 
    if (hw125_eof())
    {
        mtbdl_data.tx_status = SET_BIT; 
        return TRUE; 
    }

    return FALSE; 
}


// End the transmission 
void mtbdl_tx_end(void)
{
    hw125_close(); 

    if (mtbdl_data.tx_status)
    {
        // Transaction completed - delete the file and update the log index 
        hw125_unlink(mtbdl_data.filename); 
        mtbdl_data.tx_status = CLEAR_BIT; 
        mtbdl_data.log_index--; 
        mtbdl_write_sys_params(HW125_MODE_OAWR); 
    }
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
    // snprintf will NULL terminate the string at the screen line length so in order to use 
    // the last spot on the screen line the message length must be indexed up by one 
    snprintf(msg[0].msg, (HD44780U_LINE_LEN + MTBDL_DATA_INDEX_OFFSET), mtbdl_idle_msg[0].msg, 
             mtbdl_data.fork_psi, mtbdl_data.fork_comp, mtbdl_data.fork_reb); 
    snprintf(msg[1].msg, (HD44780U_LINE_LEN + MTBDL_DATA_INDEX_OFFSET), mtbdl_idle_msg[1].msg, 
             mtbdl_data.shock_psi, mtbdl_data.shock_lock, mtbdl_data.shock_reb); 
    snprintf(msg[2].msg, (HD44780U_LINE_LEN + MTBDL_DATA_INDEX_OFFSET), mtbdl_idle_msg[2].msg, 
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


// Format the pre TX state message 
void mtbdl_set_pretx_msg(void)
{
    // Local variables 
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_4_LINE]; 

    // Create an editable copy of the message 
    for (uint8_t i = 0; i < MTBDL_MSG_LEN_4_LINE; i++) msg[i] = mtbdl_pretx_msg[i]; 

    // Format the message with data 
    // The log index is adjusted because it will be one ahead of the most recent log 
    // file number after the most recent log has been created 
    snprintf(msg[1].msg, HD44780U_LINE_LEN, mtbdl_pretx_msg[1].msg, 
             (mtbdl_data.log_index - MTBDL_DATA_INDEX_OFFSET)); 

    // Set the screen message 
    hd44780u_set_msg(msg, MTBDL_MSG_LEN_4_LINE); 
}

//=======================================================================================


//=======================================================================================
// LEDs 

// Update LED colours 
void mtbdl_led_update(
    ws2812_led_index_t led_index, 
    uint32_t led_code)
{
    mtbdl_data.led_colour_data[led_index] = led_code; 
    ws2812_send(DEVICE_ONE, mtbdl_data.led_colour_data); 
}

//=======================================================================================
