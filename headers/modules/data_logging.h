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

//=======================================================================================


//=======================================================================================
// Macros 

#define MTBDL_ADC_BUFF_SIZE 3            // Size according to the number of ADCs used 

//=======================================================================================


//=======================================================================================
// Prototypes 

//==================================================
// Initialization 

/**
 * @brief Initialize data record 
 * 
 * @details Sets all the data handling info to its default value. This should be called 
 *          during the setup code. The arguments are saved into the data handling record 
 *          so they can be used where needed. 
 * 
 * @param rpm_irqn : wheel speed periodic interrupt index 
 * @param log_irqn : data sample periodic interrupt index 
 * @param adc : pointer to ADC port used 
 */
void mtbdl_data_init(
    IRQn_Type rpm_irqn, 
    IRQn_Type log_irqn, 
    ADC_TypeDef *adc); 


/**
 * @brief Bike parameter setup 
 * 
 * @details Creates directories on the SD card for storing system and bike parameters 
 *          as well data logs if the directories do not already exist. After establishing 
 *          directories, checks for existance of system and bike parameter files. If they 
 *          exist then they will be read and stored into the data handling record. If not 
 *          then they will be created and intialized to default values. 
 *          
 *          This function should be called during the setup code. 
 */
void mtbdl_file_sys_setup(void); 


/**
 * @brief ADC DMA setup 
 * 
 * @details Called during system setup to configure the DMA stream. This function is 
 *          called instead of doing the initialization directly in the setup so that 
 *          the ADC buffer, used to store ADC values and exists in the scope of the 
 *          data handling file, can be associated with the setup. 
 * 
 * @param dma_stream : pointer to DMA stream being used 
 * @param adc : pointer to ADC port used 
 */
void mtbdl_adc_dma_init(
    DMA_Stream_TypeDef *dma_stream, 
    ADC_TypeDef *adc); 

//==================================================


//==================================================
// Parameters 

/**
 * @brief Write bike parameters to file 
 * 
 * @details Writes the bike parameters currently stored in the data record to the bike 
 *          parameters file on the SD card. Saving this data allows for the system to 
 *          remember info after reboot. Bike parameters include the fork and shock 
 *          settings such as psi and rebound settings. These can be updated in the RX 
 *          mode. 
 * 
 * @param mode : file open mode flag - see hw125 driver 
 */
void mtbdl_write_bike_params(uint8_t mode); 


/**
 * @brief Read bike parameters on file 
 * 
 * @details Reads the bike parameters file on the SD card and stores the info in the data 
 *          record. This function is called on startup if a bike parameters file already 
 *          exists. This allows the system to use parameters previously saved before a 
 *          reboot. Bike parameters include the fork and shock settings such as psi and 
 *          rebound settings. 
 * 
 * @param mode : file open mode flag - see hw125 driver 
 */
void mtbdl_read_bike_params(uint8_t mode); 


/**
 * @brief Write system parameters to file 
 * 
 * @details Writes the system parameters currently stored in the data record to the 
 *          system parameters file on the SD card. Saving this data allows for the system 
 *          to remember info after reboot. System parameters include the IMU and 
 *          potentiometer calibration data and the log index. These can be updated after 
 *          calibration, data logging or TX mode. 
 * 
 * @param mode : file open mode flag - see hw125 driver 
 */
void mtbdl_write_sys_params(uint8_t mode); 


/**
 * @brief Read system parameters on file 
 * 
 * @details Reads the system parameters file on the SD card and stores the info in the 
 *          data record. This function is called on startup if a system parameters file 
 *          already exists. This allows the system to use parameters previously saved 
 *          before a reboot. System parameters include the IMU and potentiometer 
 *          calibration data and the log index. 
 * 
 * @param mode : file open mode flag - see hw125 driver 
 */
void mtbdl_read_sys_params(uint8_t mode); 

//==================================================


//==================================================
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
uint8_t mtbdl_log_name_prep(void); 


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
 * @see mtbdl_log_name_prep 
 */
void mtbdl_log_file_prep(void); 


/**
 * @brief Log data prep 
 * 
 * @details Resets data logging info and enables interrupts, all of which are needed 
 *          before beginning to log data correctly. Without a call to this function, no 
 *          data will be logged in the logging function. 
 */
