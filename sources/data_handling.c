/**
 * @file data_handling.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL data handling 
 * 
 * @version 0.1
 * @date 2023-05-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//=======================================================================================
// Includes 

#include "data_handling.h" 

//=======================================================================================


//=======================================================================================
// Function prototypes 

/**
 * @brief Format and write the bike parameters 
 * 
 * @details 
 */
void mtbdl_format_write_bike_params(void); 


/**
 * @brief Read and format the bike parameters 
 * 
 * @details 
 */
void mtbdl_format_read_bike_params(void); 


/**
 * @brief Format and write the system parameters 
 * 
 * @details 
 */
void mtbdl_format_write_sys_params(void); 


/**
 * @brief Read and format the system parameters 
 * 
 * @details 
 */
void mtbdl_format_read_sys_params(void); 


/**
 * @brief 
 * 
 * @details 
 */
void mtbdl_log_stream_standard(void); 


/**
 * @brief 
 * 
 * @details 
 */
void mtbdl_log_stream_blink(void); 


/**
 * @brief 
 * 
 * @details 
 */
void mtbdl_log_stream_speed(void); 


/**
 * @brief 
 * 
 * @details 
 */
void mtbdl_log_stream_accel(void); 


/**
 * @brief 
 * 
 * @details 
 */
void mtbdl_log_stream_gps(void); 


/**
 * @brief 
 * 
 * @details 
 */
void mtbdl_log_stream_user(void); 

//=======================================================================================


//=======================================================================================
// Variables 

// Data record instance 
static mtbdl_data_t mtbdl_data; 


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
void mtbdl_data_init(
    IRQn_Type rpm_irqn, 
    IRQn_Type log_irqn, 
    ADC_TypeDef *adc)
{
    // Peripherals 
    mtbdl_data.rpm_irq = rpm_irqn; 
    mtbdl_data.log_irq = log_irqn; 
    mtbdl_data.adc = adc; 

    // Bike parameters 
    mtbdl_data.fork_psi = CLEAR; 
    mtbdl_data.fork_comp = CLEAR; 
    mtbdl_data.fork_reb = CLEAR; 
    mtbdl_data.shock_psi = CLEAR; 
    mtbdl_data.shock_lock = CLEAR; 
    mtbdl_data.shock_reb = CLEAR; 

    // System parameters 
    mtbdl_data.log_index = CLEAR; 
    mtbdl_data.accel_x_rest = CLEAR; 
    mtbdl_data.accel_y_rest = CLEAR; 
    mtbdl_data.accel_z_rest = CLEAR; 
    mtbdl_data.pot_fork_rest = CLEAR; 
    mtbdl_data.pot_shock_rest = CLEAR; 

    // SD card 
    memset((void *)mtbdl_data.data_buff, CLEAR, sizeof(mtbdl_data.data_buff)); 
    memset((void *)mtbdl_data.filename, CLEAR, sizeof(mtbdl_data.filename)); 
    mtbdl_data.tx_status = CLEAR_BIT; 

    // LEDs 
    memset((void *)mtbdl_data.led_colour_data, CLEAR, sizeof(mtbdl_data.led_colour_data)); 

    // System data 
    memset((void *)mtbdl_data.adc_buff, CLEAR, sizeof(mtbdl_data.adc_buff)); 
    mtbdl_data.navstat = M8Q_NAVSTAT_NF; 
    memset((void *)mtbdl_data.deg_min_lat, CLEAR, sizeof(mtbdl_data.deg_min_lat)); 
    memset((void *)mtbdl_data.min_frac_lat, CLEAR, sizeof(mtbdl_data.min_frac_lat)); 
    mtbdl_data.NS = CLEAR; 
    memset((void *)mtbdl_data.deg_min_lon, CLEAR, sizeof(mtbdl_data.deg_min_lon)); 
    memset((void *)mtbdl_data.min_frac_lon, CLEAR, sizeof(mtbdl_data.min_frac_lon)); 
    mtbdl_data.EW = CLEAR; 
    mtbdl_data.accel_x = CLEAR; 
    mtbdl_data.accel_y = CLEAR; 
    mtbdl_data.accel_z = CLEAR; 
}


