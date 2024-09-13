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

#include "data_logging.h" 
#include "ws2812_config.h" 
#include "stm32f4xx_it.h" 

//=======================================================================================


//=======================================================================================
// Macros 

// Data logging 
#define MTBDL_LOG_PERIOD 10              // (ms) Period between data samples 

// Wheel RPM info 
#define MTBDL_REV_LOG_FREQ 2             // (Hz) Revolution calc frequency 

//=======================================================================================


//=======================================================================================
// Enums 

// Logging streams 
typedef enum {
    LOG_STREAM_STANDARD,   // Standard stream 
    LOG_STREAM_GPS,        // GPS stream 
    LOG_STREAM_ACCEL,      // Accelerometer stream 
    LOG_STREAM_SPEED,      // Wheel speed stream 
    LOG_STREAM_NUM         // Number of logging streams 
} log_stream_t; 

//=======================================================================================


//=======================================================================================
// Structures 

// Data record instance 
static mtbdl_log_t mtbdl_log; 


// Logging schedule data 
typedef struct log_stream_state_s 
{
    uint8_t counter; 
    log_stream_t stream; 
}
log_stream_state_t; 


// Logging schedule data 
typedef struct log_stream_schedule_s 
{
    log_stream_t stream; 
    uint8_t offset; 
    uint8_t counter_period; 
}
log_stream_schedule_t; 

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
void log_stream_standard(void); 


/**
 * @brief Data logging stream: wheel speed 
 * 
 * @details This function is used during data logging to record standard data for the 
 *          current logging interval and estimated wheel speed. Wheel speed is determined 
 *          by recording the number of hall effect sensor interrupts over a period of time 
 *          which means this stream has to look at previous sensor data. See the 
 *          'stream_schedule' table for the data stream sequence. 
 */
void log_stream_speed(void); 


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
void log_stream_accel(void); 


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
void log_stream_gps(void); 

//=======================================================================================


//=======================================================================================
// Variables 

// Log stream table 
static mtbdl_log_stream stream_table[LOG_STREAM_NUM] = 
{
    &log_stream_standard, 
    &log_stream_gps, 
    &log_stream_accel, 
    &log_stream_speed 
    // &log_stream_blink, 
    // &log_stream_user 
}; 


// Log stream schedule 
// Notes: 
// - The counter period and starting offsets of each stream are chosen so that no two 
//   streams will run during the same interval. 
// - The standard log stream runs when no other stream needs to run so it does not 
//   require a period and offset. 
// - This table must order log streams in the same order that they are listed in 
//   log_stream_t. 
static const log_stream_schedule_t stream_schedule[LOG_STREAM_NUM] = 
{
    // { Log stream,      starting offset, counter period } 
    {LOG_STREAM_STANDARD, 0,               0}, 
    {LOG_STREAM_GPS,      0,               20}, 
    {LOG_STREAM_ACCEL,    1,               2}, 
    {LOG_STREAM_SPEED,    2,               4} 
}; 

//=======================================================================================


//=======================================================================================
// Initialization 

// Initialize data record 
void log_init(
    IRQn_Type rpm_irqn, 
    IRQn_Type log_irqn, 
    ADC_TypeDef *adc, 
    DMA_Stream_TypeDef *dma_stream)
{
    // Reset the whole data record 
    memset((void *)&mtbdl_log, CLEAR, sizeof(mtbdl_log_t)); 

    // Peripherals 
    mtbdl_log.rpm_irq = rpm_irqn; 
    mtbdl_log.log_irq = log_irqn; 
    mtbdl_log.adc = adc; 

    // System data 
    // mtbdl_log.navstat = M8Q_NAVSTAT_NF; 

    // Configure the DMA stream 
    dma_stream_config(
        dma_stream, 
        (uint32_t)(&adc->DR), 
        (uint32_t)mtbdl_log.adc_buff, 
        (uint16_t)ADC_BUFF_SIZE); 
}

//=======================================================================================


//=======================================================================================
// Data logging 

// Log file name preparation 
uint8_t log_data_name_prep(void)
{
    uint8_t log_index = param_get_log_index(); 

    // Check the data log index is within bounds 
    // if (mtbdl_log.log_index > LOG_MAX_FILES)
    if (log_index > LOG_MAX_FILES)
    {
        // Too many log files on drive - don't create a new file name 
        return FALSE; 
    }

    // Number of log files is within the limit - generate a new log file name 
    snprintf(mtbdl_log.filename, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_log_file, 
             log_index); 
             // mtbdl_log.log_index); 

    return TRUE; 
}


