/**
 * @file data_logging.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Data logging module 
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
#define LOG_PERIOD 10                   // (ms) Period between data samples 
#define LOG_MAX_FILES 250               // Max data log file number 

// Wheel RPM info 
#define LOG_REV_FREQ 5                  // (Hz) Revolution calc frequency 

// Timing 
#define LOG_ADC_DMA_WAIT 1000           // Number of times to wait on ADC DMA to complete 

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
 * @brief Data logging stream: wheel speed 
 * 
 * @details This function is used during data logging to record standard data for the 
 *          current logging interval and estimated wheel speed. Wheel speed is determined 
 *          by recording the number of hall effect sensor interrupts over a period of time 
 *          which means this stream has to look at previous sensor data. See the 
 *          'stream_schedule' table for the data stream sequence. 
 */
void log_stream_speed(void); 

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
}; 


// Log stream schedule 
// Notes: 
// - The counter period and starting offsets of each stream are chosen so that no two 
//   streams will run during the same interval. 
// - The standard log stream runs when no other stream needs to run so it does not 
//   require a period or offset. 
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
    DMA_TypeDef *dma, 
    DMA_Stream_TypeDef *dma_stream)
{
    // Peripherals 
    mtbdl_log.rpm_irq = rpm_irqn; 
    mtbdl_log.log_irq = log_irqn; 
    mtbdl_log.adc = adc; 
    mtbdl_log.dma = dma; 
    mtbdl_log.dma_stream = dma_stream; 

    // Log file info 
    memset((void *)mtbdl_log.utc_time, CLEAR, sizeof(mtbdl_log.utc_time)); 
    memset((void *)mtbdl_log.utc_date, CLEAR, sizeof(mtbdl_log.utc_date)); 

    // ADC data 
    memset((void *)mtbdl_log.adc_buff, CLEAR, sizeof(mtbdl_log.adc_buff)); 
    memset((void *)mtbdl_log.adc_period, CLEAR, sizeof(mtbdl_log.adc_period)); 

    // GPS data 
    memset((void *)mtbdl_log.lat_str, CLEAR, sizeof(mtbdl_log.lat_str)); 
    mtbdl_log.NS = CLEAR; 
    memset((void *)mtbdl_log.lon_str, CLEAR, sizeof(mtbdl_log.lon_str)); 
    mtbdl_log.EW = CLEAR; 
    memset((void *)mtbdl_log.sog_str, CLEAR, sizeof(mtbdl_log.sog_str)); 

    // Accelerometer data 
    mtbdl_log.accel_x = CLEAR; 
    mtbdl_log.accel_y = CLEAR; 
    mtbdl_log.accel_z = CLEAR; 

    // Wheel RPM data 
    mtbdl_log.rev_count = CLEAR; 
    mtbdl_log.rev_buff_index = CLEAR; 
    memset((void *)mtbdl_log.rev_buff, CLEAR, sizeof(mtbdl_log.rev_buff)); 

    // User input data 
    mtbdl_log.trailmark = CLEAR_BIT; 

    // Logging counters 
    mtbdl_log.log_interval_divider = CLEAR; 
    mtbdl_log.gps_stream_counter = CLEAR; 
    mtbdl_log.accel_stream_counter = CLEAR; 
    mtbdl_log.speed_stream_counter = CLEAR; 
    mtbdl_log.interrupt_counter = CLEAR; 

    // Calibration data 
    memset((void *)mtbdl_log.cal_buff, CLEAR, sizeof(mtbdl_log.cal_buff)); 
    mtbdl_log.cal_adc_samples = CLEAR; 
    mtbdl_log.cal_accel_samples = CLEAR; 

    // SD card data 
    memset((void*)mtbdl_log.data_buff, CLEAR, sizeof(mtbdl_log.data_buff)); 
    memset((void*)mtbdl_log.data_str, CLEAR, sizeof(mtbdl_log.data_str)); 
    mtbdl_log.data_buff_index = CLEAR; 
    memset((void *)mtbdl_log.filename, CLEAR, sizeof(mtbdl_log.filename)); 

    // Debugging / log checking 
    mtbdl_log.overrun = CLEAR; 

    // Configure the DMA stream 
    dma_stream_config(
        mtbdl_log.dma_stream, 
        (uint32_t)(&mtbdl_log.adc->DR), 
        (uint32_t)mtbdl_log.adc_buff, 
        (uint32_t)NULL, 
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
        snprintf(mtbdl_log.data_str, 
                 MTBDL_MAX_STR_LEN, 
                 mtbdl_param_time, 
                 (char *)mtbdl_log.utc_time, 
                 (char *)mtbdl_log.utc_date); 
        hw125_puts(mtbdl_log.data_str); 

        // Logging info 
        snprintf(mtbdl_log.data_str, 
                 MTBDL_MAX_STR_LEN, 
                 mtbdl_param_data, 
                 LOG_PERIOD, 
                 LOG_REV_FREQ, 
                 LOG_REV_SAMPLE_SIZE); 
        hw125_puts(mtbdl_log.data_str); 
        
        hw125_puts(mtbdl_data_log_start); 
    }
}


