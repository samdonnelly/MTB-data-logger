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

#define MTBDL_MAX_DATA_STR_LEN 60        // Max string length containing data 
#define MTBDL_DATA_INDEX_OFFSET 1        // Log file number offset for the TX state 
#define MTBDL_MAX_SUS_SETTING 20         // Max compression and rebound setting 

// Calibration 
#define MTBDL_NUM_CAL_DATA 5             // Number of parameters that require calibration 

// Data logging 
#define MTBDL_LOG_NUM_MAX 250            // Max data log file number 
#define MTBDL_LOG_NUM_MIN 0              // Min data log files 
#define MTBDL_LOG_PERIOD 10              // (ms) Period between data samples 
#define MTBDL_NUM_LOG_STREAMS 6          // Number of data logging streams 
#define MTBDL_NUM_LOG_SEQ 25             // Number of data logging sequence steps 
#define MTBDL_LOG_COUNT_CYCLE 99         // Log sample sequence max timer counter value 
#define MTBDL_ADC_BUFF_SIZE 3            // Size according to the number of ADCs used 

// Wheel RPM info 
#define MTBDL_REV_LOG_FREQ 2             // (Hz) Revolution calc frequency 
#define MTBDL_REV_SAMPLE_SIZE 4          // Number of samples for revolution calc 

// Debugging 
#define MTBDL_DEBUG_SAMPLE_COUNT 999     // Number of data samples to take in debug mode 

//=======================================================================================


//=======================================================================================
// Enums 

/**
 * @brief User parameter index --> for RX state 
 */
typedef enum {
    MTBDL_PARM_FPSI,                     // Fork PSI 
    MTBDL_PARM_FC,                       // Fork compression setting 
    MTBDL_PARM_FR,                       // Fork rebound setting 
    MTBDL_PARM_SPSI,                     // Shock SPI 
    MTBDL_PARM_SL,                       // Shock lockout setting 
    MTBDL_PARM_SR                        // Shock rebound setting 
} mtbdl_rx_param_index_t; 


/**
 * @brief Logging streams 
 */
typedef enum {
    MTBDL_LOG_STREAM_STANDARD,           // Standard stream 
    MTBDL_LOG_STREAM_BLINK,              // LED blink stream 
    MTBDL_LOG_STREAM_SPEED,              // Wheel speed stream 
    MTBDL_LOG_STREAM_ACCEL,              // Accelerometer stream 
    MTBDL_LOG_STREAM_GPS,                // GPS stream 
    MTBDL_LOG_STREAM_USER                // User input stream 
} mtbdl_log_streams_t; 


/**
 * @brief ADC buffer index 
 */
typedef enum {
    MTBDL_ADC_SOC, 
    MTBDL_ADC_FORK, 
    MTBDL_ADC_SHOCK 
} mtbdl_adc_buff_index_t; 


/**
 * @brief Calibration data index 
 */
typedef enum {
    MTBDL_CAL_ACCEL_X, 
    MTBDL_CAL_ACCEL_Y, 
    MTBDL_CAL_ACCEL_Z, 
    MTBDL_CAL_POT_FORK, 
    MTBDL_CAL_POT_SHOCK 
} mtbdl_cal_index_t; 

//=======================================================================================


//=======================================================================================
// Structures 