// Log file preparation 
void log_data_file_prep(void)
{
    // The code moves to the directory that stores the data log files and attempts to create 
    // and open the next indexed log file. If this is successful then information and data 
    // will be written to the file. If unsuccessful then the hw125 controller will record a 
    // fault and the system will enter the fault state instead of proceeding to the data 
    // logging state. 
    
    hw125_set_dir(mtbdl_data_dir); 

    if (hw125_open(mtbdl_log.filename, HW125_MODE_WWX) == FR_OK)
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
        param_bike_format_write(); 
        param_sys_format_write(); 

        // UTC time stamp 
        m8q_get_time_utc_time(mtbdl_log.utc_time, LOG_TIME_BUFF_LEN); 
        m8q_get_time_utc_date(mtbdl_log.utc_date, LOG_TIME_BUFF_LEN); 
        snprintf(mtbdl_log.data_buff, 
                 MTBDL_MAX_STR_LEN, 
                 mtbdl_param_time, 
                 (char *)mtbdl_log.utc_time, 
                 (char *)mtbdl_log.utc_date); 
        hw125_puts(mtbdl_log.data_buff); 

        // Logging info 
        snprintf(mtbdl_log.data_buff, 
                 MTBDL_MAX_STR_LEN, 
                 mtbdl_param_data, 
                 MTBDL_LOG_PERIOD, 
                 MTBDL_REV_LOG_FREQ, 
                 MTBDL_REV_SAMPLE_SIZE); 
        hw125_puts(mtbdl_log.data_buff); 
        
        hw125_puts(mtbdl_data_log_start); 
    }
}


// Log data preparation 
void log_data_prep(void)
{
    // System info 
    mtbdl_log.trailmark = CLEAR_BIT; 
    
    // Wheel RPM info 
    mtbdl_log.rev_count = CLEAR; 
    mtbdl_log.rev_buff_index = CLEAR; 
    memset((void *)mtbdl_log.rev_buff, CLEAR, sizeof(mtbdl_log.rev_buff)); 

    // SD card data 
    memset((void *)mtbdl_log.data_buff, CLEAR, sizeof(mtbdl_log.data_buff)); 

    // Stream counters 
    mtbdl_log.gps_stream_counter = stream_schedule[LOG_STREAM_GPS].offset; 
    mtbdl_log.accel_stream_counter = stream_schedule[LOG_STREAM_ACCEL].offset; 
    mtbdl_log.speed_stream_counter = stream_schedule[LOG_STREAM_SPEED].offset; 

// #if MTBDL_DEBUG 
//     mtbdl_log.time_stop = CLEAR; 
//     mtbdl_log.time_limit = MTBDL_DEBUG_SAMPLE_COUNT; 
//     mtbdl_log.count_standard = CLEAR; 
//     mtbdl_log.count_wait = CLEAR; 
//     mtbdl_log.time_overflow = CLEAR; 
//     mtbdl_log.count_blink = CLEAR; 
//     mtbdl_log.count_speed = CLEAR; 
//     mtbdl_log.count_accel = CLEAR; 
//     mtbdl_log.count_gps = CLEAR; 
//     mtbdl_log.count_user = CLEAR; 
//     mtbdl_log.adc_count = CLEAR; 
// #endif   // MTBDL_DEBUG 

    // Enable interrupts 
    NVIC_EnableIRQ(mtbdl_log.rpm_irq);   // Enable the wheel speed interrupts 
    NVIC_EnableIRQ(mtbdl_log.log_irq);   // Enable log sample period interrupts 
}