// Log data preparation 
void log_data_prep(void)
{
    // This function sets important logging parameters to their default value so 
    // data is accurately recorded. Some logging parameters are not set here either 
    // because they don't need to be or because they get updated even while not 
    // logging so their data is up to date. 

    // ADC data 
    memset((void *)mtbdl_log.adc_period, CLEAR, sizeof(mtbdl_log.adc_period)); 
    
    // Wheel RPM info 
    mtbdl_log.rev_count = CLEAR; 
    mtbdl_log.rev_buff_index = CLEAR; 
    memset((void *)mtbdl_log.rev_buff, CLEAR, sizeof(mtbdl_log.rev_buff)); 

    // User input data 
    mtbdl_log.trailmark = CLEAR_BIT; 

    // Logging counters 
    mtbdl_log.log_interval_divider = CLEAR; 
    mtbdl_log.gps_stream_counter = stream_schedule[LOG_STREAM_GPS].offset; 
    mtbdl_log.accel_stream_counter = stream_schedule[LOG_STREAM_ACCEL].offset; 
    mtbdl_log.speed_stream_counter = stream_schedule[LOG_STREAM_SPEED].offset; 
    mtbdl_log.interrupt_counter = CLEAR; 

    // SD card data 
    memset((void*)mtbdl_log.data_buff, CLEAR, sizeof(mtbdl_log.data_buff)); 
    memset((void*)mtbdl_log.data_str, CLEAR, sizeof(mtbdl_log.data_str)); 
    mtbdl_log.data_buff_index = CLEAR; 

    // Debugging / log checking 
    mtbdl_log.overrun = CLEAR; 

    // Enable interrupts 
    NVIC_EnableIRQ(mtbdl_log.rpm_irq);   // Wheel speed 
    NVIC_EnableIRQ(mtbdl_log.log_irq);   // Log sample period 
}


