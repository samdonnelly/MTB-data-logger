/**
 * @file data_logging.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB data logging 
 * 
 * @version 0.1
 * @date 2023-05-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//=======================================================================================
// Includes 

#include "includes_app.h" 

//=======================================================================================


//=======================================================================================
// Enums 

// /**
//  * @brief User parameter index --> for RX state 
//  */
// typedef enum {
//     MTBDL_PARM_FPSI,   // Fork PSI 
//     MTBDL_PARM_FC,     // Fork compression setting 
//     MTBDL_PARM_FR,     // Fork rebound setting 
//     MTBDL_PARM_SPSI,   // Shock SPI 
//     MTBDL_PARM_SL,     // Shock lockout setting 
//     MTBDL_PARM_SR      // Shock rebound setting 
// } mtbdl_rx_param_index_t; 


/**
 * @brief Logging streams 
 */
typedef enum {
    MTBDL_LOG_STREAM_STANDARD,   // Standard stream 
    MTBDL_LOG_STREAM_BLINK,      // LED blink stream 
    MTBDL_LOG_STREAM_SPEED,      // Wheel speed stream 
    MTBDL_LOG_STREAM_ACCEL,      // Accelerometer stream 
    MTBDL_LOG_STREAM_GPS,        // GPS stream 
    MTBDL_LOG_STREAM_USER        // User input stream 
} mtbdl_log_streams_t; 


/**
 * @brief ADC buffer index 
 */