// File system setup 
void mtbdl_file_sys_setup(void)
{
    // Create "parameters" and "data" directories if they do not already exist 
    hw125_mkdir(mtbdl_data_dir); 
    hw125_mkdir(mtbdl_param_dir); 

    // Check for the existance of the bike parameters file 
    if (hw125_get_exists(mtbdl_bike_param_file) == FR_NO_FILE)
    {
        // No file - create one and write default parameter data to it 
        mtbdl_write_bike_params(HW125_MODE_WW); 
    }
    else 
    {
        // File already exists - open the file for reading 
        mtbdl_read_bike_params(HW125_MODE_OEWR); 
    }

    // Check for the existance of the system parameters file 
    if (hw125_get_exists(mtbdl_sys_param_file) == FR_NO_FILE)
    {
        // No file - create one and write default parameter data to it 
        mtbdl_write_sys_params(HW125_MODE_WW); 
    }
    else 
    {
        // File already exists - open the file for reading 
        mtbdl_read_sys_params(HW125_MODE_OEWR); 
    }
}


// ADC DMA setup 
void mtbdl_adc_dma_init(
    DMA_Stream_TypeDef *dma_strea, 
    ADC_TypeDef *adc)
{
    // Configure the DMA stream 
    dma_stream_config(
        dma_strea, 
        (uint32_t)(&adc->DR), 
        (uint32_t)mtbdl_data.adc_buff, 
        (uint16_t)MTBDL_ADC_BUFF_SIZE); 
}

//=======================================================================================


//=======================================================================================
// Parameters 

// Write bike parameters to file 
void mtbdl_write_bike_params(
    uint8_t mode)
{
    // Move to the parameters directory 
    hw125_set_dir(mtbdl_param_dir); 

    // Open the file for writing 
    hw125_open(mtbdl_bike_param_file, mode); 

    // Format and write the bike parameters 
    mtbdl_format_write_bike_params(); 

    // Close the file 
    hw125_close(); 
}


// Read bike parameter on file 
void mtbdl_read_bike_params(
    uint8_t mode)
{
    // Move to the parameters directory 
    hw125_set_dir(mtbdl_param_dir); 

    // Open the file for reading 
    hw125_open(mtbdl_bike_param_file, mode); 

    // Read and format the bikr parameters 
    mtbdl_format_read_bike_params(); 

    // Close the file 
    hw125_close(); 
}


// Write system parameters to file 
void mtbdl_write_sys_params(
    uint8_t mode)
{
    // Move to the parameters directory 
    hw125_set_dir(mtbdl_param_dir); 
    
    // Open the file for writing 
    hw125_open(mtbdl_sys_param_file, mode); 

    // Format and write the system parameters 
    mtbdl_format_write_sys_params(); 

    // Close the file 
    hw125_close(); 
}


// Read system parameters on file 
void mtbdl_read_sys_params(
    uint8_t mode)
{
    // Move to the parameters directory 
    hw125_set_dir(mtbdl_param_dir); 

    // Open the file for reading 
    hw125_open(mtbdl_sys_param_file, mode); 

    // Read and format the system parameters 
    mtbdl_format_read_sys_params(); 

    // Close the file 
    hw125_close(); 
}


// Format and write the bike parameters 
void mtbdl_format_write_bike_params(void)
{
    // Write fork parameters 
    snprintf(mtbdl_data.data_buff, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_param_fork_info, 
             mtbdl_data.fork_psi, 
             mtbdl_data.fork_comp, 
             mtbdl_data.fork_reb); 
    
    hw125_puts(mtbdl_data.data_buff); 

    // Write shock parameters 
    snprintf(mtbdl_data.data_buff, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_param_shock_info, 
             mtbdl_data.shock_psi, 
             mtbdl_data.shock_lock, 
             mtbdl_data.shock_reb); 
    
    hw125_puts(mtbdl_data.data_buff); 
}


