/**
 * @file data_logging.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB data logging interface 
 * 
 * @version 0.1
 * @date 2023-05-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _DATA_LOGGING_H_ 
#define _DATA_LOGGING_H_ 

//=======================================================================================
// Includes 

#include "includes_drivers.h" 
#include "system_parameters.h" 
#include "string_config.h" 

//=======================================================================================


//=======================================================================================
// Macros 

// #define MTBDL_ADC_BUFF_SIZE 3            // Size according to the number of ADCs used 

// #define MTBDL_MAX_DATA_STR_LEN 60        // Max string length containing data 
// #define MTBDL_DATA_INDEX_OFFSET 1        // Log file number offset for the TX state 
// #define MTBDL_MAX_SUS_SETTING 20         // Max compression and rebound setting 

// Calibration 
// #define MTBDL_NUM_CAL_DATA 5             // Number of parameters that require calibration 

// Data logging 
#define LOG_MAX_FILES 250                // Max data log file number 
#define LOG_GPS_BUFF_LEN 12              // GPS coordinate buffer size 
#define LOG_TIME_BUFF_LEN 10             // UTC time and data buff size 
// #define MTBDL_NUM_LOG_SEQ 25             // Number of data logging sequence steps 
// #define MTBDL_LOG_COUNT_CYCLE 99         // Log sample sequence max timer counter value 
// #define MTBDL_NUM_LOG_STREAMS 6          // Number of data logging streams 
// #define MTBDL_COO_BUFF_LEN 6             // Coordinate buffer size - data from M8Q driver 
// #define MTBDL_TIME_BUFF_LEN 10           // UTC time buffer size - data from M8Q driver 
// #define MTBDL_DATE_BUFF_LEN 7            // UTC date buffer size - data from M8Q driver 

// Wheel RPM info 
#define MTBDL_REV_SAMPLE_SIZE 4          // Number of samples for revolution calc 

// Debugging 
#define MTBDL_DEBUG 0                    // Conditional compilation for debugging 
#define MTBDL_DEBUG_SAMPLE_COUNT 999     // Number of data samples to take in debug mode 

//=======================================================================================


//=======================================================================================
// Enums 

// ADC buffer index 
typedef enum {
    ADC_FORK,       // Fork potentiometer 
    ADC_SHOCK,      // Shock potentiometer 
    ADC_BUFF_SIZE   // Size of buffer to hold all ADC values 
} mtbdl_adc_buff_index_t; 

//=======================================================================================


//=======================================================================================
// Structure 

// Data logging data record 
typedef struct mtbdl_log_s 
{
    // Peripherals 
    IRQn_Type rpm_irq;                          // Wheel RPM interrupt number 
    IRQn_Type log_irq;                          // Log sample period interrupt number 
    ADC_TypeDef *adc;                           // ADC port battery soc and pots 

    // // Bike parameters 
    // uint8_t fork_psi;                           // Fork pressure (psi) 
    // uint8_t fork_comp;                          // Fork compression setting 
    // uint8_t fork_reb;                           // Fork rebound setting 
    // uint8_t shock_psi;                          // Shock pressure (psi) 
    // uint8_t shock_lock;                         // Shock lockout setting 
    // uint8_t shock_reb;                          // Shock rebound setting 

    // // System parameters 
    // uint8_t log_index;                          // Data log index 
    // int16_t accel_x_rest;                       // Resting x-axis acceleration offset 
    // int16_t accel_y_rest;                       // Resting y-axis acceleration offset 
    // int16_t accel_z_rest;                       // Resting z-axis acceleration offset 
    // uint16_t pot_fork_rest;                     // Resting potentiometer reading for fork 
    // uint16_t pot_shock_rest;                    // Resting potentiometer reading for shock 

    // System data 
    uint16_t adc_buff[ADC_BUFF_SIZE];           // ADC buffer - SOC, fork pot, shock pot 
    uint8_t utc_time[LOG_TIME_BUFF_LEN];        // UTC time recorded by the GPS module 
    uint8_t utc_date[LOG_TIME_BUFF_LEN];        // UTC date recorded by the GPS module 
    uint8_t lat_str[LOG_GPS_BUFF_LEN];          // Latitude string 
    uint8_t NS;                                 // North/South indicator of latitude 
    uint8_t lon_str[LOG_GPS_BUFF_LEN];          // Longitude string 
    uint8_t EW;                                 // Eeast/West indicator of longitude 
    int16_t accel_x;                            // x-axis acceleration reading 
    int16_t accel_y;                            // y-axis acceleration reading 
    int16_t accel_z;                            // z-axis acceleration reading 
    uint8_t trailmark;                          // Trail marker flag 
    // uint8_t soc;                                // Battery SOC 
    // uint16_t navstat;                           // Navigation status of GPS module 
    // uint8_t deg_min_lat[MTBDL_COO_BUFF_LEN];    // Latitude: degrees and minutes integer part 
    // uint8_t min_frac_lat[MTBDL_COO_BUFF_LEN];   // Latitude: minuutes fractional part 
    // uint8_t deg_min_lon[MTBDL_COO_BUFF_LEN];    // Longitude: degrees and minutes integer part 
    // uint8_t min_frac_lon[MTBDL_COO_BUFF_LEN];   // Longitude: minuutes fractional part 

    // Calibration data 
    // int32_t cal_buff[MTBDL_NUM_CAL_DATA];       // Buffer that holds calibration data 
    int32_t cal_buff[PARAM_SYS_SET_NUM];        // Buffer that holds calibration data 
    int32_t cal_samples;                        // Calibration sample index 

    // // LED colour data - Green bits: 16-23, Red bits: 8-15, Blue bits: 0-7 
    // uint32_t led_colour_data[WS2812_LED_NUM]; 

    // Wheel RPM info 
    uint8_t rev_count;                          // Wheel revolution counter 
    uint8_t rev_buff_index;                     // Wheel revolution circular buffer index 
    uint8_t rev_buff[MTBDL_REV_SAMPLE_SIZE];    // Circular buffer for revolution calcs 

    // SD card 
    char data_buff[MTBDL_MAX_STR_LEN];          // Buffer for reading and writing 
    char filename[MTBDL_MAX_STR_LEN];           // Buffer for storing a file name 
    // uint8_t tx_status : 1;                      // TX transaction status 

    // Stream counters 
    uint8_t gps_stream_counter; 
    uint8_t accel_stream_counter; 
    uint8_t speed_stream_counter; 

    // Log tracking 
    // uint32_t time_count;                        // Time tracking counter for logging 
    // uint8_t stream_index;                       // Index for log stream sequencing 
    // uint8_t led_toggle : 1;                     // LED toggle bit 
    // uint8_t run_count  : 1;                     // Log stream toggle 
    // uint8_t user_input : 1;                     // User input flag 
    // uint8_t log_stream : 3;                     // Logging stream number (mtbdl_log_streams_t)

#if MTBDL_DEBUG 
    // Testing 
    uint16_t time_stop; 
    uint16_t time_limit; 
    uint16_t count_standard; 
    uint16_t count_wait; 
    uint8_t time_overflow; 
    uint8_t count_blink; 
    uint8_t count_speed; 
    uint8_t count_accel; 
    uint8_t count_gps; 
    uint8_t count_user; 
    uint16_t adc_count; 
#endif   // MTBDL_DEBUG 
}
mtbdl_log_t; 

//=======================================================================================


//=======================================================================================
// Initialization 

/**
 * @brief Initialize data logging module 
 * 
 * @details Sets all the data handling info to its default value. This should be called 
 *          during the setup code. The arguments are saved into the data handling record 
 *          so they can be used where needed. 
 *          
 *          ADC DMA setup: 
 *          Called during system setup to configure the DMA stream. This function is 
 *          called instead of doing the initialization directly in the setup so that 
 *          the ADC buffer, used to store ADC values and exists in the scope of the 
 *          data handling file, can be associated with the setup. 
 * 
 * @param rpm_irqn : wheel speed periodic interrupt index 
 * @param log_irqn : data sample periodic interrupt index 
 * @param adc : pointer to ADC port used 
 * @param dma_stream : pointer to DMA stream being used 
 */
