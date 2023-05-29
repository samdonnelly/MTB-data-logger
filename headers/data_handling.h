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

//=======================================================================================


//=======================================================================================
// Parameters 

/**
 * @brief Bike parameter setup 
 * 
 * @details 
 */
void mtbdl_parm_setup(void); 


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
 * @brief Data loggin setup 
 * 
 * @details 
 */
void mtbdl_data_setup(void); 


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
 * @brief Log file close 
 * 
 * @details 
 */
void mtbdl_log_file_close(void); 

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
 * 
 * @param navstat : GPS navigation status - read from the system 
 */
void mtbdl_set_run_prep_msg(void); 

//=======================================================================================

#endif   // _DATA_HANDLING_H_ 