// Read and format the bike parameters 
void mtbdl_format_read_bike_params(void)
{
    // Read fork parameters 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    sscanf(mtbdl_data.data_buff, 
           mtbdl_param_fork_info, 
           &mtbdl_data.fork_psi, 
           &mtbdl_data.fork_comp, 
           &mtbdl_data.fork_reb); 

    // Read shock parameters 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    sscanf(mtbdl_data.data_buff, 
           mtbdl_param_shock_info, 
           &mtbdl_data.shock_psi, 
           &mtbdl_data.shock_lock, 
           &mtbdl_data.shock_reb); 
}


// Format and write the system parameters 
void mtbdl_format_write_sys_params(void)
{
    // Write logging parameters 
    snprintf(mtbdl_data.data_buff, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_param_index, 
             mtbdl_data.log_index); 

    hw125_puts(mtbdl_data.data_buff); 
    
    // Write accelerometer calibration data 
    snprintf(mtbdl_data.data_buff, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_param_accel_rest, 
             mtbdl_data.accel_x_rest, 
             mtbdl_data.accel_y_rest, 
             mtbdl_data.accel_z_rest); 
    
    hw125_puts(mtbdl_data.data_buff); 

    // Write potentiometer calibrated starting points 
    snprintf(mtbdl_data.data_buff, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_param_pot_rest, 
             mtbdl_data.pot_fork_rest, 
             mtbdl_data.pot_shock_rest); 
    
    hw125_puts(mtbdl_data.data_buff); 
}


// Read and format the system parameters 
void mtbdl_format_read_sys_params(void)
{
    // Read logging parameters 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    sscanf(mtbdl_data.data_buff, 
           mtbdl_param_index, 
           &mtbdl_data.log_index); 

    // Read accelerometer calibration data 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    sscanf(mtbdl_data.data_buff, 
           mtbdl_param_accel_rest, 
           &mtbdl_data.accel_x_rest, 
           &mtbdl_data.accel_y_rest, 
           &mtbdl_data.accel_z_rest); 

    // Read potentiometer starting points 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    sscanf(mtbdl_data.data_buff, 
           mtbdl_param_pot_rest, 
           &mtbdl_data.pot_fork_rest, 
           &mtbdl_data.pot_shock_rest); 
}

//=======================================================================================


//=======================================================================================
// Data logging 

// Log name preparation 
uint8_t mtbdl_log_name_prep(void)
{
    // Check the data log index is within bounds 
    if (mtbdl_data.log_index > MTBDL_LOG_NUM_MAX)
    {
        // Too many log files on drive - don't create a new file name 
        return FALSE; 
    }

    // Number of log files is within the limit - generate a new log file name 
    snprintf(mtbdl_data.filename, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_log_file, 
             mtbdl_data.log_index); 

    return TRUE; 
}


// Log file prep 
void mtbdl_log_file_prep(void)
{
    // Move to the data directory 
    hw125_set_dir(mtbdl_data_dir); 

    if (hw125_open(mtbdl_data.filename, HW125_MODE_WWX) == FR_OK)
    {
        // File successfully created - write parameters and log info, update file index 
        mtbdl_format_write_bike_params(); 
        mtbdl_format_write_sys_params(); 

        // // Write file creation timestamp 
        // snprintf(mtbdl_data.data_buff, 
        //          MTBDL_MAX_DATA_STR_LEN, 
        //          mtbdl_param_time, 
        //          mtbdl_data); 
        // hw125_puts(mtbdl_data.data_buff); 

        // Write data logging info 
        snprintf(mtbdl_data.data_buff, 
                 MTBDL_MAX_DATA_STR_LEN, 
                 mtbdl_param_data, 
                 MTBDL_LOG_PERIOD, 
                 MTBDL_REV_LOG_FREQ, 
                 MTBDL_REV_SAMPLE_SIZE); 
        hw125_puts(mtbdl_data.data_buff); 
        
        hw125_puts(mtbdl_data_log_start); 
        mtbdl_data.log_index++; 
    }
}