// Logging data 
void log_data(void)
{
    if (handler_flags.tim1_trg_tim11_glbl_flag)
    {
        handler_flags.tim1_trg_tim11_glbl_flag = CLEAR_BIT; 
        log_stream_t log_stream = LOG_STREAM_STANDARD; 

        // Before executing a log stream, the ADC (suspension position) recording is 
        // kicked off so that it can be recored at the next logging interval. The 
        // data from the previous interval is isolated so it can be recored in the 
        // log file during the current interval. The wheel speed interrupt is also 
        // checked and updated as needed. 

        // TODO ADC handling 
        adc_start(mtbdl_log.adc); 

        if (handler_flags.exti4_flag)
        {
            handler_flags.exti4_flag = CLEAR; 
            mtbdl_log.rev_count++; 
        }

        // Increment the stream counters, check the schedule for a stream to call, then 
        // execute the scheduled stream. All counters must be incremented together 
        // before the stream selection so that they're guarenteed to count. 

        mtbdl_log.gps_stream_counter++; 
        mtbdl_log.accel_stream_counter++; 
        mtbdl_log.speed_stream_counter++; 

        if (mtbdl_log.gps_stream_counter >= 
            stream_schedule[LOG_STREAM_GPS].counter_period)
        {
            mtbdl_log.gps_stream_counter = CLEAR; 
            log_stream = LOG_STREAM_GPS; 
        }
        else if (mtbdl_log.accel_stream_counter >= 
                 stream_schedule[LOG_STREAM_ACCEL].counter_period)
        {
            mtbdl_log.accel_stream_counter = CLEAR; 
            log_stream = LOG_STREAM_ACCEL; 
        }
        else if (mtbdl_log.speed_stream_counter >= 
                 stream_schedule[LOG_STREAM_SPEED].counter_period)
        {
            mtbdl_log.speed_stream_counter = CLEAR; 
            log_stream = LOG_STREAM_SPEED; 
        }

        stream_table[log_stream](); 
        
        // Write the formatted data contents to the log file. Multiple strings must be 
        // written at each interval to backfill suspension position data (ADC info) 
        // which gets recoreded faster than other device data. The user trailmarker is 
        // reset after updating the log file so it's sure to be recored. 

        // TODO will change based on ADC stuff 
        hw125_puts(mtbdl_log.data_buff); 
        
        mtbdl_log.trailmark = CLEAR_BIT; 
    }

// #if MTBDL_DEBUG 
//         mtbdl_log.time_stop++; 
//         mtbdl_log.count_wait++; 
// #endif   // MTBDL_DEBUG 
        
//         //==================================================
//     }
// #if MTBDL_DEBUG 
//     else 
//     {
//         if (mtbdl_log.count_wait > 2)
//         {
//             mtbdl_log.time_overflow++; 
//         }
//         mtbdl_log.count_wait = CLEAR; 
//     }
// #endif   // MTBDL_DEBUG 
}


// Standard logging stream 
void log_stream_standard(void)
{
    // Format standard log string 
    snprintf(mtbdl_log.data_buff, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_data_log_1, 
             mtbdl_log.trailmark, 
             mtbdl_log.adc_buff[ADC_FORK], 
             mtbdl_log.adc_buff[ADC_SHOCK]); 
    
// #if MTBDL_DEBUG 
//     mtbdl_log.count_standard++; 
// #endif   // MTBDL_DEBUG 
}


// GPS logging stream 
void log_stream_gps(void)
{
    // In this data logging stream the GPS device is read from and the results are 
    // formatted so they can be saved in the log file. The GPS controller is used to 
    // update the GPS data. This involves putting the device into a read state, reading 
    // from the device, then putting the device back into an idle state. This is done 
    // so that the controller will not read from the device (and therefore consume time 
    // during another stream) while not in the GPS stream. It's also necessary to call 
    // the controller directly so we can get the new data now. The controller is used 
    // here to update data instead of directly using the device driver to be consistant 
    // with how the rest of the code is written. 

    // Update the GPS data 
    m8q_set_read_flag(); 
    m8q_controller(); 
    m8q_set_idle_flag(); 

    // Get new GPS data 
    m8q_get_position_lat_str(mtbdl_log.lat_str, LOG_GPS_BUFF_LEN); 
    mtbdl_log.NS = m8q_get_position_NS(); 
    m8q_get_position_lon_str(mtbdl_log.lon_str, LOG_GPS_BUFF_LEN); 
    mtbdl_log.EW = m8q_get_position_EW(); 

    // TODO add ground speed? 

    // Format GPS data log string 
    snprintf(mtbdl_log.data_buff, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_data_log_4, 
             mtbdl_log.trailmark, 
             mtbdl_log.adc_buff[ADC_FORK], 
             mtbdl_log.adc_buff[ADC_SHOCK], 
             (char *)mtbdl_log.lat_str, 
             (char)mtbdl_log.NS, 
             (char *)mtbdl_log.lon_str, 
             (char)mtbdl_log.EW); 

    // TODO Would still need to write the standard log stream data to fill in the 
    // other ADC values. 

// #if MTBDL_DEBUG 
//     mtbdl_log.count_gps++; 
// #endif   // MTBDL_DEBUG 
}