void log_init(
    IRQn_Type rpm_irqn, 
    IRQn_Type log_irqn, 
    ADC_TypeDef *adc, 
    DMA_Stream_TypeDef *dma_stream); 

//=======================================================================================


//=======================================================================================
// Data logging 

/**
 * @brief Log name preparation 
 * 
 * @details Checks if there is room to create a new log file, and if so, generates a new 
 *          log file name. If the number of log file is at its capacity then the function 
 *          will return false. 
 * 
 * @return uint8_t : file availability status 
 */
uint8_t log_data_name_prep(void); 


/**
 * @brief Log file prep 
 * 
 * @details Moves to the log file directory on the SD card and attempts to create and 
 *          open a new log file using the name generated in the name preparation function. 
 *          If successful, the new log file will have all the system information written 
 *          to it as a reference for the user when they go to view the file. 
 *          
 *          Note that the log file preparation function should be called before this 
 *          function so a new file name can be created. 
 * 
 * @see log_data_name_prep 
 */
void log_data_file_prep(void); 


/**
 * @brief Log data prep 
 * 
 * @details Resets data logging info and enables interrupts, all of which are needed 
 *          before beginning to log data correctly. Without a call to this function, no 
 *          data will be logged in the logging function. 
 */
void log_data_prep(void); 


