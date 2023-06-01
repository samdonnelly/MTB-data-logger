/**
 * @file data_handling.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL data handling header 
 * 
 * @version 0.1
 * @date 2023-05-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _DATA_HANDLING_H_ 
#define _DATA_HANDLING_H_ 

//=======================================================================================
// Includes 

#include "includes_app.h"
#include "includes_drivers.h"

//=======================================================================================


//=======================================================================================
// Macros 

#define MTBDL_MAX_DATA_STR_LEN 50        // Max string length containing data 
#define MTBDL_LOG_NUM_MAX 250            // Max data log file number 
#define MTBDL_LOG_NUM_MIN 0              // Min data log files 
#define MTBDL_LOG_OFFSET 1               // Log file number offset for the TX state 
#define MTBDL_MAX_SUS_SETTING 20         // Max compression and rebound setting 

//=======================================================================================


//=======================================================================================
// Enums 

/**
 * @brief User parameter index --> for RX state 
 */
typedef enum mtbdl_rx_param_index_s {
    MTBDL_PARM_FPSI,                     // Fork PSI 
    MTBDL_PARM_FC,                       // Fork compression setting 
    MTBDL_PARM_FR,                       // Fork rebound setting 
    MTBDL_PARM_SPSI,                     // Shock SPI 
    MTBDL_PARM_SL,                       // Shock lockout setting 
    MTBDL_PARM_SR                        // Shock rebound setting 
} mtbdl_rx_param_index_t; 

//=======================================================================================


//=======================================================================================
// Structures 

// Data record for the system 
typedef struct mtbdl_data_s 
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
    uint8_t accel_x_rest;                       // Resting x-axis acceleration offset 
    uint8_t accel_y_rest;                       // Resting y-axis acceleration offset 
    uint8_t accel_z_rest;                       // Resting z-axis acceleration offset 
    uint8_t pot_fork_rest;                      // Resting potentiometer reading for fork 
    uint8_t pot_shock_rest;                     // Resting potentiometer reading for shock 

    // SD card 
    char data_buff[MTBDL_MAX_DATA_STR_LEN];     // Buffer for reading and writing 
    char filename[MTBDL_MAX_DATA_STR_LEN];      // Buffer for storing a file name 
    uint8_t tx_status : 1;                      // TX transaction status 

    // System data 
    uint8_t soc;                                // Battery SOC 
    uint8_t navstat;                            // Navigation status of GPS module 
    uint8_t accel_x;                            // x-axis acceleration reading 
    uint8_t accel_y;                            // y-axis acceleration reading 
    uint8_t accel_z;                            // z-axis acceleration reading 
    uint8_t pot_fork;                           // Fork potentiometer reading 
    uint8_t pot_shock;                          // Shock potentiometer reading 
}
mtbdl_data_t; 

//=======================================================================================


//=======================================================================================
// Initialization 

/**
 * @brief Initialize data record 
 * 
 * @details 
 */
void mtbdl_data_init(void); 


/**
 * @brief Bike parameter setup 
 * 
 * @details 
 */
void mtbdl_file_sys_setup(void); 

//=======================================================================================


//=======================================================================================
// Parameters 

/**
 * @brief Read bike parameters on file 
 * 
 * @details 
 * 
 * @param mode : file open mode flag - see hw125 driver 
 */
void mtbdl_read_bike_params(
    uint8_t mode); 


/**
 * @brief Write bike parameters to file 
 * 
 * @details 
 * 
 * @param mode : file open mode flag - see hw125 driver 
 */
void mtbdl_write_bike_params(
    uint8_t mode); 


/**
 * @brief Read system parameters on file 
 * 
 * @details 
 * 
 * @param mode : file open mode flag - see hw125 driver 
 */
void mtbdl_read_sys_params(
    uint8_t mode); 


/**
 * @brief Write system parameters to file 
 * 
 * @details 
 * 
 * @param mode : file open mode flag - see hw125 driver 
 */
void mtbdl_write_sys_params(
    uint8_t mode); 

//=======================================================================================


//=======================================================================================
// Data logging 

/**
 * @brief Log name preparation 
 * 
 * @details 
 * 
 * @return uint8_t 
 */
uint8_t mtbdl_log_name_prep(void); 


/**
 * @brief Log file prep 
 * 
 * @details 
 */
void mtbdl_log_file_prep(void); 


/**
 * @brief Record data 
 * 
 * @details 
 */
void mtbdl_logging(void); 


/**
 * @brief End the data logging 
 * 
 * @details 
 */
void mtbdl_log_end(void); 

//=======================================================================================


//=======================================================================================
// RX state functions 

/**
 * @brief RX user interface start 
 * 
 * @details 
 */
void mtbdl_rx_start(void); 


/**
 * @brief Read and assign the user input 
 * 
 * @details 
 */
void mtbdl_rx(void); 

//=======================================================================================


//=======================================================================================
// TX state functions 

/**
 * @brief Prepare to send a data log file 
 * 
 * @details 
 * 
 * @return uint8_t 
 */
uint8_t mtbdl_tx_prep(void); 


/**
 * @brief Transfer data log contents 
 * 
 * @details 
 * 
 * @return uint8_t : 
 */
uint8_t mtbdl_tx(void); 


/**
 * @brief Close the log file and delete it 
 * 
 * @details 
 */
void mtbdl_tx_end(void); 

//=======================================================================================


//=======================================================================================
// Screen message formatting 

/**
 * @brief Format the idle state message 
 * 
 * @details The idle state message contains system values that are relevant to the user 
 *          and these values can change, This function updates the values of the idle 
 *          state message and triggers a write of this message to the screen. A list of 
 *          the values that the message contains are listed in the parameters below. 
 */
void mtbdl_set_idle_msg(void); 


/**
 * @brief Format the run prep state message 
 * 
 * @details The run prep state message contains the GPS position lock status. This 
 *          information is displayed to the user before entering the run mode and allows 
 *          the user to know if they have GPS lock before beginning to record data. This 
 *          function updates GPS status information and triggers a write of this message 
 *          to the screen. 
 */
void mtbdl_set_run_prep_msg(void); 


/**
 * @brief Format the pre TX state message 
 * 
 * @details 
 */
void mtbdl_set_pretx_msg(void); 

//=======================================================================================

#endif   // _DATA_HANDLING_H_ 