typedef enum {
    MTBDL_ADC_SOC,    // Battery State of Charge (SOC) 
    MTBDL_ADC_FORK,   // Fork potentiometer 
    MTBDL_ADC_SHOCK   // Shock potentiometer 
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

// Data record instance 
static mtbdl_data_t mtbdl_data; 


// Logging schedule data 
typedef struct mtbdl_log_stream_state_s 
{
    uint8_t counter; 
    mtbdl_log_streams_t stream; 
}
mtbdl_log_stream_state_t; 

//=======================================================================================


//=======================================================================================
// Prototypes 

/**
 * @brief Logging state machine function pointer 
 */
typedef void (*mtbdl_log_stream)(void); 


/**
 * @brief Data logging stream: standard data only 
 * 
 * @details This function is used during data logging only to record standard data for 
 *          the current logging interval. Standard data is data that gets recorded every 
 *          logging interval and it includes trail markers and suspension position 
 *          measurements. This is the default logging stream for when no other stream is 
 *          scheduled. See the 'stream_schedule' table for the data stream sequence. 
 */
void mtbdl_log_stream_standard(void); 


/**
 * @brief Data logging stream: user LED feedback 
 * 
 * @details This function is used during data logging to record standard data for the 
 *          current logging interval and toggle the data logging mode LED. The LED is 
 *          used as visual feedback for the user to help identify the systems state. 
 *          This stream is used periodically to create a blinking effect for the LED. 
 *          See the 'stream_schedule' table for the data stream sequence. 
 * 
 * @see mtbdl_log_stream_standard 
 */
void mtbdl_log_stream_blink(void); 


/**
 * @brief Data logging stream: wheel speed 
 * 
 * @details This function is used during data logging to record standard data for the 
 *          current logging interval and estimated wheel speed. Wheel speed is determined 
 *          by recording the number of hall effect sensor interrupts over a period of time 
 *          which means this stream has to look at previous sensor data. See the 
 *          'stream_schedule' table for the data stream sequence. 
 */
void mtbdl_log_stream_speed(void); 


/**
 * @brief Data logging stream: acceleration 
 * 
 * @details This function is used during data logging to record standard data and IMU 
 *          data for the current logging interval. This stream is executed across two 
 *          intervals, twice in a row. The first pass will trigger a read from the device 
 *          and the second pass will take the data and write it to the log file. This is 
 *          done to help prevent the interval time from being exceeded. See the 
 *          'stream_schedule' table for the data stream sequence. 
 */
void mtbdl_log_stream_accel(void); 


/**
 * @brief Data logging stream: GPS location 
 * 
 * @details This function is used during data logging to record standard data and GPS 
 *          position for the current logging interval. The GPS device is only read from 
 *          during intervals where no other data besides standard data is being read 
 *          and recorded. That way an interval will not exceed its max time and this 
 *          stream can be only for recording the most recent GPS position. See the 
 *          'stream_schedule' table for the data stream sequence. 
 */
void mtbdl_log_stream_gps(void); 


/**
 * @brief Data logging stream: user input feedback LEDs 
 * 
 * @details This function is used during data logging to record standard data for the 
 *          current logging interval and light up the trail marker LED. The trail marker 
 *          LED lights up when the user presses the trail marker button so the user can 
 *          see that the input was recorded. When this happens, a 1 will be recorded for 
 *          the trail marker so points of interest in the data log can be marked/noted 
 *          See the 'stream_schedule' table for the data stream sequence. 
 */
void mtbdl_log_stream_user(void); 

//=======================================================================================


//=======================================================================================
// Variables 

// Log stream table 
static mtbdl_log_stream stream_table[MTBDL_NUM_LOG_STREAMS] = 
{
    &mtbdl_log_stream_standard, 
    &mtbdl_log_stream_blink, 
    &mtbdl_log_stream_speed, 
    &mtbdl_log_stream_accel, 
    &mtbdl_log_stream_gps, 
    &mtbdl_log_stream_user 
}; 


// Log sequence table 
static const mtbdl_log_stream_state_t stream_schedule[MTBDL_NUM_LOG_SEQ] = 
{
    // {count value that triggers log stream, stream to go to on trigger} 
    {0,    MTBDL_LOG_STREAM_BLINK},   // Step 0 
    {6,    MTBDL_LOG_STREAM_ACCEL},   // Step 1 
    {7,    MTBDL_LOG_STREAM_ACCEL},   // Step 2 
    {10,   MTBDL_LOG_STREAM_BLINK},   // Step 3 
    {14,   MTBDL_LOG_STREAM_SPEED},   // Step 4 
    {16,   MTBDL_LOG_STREAM_ACCEL},   // Step 5 
    {17,   MTBDL_LOG_STREAM_ACCEL},   // Step 6 
    {20,   MTBDL_LOG_STREAM_GPS},     // Step 7 
    {26,   MTBDL_LOG_STREAM_ACCEL},   // Step 8 
    {27,   MTBDL_LOG_STREAM_ACCEL},   // Step 9 
    {36,   MTBDL_LOG_STREAM_ACCEL},   // Step 10 
    {37,   MTBDL_LOG_STREAM_ACCEL},   // Step 11 
    {46,   MTBDL_LOG_STREAM_ACCEL},   // Step 12 
    {47,   MTBDL_LOG_STREAM_ACCEL},   // Step 13 
    {56,   MTBDL_LOG_STREAM_ACCEL},   // Step 14 
    {57,   MTBDL_LOG_STREAM_ACCEL},   // Step 15 
    {64,   MTBDL_LOG_STREAM_SPEED},   // Step 16 
    {66,   MTBDL_LOG_STREAM_ACCEL},   // Step 17 
    {67,   MTBDL_LOG_STREAM_ACCEL},   // Step 18 
    {76,   MTBDL_LOG_STREAM_ACCEL},   // Step 29 
    {77,   MTBDL_LOG_STREAM_ACCEL},   // Step 20 
    {86,   MTBDL_LOG_STREAM_ACCEL},   // Step 21 
    {87,   MTBDL_LOG_STREAM_ACCEL},   // Step 22 
    {96,   MTBDL_LOG_STREAM_ACCEL},   // Step 23 
    {97,   MTBDL_LOG_STREAM_ACCEL}    // Step 24 
}; 

//=======================================================================================


//=======================================================================================
// Initialization 

// Initialize data record 
void data_log_init(
    IRQn_Type rpm_irqn, 
    IRQn_Type log_irqn, 
    ADC_TypeDef *adc, 
    DMA_Stream_TypeDef *dma_stream)
{
    // Reset the whole data record 
    memset((void *)&mtbdl_data, CLEAR, sizeof(mtbdl_data_t)); 

    // Peripherals 
    mtbdl_data.rpm_irq = rpm_irqn; 
    mtbdl_data.log_irq = log_irqn; 
    mtbdl_data.adc = adc; 

    // System data 
    // mtbdl_data.navstat = M8Q_NAVSTAT_NF; 

    // Configure the DMA stream 
    dma_stream_config(
        dma_stream, 
        (uint32_t)(&adc->DR), 
        (uint32_t)mtbdl_data.adc_buff, 
        (uint16_t)MTBDL_ADC_BUFF_SIZE); 
}

//=======================================================================================


//=======================================================================================
// Data logging 

// Log name preparation 
uint8_t mtbdl_log_name_prep(void)
{
    uint8_t log_index = param_get_log_index(); 

    // Check the data log index is within bounds 
    // if (mtbdl_data.log_index > MTBDL_LOG_NUM_MAX)
    if (log_index > MTBDL_LOG_NUM_MAX)
    {
        // Too many log files on drive - don't create a new file name 
        return FALSE; 
    }

    // Number of log files is within the limit - generate a new log file name 
    snprintf(mtbdl_data.filename, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_log_file, 
             log_index); 
             // mtbdl_data.log_index); 

    return TRUE; 
}


// Log file prep 
void mtbdl_log_file_prep(void)
{
    // The code moves to the directory that stores the data log files and attempts to create 
    // and open the next indexed log file. If this is successful then information and data 
    // will be written to the file. If unsuccessful then the hw125 controller will record a 
    // fault and the system will enter the fault state instead of proceeding to the data 
    // logging state. 
    
    hw125_set_dir(mtbdl_data_dir); 

    if (hw125_open(mtbdl_data.filename, HW125_MODE_WWX) == FR_OK)
    {
        // File successfully created 
        // Write the bike and system parameters, file creation time stamp (UTC format) and 
        // logging info (sample period and RPM calculation info) to the head of the log file 
        // so the user can associate the information to the logged data. 
        // For readability and easier post processing of the collected data (not done within 
        // this system), a line is written to indicate the start of the data logging 
        // information. The data log index is then incremented which allows the code to keep 
        // track of the number of log files that have been created. 
        
        // Bike and system parameters 
        // mtbdl_format_write_bike_params(); 
        // mtbdl_format_write_sys_params(); 
        param_bike_format_write(); 
        param_sys_format_write(); 

        // UTC time stamp 
        m8q_get_time_utc_time(mtbdl_data.utc_time, MTBDL_TIME_BUFF_LEN); 
        m8q_get_time_utc_date(mtbdl_data.utc_date, MTBDL_DATE_BUFF_LEN); 
        snprintf(
            mtbdl_data.data_buff, 
            MTBDL_MAX_STR_LEN, 
            mtbdl_param_time, 
            (char *)mtbdl_data.utc_time, 
            (char *)mtbdl_data.utc_date); 
        hw125_puts(mtbdl_data.data_buff); 

        // Logging info 
        snprintf(
            mtbdl_data.data_buff, 
            MTBDL_MAX_STR_LEN, 
            mtbdl_param_data, 
            MTBDL_LOG_PERIOD, 
            MTBDL_REV_LOG_FREQ, 
            MTBDL_REV_SAMPLE_SIZE); 
        hw125_puts(mtbdl_data.data_buff); 
        
        hw125_puts(mtbdl_data_log_start); 

        // Log index gets incremented when closing the log file. 
        // mtbdl_data.log_index++; 
    }
}


// Log data prep 
void mtbdl_log_data_prep(void)
{
    // Wheel RPM info 
    mtbdl_data.rev_count = CLEAR; 
    mtbdl_data.rev_buff_index = CLEAR; 
    memset((void *)mtbdl_data.rev_buff, CLEAR, sizeof(mtbdl_data.rev_buff)); 

    // Log tracking 
    mtbdl_data.time_count = CLEAR; 
    mtbdl_data.stream_index = CLEAR; 
    mtbdl_data.led_toggle = CLEAR_BIT; 
    mtbdl_data.run_count = CLEAR_BIT; 
    mtbdl_data.trailmark = CLEAR_BIT; 
    mtbdl_data.user_input = CLEAR_BIT; 
    mtbdl_data.log_stream = MTBDL_LOG_STREAM_STANDARD; 

    // GPS - put into a continuous read state 
    m8q_set_read_flag(); 

#if MTBDL_DEBUG 
    mtbdl_data.time_stop = CLEAR; 
    mtbdl_data.time_limit = MTBDL_DEBUG_SAMPLE_COUNT; 
    mtbdl_data.count_standard = CLEAR; 
    mtbdl_data.count_wait = CLEAR; 
    mtbdl_data.time_overflow = CLEAR; 
    mtbdl_data.count_blink = CLEAR; 
    mtbdl_data.count_speed = CLEAR; 
    mtbdl_data.count_accel = CLEAR; 
    mtbdl_data.count_gps = CLEAR; 
    mtbdl_data.count_user = CLEAR; 
    mtbdl_data.adc_count = CLEAR; 
#endif   // MTBDL_DEBUG 

    // Peripherals 
    NVIC_EnableIRQ(mtbdl_data.rpm_irq);   // Enable the wheel speed interrupts 
    NVIC_EnableIRQ(mtbdl_data.log_irq);   // Enable log sample period interrupts 
}


// Logging data 
void mtbdl_logging(void)
{
    // Check for sampling trigger 
#if MTBDL_DEBUG 
    if (handler_flags.tim1_trg_tim11_glbl_flag && (mtbdl_data.time_stop < mtbdl_data.time_limit))
#else   // MTBDL_DEBUG 
    if (handler_flags.tim1_trg_tim11_glbl_flag)
#endif   // MTBDL_DEBUG 
    {
        handler_flags.tim1_trg_tim11_glbl_flag = CLEAR_BIT; 

        //==================================================
        // Update data 
        
        // Start the ADC DMA read 
        adc_start(mtbdl_data.adc); 

        // Record wheel speed input if available 
        if (handler_flags.exti4_flag)
        {
            handler_flags.exti4_flag = CLEAR; 
            mtbdl_data.rev_count++; 
        }
        
        //==================================================

        //==================================================
        // Update the next log stream 

        if (mtbdl_data.time_count == stream_schedule[mtbdl_data.stream_index].counter)
        {
            // Update the log stream and stream sequence index 
            mtbdl_data.log_stream = stream_schedule[mtbdl_data.stream_index].stream; 
            mtbdl_data.stream_index = (mtbdl_data.stream_index < (MTBDL_NUM_LOG_SEQ-1)) ? 
                                      (mtbdl_data.stream_index + 1) : CLEAR; 

            // Disable GPS reads 
            m8q_set_idle_flag(); 
        }
        else if (mtbdl_data.user_input) 
        {
            // User input stream - run when there are no other non-standard streams to run 
            mtbdl_data.log_stream = MTBDL_LOG_STREAM_USER; 
            mtbdl_data.user_input = CLEAR_BIT; 

            // Disable GPS reads 
            m8q_set_idle_flag(); 
        }
        else 
        {
            mtbdl_data.log_stream = MTBDL_LOG_STREAM_STANDARD; 

            // Enable GPS reads - no other critical tasks to be done 
            m8q_set_read_flag(); 
        }
        
        //==================================================

        //==================================================
        // Read and record data 

        // Execute a log stream 
        stream_table[mtbdl_data.log_stream](); 

        // Write to SD card with formatted string 
        hw125_puts(mtbdl_data.data_buff); 

        //==================================================

        //==================================================
        // Update tracking info 
        
        // Clear the trail marker flag 
        mtbdl_data.trailmark = CLEAR_BIT; 

        // Update time count 
        mtbdl_data.time_count = (mtbdl_data.time_count < MTBDL_LOG_COUNT_CYCLE) ? 
                                (mtbdl_data.time_count + 1) : CLEAR; 

#if MTBDL_DEBUG 
        mtbdl_data.time_stop++; 
        mtbdl_data.count_wait++; 
#endif   // MTBDL_DEBUG 
        
        //==================================================
    }
#if MTBDL_DEBUG 
    else 
    {
        if (mtbdl_data.count_wait > 2)
        {
            mtbdl_data.time_overflow++; 
        }
        mtbdl_data.count_wait = CLEAR; 
    }
#endif   // MTBDL_DEBUG 
}


// Standard logging stream 
void mtbdl_log_stream_standard(void)
{
    // Format standard log string 
    snprintf(
        mtbdl_data.data_buff, 
        MTBDL_MAX_STR_LEN, 
        mtbdl_data_log_1, 
        mtbdl_data.trailmark, 
        mtbdl_data.adc_buff[MTBDL_ADC_FORK], 
        mtbdl_data.adc_buff[MTBDL_ADC_SHOCK]); 
    
#if MTBDL_DEBUG 
    mtbdl_data.count_standard++; 
#endif   // MTBDL_DEBUG 
}


// LED blink logging stream 
void mtbdl_log_stream_blink(void)
{
    // Local variables 
    uint32_t led_colour_code; 

    // Toggle the LED state (on/off) 
    if (mtbdl_data.led_toggle)
    {
        mtbdl_data.led_toggle = CLEAR_BIT; 
        led_colour_code = mtbdl_led_clear; 
    }
    else 
    {
        mtbdl_data.led_toggle = SET_BIT; 
        led_colour_code = mtbdl_led0_1; 
    }

    mtbdl_led_update(WS2812_LED_0, led_colour_code); 

    // Format standard log string 
    mtbdl_log_stream_standard(); 

#if MTBDL_DEBUG 
    mtbdl_data.count_blink++; 
    mtbdl_data.count_standard--; 
#endif   // MTBDL_DEBUG 
}


// Wheel speed logging stream 
void mtbdl_log_stream_speed(void)
{
    // Local variables 
    uint8_t revs = CLEAR; 

    // Update the revolution record 
    mtbdl_data.rev_buff[mtbdl_data.rev_buff_index] = mtbdl_data.rev_count; 
    mtbdl_data.rev_count = CLEAR; 
    mtbdl_data.rev_buff_index = (mtbdl_data.rev_buff_index < (MTBDL_REV_SAMPLE_SIZE-1)) ? 
                                (mtbdl_data.rev_buff_index + 1) : CLEAR; 

    // Record the total revolutions over the last MTBDL_REV_SAMPLE_SIZE samples 
    for (uint8_t i = CLEAR; i < MTBDL_REV_SAMPLE_SIZE; i++)
    {
        revs += mtbdl_data.rev_buff[i]; 
    }

    // Format wheel speed data log string 
    snprintf(
        mtbdl_data.data_buff, 
        MTBDL_MAX_STR_LEN, 
        mtbdl_data_log_2, 
        mtbdl_data.trailmark, 
        mtbdl_data.adc_buff[MTBDL_ADC_FORK], 
        mtbdl_data.adc_buff[MTBDL_ADC_SHOCK], 
        revs); 

#if MTBDL_DEBUG 
    mtbdl_data.count_speed++; 
#endif   // MTBDL_DEBUG 
}


// Accelerometer logging stream 
void mtbdl_log_stream_accel(void)
{
    // Determine whether to trigger an accelerometer read or get the updated data 
    if (mtbdl_data.run_count)
    {
        mtbdl_data.run_count = CLEAR_BIT; 

        // Record new accelerometer data 
        mpu6050_get_accel_raw(
            DEVICE_ONE, 
            &mtbdl_data.accel_x, 
            &mtbdl_data.accel_y, 
            &mtbdl_data.accel_z); 

        // Format accel data log string 
        snprintf(
            mtbdl_data.data_buff, 
            MTBDL_MAX_STR_LEN, 
            mtbdl_data_log_3, 
            mtbdl_data.trailmark, 
            mtbdl_data.adc_buff[MTBDL_ADC_FORK], 
            mtbdl_data.adc_buff[MTBDL_ADC_SHOCK], 
            mtbdl_data.accel_x, 
            mtbdl_data.accel_y, 
            mtbdl_data.accel_z); 
    }
    else 
    {
        mtbdl_data.run_count = SET_BIT; 

        // Trigger a read from the accelerometer 
        mpu6050_set_read_flag(DEVICE_ONE); 

        // Format standard log string 
        mtbdl_log_stream_standard(); 
    }

#if MTBDL_DEBUG 
    mtbdl_data.count_accel++; 
    if (mtbdl_data.run_count) mtbdl_data.count_standard--; 
#endif   // MTBDL_DEBUG 
}


// GPS logging stream 
void mtbdl_log_stream_gps(void)
{
    // Record new GPS data 
    m8q_get_position_lat_str(mtbdl_data.lat_str, MTBDL_COO_BUFF_LEN); 
    mtbdl_data.NS = m8q_get_position_NS(); 
    m8q_get_position_lon_str(mtbdl_data.lon_str, MTBDL_COO_BUFF_LEN); 
    mtbdl_data.EW = m8q_get_position_EW(); 

    // Format GPS data log string 
    snprintf(
        mtbdl_data.data_buff, 
        MTBDL_MAX_STR_LEN, 
        mtbdl_data_log_4, 
        mtbdl_data.trailmark, 
        mtbdl_data.adc_buff[MTBDL_ADC_FORK], 
        mtbdl_data.adc_buff[MTBDL_ADC_SHOCK], 
        (char *)mtbdl_data.lat_str, 
        (char)mtbdl_data.NS, 
        (char *)mtbdl_data.lon_str, 
        (char)mtbdl_data.EW); 

#if MTBDL_DEBUG 
    mtbdl_data.count_gps++; 
#endif   // MTBDL_DEBUG 
}


// User input logging stream 
void mtbdl_log_stream_user(void)
{
    // Light up the trailmarker button LED 
    mtbdl_led_update(WS2812_LED_6, mtbdl_led6_1); 

    // Format standard log string 
    mtbdl_log_stream_standard(); 

#if MTBDL_DEBUG 
    mtbdl_data.count_user++; 
    mtbdl_data.count_standard--; 
#endif   // MTBDL_DEBUG 
}


// Log file close 
void mtbdl_log_end(void)
{
    // Reset system settings 
    NVIC_DisableIRQ(mtbdl_data.rpm_irq);   // Disable wheel speed interrupts 
    NVIC_DisableIRQ(mtbdl_data.log_irq);   // Disable log sample period interrupts 
    mtbdl_led_update(WS2812_LED_0, mtbdl_led_clear); 

    // Close up the log file 
    hw125_puts(mtbdl_data_log_end); 
    hw125_close(); 

    // Update the log index 
    // mtbdl_write_sys_params(HW125_MODE_OAWR); 
    param_update_log_index(PARAM_LOG_INDEX_INC); 

    // GPS - put back into a continuous read state 
    m8q_set_read_flag(); 
}

//=======================================================================================


//=======================================================================================
// Calibration functions 

// Calibration data prep 
void mtbdl_cal_prep(void)
{
    // Reset the calibration data 
    memset((void *)mtbdl_data.cal_buff, CLEAR, sizeof(mtbdl_data.cal_buff)); 
    mtbdl_data.cal_index = CLEAR; 

    // Trigger an ADC and accelerometer read so the data is available for the first 
    // time data is recorded 
    adc_start(mtbdl_data.adc); 
    mpu6050_set_read_flag(DEVICE_ONE); 

    // Enable log sample period interrupts 
    NVIC_EnableIRQ(mtbdl_data.log_irq); 
}


// Calibration 
void mtbdl_calibrate(void)
{
    // Record new data periodically 
    if (handler_flags.tim1_trg_tim11_glbl_flag)
    {
        handler_flags.tim1_trg_tim11_glbl_flag = CLEAR_BIT; 
        mtbdl_data.cal_index++; 

        // Record new accel data 
        mpu6050_get_accel_raw(
            DEVICE_ONE, 
            &mtbdl_data.accel_x_rest, 
            &mtbdl_data.accel_y_rest, 
            &mtbdl_data.accel_z_rest); 

        // Sum all the data into the calibration buffer. This data will be averaged once the 
        // calibration state is left. 
        mtbdl_data.cal_buff[MTBDL_CAL_ACCEL_X] += (int32_t)mtbdl_data.accel_x_rest; 
        mtbdl_data.cal_buff[MTBDL_CAL_ACCEL_Y] += (int32_t)mtbdl_data.accel_y_rest; 
        mtbdl_data.cal_buff[MTBDL_CAL_ACCEL_Z] += (int32_t)mtbdl_data.accel_z_rest; 
        mtbdl_data.cal_buff[MTBDL_CAL_POT_FORK] += (int32_t)mtbdl_data.adc_buff[MTBDL_ADC_FORK]; 
        mtbdl_data.cal_buff[MTBDL_CAL_POT_SHOCK] += (int32_t)mtbdl_data.adc_buff[MTBDL_ADC_SHOCK]; 

        // Trigger an ADC and accelerometer read so the data is available for the next 
        // time data is recorded 
        adc_start(mtbdl_data.adc); 
        mpu6050_set_read_flag(DEVICE_ONE); 
    }
}


// Calibration calculation 
void mtbdl_cal_calc(void)
{
    // Disable log sample period interrupts 
    NVIC_DisableIRQ(mtbdl_data.log_irq); 

    // Calculate and record calibration data 
    // This data is an average of all the samples taken during calibration 
    mtbdl_data.accel_x_rest = 
        (int16_t)(mtbdl_data.cal_buff[MTBDL_CAL_ACCEL_X] / mtbdl_data.cal_index); 
    mtbdl_data.accel_y_rest = 
        (int16_t)(mtbdl_data.cal_buff[MTBDL_CAL_ACCEL_Y] / mtbdl_data.cal_index); 
    mtbdl_data.accel_z_rest = 
        (int16_t)(mtbdl_data.cal_buff[MTBDL_CAL_ACCEL_Z] / mtbdl_data.cal_index); 
    mtbdl_data.pot_fork_rest = 
        (uint16_t)(mtbdl_data.cal_buff[MTBDL_CAL_POT_FORK] / mtbdl_data.cal_index); 
    mtbdl_data.pot_shock_rest = 
        (uint16_t)(mtbdl_data.cal_buff[MTBDL_CAL_POT_SHOCK] / mtbdl_data.cal_index); 

    // TODO update the parameter files? 
}

//=======================================================================================


//=======================================================================================
// Data checks and updates 

// Update LED colours 
void mtbdl_led_update(
    ws2812_led_index_t led_index, 
    uint32_t led_code)
{
    mtbdl_data.led_colour_data[led_index] = led_code; 
    ws2812_send(DEVICE_ONE, mtbdl_data.led_colour_data); 
}

// Set trail marker flag 
void mtbdl_set_trailmark(void)
{
    mtbdl_data.trailmark = SET_BIT; 
    mtbdl_data.user_input = SET_BIT; 
}

//=======================================================================================


//=======================================================================================
// Function prototypes 

// /**
//  * @brief Format and write the bike parameters 
//  * 
//  * @details Formats bike parameters (such as fork and shock settings) from the data 
//  *          record into strings and writes the strings to the SD card. This function is 
//  *          used when writing settings to both the bike parameters file and to newly 
//  *          created log files. The bike parameters file is written upon creation and 
//  *          when saving new settings. 
//  */
// void mtbdl_format_write_bike_params(void); 


// /**
//  * @brief Read and format the bike parameters 
//  * 
//  * @details Reads the bike settings/configuration from the SD card and saves the data 
//  *          into the data record. These settings are stored in the bike parameters file 
//  *          and this function is only called during startup if the file already exists. 
//  */
// void mtbdl_format_read_bike_params(void); 


// /**
//  * @brief Format and write the system parameters 
//  * 
//  * @details Formats system parameters (such as IMU and potentiometer calibration data) 
//  *          from the data record into strings and writes the strings to the SD card. 
//  *          This function is used when writing settings to both the system parameters 
//  *          file and to newly created log files. The system parameters file is written 
//  *          upon creation, when saving new settings and keeping track of the log file 
//  *          number/index. 
//  */
// void mtbdl_format_write_sys_params(void); 


// /**
//  * @brief Read and format the system parameters 
//  * 
//  * @details Reads the system settings from the SD card and saves the data into the data 
//  *          record. These settings are stored in the system parameters file and this 
//  *          function is only called during startup if the file already exists. 
//  */
// void mtbdl_format_read_sys_params(void); 

//=======================================================================================


//=======================================================================================
// Initialization 

// // ADC DMA setup 
// void mtbdl_adc_dma_init(
//     DMA_Stream_TypeDef *dma_stream, 
//     ADC_TypeDef *adc)
// {
//     // Configure the DMA stream 
//     dma_stream_config(
//         dma_stream, 
//         (uint32_t)(&adc->DR), 
//         (uint32_t)mtbdl_data.adc_buff, 
//         (uint16_t)MTBDL_ADC_BUFF_SIZE); 
// }


// // File system setup 
// void mtbdl_file_sys_setup(void)
// {
//     // Create "parameters" and "data" directories if they do not already exist 
//     hw125_mkdir(mtbdl_data_dir); 
//     hw125_mkdir(mtbdl_param_dir); 

//     // Check for the existance of the bike parameters file 
//     if (hw125_get_exists(mtbdl_bike_param_file) == FR_NO_FILE)
//     {
//         // No file - create one and write default parameter data to it 
//         mtbdl_write_bike_params(HW125_MODE_WW); 
//     }
//     else 
//     {
//         // File already exists - open the file for reading 
//         mtbdl_read_bike_params(HW125_MODE_OEWR); 
//     }

//     // Check for the existance of the system parameters file 
//     if (hw125_get_exists(mtbdl_sys_param_file) == FR_NO_FILE)
//     {
//         // No file - create one and write default parameter data to it 
//         mtbdl_write_sys_params(HW125_MODE_WW); 
//     }
//     else 
//     {
//         // File already exists - open the file for reading 
//         mtbdl_read_sys_params(HW125_MODE_OEWR); 
//     }
// }

//=======================================================================================


//=======================================================================================
// Parameters 

// // Write bike parameters to file 
// void mtbdl_write_bike_params(uint8_t mode)
// {
//     // Move to the parameters directory, open the bike parameters file for writing, 
//     // format and write the bike parameters from the data record to the file, then close 
//     // the file. 
//     hw125_set_dir(mtbdl_param_dir); 
//     hw125_open(mtbdl_bike_param_file, mode); 
//     mtbdl_format_write_bike_params(); 
//     hw125_close(); 
// }


// // Read bike parameter on file 
// void mtbdl_read_bike_params(uint8_t mode)
// {
//     // Move to the parameters directory, open the bike parameters file for reading, 
//     // read the parameters and store them in the data record, then close the file. 
//     hw125_set_dir(mtbdl_param_dir); 
//     hw125_open(mtbdl_bike_param_file, mode); 
//     mtbdl_format_read_bike_params(); 
//     hw125_close(); 
// }


// // Write system parameters to file 
// void mtbdl_write_sys_params(uint8_t mode)
// {
//     // Move to the parameters directory, open the system parameters file for writing, 
//     // format and write the system parameters from the data record to the file, then 
//     // close the file. 
//     hw125_set_dir(mtbdl_param_dir); 
//     hw125_open(mtbdl_sys_param_file, mode); 
//     mtbdl_format_write_sys_params(); 
//     hw125_close(); 
// }


// // Read system parameters on file 
// void mtbdl_read_sys_params(uint8_t mode)
// {
//     // Move to the parameters directory, open the system parameters file for reading, 
//     // read the parameters and store them in the data record, then close the file. 
//     hw125_set_dir(mtbdl_param_dir); 
//     hw125_open(mtbdl_sys_param_file, mode); 
//     mtbdl_format_read_sys_params(); 
//     hw125_close(); 
// }


// // Format and write the bike parameters 
// void mtbdl_format_write_bike_params(void)
// {
//     // Write fork parameters 
//     snprintf(
//         mtbdl_data.data_buff, 
//         MTBDL_MAX_DATA_STR_LEN, 
//         mtbdl_param_fork_info, 
//         mtbdl_data.fork_psi, 
//         mtbdl_data.fork_comp, 
//         mtbdl_data.fork_reb); 
    
//     hw125_puts(mtbdl_data.data_buff); 

//     // Write shock parameters 
//     snprintf(
//         mtbdl_data.data_buff, 
//         MTBDL_MAX_DATA_STR_LEN, 
//         mtbdl_param_shock_info, 
//         mtbdl_data.shock_psi, 
//         mtbdl_data.shock_lock, 
//         mtbdl_data.shock_reb); 
    
//     hw125_puts(mtbdl_data.data_buff); 
// }


// // Read and format the bike parameters 
// void mtbdl_format_read_bike_params(void)
// {
//     // Read fork parameters 
//     hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

//     sscanf(
//         mtbdl_data.data_buff, 
//         mtbdl_param_fork_info, 
//         &mtbdl_data.fork_psi, 
//         &mtbdl_data.fork_comp, 
//         &mtbdl_data.fork_reb); 

//     // Read shock parameters 
//     hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

//     sscanf(
//         mtbdl_data.data_buff, 
//         mtbdl_param_shock_info, 
//         &mtbdl_data.shock_psi, 
//         &mtbdl_data.shock_lock, 
//         &mtbdl_data.shock_reb); 
// }


// // Format and write the system parameters 
// void mtbdl_format_write_sys_params(void)
// {
//     // Write logging parameters 
//     snprintf(
//         mtbdl_data.data_buff, 
//         MTBDL_MAX_DATA_STR_LEN, 
//         mtbdl_param_index, 
//         mtbdl_data.log_index); 

//     hw125_puts(mtbdl_data.data_buff); 
    
//     // Write accelerometer calibration data 
//     snprintf(
//         mtbdl_data.data_buff, 
//         MTBDL_MAX_DATA_STR_LEN, 
//         mtbdl_param_accel_rest, 
//         mtbdl_data.accel_x_rest, 
//         mtbdl_data.accel_y_rest, 
//         mtbdl_data.accel_z_rest); 
    
//     hw125_puts(mtbdl_data.data_buff); 

//     // Write potentiometer calibrated starting points 
//     snprintf(
//         mtbdl_data.data_buff, 
//         MTBDL_MAX_DATA_STR_LEN, 
//         mtbdl_param_pot_rest, 
//         mtbdl_data.pot_fork_rest, 
//         mtbdl_data.pot_shock_rest); 
    
//     hw125_puts(mtbdl_data.data_buff); 
// }


// // Read and format the system parameters 
// void mtbdl_format_read_sys_params(void)
// {
//     // Read logging parameters 
//     hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

//     sscanf(
//         mtbdl_data.data_buff, 
//         mtbdl_param_index, 
//         &mtbdl_data.log_index); 

//     // Read accelerometer calibration data 
//     hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

//     sscanf(
//         mtbdl_data.data_buff, 
//         mtbdl_param_accel_rest, 
//         &mtbdl_data.accel_x_rest, 
//         &mtbdl_data.accel_y_rest, 
//         &mtbdl_data.accel_z_rest); 

//     // Read potentiometer starting points 
//     hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

//     sscanf(
//         mtbdl_data.data_buff, 
//         mtbdl_param_pot_rest, 
//         &mtbdl_data.pot_fork_rest, 
//         &mtbdl_data.pot_shock_rest); 
// }

//=======================================================================================


//=======================================================================================
// RX state functions 

// // RX user interface start 
// void mtbdl_rx_prep(void)
// {
//     hc05_send(mtbdl_rx_prompt); 
//     hc05_clear(); 
// }


// // Read user input 
// void mtbdl_rx(void)
// {
//     unsigned int param_index, temp_data; 

//     // Read Bluetooth data if available 
//     if (hc05_data_status())
//     {
//         // Read and parse the data from the HC-05 
//         hc05_read(mtbdl_data.data_buff, MTBDL_MAX_STR_LEN); 
//         sscanf(
//             mtbdl_data.data_buff, 
//             mtbdl_rx_input, 
//             &param_index, 
//             &temp_data); 

//         // Check for a data match 
//         switch (param_index)
//         {
//             case MTBDL_PARM_FPSI:
//                 mtbdl_data.fork_psi = temp_data; 
//                 break;

//             case MTBDL_PARM_FC:
//                 if (temp_data <= MTBDL_MAX_SUS_SETTING)
//                 {
//                     mtbdl_data.fork_comp = temp_data; 
//                 }
//                 break;

//             case MTBDL_PARM_FR:
//                 if (temp_data <= MTBDL_MAX_SUS_SETTING)
//                 {
//                     mtbdl_data.fork_reb = temp_data; 
//                 }
//                 break;

//             case MTBDL_PARM_SPSI:
//                 mtbdl_data.shock_psi = temp_data; 
//                 break;

//             case MTBDL_PARM_SL:
//                 if (temp_data <= MTBDL_MAX_SUS_SETTING)
//                 {
//                     mtbdl_data.shock_lock = temp_data; 
//                 }
//                 break;

//             case MTBDL_PARM_SR:
//                 if (temp_data <= MTBDL_MAX_SUS_SETTING)
//                 {
//                     mtbdl_data.shock_reb = temp_data; 
//                 }
//                 break;
            
//             default: 
//                 break;
//         }

//         // Provide a user prompt 
//         mtbdl_rx_prep(); 
//     }
// }

//=======================================================================================


//=======================================================================================
// TX state functions 

// // Check log file count 
// uint8_t mtbdl_tx_check(void)
// {
//     // return mtbdl_data.log_index; 
//     return param_get_log_index(); 
// }


// // Prepare to send a data log file 
// uint8_t mtbdl_tx_prep(void)
// {
//     // Check if there are no log files 
//     // if (!mtbdl_tx_check())
//     if (!param_get_log_index())
//     {
//         return FALSE; 
//     }

//     // Log files exist. Move to the data directory. 
//     hw125_set_dir(mtbdl_data_dir); 

//     // Generate a log file name. The log index is adjusted because it will be one ahead 
//     // of the most recent log file number after the most recent log has been created 
//     snprintf(
//         mtbdl_data.filename, 
//         MTBDL_MAX_STR_LEN, 
//         mtbdl_log_file, 
//         // (mtbdl_data.log_index - MTBDL_DATA_INDEX_OFFSET)); 
//         (param_get_log_index() - MTBDL_DATA_INDEX_OFFSET)); 

//     // Check for the existance of the specified file number 
//     if (hw125_get_exists(mtbdl_data.filename) == FR_NO_FILE)
//     {
//         return FALSE; 
//     }

//     // Open the file 
//     hw125_open(mtbdl_data.filename, HW125_MODE_OAWR); 

//     return TRUE; 
// }


// // Transfer data log contents 
// uint8_t mtbdl_tx(void)
// {
//     // Read a line from the data log and send it out via the Bluetooth module. 
//     hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_STR_LEN); 
//     hc05_send(mtbdl_data.data_buff); 

//     // Check for end of file - if true we can stop the transaction 
//     if (hw125_eof())
//     {
//         mtbdl_data.tx_status = SET_BIT; 
//         return TRUE; 
//     }

//     return FALSE; 
// }


// // End the transmission 
// void mtbdl_tx_end(void)
// {
//     hw125_close(); 

//     if (mtbdl_data.tx_status)
//     {
//         // Transaction completed - delete the file and update the log index 
//         hw125_unlink(mtbdl_data.filename); 
//         mtbdl_data.tx_status = CLEAR_BIT; 
//         // mtbdl_data.log_index--; 
//         // mtbdl_write_sys_params(HW125_MODE_OAWR); 
//         param_update_log_index(PARAM_LOG_INDEX_DEC); 
//     }
// }

//=======================================================================================


//=======================================================================================
// Screen message formatting 

// // Format the idle state message 
// void mtbdl_set_idle_msg(void)
// {
//     hd44780u_msgs_t msg[MTBDL_MSG_LEN_4_LINE]; 

//     // Create an editable copy of the message 
//     for (uint8_t i = CLEAR; i < MTBDL_MSG_LEN_4_LINE; i++) 
//     {
//         msg[i] = mtbdl_idle_msg[i]; 
//     }

//     // Format the message with data 
//     // snprintf will NULL terminate the string at the screen line length so in order to use 
//     // the last spot on the screen line the message length must be indexed up by one 
//     snprintf(
//         msg[HD44780U_L1].msg, 
//         (HD44780U_LINE_LEN + MTBDL_DATA_INDEX_OFFSET), 
//         mtbdl_idle_msg[HD44780U_L1].msg, 
//         mtbdl_data.fork_psi, 
//         mtbdl_data.fork_comp, 
//         mtbdl_data.fork_reb); 
    
//     snprintf(
//         msg[HD44780U_L2].msg, 
//         (HD44780U_LINE_LEN + MTBDL_DATA_INDEX_OFFSET), 
//         mtbdl_idle_msg[HD44780U_L2].msg, 
//         mtbdl_data.shock_psi, 
//         mtbdl_data.shock_lock, 
//         mtbdl_data.shock_reb); 
    
//     snprintf(
//         msg[HD44780U_L3].msg, 
//         (HD44780U_LINE_LEN + MTBDL_DATA_INDEX_OFFSET), 
//         mtbdl_idle_msg[HD44780U_L3].msg, 
//         mtbdl_data.adc_buff[MTBDL_ADC_SOC], 
//         (char)(mtbdl_data.navstat >> SHIFT_8), 
//         (char)mtbdl_data.navstat); 

//     // Set the screen message 
//     hd44780u_set_msg(msg, MTBDL_MSG_LEN_4_LINE); 
// }


// // Format the run prep state message 
// void mtbdl_set_run_prep_msg(void)
// {
//     hd44780u_msgs_t msg[MTBDL_MSG_LEN_3_LINE]; 

//     // Create an editable copy of the message 
//     for (uint8_t i = CLEAR; i < MTBDL_MSG_LEN_3_LINE; i++) 
//     {
//         msg[i] = mtbdl_run_prep_msg[i]; 
//     }

//     // Convert the NAVSTAT code to an easily readable value 

//     // Format the message with data 
//     snprintf(
//         msg[HD44780U_L1].msg, 
//         HD44780U_LINE_LEN, 
//         mtbdl_run_prep_msg[HD44780U_L1].msg, 
//         (char)(mtbdl_data.navstat >> SHIFT_8), 
//         (char)mtbdl_data.navstat); 

//     // Set the screen message 
//     hd44780u_set_msg(msg, MTBDL_MSG_LEN_3_LINE); 
// }


// // Format the pre TX state message 
// void mtbdl_set_pretx_msg(void)
// {
//     hd44780u_msgs_t msg[MTBDL_MSG_LEN_4_LINE]; 

//     // Create an editable copy of the message 
//     for (uint8_t i = CLEAR; i < MTBDL_MSG_LEN_4_LINE; i++) 
//     {
//         msg[i] = mtbdl_pretx_msg[i]; 
//     }

//     // Format the message with data 
//     // The log index is adjusted because it will be one ahead of the most recent log 
//     // file number after the most recent log has been created 
//     snprintf(
//         msg[HD44780U_L2].msg, 
//         HD44780U_LINE_LEN, 
//         mtbdl_pretx_msg[HD44780U_L2].msg, 
//         // (mtbdl_data.log_index - MTBDL_DATA_INDEX_OFFSET)); 
//         (param_get_log_index() - MTBDL_DATA_INDEX_OFFSET)); 

//     // Set the screen message 
//     hd44780u_set_msg(msg, MTBDL_MSG_LEN_4_LINE); 
// }

//=======================================================================================
