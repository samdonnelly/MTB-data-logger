/**
 * @file user_init.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL main application code initialization 
 * 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

//=======================================================================================
// Includes 

#include "mtbdl_init.h"

//=======================================================================================


// User init function 
void mtbdl_init()
{
    // Local variables 

    //===================================================
    // General setup 

    // Initialize GPIO ports 
    gpio_port_init(); 

#if MTBDL_DEBUG 

    // Serial terminal output 
    uart_init(
        USART2, 
        GPIOA, 
        PIN_3, 
        PIN_2, 
        UART_BAUD_9600, 
        UART_CLOCK_42); 

#endif   // MTBDL_DEBUG 

    // Initialize interrupt handler flags 
    int_handler_init(); 
    
    //===================================================

    //===================================================
    // Timers 

    // General purpose 1us counter 
    tim_9_to_11_counter_init(
        TIM9, 
        TIM_84MHZ_1US_PSC, 
        0xFFFF,  // Max ARR value 
        TIM_UP_INT_DISABLE); 
    tim_enable(TIM9); 

    // Periodic (counter update) interrupt timer for user button status checks 
    tim_9_to_11_counter_init(
        TIM10, 
        TIM_84MHZ_100US_PSC, 
        0x0032,  // ARR=50, (50 counts)*(100us/count) = 5ms 
        TIM_UP_INT_ENABLE); 
    tim_enable(TIM10); 

    // Enable the interrupt handlers 
    nvic_config(TIM1_UP_TIM10_IRQn, EXTI_PRIORITY_1); 

    //===================================================

    //===================================================
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
    
    //===================================================

    //===================================================
    // SPI setup 

    // For SD card 
    spi_init(
        SPI2, 
        GPIOB, 
        PIN_10,  // SCK pin 
        PIN_14,  // MISO pin 
        PIN_15,  // MOSI pin 
        SPI_BR_FPCLK_8, 
        SPI_CLOCK_MODE_0); 

    // SD card slave select pin setup 
    spi_ss_init(GPIOB, PIN_12); 

    //===================================================

    //===================================================
    // UART setup 

    // For HC-05 
    uart_init(
        USART1, 
        GPIOA, 
        PIN_10, 
        PIN_9, 
        UART_BAUD_115200, 
        UART_CLOCK_84); 

    //===================================================

    //===================================================
    // ADC setup 
    //===================================================

    //===================================================
    // DMA setup 
    //===================================================

    //===================================================
    // MPU-6050 IMU setup 

    // Driver 
    mpu6050_init(
        DEVICE_ONE, 
        I2C1, 
        MPU6050_ADDR_1,
        MPU6050_STBY_MASK, 
        MPU6050_DLPF_CFG_1,
        MPU6050_SMPLRT_DIV,
        MPU6050_AFS_SEL_4,
        MPU6050_FS_SEL_500); 

    // Controller 
    mpu6050_controller_init(
        DEVICE_ONE, 
        TIM9, 
        MPU6050_RATE); 

    //===================================================

    //===================================================
    // HD44780U LCD setup 

    // Driver 
    hd44780u_init(I2C1, TIM9, PCF8574_ADDR_HHH);

    // Contoller 
    hd44780u_controller_init(TIM9); 

    //===================================================

    //===================================================
    // M8Q GPS setup 
    //===================================================

    //===================================================
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
    
    //===================================================

    //===================================================
    // SD card setup 

    // User initialization 
    hw125_user_init(SPI2, GPIOB, GPIOX_PIN_12); 

    // Controller init 
    hw125_controller_init(mtbdl_dir); 

    //===================================================

    //===================================================
    // User button setup 

    // Configure the GPIO inputs for each user button 
    gpio_pin_init(GPIOC, PIN_0, MODER_INPUT, OTYPER_PP, OSPEEDR_HIGH, PUPDR_PU); 
    gpio_pin_init(GPIOC, PIN_1, MODER_INPUT, OTYPER_PP, OSPEEDR_HIGH, PUPDR_PU); 
    gpio_pin_init(GPIOC, PIN_2, MODER_INPUT, OTYPER_PP, OSPEEDR_HIGH, PUPDR_PU); 
    gpio_pin_init(GPIOC, PIN_3, MODER_INPUT, OTYPER_PP, OSPEEDR_HIGH, PUPDR_PU); 

    // Initialize the button debouncer 
    debounce_init(GPIOX_PIN_0 | GPIOX_PIN_1 | GPIOX_PIN_2 | GPIOX_PIN_3); 

    //===================================================

    //===================================================
    // Wheel speed sensor setup (external interrupt) 

    // Enable external interrupts 
    exti_init(); 

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

    // Set the interrupt priority and disable 
    NVIC_SetPriority(EXTI4_IRQn, EXTI_PRIORITY_0); 
    NVIC_DisableIRQ(EXTI4_IRQn); 

    //===================================================

    //===================================================
    // LED setup 

    // WS2812 (Neopixels) 
    ws2812_init(
        DEVICE_ONE, 
        TIM3, 
        TIM_CHANNEL_1, 
        GPIOC, 
        PIN_6); 
    
    //===================================================

    //===================================================
    // System setup 

    // MTB DL application init 
    mtbdl_app_init(
        TIM9, 
        GPIOC, 
        GPIOX_PIN_0, 
        GPIOX_PIN_1, 
        GPIOX_PIN_2, 
        GPIOX_PIN_3); 

    // Data record init 
    mtbdl_data_init(EXTI4_IRQn); 

    //===================================================
}