// Accelerometer logging stream 
void log_stream_accel(void)
{
    // In this data logging stream the IMU device is read from and the 3-axis 
    // acceleration results are formatted so they can be saved in the log file. The IMU 
    // controller is used to update the acceleration data. This involves telling the 
    // controller it can read from the device, then calling the controller to read new 
    // data. This is done so that the controller will not read from the device (and 
    // therefore consume time during another stream) while not in the IMU stream. It's 
    // also necessary to call the controller directly so we can get the new data now. 
    // The controller is used here to update data instead of directly using the device 
    // driver to be consistant with how the rest of the code is written. 

    // Update the accelerometer data 
    mpu6050_set_read_flag(DEVICE_ONE); 
    mpu6050_controller(DEVICE_ONE); 

    // Get new accelerometer data 
    mpu6050_get_accel_raw(
        DEVICE_ONE, 
        &mtbdl_log.accel_x, 
        &mtbdl_log.accel_y, 
        &mtbdl_log.accel_z); 

    // Format accelerometer data log string 
    snprintf(mtbdl_log.data_buff, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_data_log_3, 
             mtbdl_log.trailmark, 
             mtbdl_log.adc_buff[ADC_FORK], 
             mtbdl_log.adc_buff[ADC_SHOCK], 
             mtbdl_log.accel_x, 
             mtbdl_log.accel_y, 
             mtbdl_log.accel_z); 

    // TODO Would still need to write the standard log stream data to fill in the 
    // other ADC values. 

// #if MTBDL_DEBUG 
//     mtbdl_log.count_accel++; 
//     if (mtbdl_log.run_count) mtbdl_log.count_standard--; 
// #endif   // MTBDL_DEBUG 
}


// Wheel speed logging stream 
void log_stream_speed(void)
{
    // In this data logging stream the wheel revolutions from the previous speed logging 
    // interval are recored in a circular buffer. The revolutions from each index of the 
    // buffer are summed together and recorded in the log file. This provides a 
    // revolution count across the X most recent intervals which can be used along with 
    // the total time of X intervals to estimate an RPM in post processing. The time of 
    // a single interval along with the buffer size is written at the top of each log 
    // file. The oldest piece of data in the buffer gets overwritten by the newest at 
    // each interval. 
    
    // Record the revolutions from the interval in the circular buffer and update the 
    // buffer index. 

    mtbdl_log.rev_buff[mtbdl_log.rev_buff_index++] = mtbdl_log.rev_count; 
    mtbdl_log.rev_count = CLEAR; 

    if (mtbdl_log.rev_buff_index >= MTBDL_REV_SAMPLE_SIZE)
    {
        mtbdl_log.rev_buff_index = CLEAR; 
    }

    // Sum all the buffer values to get the total revolutions over the X most recent 
    // intervals. The revolution count is cleared both before doing this as well as after 
    // logging the total count so the revolution count can go back to recording the 
    // count per interval. 

    for (uint8_t i = CLEAR; i < MTBDL_REV_SAMPLE_SIZE; i++)
    {
        mtbdl_log.rev_count += mtbdl_log.rev_buff[i]; 
    }

    // Format wheel speed data log string 
    snprintf(mtbdl_log.data_buff, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_data_log_2, 
             mtbdl_log.trailmark, 
             mtbdl_log.adc_buff[ADC_FORK], 
             mtbdl_log.adc_buff[ADC_SHOCK], 
             mtbdl_log.rev_count); 

    mtbdl_log.rev_count = CLEAR; 

// #if MTBDL_DEBUG 
//     mtbdl_log.count_speed++; 
// #endif   // MTBDL_DEBUG 
}


// Log file close 
void log_data_end(void)
{
    // Disable interrupts 
    NVIC_DisableIRQ(mtbdl_log.rpm_irq);   // Disable wheel speed interrupts 
    NVIC_DisableIRQ(mtbdl_log.log_irq);   // Disable log sample period interrupts 

    // Terminate and close the log file 
    hw125_puts(mtbdl_data_log_end); 
    hw125_close(); 

    // Update the log index 
    param_update_log_index(PARAM_LOG_INDEX_INC); 
}

//=======================================================================================


//=======================================================================================
// Calibration functions 