// Data record for the system 
typedef struct mtbdl_data_s 
{
    // Peripherals 
    IRQn_Type rpm_irq;                          // Wheel RPM interrupt number 
    IRQn_Type log_irq;                          // Log sample period interrupt number 
    ADC_TypeDef *adc;                            // ADC port battery soc and pots 

    // Bike parameters 
    uint8_t fork_psi;                           // Fork pressure (psi) 
    uint8_t fork_comp;                          // Fork compression setting 
    uint8_t fork_reb;                           // Fork rebound setting 
    uint8_t shock_psi;                          // Shock pressure (psi) 
    uint8_t shock_lock;                         // Shock lockout setting 
    uint8_t shock_reb;                          // Shock rebound setting 

    // System parameters 
    uint8_t log_index;                          // Data log index 
    int16_t accel_x_rest;                       // Resting x-axis acceleration offset 
    int16_t accel_y_rest;                       // Resting y-axis acceleration offset 
    int16_t accel_z_rest;                       // Resting z-axis acceleration offset 
    uint16_t pot_fork_rest;                     // Resting potentiometer reading for fork 
    uint16_t pot_shock_rest;                    // Resting potentiometer reading for shock 

    // System data 
    uint8_t soc;                                // Battery SOC 
    uint16_t adc_buff[MTBDL_ADC_BUFF_SIZE];     // ADC buffer - SOC, fork pot, shock pot 
    uint16_t navstat;                           // Navigation status of GPS module 
    uint8_t utc_time[M8Q_TIME_CHAR_LEN+1];      // UTC time recorded by the GPS module 
    uint8_t utc_date[M8Q_DATE_CHAR_LEN+1];      // UTC date recorded by the GPS module 
    uint8_t deg_min_lat[M8Q_COO_LEN];           // Latitude: degrees and minutes integer part 
    uint8_t min_frac_lat[M8Q_COO_LEN];          // Latitude: minuutes fractional part 
    uint8_t NS;                                 // North/South indicator of latitude 
    uint8_t deg_min_lon[M8Q_COO_LEN];           // Longitude: degrees and minutes integer part 
    uint8_t min_frac_lon[M8Q_COO_LEN];          // Longitude: minuutes fractional part 
    uint8_t EW;                                 // Eeast/West indicator of longitude 
    int16_t accel_x;                            // x-axis acceleration reading 
    int16_t accel_y;                            // y-axis acceleration reading 
    int16_t accel_z;                            // z-axis acceleration reading 

    // Calibration data 
    int32_t cal_buff[MTBDL_NUM_CAL_DATA];       // Buffer that holds calibration data 
    int32_t cal_index;                          // Calibration sample index 

    // LED colour data - Green bits: 16-23, Red bits: 8-15, Blue bits: 0-7 
    uint32_t led_colour_data[WS2812_LED_NUM]; 

    // Wheel RPM info 
    uint8_t rev_count;                          // Wheel revolution counter 
    uint8_t rev_buff_index;                     // Wheel revolution circular buffer index 
    uint8_t rev_buff[MTBDL_REV_SAMPLE_SIZE];    // Circular buffer for revolution calcs 

    // SD card 
    char data_buff[MTBDL_MAX_DATA_STR_LEN];     // Buffer for reading and writing 
    char filename[MTBDL_MAX_DATA_STR_LEN];      // Buffer for storing a file name 
    uint8_t tx_status : 1;                      // TX transaction status 

    // Log tracking 
    uint32_t time_count;                        // Time tracking counter for logging 
    uint8_t stream_index;                       // Index for log stream sequencing 
    uint8_t led_toggle : 1;                     // LED toggle bit 
    uint8_t run_count  : 1;                     // Log stream toggle 
    uint8_t trailmark  : 1;                     // Trail marker flag 
    uint8_t user_input : 1;                     // User input flag 
    uint8_t log_stream : 3;                     // Logging stream number (mtbdl_log_streams_t)

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
mtbdl_data_t; 


// 
typedef struct mtbdl_log_stream_state_s 
{
    uint8_t counter; 
    mtbdl_log_streams_t stream; 
}
mtbdl_log_stream_state_t; 

//=======================================================================================


//=======================================================================================
// Function pointers 

/**
 * @brief Logging state machine function pointer 
 */
typedef void (*mtbdl_log_stream)(void); 

//=======================================================================================


//=======================================================================================
// Initialization 

/**
 * @brief Initialize data record 
 * 
 * @details 
 * 
 * @param rpm_irqn : 
 * @param log_irqn : 
 * @param adc : 
 */
void mtbdl_data_init(
    IRQn_Type rpm_irqn, 
    IRQn_Type log_irqn, 
    ADC_TypeDef *adc); 


/**
 * @brief Bike parameter setup 
 * 
 * @details 
 */
void mtbdl_file_sys_setup(void); 


/**
 * @brief ADC DMA setup 
 * 
 * @details 
 * 
 * @param dma_strea 
 * @param adc 
 */
void mtbdl_adc_dma_init(
    DMA_Stream_TypeDef *dma_strea, 
    ADC_TypeDef *adc); 

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

//=======================================================================================


//=======================================================================================
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

//=======================================================================================


//=======================================================================================
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

//=======================================================================================


//=======================================================================================
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


//=======================================================================================
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


/**
 * @brief Update the navigation status 
 * 
 * @details 
 * 
 * @return uint8_t 
 */
uint8_t mtbdl_navstat_check(void); 

//=======================================================================================

#endif   // _DATA_HANDLING_H_ 
