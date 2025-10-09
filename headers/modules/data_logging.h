/**
 * @file data_logging.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Data logging module interface 
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

// Data logging sequence/timing 
#define LOG_PERIOD 10                    // (ms) Period between data samples 
#define LOG_PERIOD_DIVIDER 5             // LOG_PERIOD * this == non-ADC log stream period 
#define LOG_GPS_OFFSET 0                 // GPS stream starting log offset 
#define LOG_GPS_PERIOD 20                // GPS stream counter period 
#define LOG_ACCEL_OFFSET 1               // ACCEL stream starting log offset 
#define LOG_ACCEL_PERIOD 2               // ACCEL stream counter period 
#define LOG_SPEED_OFFSET 2               // SPEED stream starting log offset 
#define LOG_SPEED_PERIOD 4               // SPEED stream counter period 

// Buffer sizes 
#define LOG_GPS_BUFF_LEN 12              // GPS coordinate buffer size 
#define LOG_TIME_BUFF_LEN 10             // UTC time and data buff size 
#define LOG_MAX_LOG_LEN (LOG_PERIOD_DIVIDER*MTBDL_MAX_STR_LEN) 

// Wheel RPM info 
#define LOG_REV_SAMPLE_SIZE 20           // Number of samples for revolution calc 

//=======================================================================================


//=======================================================================================
// Enums 

// ADC buffer index 
typedef enum {
    ADC_SOC,        // Battery voltage 
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
    ADC_TypeDef *adc;                           // ADC port for battery soc and pots 
    DMA_TypeDef *dma;                           // DMA port for ADC transfers 
    DMA_Stream_TypeDef *dma_stream;             // DMA stream for ADC transfers 

    // Log file info 
    uint8_t utc_time[LOG_TIME_BUFF_LEN];        // UTC time 
    uint8_t utc_date[LOG_TIME_BUFF_LEN];        // UTC date 

    // ADC data - buffers to store current and pervious values - SOC, fork pot, shock pot 
    uint16_t adc_buff[ADC_BUFF_SIZE]; 
    uint16_t adc_period[LOG_PERIOD_DIVIDER][ADC_BUFF_SIZE]; 

    // GPS data 
    uint8_t lat_str[LOG_GPS_BUFF_LEN];          // Latitude string 
    uint8_t NS;                                 // North/South indicator of latitude 
    uint8_t lon_str[LOG_GPS_BUFF_LEN];          // Longitude string 
    uint8_t EW;                                 // East/West indicator of longitude 
    uint8_t sog_str[LOG_GPS_BUFF_LEN];          // Speed over ground string 

    // Accelerometer data 
    int16_t accel[NUM_AXES]; 

    // Wheel revolution data 
    uint8_t rev_count;                          // Wheel revolution counter 
    uint8_t rev_buff_index;                     // Wheel revolution circular buffer index 
    uint8_t rev_buff[LOG_REV_SAMPLE_SIZE];      // Wheel revolution circular buffer

    // User input data 
    uint8_t trailmark;                          // Trail marker flag 

    // Logging counters 
    uint8_t log_interval_divider;               // Controls timing of non-ADC streams 
    uint8_t gps_stream_counter;                 // GPS log stream counter 
    uint8_t accel_stream_counter;               // Accelerometer log stream counter 
    uint8_t speed_stream_counter;               // Wheel speed log stream counter 
    uint8_t interrupt_counter;                  // Counts interrupts called 

    // Calibration data 
    int32_t cal_buff[PARAM_SYS_SET_NUM];        // Calibration data buffer 
    int32_t cal_adc_samples;                    // Number of ADC calibration samples 
    int32_t cal_accel_samples;                  // Number of accelerometer calibration samples 

    // SD card data - buffers to hold interval data, log data string and log file name 
    char data_buff[LOG_PERIOD_DIVIDER][MTBDL_MAX_STR_LEN]; 
    char data_str[LOG_MAX_LOG_LEN]; 
    uint8_t data_buff_index; 
    char filename[MTBDL_MAX_STR_LEN]; 

    // Debugging / log checking 
    uint8_t overrun;                            // Checks if data has been skipped 
}
mtbdl_log_t; 

//=======================================================================================


//=======================================================================================
// Initialization 

/**
 * @brief Initialize data logging module 
 * 
 * @details Sets all the data handling info to its default value and configures the DMA 
 *          stream. DMA stream configuration is done here instead of in the setup file so 
 *          the buffer used to store ADC values (for suspension position and battery SOC) 
 *          is within scope to set as the DMA memory address. 
 * 
 * @param rpm_irqn : wheel speed periodic interrupt index 
 * @param log_irqn : data sample periodic interrupt index 
 * @param adc : ADC port used 
 * @param dma : DMA port to use 
 * @param dma_stream : DMA stream being used 
 */
void log_init(
    IRQn_Type rpm_irqn, 
    IRQn_Type log_irqn, 
    ADC_TypeDef *adc, 
    DMA_TypeDef *dma, 
    DMA_Stream_TypeDef *dma_stream); 

//=======================================================================================


//=======================================================================================
// Data logging 