/**
 * @brief Logging data 
 * 
 * @details Logs bike data every sample period which is triggered by the sample period 
 *          interrupt. When triggered, data will be logged based on a predefined schedule 
 *          that samples data only as often as it's needed. Data includes trailmarkers 
 *          set by a user button press, suspension position potentiometer readings, wheel 
 *          speed calculations based on hall effect sensor frequency, IMU rates and GPS 
 *          position. Trailmarkers and suspension position are the only pieces of data 
 *          that get logged every interval/period. When data is read, it gets written to 
 *          the open log file on the SD card. This function should be called continuously 
 *          while in the data logging state. 
 *          
 *          Before calling this function, the log file and data need to be prepared. 
 * 
 * @see log_data_name_prep 
 * @see log_data_file_prep 
 * @see log_data_prep 
 */
void log_data(void); 


/**
 * @brief End the data logging 
 * 
 * @details Disables interrupts, saves and closes the log file, and updates the log index. 
 *          This function must be called after the logging function in order to finish 
 *          the logging process. 
 */
void log_data_end(void); 

//=======================================================================================


//=======================================================================================
// Calibration 

/**
 * @brief Calibration data prep 
 * 
 * @details Prepare the calibration data, enables reading from the IMU and potentiometers 
 *          and enables data sampling interrupt. This must be called before using the 
 *          calibration function. 
 *          
 *          Calibration requires finding the resting values/offsets of the bike IMU and 
 *          suspension potentiometers. These offsets are used "zero" the readings so 
 *          data logs values are more accurate. 
 * 
 * @see log_calibration 
 */
void log_calibration_prep(void); 


/**
 * @brief Calibration 
 * 
 * @details Records IMU and suspension potentiometer values periodically (every time the 
 *          data sampling interrupt runs) by summing each successive reading. The number 
 *          of times the values are recorded is tracked. The sum of these values will be 
 *          used to find the average reading over a period of time in the calibration 
 *          calculation function. Note that since the values get summed together, the 
 *          amount of time this function is continuously called should be considered. 
 *          
 *          Calibration requires finding the resting values/offsets of the bike IMU and 
 *          suspension potentiometers. These offsets are used "zero" the readings so 
 *          data logs values are more accurate. 
 * 
 * @see log_calibration_prep 
 * @see log_calibration_calculation 
 */
void log_calibration(void); 


