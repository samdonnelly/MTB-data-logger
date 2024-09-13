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

// Data logging 
#define LOG_MAX_FILES 250                // Max data log file number 
#define LOG_GPS_BUFF_LEN 12              // GPS coordinate buffer size 
#define LOG_TIME_BUFF_LEN 10             // UTC time and data buff size 

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

    // Calibration data 
    int32_t cal_buff[PARAM_SYS_SET_NUM];        // Buffer that holds calibration data 
    int32_t cal_samples;                        // Calibration sample index 

    // Wheel RPM info 
    uint8_t rev_count;                          // Wheel revolution counter 
    uint8_t rev_buff_index;                     // Wheel revolution circular buffer index 
    uint8_t rev_buff[MTBDL_REV_SAMPLE_SIZE];    // Circular buffer for revolution calcs 

    // SD card 
    char data_buff[MTBDL_MAX_STR_LEN];          // Buffer for reading and writing 
    char filename[MTBDL_MAX_STR_LEN];           // Buffer for storing a file name 

    // Stream counters 
    uint8_t gps_stream_counter; 
    uint8_t accel_stream_counter; 
    uint8_t speed_stream_counter; 

// #if MTBDL_DEBUG 
//     // Testing 
//     uint16_t time_stop; 
//     uint16_t time_limit; 
//     uint16_t count_standard; 
//     uint16_t count_wait; 
//     uint8_t time_overflow; 
//     uint8_t count_blink; 
//     uint8_t count_speed; 
//     uint8_t count_accel; 
//     uint8_t count_gps; 
//     uint8_t count_user; 
//     uint16_t adc_count; 
// #endif   // MTBDL_DEBUG 
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

#endif   // _DATA_LOGGING_H_ 