void mtbdl_log_data_prep(void); 


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
 * @see mtbdl_log_name_prep 
 * @see mtbdl_log_file_prep 
 * @see mtbdl_log_data_prep 
 */
void mtbdl_logging(void); 


/**
 * @brief End the data logging 
 * 
 * @details Disables interrupts, saves and closes the log file, and updates the log index. 
 *          This function must be called after the logging function in order to finish 
 *          the logging process. 
 */
void mtbdl_log_end(void); 

//==================================================


//==================================================
// RX state functions 

/**
 * @brief RX user interface start 
 * 
 * @details Passes a user prompt to the Bluetooth module which gets sent to an external 
 *          device. Note that the Bluetooth module must be connected to a device that can 
 *          display the sent messages for this to have an effect. The prompt tells the 
 *          user that the system is ready to receive its input. 
 */
void mtbdl_rx_prep(void); 


/**
 * @brief Read user input 
 * 
 * @details Poles the Bluetooth module for new data, then once new data is available it's 
 *          read and checked against valid commands. If a match is found then system 
 *          settings/parameters are updated. If the user input does not match one of the 
 *          available commands then nothing will change. This function should be called 
 *          continuously to check for data and provide a new user prompt after data is 
 *          input. 
 */
void mtbdl_rx(void); 

//==================================================


//==================================================
// TX state functions 

/**
 * @brief Check log file count 
 * 
 * @return uint8_t : log file index 
 */
uint8_t mtbdl_tx_check(void); 


/**
 * @brief Prepare to send a data log file 
 * 
 * @details If a log file exists that matches the most recent data log index then this 
 *          function will open that file and return true. Otherwise it will return false. 
 * 
 * @return uint8_t : status of the file check 
 */
uint8_t mtbdl_tx_prep(void); 


/**
 * @brief Transfer data log contents 
 * 
 * @details Reads a single line from an open data log file and passes the info to the 
 *          Bluetooth module for sending out to an external device. The end of the file 
 *          is checked for after each line. Once the end of the file is seen the function 
 *          will return false. This function does not loop so it needs to be repeatedly 
 *          called. 
 * 
 *          Note that this function does not check for a valid open file. The TX prep 
 *          function should be called before this function to make sure there is a file 
 *          open before trying to read from the SD card. 
 * 
 * @see mtbdl_tx_prep 
 * 
 * @return uint8_t : end of file status 
 */
uint8_t mtbdl_tx(void); 


/**
 * @brief Close the log file and delete it 
 * 
 * @details Closes the open data log file, and if the transaction completed successfully 
 *          then the log file will be deleted and the log index updated. Note that this 
 *          function should only be called after 'mtbdl_tx' is done being called. 
 */
void mtbdl_tx_end(void); 

//==================================================


//==================================================
// Calibration functions 

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
 * @see mtbdl_calibrate 
 */
void mtbdl_cal_prep(void); 


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
 * @see mtbdl_cal_prep 
 * @see mtbdl_cal_calc 
 */
void mtbdl_calibrate(void); 


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
 * @see mtbdl_calibrate 
 */
void mtbdl_cal_calc(void); 

//==================================================


//==================================================
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
 * @details The pre TX state message contains the current log file index which tells the 
 *          user how many log files are available to be sent. This function adds the log 
 *          index to the message before writing it to the screen. 
 */
void mtbdl_set_pretx_msg(void); 

//==================================================


//==================================================
// Data checks and updates 

/**
 * @brief Update LED colours 
 * 
 * @details Assigns the specified colour to the LED and updates the LED output. This 
 *          function only needs to update one LED at a time because LEDs in this system 
 *          are only used as indicators for the user. 
 * 
 * @param led_index : LED to update 
 * @param led_code : colour code that that sets the colour of the LED 
 */
void mtbdl_led_update(
    ws2812_led_index_t led_index, 
    uint32_t led_code); 


/**
 * @brief Set trail marker flag 
 * 
 * @details Used by the run state while data logging to set the trail marker flag when 
 *          the marker button is pushed. The trail marker gets written to the log file 
 *          so the used can identify points within the log. 
 */
void mtbdl_set_trailmark(void); 

//==================================================

//=======================================================================================

#endif   // _DATA_LOGGING_H_ 