// Calibration data prep 
void log_calibration_prep(void)
{
    // Reset the calibration data 
    memset((void *)mtbdl_log.cal_buff, CLEAR, sizeof(mtbdl_log.cal_buff)); 
    mtbdl_log.cal_samples = CLEAR; 

    // Trigger an ADC and accelerometer read so the data is available for the first 
    // time data is recorded 
    adc_start(mtbdl_log.adc); 
    mpu6050_set_read_flag(DEVICE_ONE); 

    // Enable log sample period interrupts 
    NVIC_EnableIRQ(mtbdl_log.log_irq); 
}


// Calibration 
void log_calibration(void)
{
    // Record new data periodically 
    if (handler_flags.tim1_trg_tim11_glbl_flag)
    {
        handler_flags.tim1_trg_tim11_glbl_flag = CLEAR_BIT; 
        mtbdl_log.cal_samples++; 

        // Record new accel data 
        mpu6050_get_accel_raw(
            DEVICE_ONE, 
            &mtbdl_log.accel_x, 
            &mtbdl_log.accel_y, 
            &mtbdl_log.accel_z); 

        // Sum all the data into the calibration buffer. This data will be averaged once the 
        // calibration state is left. 
        mtbdl_log.cal_buff[PARAM_SYS_SET_AX_REST] += (int32_t)mtbdl_log.accel_x; 
        mtbdl_log.cal_buff[PARAM_SYS_SET_AY_REST] += (int32_t)mtbdl_log.accel_y; 
        mtbdl_log.cal_buff[PARAM_SYS_SET_AZ_REST] += (int32_t)mtbdl_log.accel_z; 
        mtbdl_log.cal_buff[PARAM_SYS_SET_FORK_REST] += (int32_t)mtbdl_log.adc_buff[ADC_FORK]; 
        mtbdl_log.cal_buff[PARAM_SYS_SET_SHOCK_REST] += (int32_t)mtbdl_log.adc_buff[ADC_SHOCK]; 

        // Trigger an ADC and accelerometer read so the data is available for the next 
        // time data is recorded 
        adc_start(mtbdl_log.adc); 
        mpu6050_set_read_flag(DEVICE_ONE); 
    }
}


// Calibration calculation 
void log_calibration_calculation(void)
{
    // Disable log sample period interrupts 
    NVIC_DisableIRQ(mtbdl_log.log_irq); 

    // Average the samples taken during calibration and update the system parameters with 
    // the calculated values. 

    mtbdl_log.accel_x = 
        (int16_t)(mtbdl_log.cal_buff[PARAM_SYS_SET_AX_REST] / mtbdl_log.cal_samples); 
    
    mtbdl_log.accel_y = 
        (int16_t)(mtbdl_log.cal_buff[PARAM_SYS_SET_AY_REST] / mtbdl_log.cal_samples); 
    
    mtbdl_log.accel_z = 
        (int16_t)(mtbdl_log.cal_buff[PARAM_SYS_SET_AZ_REST] / mtbdl_log.cal_samples); 
    
    mtbdl_log.adc_buff[ADC_FORK] = 
        (uint16_t)(mtbdl_log.cal_buff[PARAM_SYS_SET_FORK_REST] / mtbdl_log.cal_samples); 
    
    mtbdl_log.adc_buff[ADC_SHOCK] = 
        (uint16_t)(mtbdl_log.cal_buff[PARAM_SYS_SET_SHOCK_REST] / mtbdl_log.cal_samples); 

    param_update_system_setting(PARAM_SYS_SET_AX_REST, (void *)&mtbdl_log.accel_x); 
    param_update_system_setting(PARAM_SYS_SET_AY_REST, (void *)&mtbdl_log.accel_y); 
    param_update_system_setting(PARAM_SYS_SET_AZ_REST, (void *)&mtbdl_log.accel_z); 
    param_update_system_setting(PARAM_SYS_SET_FORK_REST, (void *)&mtbdl_log.adc_buff[ADC_FORK]); 
    param_update_system_setting(PARAM_SYS_SET_SHOCK_REST, (void *)&mtbdl_log.adc_buff[ADC_SHOCK]); 

    param_write_sys_params(HW125_MODE_OEW); 
}

//=======================================================================================


//=======================================================================================
// Setters 

// Set trail marker flag 
void log_set_trailmark(void)
{
    mtbdl_log.trailmark = SET_BIT; 
}

//=======================================================================================
