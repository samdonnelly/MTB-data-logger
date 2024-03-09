/**
 * @file data_handling.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL data handling interface 
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

//==================================================


//==================================================
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
 * @brief Log data prep 
 * 
 * @details 
 */
void mtbdl_log_data_prep(void); 


/**
 * @brief Logging data 
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

//==================================================


//==================================================
// RX state functions 

/**
 * @brief RX user interface preparation 
 * 
 * @details 
 */
void mtbdl_rx_prep(void); 


/**
 * @brief Read and assign the user input 
 * 
 * @details 
 */
void mtbdl_rx(void); 

//==================================================


//==================================================
// TX state functions 

/**
 * @brief Check if there are no log files 
 * 
 * @details 
 * 
 * @return uint8_t 
 */
uint8_t mtbdl_tx_check(void); 


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

//==================================================


//==================================================
// Calibration functions 

/**
 * @brief Calibration data prep 
 * 
 * @details 
 */
void mtbdl_cal_prep(void); 


/**
 * @brief Calibration 
 * 
 * @details 
 */
void mtbdl_calibrate(void); 


/**
 * @brief Calibration calculation 
 * 
 * @details 
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
 * @details 
 */
void mtbdl_set_pretx_msg(void); 

//==================================================


//==================================================
// LEDs 

/**
 * @brief Update LED colours 
 * 
 * @details 
 * 
 * @param led_index 
 * @param led_code 
 */
void mtbdl_led_update(
    ws2812_led_index_t led_index, 
    uint32_t led_code); 


/**
 * @brief Set trail marker flag 
 * 
 * @details 
 */
void mtbdl_set_trailmark(void); 

//==================================================

//=======================================================================================

#endif   // _DATA_HANDLING_H_ 
