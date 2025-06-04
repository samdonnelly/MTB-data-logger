/**
 * @file mtbdl_setup.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB data logger setup 
 * 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

//=======================================================================================
// Includes 

#include "mtbdl.h" 

//=======================================================================================


//=======================================================================================
// Macros 

// MPU-6050 config 
#define MPU6050_STBY_MASK 0x00           // Axis standby status mask 
#define MPU6050_SMPLRT_DIVIDER 0         // Sample Rate Divider 
#define MPU6050_RATE 250000              // Time between reading new data (us) 

//=======================================================================================


//=======================================================================================
// Initialization 

// User init function 
void mtbdl_init()
{
    //==================================================
    // General setup 

    // Initialize GPIO ports 
    gpio_port_init(); 

    // Initialize interrupt handler flags 
    int_handler_init(); 
    
    //==================================================

    //==================================================
    // Timers 

    // General purpose 1us counter 
    tim_9_to_11_counter_init(
        TIM9, 
        TIM_84MHZ_1US_PSC, 
        0xFFFF,  // Max ARR value 
        TIM_UP_INT_DISABLE); 
    tim_enable(TIM9); 

    // Periodic (counter update) interrupt timer for user button status checks and LED 
    // updates. The interrupt is enabled at the end of the setup. 
    tim_9_to_11_counter_init(
        TIM10, 
        TIM_84MHZ_100US_PSC, 
        0x0032,  // ARR=50, (50 counts)*(100us/count) = 5ms 
        TIM_UP_INT_ENABLE); 
    tim_enable(TIM10); 

    // Periodic (counter update) interrupt timer for data log timing. The interrupt is 
    // further configured at the end of the setup. 
    tim_9_to_11_counter_init(
        TIM11, 
        TIM_84MHZ_100US_PSC, 
        0x0064,  // ARR=100, (100 counts)*(100us/count) = 10ms 
        TIM_UP_INT_ENABLE); 
    tim_enable(TIM11); 

    //==================================================

    //==================================================
    // I2C setup 

    // For MPU-6050, LCD screen and M8Q GPS 
    i2c_init(
        I2C1, 
        PIN_9, 
        GPIOB, 
        PIN_8, 
        GPIOB, 
        I2C_MODE_SM,
        I2C_APB1_42MHZ,
        I2C_CCR_SM_42_100,
        I2C_TRISE_1000_42);
    
    //==================================================

    //==================================================
    // SPI setup 

    // For SD card 
    spi_init(
        SPI2, 
        GPIOB, 
        PIN_10,             // SCK pin 
        GPIOB, 
        PIN_14,             // MISO pin 
        PIN_15,             // MOSI pin 
        SPI_BR_FPCLK_8, 
        SPI_CLOCK_MODE_0); 

    // SD card slave select pin setup 
    spi_ss_init(GPIOB, PIN_12); 

    //==================================================

    //==================================================
    // UART setup 

    // Serial terminal 
    uart_init(
        USART2, 
        GPIOA, 
        PIN_3, 
        PIN_2, 
        UART_PARAM_DISABLE, 
        CLEAR, 
        UART_FRAC_42_9600, 
        UART_MANT_42_9600, 
        UART_PARAM_DISABLE, 
        UART_PARAM_ENABLE); 

    // HC-05 
    uart_init(
        USART1, 
        GPIOA, 
        PIN_10, 
        PIN_9, 
        UART_PARAM_DISABLE, 
        CLEAR, 
        UART_FRAC_84_115200, 
        UART_MANT_84_115200, 
        UART_PARAM_DISABLE, 
        UART_PARAM_DISABLE); 

    //==================================================

    //==================================================
    // ADC setup 

    // Initialize the ADC port (called once) 
    adc1_clock_enable(RCC); 
    adc_port_init(
        ADC1, 
        ADC1_COMMON, 
        ADC_PCLK2_4, 
        ADC_RES_10, 
        ADC_PARAM_ENABLE,      // ADC_EOC_EACH 
        ADC_PARAM_DISABLE,     // ADC_EOC_INT_DISABLE 
        ADC_PARAM_ENABLE,      // ADC_SCAN_ENABLE 
        ADC_PARAM_DISABLE,     // ADC_CONT_DISABLE 
        ADC_PARAM_ENABLE,      // ADC_DMA_ENABLE 
        ADC_PARAM_ENABLE,      // ADC_DDS_ENABLE 
        ADC_PARAM_DISABLE);    // ADC_OVR_INT_DISABLE 

    // Initialize the ADC pins and channels 
    adc_pin_init(ADC1, GPIOA, PIN_6, ADC_CHANNEL_6, ADC_SMP_15);   // Battery 
    adc_pin_init(ADC1, GPIOA, PIN_7, ADC_CHANNEL_7, ADC_SMP_15);   // Fork 
    adc_pin_init(ADC1, GPIOA, PIN_4, ADC_CHANNEL_4, ADC_SMP_15);   // Shock 

    // Set the ADC conversion sequence 
    adc_seq(ADC1, ADC_CHANNEL_6, ADC_SEQ_1);   // Battery 
    adc_seq(ADC1, ADC_CHANNEL_7, ADC_SEQ_2);   // Fork 
    adc_seq(ADC1, ADC_CHANNEL_4, ADC_SEQ_3);   // Shock 

    // Set the sequence length (called once and only for more than one channel) 
    adc_seq_len_set(ADC1, (adc_seq_num_t)ADC_BUFF_SIZE); 

    // Turn the ADC on 
    adc_on(ADC1); 

    //==================================================

    //==================================================
    // DMA setup 

    // Initialize the DMA stream 
    dma_stream_init(
        DMA2, 
        DMA2_Stream0, 
        DMA_CHNL_0, 
        DMA_DIR_PM, 
        DMA_CM_ENABLE,
        DMA_PRIOR_VHI, 
        DMA_DBM_DISABLE,        // Double buffer mode configuration 
        DMA_ADDR_INCREMENT, 
        DMA_ADDR_FIXED, 
        DMA_DATA_SIZE_HALF, 
        DMA_DATA_SIZE_HALF); 

    // Configure the DMA stream 
    // The ADC DMA stream is configured in the data logging module init function. This 
    // is done so the ADC buffer in the data logging module can be used. The stream 
    // must be configured before we can enable the DMA. 

    // The DMA is enabled at the end of the setup so that all configuration can be 
    // done before enabling. 

    //==================================================

    //==================================================
    // External interrupts 

    // Initialize external interrupts 
    exti_init(); 

    // Wheel revolutions interrupt configuration 
    exti_config(
        GPIOC, 
        EXTI_PC, 
        PIN_4, 
        PUPDR_PU, 
        EXTI_L4, 
        EXTI_INT_NOT_MASKED, 
        EXTI_EVENT_MASKED, 
        EXTI_RISE_TRIG_DISABLE, 
        EXTI_FALL_TRIG_ENABLE); 

    // Further interrupt setup is done at the end. 

    //==================================================

    //==================================================
    // HD44780U LCD setup 

    // Must come before setup of other devices on the same I2C bus 

    // Driver 
    hd44780u_init(I2C1, TIM9, PCF8574_ADDR_HHH);

    // Contoller 
    hd44780u_controller_init(TIM9); 

    //==================================================

    //==================================================
    // MPU-6050 IMU setup 

    // Driver 
    mpu6050_init(
        DEVICE_ONE, 
        I2C1, 
        MPU6050_ADDR_1,
        MPU6050_STBY_MASK, 
        MPU6050_DLPF_CFG_1,
        MPU6050_SMPLRT_DIVIDER,
        MPU6050_AFS_SEL_4,
        MPU6050_FS_SEL_500); 

    // Controller 
    mpu6050_controller_init(
        DEVICE_ONE, 
        TIM9, 
        MPU6050_RATE); 
    // Set the sample type to accelerometer and read method to read on request 
    mpu6050_set_read_state(DEVICE_ONE, MPU6050_READ_READY); 

    //==================================================

    //==================================================
    // M8Q GPS setup 

    // M8Q device setup 
    m8q_init(
        I2C1, 
        &m8q_config_msgs[0][0], 
        M8Q_CONFIG_MSG_NUM, 
        M8Q_CONFIG_MSG_MAX_LEN, 
        CLEAR); 

    // Set up low power and TX ready pins 
    m8q_pwr_pin_init(GPIOC, PIN_10); 
    m8q_txr_pin_init(GPIOC, PIN_11); 

    // Controller 
    m8q_controller_init(TIM9); 
    
    //==================================================

    //==================================================
    // HC-05 Bluetooth setup 

    // HC-05 driver 
    hc05_init(
        USART1, 
        TIM9, 
        GPIOA,          // AT pin GPIO 
        PIN_8,          // AT pin 
        GPIOA,          // EN pin GPIO 
        PIN_12,         // EN pin 
        GPIOA,          // STATE pin GPIO 
        PIN_11);        // STATE pin 
    
    //==================================================

    //==================================================
    // SD card setup 

    // User initialization 
    hw125_user_init(SPI2, GPIOB, GPIOX_PIN_12); 

    // Controller init 
    hw125_controller_init(mtbdl_dir); 

    //==================================================

    //==================================================
    // LED setup 

    // WS2812 (Neopixels) 
    ws2812_init(
        DEVICE_ONE, 
        TIM3, 
        TIMER_CH1, 
        GPIOC, 
        PIN_6); 
    
    //==================================================

    //==================================================
    // Main setup 

    // System information 
    mtbdl_trackers.state = MTBDL_INIT_STATE; 
    mtbdl_trackers.fault_code = CLEAR; 

    // Timing information 
    mtbdl_trackers.timer_nonblocking = TIM9; 
    mtbdl_trackers.delay_timer.clk_freq = tim_get_pclk_freq(mtbdl_trackers.timer_nonblocking); 
    mtbdl_trackers.delay_timer.time_cnt_total = CLEAR; 
    mtbdl_trackers.delay_timer.time_cnt = CLEAR; 
    mtbdl_trackers.delay_timer.time_start = SET_BIT; 
    mtbdl_trackers.led_state = CLEAR; 

    // Screen messages 
    memset((void *)mtbdl_trackers.msg, CLEAR, sizeof(mtbdl_trackers.msg)); 
    mtbdl_trackers.msg_len = CLEAR; 

    // User buttons 
    mtbdl_trackers.btn_press = UI_BTN_NONE; 

    // State flags 
    mtbdl_trackers.init = SET_BIT; 
    mtbdl_trackers.idle = CLEAR_BIT; 
    mtbdl_trackers.run = CLEAR_BIT; 
    mtbdl_trackers.data_select = CLEAR_BIT; 
    mtbdl_trackers.tx = CLEAR_BIT; 
    mtbdl_trackers.rx = CLEAR_BIT; 
    mtbdl_trackers.calibrate = CLEAR_BIT; 
    mtbdl_trackers.low_pwr = CLEAR_BIT; 
    mtbdl_trackers.noncrit_fault = CLEAR_BIT; 
    mtbdl_trackers.fault = CLEAR_BIT; 
    mtbdl_trackers.reset = CLEAR_BIT; 

    // Create a directory for logging fault codes if it does not already exit 
    hw125_mkdir(mtbdl_fault_dir); 

    //==================================================

    //==================================================
    // User interface setup 

    ui_init(GPIOC, PIN_0, PIN_1, PIN_2, PIN_3); 

    //==================================================

    //==================================================
    // Data logging setup 

    // This function handles DMA stream init so that the correct ADC buffer can be used. 
    log_init(
        EXTI4_IRQn, 
        TIM1_TRG_COM_TIM11_IRQn, 
        ADC1, 
        DMA2, 
        DMA2_Stream0); 
    
    //==================================================

    //==================================================
    // System parameters setup 

    param_init(); 
    
    //==================================================

    //==================================================
    // Finalize setup 

    // Enable the DMA stream. This is done here because the data logging module does 
    // further DMA configuration after the DMA setup section. 
    dma_stream_enable(DMA2_Stream0); 

    // Periodic interrupt (button and LED updates): Enable the interrupt handler 
    nvic_config(TIM1_UP_TIM10_IRQn, EXTI_PRIORITY_2); 

    // Periodic interrupt (data logging): Set the interrupt priority and disable until 
    // data logging starts 
    NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, EXTI_PRIORITY_1); 
    NVIC_DisableIRQ(TIM1_TRG_COM_TIM11_IRQn); 

    // External interrupt (wheel speed sensor): Set the interrupt priority and disable 
    // until data logging starts 
    NVIC_SetPriority(EXTI4_IRQn, EXTI_PRIORITY_0); 
    NVIC_DisableIRQ(EXTI4_IRQn); 

    //==================================================
}

//=======================================================================================