// Logging data 
void log_data(void)
{
    // Recording of log data is handled here. A periodic interrupt runs to trigger ADC 
    // conversions and keep track of logging intervals which in turn triggers the reading 
    // and recording of other data in this function. Multiple intervals of data are 
    // recorded before compiling them together and writing them to the SD card. This is 
    // done to give time for other processes to complete that may accumulate to a time 
    // longer than the sampling interval. 

    // External wheel revolution interrupt. This interrupt does not happen fast enough 
    // for the revolution counter to need to be in the interrupt handler so it's easier 
    // to keep it here. 
    if (handler_flags.exti4_flag)
    {
        handler_flags.exti4_flag = CLEAR; 
        mtbdl_log.rev_count++; 
    }    
    
    // 'interrupt_counter' gets incremented in the perodic interrupt callback function 
    // below. Using this variable instead of just the interrupt handler flag to trigger 
    // logging streams allows for multiple interrupts to occur while data is being read 
    // and processed without missing any ADC data. 
    if (mtbdl_log.interrupt_counter)
    {
        // This is decremented here specifically so data overruns can be detected. 
        mtbdl_log.interrupt_counter--; 

        if (mtbdl_log.log_interval_divider >= LOG_PERIOD_DIVIDER)
        {
            mtbdl_log.log_interval_divider = CLEAR; 

            // If the code enters this process and the interrupt counter is greater than 
            // 0 then it means interrupts have occured without the data being processed 
            // and therefore means we've lost data. This logging process (that involves 
            // the use of 'interrupt_counter' and 'log_interval_divider') is designed to 
            // give time for longer processes to complete without dropping any data from 
            // each logging interval. 'overrun' keeps track of how many times the code 
            // has lost data. 
            if (mtbdl_log.interrupt_counter)
            {
                mtbdl_log.overrun++; 
            }

            // Increment the stream counters, check the schedule for a stream to call, 
            // execute the scheduled stream, then write data from the previous X intervals 
            // to the SD card. All counters must be incremented together before the stream 
            // selection so that they're guarenteed to count. Streams are coordinated 
            // using the 'stream_schedule' table which ensures multiple devices (excluding 
            // ADC reads) are not read from at the same interval. This is due to the 
            // tight sampling window that needs to be maintained. 

            log_stream_t log_stream = LOG_STREAM_STANDARD; 

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

            hw125_puts(mtbdl_log.data_str); 
            mtbdl_log.data_buff_index = CLEAR; 
        }
        else 
        {
            // For every interrupt that occurs that doesn't trigger a logging stream, the 
            // latest ADC data is formatted and stored so that it can be compiled and 
            // written to the SD card when a logging stream occurs later. A buffer that 
            // can store multiple intervals of ADC data is used so that old intervals can 
            // still be formatted and recorded even if interrupts have triggered new ADC 
            // conversions before this has had a chance to run. 

            snprintf(mtbdl_log.data_buff[mtbdl_log.data_buff_index], 
                     MTBDL_MAX_STR_LEN, 
                     mtbdl_data_log_default, 
                     mtbdl_log.trailmark, 
                     mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_FORK], 
                     mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_SHOCK]); 

            mtbdl_log.data_buff_index++; 
        }
        
        // The trail marker flag gets cleared at the end so that it's status can be used 
        // in the strings that will be logged to the SD card. 
        mtbdl_log.trailmark = CLEAR_BIT; 
    }
}


// Data logging ADC interrupt callback 
void log_data_adc_handler(void)
{
    handler_flags.tim1_trg_tim11_glbl_flag = CLEAR_BIT; 
    mtbdl_log.interrupt_counter++; 

    if (mtbdl_log.log_interval_divider < LOG_PERIOD_DIVIDER)
    {
        mtbdl_log.adc_period[mtbdl_log.log_interval_divider][ADC_SOC] = 
            mtbdl_log.adc_buff[ADC_SOC]; 
        
        mtbdl_log.adc_period[mtbdl_log.log_interval_divider][ADC_FORK] = 
            mtbdl_log.adc_buff[ADC_FORK]; 
        
        mtbdl_log.adc_period[mtbdl_log.log_interval_divider][ADC_SHOCK] = 
            mtbdl_log.adc_buff[ADC_SHOCK]; 

        mtbdl_log.log_interval_divider++; 
    }

    adc_start(mtbdl_log.adc); 
}