/**
 * @brief Calibration calculation 
 * 
 * @details Disables the data sampling interrupt and averages the sensor readings 
 *          accumulated during the calibration function to provide new sensor calibration/
 *          offset values. This function only provides new calibration data if the 
 *          calibration function is called first. Note that this function does not update 
 *          the parameters file. That should be done separately if values are to be saved. 
 *          
 *          Calibration requires finding the resting values/offsets of the bike IMU and 
 *          suspension potentiometers. These offsets are used "zero" the readings so 
 *          data logs values are more accurate. 
 * 
 * @see log_calibration 
 */
void log_calibration_calculation(void); 

//=======================================================================================


//=======================================================================================
// Setters 

/**
 * @brief Set trail marker flag 
 * 
 * @details Used by the run state while data logging to set the trail marker flag when 
 *          the marker button is pushed. The trail marker gets written to the log file 
 *          so the used can identify points within the log. 
 */
void log_set_trailmark(void); 

//=======================================================================================


//=======================================================================================
// To be deleted 

//==================================================
// Initialization 

// /**
//  * @brief Bike parameter setup 
//  * 
//  * @details Creates directories on the SD card for storing system and bike parameters 
//  *          as well data logs if the directories do not already exist. After establishing 
//  *          directories, checks for existance of system and bike parameter files. If they 
//  *          exist then they will be read and stored into the data handling record. If not 
//  *          then they will be created and intialized to default values. 
//  *          
//  *          This function should be called during the setup code. 
//  */
// void mtbdl_file_sys_setup(void); 


// /**
//  * @brief ADC DMA setup 
//  * 
//  * @details Called during system setup to configure the DMA stream. This function is 
//  *          called instead of doing the initialization directly in the setup so that 
//  *          the ADC buffer, used to store ADC values and exists in the scope of the 
//  *          data handling file, can be associated with the setup. 
//  * 
//  * @param dma_stream : pointer to DMA stream being used 
//  * @param adc : pointer to ADC port used 
//  */
// void mtbdl_adc_dma_init(
//     DMA_Stream_TypeDef *dma_stream, 
//     ADC_TypeDef *adc); 

//==================================================


//==================================================
// Parameters 

// /**
//  * @brief Write bike parameters to file 
//  * 
//  * @details Writes the bike parameters currently stored in the data record to the bike 
//  *          parameters file on the SD card. Saving this data allows for the system to 
//  *          remember info after reboot. Bike parameters include the fork and shock 
//  *          settings such as psi and rebound settings. These can be updated in the RX 
//  *          mode. 
//  * 
//  * @param mode : file open mode flag - see hw125 driver 
//  */
// void mtbdl_write_bike_params(uint8_t mode); 


// /**
//  * @brief Read bike parameters on file 
//  * 
//  * @details Reads the bike parameters file on the SD card and stores the info in the data 
//  *          record. This function is called on startup if a bike parameters file already 
//  *          exists. This allows the system to use parameters previously saved before a 
//  *          reboot. Bike parameters include the fork and shock settings such as psi and 
//  *          rebound settings. 
//  * 
//  * @param mode : file open mode flag - see hw125 driver 
//  */
// void mtbdl_read_bike_params(uint8_t mode); 


// /**
//  * @brief Write system parameters to file 
//  * 
//  * @details Writes the system parameters currently stored in the data record to the 
//  *          system parameters file on the SD card. Saving this data allows for the system 
//  *          to remember info after reboot. System parameters include the IMU and 
//  *          potentiometer calibration data and the log index. These can be updated after 
//  *          calibration, data logging or TX mode. 
//  * 
//  * @param mode : file open mode flag - see hw125 driver 
//  */
// void mtbdl_write_sys_params(uint8_t mode); 


// /**
//  * @brief Read system parameters on file 
//  * 
//  * @details Reads the system parameters file on the SD card and stores the info in the 
//  *          data record. This function is called on startup if a system parameters file 
//  *          already exists. This allows the system to use parameters previously saved 
//  *          before a reboot. System parameters include the IMU and potentiometer 
//  *          calibration data and the log index. 
//  * 
//  * @param mode : file open mode flag - see hw125 driver 
//  */
// void mtbdl_read_sys_params(uint8_t mode); 

//==================================================


//==================================================
// RX state functions 