/**
 * @brief Log name preparation 
 * 
 * @details Checks if there is room to create a new log file, and if so, generates a new 
 *          log file name. If the number of log files is at its capacity then the function 
 *          will return false and a new name will not be generated. 
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
 *          data will be logged in the logging function. A new file should be prepared 
 *          before calling this function. 
 * 
 * @see log_data_name_prep 
 * @see log_data_file_prep 
 */
void log_data_prep(void); 


/**
 * @brief Logging data 
 * 
 * @details Logs data at fixed intervals to the created log file. This function must be 
 *          called continuously while in data logging mode in order for all data to be 
 *          captured. 
 *          
 *          A periodic interrupt will call the log_data_adc_handler function which 
 *          records ADC data and increments an interrupt counter. This function looks for 
 *          the interrupt counter to be greater than 0. If this is true then it writes 
 *          data from the previous interval to the SD card. Data that can be written 
 *          includes ADC data (suspension position), GPS location, IMU orientation, wheel 
 *          speed and user input/flags. ADC data gets recorded each interval, while GPS, 
 *          IMU and speed data gets recorded at a slower frequency but on a fixed 
 *          schedule. The schedule for this data is defined in such a way that only 
 *          one of GPS, IMU or speed data will get read and recorded during an interval. 
 *          This is done because the interval is short and too much data handling could 
 *          lead to a loss of data. 
 *          
 *          The perodic interrupt triggers every 10ms. 
 *          
 *          This function will also increment a revolution counter triggered by an 
 *          external interrupt which is used to help with the wheel speed calculation. 
 *          
 *          Before calling this function, the log file and data need to be prepared using 
 *          the prep functions above. 
 * 
 * @see log_data_adc_handler 
 * @see log_data_name_prep 
 * @see log_data_file_prep 
 * @see log_data_prep 
 */
void log_data(void); 


/**
 * @brief Data logging interrupt callback 
 * 
 * @details When in data logging mode, a periodic interrupt is used to keep track of 
 *          when to record data. The interrupt handler calls this function. In this 
 *          function an interrupt counter is incremented and ADC data from the previous 
 *          interval recorded. The counter is used to trigger a data recording in the 
 *          log file when the log_data function is called. Before exiting, this function 
 *          will start the next ADC conversion which will be recored the next time the 
 *          interrupt is triggered. 
 * 
 * @see log_data 
 */
void log_data_adc_handler(void); 


/**
 * @brief End data logging 
 * 
 * @details Disables interrupts and if a log file is currently open then saves and closes 
 *          the file and updates the log file index. This function must be called once 
 *          data logging is over. 
 */
void log_data_end(void); 

//=======================================================================================


//=======================================================================================
// Calibration 

/**
 * @brief Calibration data prep 
 * 
 * @details Resets the data used to calibrate the system and enables data sampling 
 *          interrupts. This must be called before using the calibration function. 
 *          
 *          Calibration requires finding the resting values/offsets of the bike IMU and 
 *          suspension potentiometers. These offsets are used "zero" the readings. 
 * 
 * @see log_calibration 
 */
void log_calibration_prep(void); 


/**
 * @brief Calibration 
 * 
 * @details Records IMU and suspension ADC values periodically. All the read values for 
 *          a particular parameter get summed and the number of samples recorded is 
 *          kept track of. This data is then used by the log_calibration_calculation 
 *          function to calculate the calibration value. The result of this operation 
 *          is determining a value that will allow all IMU and ADC data to be "zeroed". 
 *          
 *          This function must be called continuously during calibration mode so an 
 *          appropriate amount of sample can be recorded. Note that the amount of time 
 *          spent during calibration should be considered so that the sum of all 
 *          parameter data does not exceed the maximum data size of the sum (signed 
 *          32-bit). 
 *          
 *          Calibration runs for 5 seconds. 
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
 *          calibration function is called first. After new values are found, they get 
 *          written to the system parameters. The end result of calibration is having 
 *          values that will allow all IMU and ADC data to be "zeroed". 
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
 * @details Sets the trail marker flag which gets recored in each interval while data 
 *          logging. This function should be called when the user presses the trail 
 *          marker button while in data logging mode. Calling this function outside of 
 *          data logging mode will have no affect. A trail marker is used to help the 
 *          user identify points of interest during a data log. 
 */
void log_set_trailmark(void); 

//=======================================================================================


//=======================================================================================
// Getters 

/**
 * @brief Get battery voltage (ADC value) 
 * 
 * @details This function is used to fetch the current battery voltage so that the 
 *          battery SOC can be calculated. Battery SOC is calculated in the UI module. 
 *          While data logging, the ADC gets read constantly so there is no need to 
 *          manually updated the battery voltage reading. However, when not in data 
 *          logging mode, this function will trigger an ADC read to get the latest 
 *          battery voltage data.
 *          
 *          Note that the value returned will depend on both the ADC resolution set and 
 *          the voltage range of the battery. The SOC calculation should account for 
 *          this. 
 * 
 * @return uint16_t : battery ADC that represents voltage 
 */
uint16_t log_get_batt_voltage(void); 

//=======================================================================================

#endif   // _DATA_LOGGING_H_ 