// Standard logging stream 
void log_stream_standard(void)
{
    // Format standard (ADC) log string 
    snprintf(mtbdl_log.data_str, 
             LOG_MAX_LOG_LEN, 
             mtbdl_data_log_adc, 
             mtbdl_log.data_buff[BYTE_0], 
             mtbdl_log.data_buff[BYTE_1], 
             mtbdl_log.data_buff[BYTE_2], 
             mtbdl_log.data_buff[BYTE_3], 
             mtbdl_log.trailmark, 
             mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_FORK], 
             mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_SHOCK]); 
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

    m8q_set_read_flag(); 
    m8q_controller(); 
    m8q_set_idle_flag(); 

    m8q_get_position_lat_str(mtbdl_log.lat_str, LOG_GPS_BUFF_LEN); 
    mtbdl_log.NS = m8q_get_position_NS(); 
    m8q_get_position_lon_str(mtbdl_log.lon_str, LOG_GPS_BUFF_LEN); 
    mtbdl_log.EW = m8q_get_position_EW(); 
    m8q_get_position_sog_str(mtbdl_log.sog_str, LOG_GPS_BUFF_LEN); 

    // Format GPS data log string 
    snprintf(mtbdl_log.data_str, 
             LOG_MAX_LOG_LEN, 
             mtbdl_data_log_gps, 
             mtbdl_log.data_buff[BYTE_0], 
             mtbdl_log.data_buff[BYTE_1], 
             mtbdl_log.data_buff[BYTE_2], 
             mtbdl_log.data_buff[BYTE_3], 
             mtbdl_log.trailmark, 
             mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_FORK], 
             mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_SHOCK], 
             (char *)mtbdl_log.sog_str, 
             (char *)mtbdl_log.lat_str, 
             (char)mtbdl_log.NS, 
             (char *)mtbdl_log.lon_str, 
             (char)mtbdl_log.EW); 
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

    mpu6050_set_read_flag(DEVICE_ONE); 
    mpu6050_controller(DEVICE_ONE); 

    mpu6050_get_accel_raw(
        DEVICE_ONE, 
        &mtbdl_log.accel_x, 
        &mtbdl_log.accel_y, 
        &mtbdl_log.accel_z); 

    // Format accelerometer data log string 
    snprintf(mtbdl_log.data_str, 
             LOG_MAX_LOG_LEN, 
             mtbdl_data_log_accel, 
             mtbdl_log.data_buff[BYTE_0], 
             mtbdl_log.data_buff[BYTE_1], 
             mtbdl_log.data_buff[BYTE_2], 
             mtbdl_log.data_buff[BYTE_3], 
             mtbdl_log.trailmark, 
             mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_FORK], 
             mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_SHOCK], 
             mtbdl_log.accel_x, 
             mtbdl_log.accel_y, 
             mtbdl_log.accel_z); 
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

    // This is used because mtbdl_log.rev_count gets updated by an interrupt. 
    uint8_t revs = CLEAR; 

    mtbdl_log.rev_buff[mtbdl_log.rev_buff_index++] = mtbdl_log.rev_count; 
    mtbdl_log.rev_count = CLEAR; 

    if (mtbdl_log.rev_buff_index >= LOG_REV_SAMPLE_SIZE)
    {
        mtbdl_log.rev_buff_index = CLEAR; 
    }

    for (uint8_t i = CLEAR; i < LOG_REV_SAMPLE_SIZE; i++)
    {
        revs += mtbdl_log.rev_buff[i]; 
    }

    // Format wheel speed data log string 
    snprintf(mtbdl_log.data_str, 
             LOG_MAX_LOG_LEN, 
             mtbdl_data_log_speed, 
             mtbdl_log.data_buff[BYTE_0], 
             mtbdl_log.data_buff[BYTE_1], 
             mtbdl_log.data_buff[BYTE_2], 
             mtbdl_log.data_buff[BYTE_3], 
             mtbdl_log.trailmark, 
             mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_FORK], 
             mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_SHOCK], 
             revs); 
}


// Log file close 
void log_data_end(void)
{
    // Disable interrupts 
    NVIC_DisableIRQ(mtbdl_log.rpm_irq);   // Wheel speed 
    NVIC_DisableIRQ(mtbdl_log.log_irq);   // Log sample period 

    // Terminate and close the log file then update the log index now that a new log 
    // file has been created, written to and stored. 

    snprintf(mtbdl_log.data_str, 
             LOG_MAX_LOG_LEN, 
             mtbdl_data_log_end, 
             mtbdl_log.overrun); 
    hw125_puts(mtbdl_log.data_str); 
    // hw125_puts(mtbdl_data_log_end); 
    hw125_close(); 
    param_update_log_index(PARAM_LOG_INDEX_INC); 
}

//=======================================================================================


//=======================================================================================
// Calibration functions 