// Log data prep 
void mtbdl_log_data_prep(void)
{
    // Wheel RPM info 
    mtbdl_data.rev_count = CLEAR; 
    mtbdl_data.rev_buff_index = CLEAR; 
    memset((void *)mtbdl_data.rev_buff, CLEAR, sizeof(mtbdl_data.rev_buff)); 

    // GPS - put into a controlled read state 
    m8q_set_read_ready(); 
    m8q_set_read_flag(); 

    // Log tracking 
    mtbdl_data.time_count = CLEAR; 
    mtbdl_data.stream_index = CLEAR; 
    mtbdl_data.led_toggle = CLEAR_BIT; 
    mtbdl_data.run_count = CLEAR_BIT; 
    mtbdl_data.trailmark = CLEAR_BIT; 
    mtbdl_data.user_input = CLEAR_BIT; 

#if MTBDL_DEBUG 
    mtbdl_data.time_stop = CLEAR; 
    mtbdl_data.time_limit = 999; 
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
            m8q_clear_read_flag(); 
        }
        else if (mtbdl_data.user_input) 
        {
            // User input stream - run when there are no other non-standard streams to run 
            mtbdl_data.log_stream = MTBDL_LOG_STREAM_USER; 
            mtbdl_data.user_input = CLEAR_BIT; 

            // Disable GPS reads 
            m8q_clear_read_flag(); 
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
        MTBDL_MAX_DATA_STR_LEN, 
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
    for (uint8_t i = 0; i < MTBDL_REV_SAMPLE_SIZE; i++)
    {
        revs += mtbdl_data.rev_buff[i]; 
    }

    // Format wheel speed data log string 
    snprintf(
        mtbdl_data.data_buff, 
        MTBDL_MAX_DATA_STR_LEN, 
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

        // Record new accel data 
        mpu6050_get_accel_raw(
            DEVICE_ONE, 
            &mtbdl_data.accel_x, 
            &mtbdl_data.accel_y, 
            &mtbdl_data.accel_z); 

        // Format accel data log string 
        snprintf(
            mtbdl_data.data_buff, 
            MTBDL_MAX_DATA_STR_LEN, 
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
    m8q_get_lat_str(mtbdl_data.deg_min_lat, mtbdl_data.min_frac_lat); 
    mtbdl_data.NS = m8q_get_NS(); 
    m8q_get_long_str(mtbdl_data.deg_min_lon, mtbdl_data.min_frac_lon); 
    mtbdl_data.EW = m8q_get_EW(); 

    // Format GPS data log string 
    snprintf(
        mtbdl_data.data_buff, 
        MTBDL_MAX_DATA_STR_LEN, 
        mtbdl_data_log_4, 
        mtbdl_data.trailmark, 
        mtbdl_data.adc_buff[MTBDL_ADC_FORK], 
        mtbdl_data.adc_buff[MTBDL_ADC_SHOCK], 
        (char *)mtbdl_data.deg_min_lat, 
        (char *)mtbdl_data.min_frac_lat, 
        (char)mtbdl_data.NS, 
        (char *)mtbdl_data.deg_min_lon, 
        (char *)mtbdl_data.min_frac_lon, 
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
    mtbdl_write_sys_params(HW125_MODE_OAWR); 

    // GPS - put back into a continuous read state 
    m8q_clear_read_ready(); 
    m8q_clear_read_flag(); 
}

//=======================================================================================


//=======================================================================================
// RX state functions 

// RX user interface start 
void mtbdl_rx_prep(void)
{
    hc05_send(mtbdl_rx_prompt); 
    hc05_clear(); 
}


// Read and assign the user input 
void mtbdl_rx(void)
{
    // Local variables 
    unsigned int param_index; 
    unsigned int temp_data; 

    // Read Bluetooth data if available 
    if (hc05_data_status())
    {
        // Read and parse the data from the HC-05 
        hc05_read(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 
        sscanf(mtbdl_data.data_buff, 
               mtbdl_rx_input, 
               &param_index, 
               &temp_data); 

        // Check for a data match 
        switch (param_index)
        {
            case MTBDL_PARM_FPSI:
                mtbdl_data.fork_psi = temp_data; 

                break;

            case MTBDL_PARM_FC:
                if (temp_data <= MTBDL_MAX_SUS_SETTING)
                {
                    mtbdl_data.fork_comp = temp_data; 
                }

                break;

            case MTBDL_PARM_FR:
                if (temp_data <= MTBDL_MAX_SUS_SETTING)
                {
                    mtbdl_data.fork_reb = temp_data; 
                }

                break;

            case MTBDL_PARM_SPSI:
                mtbdl_data.shock_psi = temp_data; 

                break;

            case MTBDL_PARM_SL:
                if (temp_data <= MTBDL_MAX_SUS_SETTING)
                {
                    mtbdl_data.shock_lock = temp_data; 
                }

                break;

            case MTBDL_PARM_SR:
                if (temp_data <= MTBDL_MAX_SUS_SETTING)
                {
                    mtbdl_data.shock_reb = temp_data; 
                }

                break;
            
            default:
                break;
        }

        // Provide a user prompt 
        mtbdl_rx_prep(); 
    }
}

//=======================================================================================


//=======================================================================================
// TX state functions 

// Check if there are no log files 
uint8_t mtbdl_tx_check(void)
{
    if (mtbdl_data.log_index != MTBDL_LOG_NUM_MIN)
    {
        return TRUE; 
    }

    return FALSE; 
}


// Prepare to send a data log file 
uint8_t mtbdl_tx_prep(void)
{
    // Check if there are no log files 
    if (!mtbdl_tx_check())
    {
        return FALSE; 
    }

    // Log files exist - generate a new log file name 
    // The log index is adjusted because it will be one ahead of the most recent log 
    // file number after the most recent log has been created 
    snprintf(mtbdl_data.filename, 
             MTBDL_MAX_DATA_STR_LEN, 
             mtbdl_log_file, 
             (mtbdl_data.log_index - MTBDL_DATA_INDEX_OFFSET)); 

    // Move to the data directory 
    hw125_set_dir(mtbdl_data_dir); 

    // Check for the existance of the specified file number 
    if (hw125_get_exists(mtbdl_data.filename) == FR_NO_FILE)
    {
        return FALSE; 
    }

    // Open the file 
    hw125_open(mtbdl_data.filename, HW125_MODE_OAWR); 

    return TRUE; 
}


// Transfer data log contents 
uint8_t mtbdl_tx(void)
{
    // Read a line from the data log 
    hw125_gets(mtbdl_data.data_buff, MTBDL_MAX_DATA_STR_LEN); 

    // Send the data over Bluetooth 
    hc05_send(mtbdl_data.data_buff); 

    // Check for end of file - if true we can stop the transaction 
    if (hw125_eof())
    {
        mtbdl_data.tx_status = SET_BIT; 
        return TRUE; 
    }

    return FALSE; 
}


// End the transmission 
void mtbdl_tx_end(void)
{
    hw125_close(); 

    if (mtbdl_data.tx_status)
    {
        // Transaction completed - delete the file and update the log index 
        hw125_unlink(mtbdl_data.filename); 
        mtbdl_data.tx_status = CLEAR_BIT; 
        mtbdl_data.log_index--; 
        mtbdl_write_sys_params(HW125_MODE_OAWR); 
    }
}

//=======================================================================================


//=======================================================================================
// Screen message formatting 

// Format the idle state message 
void mtbdl_set_idle_msg(void)
{
    // Local variables 
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_4_LINE]; 

    // Create an editable copy of the message 
    for (uint8_t i = 0; i < MTBDL_MSG_LEN_4_LINE; i++) msg[i] = mtbdl_idle_msg[i]; 

    // Format the message with data 
    // snprintf will NULL terminate the string at the screen line length so in order to use 
    // the last spot on the screen line the message length must be indexed up by one 
    snprintf(
        msg[HD44780U_L1].msg, 
        (HD44780U_LINE_LEN + MTBDL_DATA_INDEX_OFFSET), 
        mtbdl_idle_msg[HD44780U_L1].msg, 
        mtbdl_data.fork_psi, 
        mtbdl_data.fork_comp, 
        mtbdl_data.fork_reb); 
    
    snprintf(
        msg[HD44780U_L2].msg, 
        (HD44780U_LINE_LEN + MTBDL_DATA_INDEX_OFFSET), 
        mtbdl_idle_msg[HD44780U_L2].msg, 
        mtbdl_data.shock_psi, 
        mtbdl_data.shock_lock, 
        mtbdl_data.shock_reb); 
    
    snprintf(
        msg[HD44780U_L3].msg, 
        (HD44780U_LINE_LEN + MTBDL_DATA_INDEX_OFFSET), 
        mtbdl_idle_msg[HD44780U_L3].msg, 
        mtbdl_data.adc_buff[MTBDL_ADC_SOC], 
        (char)(mtbdl_data.navstat >> SHIFT_8), 
        (char)mtbdl_data.navstat); 

    // Set the screen message 
    hd44780u_set_msg(msg, MTBDL_MSG_LEN_4_LINE); 
}


// Format the run prep state message 
void mtbdl_set_run_prep_msg(void)
{
    // Local variables 
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_3_LINE]; 

    // Create an editable copy of the message 
    for (uint8_t i = 0; i < MTBDL_MSG_LEN_3_LINE; i++) msg[i] = mtbdl_run_prep_msg[i]; 

    // Convert the NAVSTAT code to an easily readable value 

    // Format the message with data 
    snprintf(
        msg[HD44780U_L1].msg, 
        HD44780U_LINE_LEN, 
        mtbdl_run_prep_msg[HD44780U_L1].msg, 
        (char)(mtbdl_data.navstat >> SHIFT_8), 
        (char)mtbdl_data.navstat); 

    // Set the screen message 
    hd44780u_set_msg(msg, MTBDL_MSG_LEN_3_LINE); 
}


// Format the pre TX state message 
void mtbdl_set_pretx_msg(void)
{
    // Local variables 
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_4_LINE]; 

    // Create an editable copy of the message 
    for (uint8_t i = 0; i < MTBDL_MSG_LEN_4_LINE; i++) msg[i] = mtbdl_pretx_msg[i]; 

    // Format the message with data 
    // The log index is adjusted because it will be one ahead of the most recent log 
    // file number after the most recent log has been created 
    snprintf(
        msg[HD44780U_L2].msg, 
        HD44780U_LINE_LEN, 
        mtbdl_pretx_msg[HD44780U_L2].msg, 
        (mtbdl_data.log_index - MTBDL_DATA_INDEX_OFFSET)); 

    // Set the screen message 
    hd44780u_set_msg(msg, MTBDL_MSG_LEN_4_LINE); 
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


// Update the navigation status 
uint8_t mtbdl_navstat_check(void)
{
    // Local variables 
    static uint16_t navstat_check = M8Q_NAVSTAT_NF; 
    uint8_t nav_stat_change = FALSE; 

    // Update the data record with the navigation status and return the status 
    mtbdl_data.navstat = m8q_get_navstat(); 

    if (mtbdl_data.navstat != navstat_check)
    {
        nav_stat_change = TRUE; 
        navstat_check = mtbdl_data.navstat; 
    }
    
    return nav_stat_change; 
}

//=======================================================================================