// /**
//  * @brief RX user interface start 
//  * 
//  * @details Passes a user prompt to the Bluetooth module which gets sent to an external 
//  *          device. Note that the Bluetooth module must be connected to a device that can 
//  *          display the sent messages for this to have an effect. The prompt tells the 
//  *          user that the system is ready to receive its input. 
//  */
// void mtbdl_rx_prep(void); 


// /**
//  * @brief Read user input 
//  * 
//  * @details Poles the Bluetooth module for new data, then once new data is available it's 
//  *          read and checked against valid commands. If a match is found then system 
//  *          settings/parameters are updated. If the user input does not match one of the 
//  *          available commands then nothing will change. This function should be called 
//  *          continuously to check for data and provide a new user prompt after data is 
//  *          input. 
//  */
// void mtbdl_rx(void); 

//==================================================


//==================================================
// TX state functions 

// /**
//  * @brief Check log file count 
//  * 
//  * @return uint8_t : log file index 
//  */
// uint8_t mtbdl_tx_check(void); 


// /**
//  * @brief Prepare to send a data log file 
//  * 
//  * @details If a log file exists that matches the most recent data log index then this 
//  *          function will open that file and return true. Otherwise it will return false. 
//  * 
//  * @return uint8_t : status of the file check 
//  */
// uint8_t mtbdl_tx_prep(void); 


// /**
//  * @brief Transfer data log contents 
//  * 
//  * @details Reads a single line from an open data log file and passes the info to the 
//  *          Bluetooth module for sending out to an external device. The end of the file 
//  *          is checked for after each line. Once the end of the file is seen the function 
//  *          will return false. This function does not loop so it needs to be repeatedly 
//  *          called. 
//  * 
//  *          Note that this function does not check for a valid open file. The TX prep 
//  *          function should be called before this function to make sure there is a file 
//  *          open before trying to read from the SD card. 
//  * 
//  * @see mtbdl_tx_prep 
//  * 
//  * @return uint8_t : end of file status 
//  */
// uint8_t mtbdl_tx(void); 


// /**
//  * @brief Close the log file and delete it 
//  * 
//  * @details Closes the open data log file, and if the transaction completed successfully 
//  *          then the log file will be deleted and the log index updated. Note that this 
//  *          function should only be called after 'mtbdl_tx' is done being called. 
//  */
// void mtbdl_tx_end(void); 

//==================================================


//==================================================
// Screen message formatting 

// /**
//  * @brief Format the idle state message 
//  * 
//  * @details The idle state message contains system values that are relevant to the user 
//  *          and these values can change, This function updates the values of the idle 
//  *          state message and triggers a write of this message to the screen. A list of 
//  *          the values that the message contains are listed in the parameters below. 
//  */
// void mtbdl_set_idle_msg(void); 


// /**
//  * @brief Format the run prep state message 
//  * 
//  * @details The run prep state message contains the GPS position lock status. This 
//  *          information is displayed to the user before entering the run mode and allows 
//  *          the user to know if they have GPS lock before beginning to record data. This 
//  *          function updates GPS status information and triggers a write of this message 
//  *          to the screen. 
//  */
// void mtbdl_set_run_prep_msg(void); 


// /**
//  * @brief Format the pre TX state message 
//  * 
//  * @details The pre TX state message contains the current log file index which tells the 
//  *          user how many log files are available to be sent. This function adds the log 
//  *          index to the message before writing it to the screen. 
//  */
// void mtbdl_set_pretx_msg(void); 

//==================================================


//==================================================
// Data checks and updates 

// /**
//  * @brief Update LED colours 
//  * 
//  * @details Assigns the specified colour to the LED and updates the LED output. This 
//  *          function only needs to update one LED at a time because LEDs in this system 
//  *          are only used as indicators for the user. 
//  * 
//  * @param led_index : LED to update 
//  * @param led_code : colour code that that sets the colour of the LED 
//  */
// void mtbdl_led_update(
//     ws2812_led_index_t led_index, 
//     uint32_t led_code); 

//==================================================

//=======================================================================================

#endif   // _DATA_LOGGING_H_ 