// Calibration data prep 
void log_calibration_prep(void)
{
    // ADC data 
    memset((void *)mtbdl_log.adc_period, CLEAR, sizeof(mtbdl_log.adc_period)); 

    // Logging counters 
    mtbdl_log.log_interval_divider = CLEAR; 
    mtbdl_log.accel_stream_counter = stream_schedule[LOG_STREAM_ACCEL].offset; 
    mtbdl_log.interrupt_counter = CLEAR; 
    
    // Calibration data 
    memset((void *)mtbdl_log.cal_buff, CLEAR, sizeof(mtbdl_log.cal_buff)); 
    mtbdl_log.cal_adc_samples = CLEAR; 
    mtbdl_log.cal_accel_samples = CLEAR; 

    // SD card data 
    memset((void*)mtbdl_log.data_buff, CLEAR, sizeof(mtbdl_log.data_buff)); 
    memset((void*)mtbdl_log.data_str, CLEAR, sizeof(mtbdl_log.data_str)); 
    mtbdl_log.data_buff_index = CLEAR; 

    // Enable log sample period interrupts 
    NVIC_EnableIRQ(mtbdl_log.log_irq); 
}


// Calibration 
void log_calibration(void)
{
    if (mtbdl_log.interrupt_counter)
    {
        if (mtbdl_log.log_interval_divider >= LOG_PERIOD_DIVIDER)
        {
            mtbdl_log.log_interval_divider = CLEAR; 

            if (++mtbdl_log.accel_stream_counter >= 
                    stream_schedule[LOG_STREAM_ACCEL].counter_period)
            {
                mtbdl_log.accel_stream_counter = CLEAR; 
                mtbdl_log.cal_accel_samples++; 

                log_stream_accel(); 

                mtbdl_log.cal_buff[PARAM_SYS_SET_AX_REST] += (int32_t)mtbdl_log.accel_x; 
                mtbdl_log.cal_buff[PARAM_SYS_SET_AY_REST] += (int32_t)mtbdl_log.accel_y; 
                mtbdl_log.cal_buff[PARAM_SYS_SET_AZ_REST] += (int32_t)mtbdl_log.accel_z; 
            }
        }

        mtbdl_log.cal_buff[PARAM_SYS_SET_FORK_REST] += 
            (int32_t)mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_FORK]; 
        mtbdl_log.cal_buff[PARAM_SYS_SET_SHOCK_REST] += 
            (int32_t)mtbdl_log.adc_period[mtbdl_log.data_buff_index][ADC_SHOCK]; 
        
        if (++mtbdl_log.data_buff_index >= LOG_PERIOD_DIVIDER)
        {
            mtbdl_log.data_buff_index = CLEAR; 
        }

        mtbdl_log.cal_adc_samples++; 
        mtbdl_log.interrupt_counter--; 
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
        (int16_t)(mtbdl_log.cal_buff[PARAM_SYS_SET_AX_REST] / mtbdl_log.cal_accel_samples); 
    
    mtbdl_log.accel_y = 
        (int16_t)(mtbdl_log.cal_buff[PARAM_SYS_SET_AY_REST] / mtbdl_log.cal_accel_samples); 
    
    mtbdl_log.accel_z = 
        (int16_t)(mtbdl_log.cal_buff[PARAM_SYS_SET_AZ_REST] / mtbdl_log.cal_accel_samples); 
    
    mtbdl_log.adc_buff[ADC_FORK] = 
        (uint16_t)(mtbdl_log.cal_buff[PARAM_SYS_SET_FORK_REST] / mtbdl_log.cal_adc_samples); 
    
    mtbdl_log.adc_buff[ADC_SHOCK] = 
        (uint16_t)(mtbdl_log.cal_buff[PARAM_SYS_SET_SHOCK_REST] / mtbdl_log.cal_adc_samples); 

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


//=======================================================================================
// Getters 

// Get battery voltage (ADC value) 
uint16_t log_get_batt_voltage(void)
{
    // Update the current ADC values if the data logging periodic interrupt is not 
    // enabled (i.e. if data logging or calibration is not happening). 
    if (!NVIC_GetEnableIRQ(mtbdl_log.log_irq))
    {
        uint16_t timeout = LOG_ADC_DMA_WAIT; 

        dma_clear_int_flags(mtbdl_log.dma); 
        adc_start(mtbdl_log.adc); 
        while(!dma_get_tc_status(mtbdl_log.dma, mtbdl_log.dma_stream) && --timeout); 
    }

    return mtbdl_log.adc_buff[ADC_SOC]; 
}

//=======================================================================================
