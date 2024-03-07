/**
 * @file mtbdl_app.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL main application code 
 * 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

//=======================================================================================
// Includes 

#include "mtbdl_interface.h" 

//=======================================================================================


//=======================================================================================
// Macros 

// System info 
#define MTBDL_NUM_STATES 20              // Number of system states 

// Timing info 
#define MTBDL_LCD_SLEEP 10000000         // (us) Inactive time before screen backlight off 
#define MTBDL_LCD_LP_SLEEP 3000000       // (us) Low power state message display time 
#define MTBDL_STATE_CHECK_SLOW 5000000   // (us) Long period time for non-blocking timer 
#define MTBDL_STATE_CHECK_NORM 1000000   // (us) Normal period time for non-blocking timer 
#define MTBDL_STATE_CHECK_FAST 100000    // (us) Short period time for non-blocking timer 

//=======================================================================================


//=======================================================================================
// Enums 

/**
 * @brief MTB data logger controller states 
 */
typedef enum {
    MTBDL_INIT_STATE,            // State 0  : Startup 
    MTBDL_IDLE_STATE,            // State 1  : Idle 
    MTBDL_RUN_PREP_STATE,        // State 2  : Run prep 
    MTBDL_RUN_COUNTDOWN_STATE,   // State 3  : Run countdown 
    MTBDL_RUN_STATE,             // State 4  : Run 
    MTBDL_POSTRUN_STATE,         // State 5  : Post-run 
    MTBDL_DATA_SELECT_STATE,     // State 6  : Data transfer selection 
    MTBDL_DEV_SEARCH_STATE,      // State 7  : Search for Bluetooth connection 
    MTBDL_PRERX_STATE,           // State 8  : Pre data receive 
    MTBDL_RX_STATE,              // State 9  : Data receive 
    MTBDL_POSTRX_STATE,          // State 10 : Post data receive 
    MTBDL_PRETX_STATE,           // State 11 : Pre data send 
    MTBDL_TX_STATE,              // State 12 : Data send 
    MTBDL_POSTTX_STATE,          // State 13 : Post data send 
    MTBDL_PRECALIBRATE_STATE,    // State 14 : Pre calibration 
    MTBDL_CALIBRATE_STATE,       // State 15 : Calibration 
    MTBDL_POSTCALIBRATE_STATE,   // State 16 : Post calibration 
    MTBDL_LOWPWR_STATE,          // State 17 : Low power mode 
    MTBDL_FAULT_STATE,           // State 18 : Fault 
    MTBDL_RESET_STATE            // State 19 : Reset 
} mtbdl_states_t; 


/**
 * @brief State check counter values 
 * 
 * @details 
 */
typedef enum {
    MTBDL_STATE_CHECK_CNT_1 = 1, 
    MTBDL_STATE_CHECK_CNT_2, 
    MTBDL_STATE_CHECK_CNT_3 
} mtbdl_state_check_count_t; 

//=======================================================================================


//=======================================================================================
// Structures 

// MTB DL system trackers 
typedef struct mtbdl_trackers_s 
{
    // System information 
    mtbdl_states_t state;                       // State of the system controller 
    uint16_t fault_code;                        // System fault code 
    GPIO_TypeDef *user_btn_port;                // GPIO port for user buttons 

    // Timing information 
    TIM_TypeDef *timer_nonblocking;             // Timer used for non-blocking delays 
    tim_compare_t delay_timer;                  // Delay timing info 
    uint8_t led_state;                          // LED counter used with non-blocking timer 

    // Screen message 
    hd44780u_msgs_t *msg;                       // Message content 
    uint8_t msg_len;                            // Message length (number of lines) 

    // User buttons 
    uint8_t user_btn_1;                         // User button 1 pin number 
    uint8_t user_btn_2;                         // User button 2 pin number 
    uint8_t user_btn_3;                         // User button 3 pin number 
    uint8_t user_btn_4;                         // User button 4 pin number 
    uint8_t user_btn_1_block : 1;               // Stops repeated actions on btn 1 press 
    uint8_t user_btn_2_block : 1;               // Stops repeated actions on btn 2 press 
    uint8_t user_btn_3_block : 1;               // Stops repeated actions on btn 3 press 
    uint8_t user_btn_4_block : 1;               // Stops repeated actions on btn 4 press 

    // State flags 
    uint8_t init          : 1;                  // Ensures the init state is run 
    uint8_t idle          : 1;                  // Idle state flag 
    uint8_t run           : 1;                  // Run state flag 
    uint8_t data_select   : 1;                  // Data transfer select state flag 
    uint8_t tx            : 1;                  // Send/transmit data state flag 
    uint8_t rx            : 1;                  // Read/receive data state flag 
    uint8_t calibrate     : 1;                  // Calibration state flag 
    uint8_t low_pwr       : 1;                  // Low power state flag 
    uint8_t noncrit_fault : 1;                  // Non-critical fault state flag 
    uint8_t fault         : 1;                  // Fault state flag 
    uint8_t reset         : 1;                  // Reset state flag 
}
mtbdl_trackers_t; 


// Instance of the system data trackers 
static mtbdl_trackers_t mtbdl_trackers; 

//=======================================================================================


//=======================================================================================
// Function pointers 

/**
 * @brief System state machine function pointer 
 */
typedef void (*mtbdl_func_ptr_t)(
    mtbdl_trackers_t *mtbdl); 

//=======================================================================================


//=======================================================================================
// Function prototypes 

/**
 * @brief Initialization state 
 * 
 * @details First state to run on system startup and after the main controller resets. 
 *          Used to set up or reset any devices or data. Defaults to the idle state when 
 *          done. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_init_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Idle state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_idle_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Run prep state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_run_prep_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Run countdown state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_run_countdown_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Run state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_run_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Post run state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_postrun_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Data transfer selection state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_data_select_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Search for Bluetooth connection state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_dev_search_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Pre RX state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_prerx_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief RX state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_rx_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Post RX state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_postrx_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief PRE TX state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_pretx_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief TX state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_tx_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Post TX state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_posttx_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Pre calibration state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_precalibrate_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Calibrate state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_calibrate_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Post calibration state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_postcalibrate_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Low power state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_lowpwr_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Fault state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_fault_state(
    mtbdl_trackers_t *mtbdl); 


/**
 * @brief Reset state 
 * 
 * @details 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_reset_state(
    mtbdl_trackers_t *mtbdl); 

//=======================================================================================


//=======================================================================================
// Global variables 

// Function pointers to system controller states 
static mtbdl_func_ptr_t mtbdl_state_table[MTBDL_NUM_STATES] = 
{
    &mtbdl_init_state,                  // State 0  : Startup 
    &mtbdl_idle_state,                  // State 1  : Idle 
    &mtbdl_run_prep_state,              // State 2  : Run prep 
    &mtbdl_run_countdown_state,         // State 3  : Run countdown 
    &mtbdl_run_state,                   // State 4  : Run 
    &mtbdl_postrun_state,               // State 5  : Post run 
    &mtbdl_data_select_state,           // State 6  : Data transfer selection 
    &mtbdl_dev_search_state,            // State 7  : Search for Bluetooth connection 
    &mtbdl_prerx_state,                 // State 8  : Pre data receive 
    &mtbdl_rx_state,                    // State 9  : Data receive 
    &mtbdl_postrx_state,                // State 10 : Post data receive 
    &mtbdl_pretx_state,                 // State 11 : Pre data send 
    &mtbdl_tx_state,                    // State 12 : Data send 
    &mtbdl_posttx_state,                // State 13 : Post data send 
    &mtbdl_precalibrate_state,          // State 14 : Pre calibration 
    &mtbdl_calibrate_state,             // State 15 : Calibration 
    &mtbdl_postcalibrate_state,         // State 16 : Post calibration 
    &mtbdl_lowpwr_state,                // State 17 : Low power mode 
    &mtbdl_fault_state,                 // State 18 : Fault 
    &mtbdl_reset_state                  // State 19 : Reset 
}; 

//=======================================================================================


//=======================================================================================
// System controller 

// MTB DL controller init 
void mtbdl_app_init(
    TIM_TypeDef *timer_nonblocking, 
    GPIO_TypeDef *user_btn_gpio, 
    gpio_pin_num_t user_btn_1, 
    gpio_pin_num_t user_btn_2, 
    gpio_pin_num_t user_btn_3, 
    gpio_pin_num_t user_btn_4)
{
    // System information 
    mtbdl_trackers.state = MTBDL_INIT_STATE; 
    mtbdl_trackers.fault_code = CLEAR; 
    mtbdl_trackers.user_btn_port = user_btn_gpio; 

    // Timing information 
    mtbdl_trackers.timer_nonblocking = timer_nonblocking; 
    mtbdl_trackers.delay_timer.clk_freq = tim_get_pclk_freq(timer_nonblocking); 
    mtbdl_trackers.delay_timer.time_cnt_total = CLEAR; 
    mtbdl_trackers.delay_timer.time_cnt = CLEAR; 
    mtbdl_trackers.delay_timer.time_start = SET_BIT; 
    mtbdl_trackers.led_state = CLEAR; 

    // User buttons 
    mtbdl_trackers.user_btn_1 = (uint8_t)user_btn_1; 
    mtbdl_trackers.user_btn_2 = (uint8_t)user_btn_2; 
    mtbdl_trackers.user_btn_3 = (uint8_t)user_btn_3; 
    mtbdl_trackers.user_btn_4 = (uint8_t)user_btn_4; 
    mtbdl_trackers.user_btn_1_block = CLEAR; 
    mtbdl_trackers.user_btn_2_block = CLEAR; 
    mtbdl_trackers.user_btn_3_block = CLEAR; 
    mtbdl_trackers.user_btn_4_block = CLEAR; 

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
}


// MTB DL application 
void mtbdl_app(void)
{
    // Local variables 
    mtbdl_states_t next_state = mtbdl_trackers.state; 

    //===================================================
    // Device status checks 

    if (hd44780u_get_fault_code())
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_0); 
    }
    if (mpu6050_get_fault_code(DEVICE_ONE))
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_1); 
    }
    if (m8q_get_fault_code())
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_2); 
    }
    if (hw125_get_fault_code())
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_3); 
    }
    if (hc05_get_status())
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_4); 
    }

    //===================================================

    //===================================================
    // User buttons 

    // Update user input button status 
    if (handler_flags.tim1_up_tim10_glbl_flag)
    {
        handler_flags.tim1_up_tim10_glbl_flag = CLEAR; 
        debounce((uint8_t)gpio_port_read(mtbdl_trackers.user_btn_port)); 
    }

    // Free the button pressed status as soon as possible & turn the LEDs off 
    if (debounce_released(mtbdl_trackers.user_btn_1) && mtbdl_trackers.user_btn_1_block)
    {
        mtbdl_trackers.user_btn_1_block = CLEAR; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_led_clear); 
    }
    if (debounce_released(mtbdl_trackers.user_btn_2) && mtbdl_trackers.user_btn_2_block)
    {
        mtbdl_trackers.user_btn_2_block = CLEAR; 
        mtbdl_led_update(WS2812_LED_6, mtbdl_led_clear); 
    }
    if (debounce_released(mtbdl_trackers.user_btn_3) && mtbdl_trackers.user_btn_3_block)
    {
        mtbdl_trackers.user_btn_3_block = CLEAR; 
        mtbdl_led_update(WS2812_LED_5, mtbdl_led_clear); 
    }
    if (debounce_released(mtbdl_trackers.user_btn_4) && mtbdl_trackers.user_btn_4_block)
    {
        mtbdl_trackers.user_btn_4_block = CLEAR; 
        mtbdl_led_update(WS2812_LED_4, mtbdl_led_clear); 
    }
    
    //===================================================

    //===================================================
    // System state machine 

    switch (next_state)
    {
        case MTBDL_INIT_STATE: 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_IDLE_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.run)
            {
                next_state = MTBDL_RUN_PREP_STATE; 
            }
            else if (mtbdl_trackers.data_select)
            {
                next_state = MTBDL_DATA_SELECT_STATE; 
            }
            else if (mtbdl_trackers.calibrate)
            {
                next_state = MTBDL_PRECALIBRATE_STATE;  
            }

            break; 

        case MTBDL_RUN_PREP_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_POSTRUN_STATE; 
            }
            else if (mtbdl_trackers.run)
            {
                next_state = MTBDL_RUN_COUNTDOWN_STATE; 
            }

            break; 

        case MTBDL_RUN_COUNTDOWN_STATE: 
            if (mtbdl_trackers.run)
            {
                next_state = MTBDL_RUN_STATE; 
            }

            break; 

        case MTBDL_RUN_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.run)
            {
                next_state = MTBDL_POSTRUN_STATE; 
            }

            break; 

        case MTBDL_POSTRUN_STATE: 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_DATA_SELECT_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.data_select)
            {
                next_state = MTBDL_DEV_SEARCH_STATE; 
            }
            else if (mtbdl_trackers.tx)
            {
                next_state = MTBDL_PRETX_STATE; 
            }

            break; 

        case MTBDL_DEV_SEARCH_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.data_select && mtbdl_trackers.rx)
            {
                next_state = MTBDL_PRERX_STATE; 
            }
            else if (mtbdl_trackers.data_select && mtbdl_trackers.tx)
            {
                next_state = MTBDL_PRETX_STATE; 
            }

            break; 

        case MTBDL_PRERX_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_POSTRX_STATE; 
            }
            else if (mtbdl_trackers.rx)
            {
                next_state = MTBDL_RX_STATE; 
            }

            break; 

        case MTBDL_RX_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.rx || mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_POSTRX_STATE; 
            }

            break; 

        case MTBDL_POSTRX_STATE: 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_PRETX_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_POSTTX_STATE; 
            }
            else if (mtbdl_trackers.tx)
            {
                next_state = MTBDL_TX_STATE; 
            }

            break; 

        case MTBDL_TX_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.tx || mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_POSTTX_STATE; 
            }

            break; 

        case MTBDL_POSTTX_STATE: 
            if (mtbdl_trackers.tx)
            {
                next_state = MTBDL_PRETX_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_PRECALIBRATE_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.calibrate)
            {
                next_state = MTBDL_CALIBRATE_STATE;  
            }

            break; 
        
        case MTBDL_CALIBRATE_STATE: 
            if (mtbdl_trackers.calibrate)
            {
                next_state = MTBDL_POSTCALIBRATE_STATE;  
            }

            break; 

        case MTBDL_POSTCALIBRATE_STATE: 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_LOWPWR_STATE: 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_FAULT_STATE: 
            if (mtbdl_trackers.reset)
            {
                next_state = MTBDL_RESET_STATE; 
            }
            
            break; 

        case MTBDL_RESET_STATE: 
            if (mtbdl_trackers.init)
            {
                next_state = MTBDL_INIT_STATE; 
            }
            break; 

        default: 
            next_state = MTBDL_INIT_STATE; 
            break; 
    }

    //===================================================

    // Execute the state 
    mtbdl_state_table[next_state](&mtbdl_trackers); 

    // Update the state 
    mtbdl_trackers.state = next_state; 

    // Call device controllers 
    hd44780u_controller(); 
    hw125_controller(); 
    mpu6050_controller(DEVICE_ONE); 
    m8q_controller(); 
}

//=======================================================================================


//=======================================================================================
// State functions 

// Init state 
void mtbdl_init_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->init)
    {
        // Display the startup message 
        hd44780u_set_msg(mtbdl_welcome_msg, MTBDL_MSG_LEN_1_LINE); 

        // Turn on the init state LED 
        mtbdl_led_update(WS2812_LED_2, mtbdl_led2_3); 
        
        mtbdl->init = CLEAR_BIT; 
    }
    
    //==================================================

    //==================================================
    // Checks 

    // Button 4 - Re-initialize the screen if it doesn't work 
    if (debounce_pressed(mtbdl->user_btn_4) && !(mtbdl->user_btn_4_block))
    {
        mtbdl->user_btn_4_block = SET_BIT; 
        hd44780u_set_reset_flag(); 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->init = SET_BIT; 
    }

    // Wait for the SD card to be mounted then access the file system 
    if (hw125_get_state() == HW125_ACCESS_STATE)
    {
        // Set the SD card check flag 
        hw125_set_check_flag(); 

        // Set up the file structure and system info 
        mtbdl_file_sys_setup(); 
    }

    //==================================================

    //==================================================
    // State exit 

    // Wait for a short period of time before leaving the init state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_SLOW, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Clear the screen startup message 
        hd44780u_set_clear_flag(); 

        // Turn off the init state LED 
        mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 

        // Set the idle state flag when ready 
        mtbdl->idle = SET_BIT; 
    }

    //==================================================
}


// Idle state 
void mtbdl_idle_state(
    mtbdl_trackers_t *mtbdl)
{
    // Local variables 
    static uint32_t time_count = CLEAR; 

    //==================================================
    // State entry 

    if (mtbdl->idle)
    {
        // Display the idle state message 
        mtbdl_set_idle_msg(); 

        // Set the screen to power save mode 
        hd44780u_set_pwr_save_flag(); 
        hd44780u_set_sleep_time(MTBDL_LCD_SLEEP); 

        // Put the HC-05 into low power mode 
        hc05_off(); 

        // Put the MPU-6050 into low power mode 
        mpu6050_set_low_power(DEVICE_ONE); 
        
        mtbdl->idle = CLEAR_BIT; 
    }

    //==================================================

    //==================================================
    // Checks 

    // Button 1 - triggers the pre run state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->run = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_led7_1); 
    }
    
    // Button 2 - triggers the data transfer selection state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->data_select = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_6, mtbdl_led6_1); 
    }
    
    // Button 3 - triggers the alternate functions state 
    else if (debounce_pressed(mtbdl->user_btn_3) && !(mtbdl->user_btn_3_block))
    {
        mtbdl->calibrate = SET_BIT; 
        mtbdl->user_btn_3_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_5, mtbdl_led5_1); 
    }
    
    // Button 4 - Turns the screen backlight on 
    else if (debounce_pressed(mtbdl->user_btn_4) && !(mtbdl->user_btn_4_block))
    {
        mtbdl->user_btn_4_block = SET_BIT; 
        hd44780u_wake_up(); 
        mtbdl_led_update(WS2812_LED_4, mtbdl_led4_1); 
    }

    // Check for GPS position lock - update the screen message and LED with the status 
    // Calculate the battery SOC - update the screen message with the status 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_FAST, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        time_count += MTBDL_STATE_CHECK_FAST; 

        if (time_count == MTBDL_STATE_CHECK_FAST)
        {
            // Turn off the GPS LED 
            mtbdl_led_update(WS2812_LED_1, mtbdl_led_clear); 
        }
        else if (time_count >= MTBDL_STATE_CHECK_SLOW)
        {
            if (m8q_get_position_navstat_lock())
            {
                // Turn on the GPS LED if there is a position lock 
                mtbdl_led_update(WS2812_LED_1, mtbdl_led1_1); 
            }

            // Read the battery ADC and calculate the SOC 

            mtbdl_set_idle_msg(); 
            time_count = CLEAR; 
        }
    }

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->run || mtbdl->data_select || mtbdl->calibrate || mtbdl->fault_code)
    {
        // Clear the idle state message and take the screen out of power save mode 
        hd44780u_set_clear_flag(); 
        hd44780u_clear_pwr_save_flag(); 

        // Turn off the GPS LED 
        mtbdl_led_update(WS2812_LED_1, mtbdl_led_clear); 

        mtbdl->delay_timer.time_start = SET_BIT; 
        time_count = CLEAR; 
    }

    //==================================================
}


// Pre run state 
void mtbdl_run_prep_state(
    mtbdl_trackers_t *mtbdl)
{
    // Local variables 
    static uint32_t time_count = CLEAR; 

    //==================================================
    // State entry 

    if (mtbdl->run)
    {
        // Check the log file name 
        if (mtbdl_log_name_prep()) 
        {
            // New file name created - display the run prep state message 
            mtbdl_set_run_prep_msg(); 
            mtbdl->run = CLEAR_BIT; 

            // Make sure the M8Q is out of low power mode 
            m8q_clear_low_pwr_flag(); 
        }
        else 
        {
            // Too many log files saved - abort 
            mtbdl->noncrit_fault = SET_BIT; 
            mtbdl->msg = mtbdl_ncf_excess_files_msg; 
            mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
        }
    }

    //==================================================

    //==================================================
    // Checks 

    // Button 1 - triggers the run state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->run = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_led7_1); 

        // Prepare the log file 
        mtbdl_log_file_prep(); 
    }
    
    // Button 2 - cancels the run state --> triggers idle state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_6, mtbdl_led6_1); 
    }

    // Check the GPS position lock status and update the status LEDs 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_FAST, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        time_count += MTBDL_STATE_CHECK_FAST; 

        // GPS status and LED update 
        if (time_count == MTBDL_STATE_CHECK_FAST)
        {
            // Turn off the GPS LED 
            mtbdl_led_update(WS2812_LED_1, mtbdl_led_clear); 
        }
        else if (time_count == MTBDL_STATE_CHECK_NORM)
        {
            // Turn on the data logging LED 
            mtbdl_led_update(WS2812_LED_0, mtbdl_led0_1); 
        }
        else if (time_count == (MTBDL_STATE_CHECK_NORM + MTBDL_STATE_CHECK_FAST))
        {
            // Turn off the data logging LED 
            mtbdl_led_update(WS2812_LED_0, mtbdl_led_clear); 
        }
        else if (time_count >= MTBDL_STATE_CHECK_SLOW)
        {
            if (m8q_get_position_navstat_lock())
            {
                // Turn on the GPS LED if there is a GPS position lock 
                mtbdl_led_update(WS2812_LED_1, mtbdl_led1_1); 
            }

            mtbdl_set_run_prep_msg(); 
            time_count = CLEAR; 
        }
    }

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->run || mtbdl->idle || mtbdl->fault_code)
    {
        // Clear the run prep state message 
        hd44780u_set_clear_flag(); 

        // Make sure the data logging and GPS LEDs are off 
        mtbdl_led_update(WS2812_LED_0, mtbdl_led_clear); 
        mtbdl_led_update(WS2812_LED_1, mtbdl_led_clear); 

        mtbdl->delay_timer.time_start = SET_BIT; 
        time_count = CLEAR; 
    }

    //==================================================
}


// Run countdown state 
void mtbdl_run_countdown_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->run)
    {
        // Display the run countdown state message 
        hd44780u_set_msg(mtbdl_run_countdown_msg, MTBDL_MSG_LEN_1_LINE); 

        // Turn on the data logging LED 
        mtbdl_led_update(WS2812_LED_0, mtbdl_led0_1); 

        // SD card will be written to constantly so no need for the check state 
        hw125_clear_check_flag(); 

        // Take the MPU-6050 out of low power mode 
        mpu6050_clear_low_power(DEVICE_ONE); 
        
        mtbdl->run = CLEAR_BIT; 
    }

    //==================================================

    //==================================================
    // State exit 

    // Wait for a short period of time before leaving the init state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_SLOW, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Put the screen in low power mode 
        hd44780u_set_low_pwr_flag(); 

        // Turn off the data logging LED 
        mtbdl_led_update(WS2812_LED_0, mtbdl_led_clear); 

        // Prep the logging data 
        mtbdl_log_data_prep(); 

        // Set the run state flag when ready 
        mtbdl->run = SET_BIT; 
    }

    //==================================================
}


// Run state 
void mtbdl_run_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    mtbdl->run = CLEAR_BIT; 

    //==================================================
    
    //==================================================
    // Check user button input 

    // Button 1 - stops the run state --> triggers the post run state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->run = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_led7_1); 
    }
    
    // Button 2 - Sets a marker 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->user_btn_2_block = SET_BIT; 
        mtbdl_set_trailmark(); 
    }
    
    //==================================================

    //==================================================
    // Record data 

    // Log the system data 
    mtbdl_logging(); 

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->run || mtbdl->fault_code)
    {
        // Take the screen out of low power mode 
        hd44780u_clear_low_pwr_flag(); 

        mtbdl->msg = mtbdl_postrun_msg; 
        mtbdl->msg_len = MTBDL_MSG_LEN_2_LINE; 
    }

    //==================================================
}


// Post run state 
void mtbdl_postrun_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->run)
    {
        // Close the open data log file if there was no non-critical faults 
        if (!mtbdl->noncrit_fault)
        {
            mtbdl_log_end(); 
        }

        // Turn on the data logging LED 
        mtbdl_led_update(WS2812_LED_0, mtbdl_led0_1); 

        hd44780u_set_msg(mtbdl->msg, mtbdl->msg_len); 
        mtbdl->noncrit_fault = CLEAR_BIT; 
        mtbdl->run = CLEAR_BIT; 
    }

    //==================================================
    
    //==================================================
    // State exit 

    // Wait for a short period of time before leaving the init state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_SLOW, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Clear the post run state message 
        hd44780u_set_clear_flag(); 

        // Turn off the data logging LED 
        mtbdl_led_update(WS2812_LED_0, mtbdl_led0_1); 

        // Set the SD card controller check flag 
        hw125_set_check_flag(); 

        // Turn off the data logging LED 
        mtbdl_led_update(WS2812_LED_0, mtbdl_led_clear); 

        // Set the idle state flag when ready 
        mtbdl->idle = SET_BIT; 
    }
    
    //==================================================
}


// Data transfer selection 
void mtbdl_data_select_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->data_select)
    {
        // Display the data select state message 
        hd44780u_set_msg(mtbdl_data_select_msg, MTBDL_MSG_LEN_3_LINE); 
        
        mtbdl->data_select = CLEAR_BIT; 
    }
    
    //==================================================

    //==================================================
    // Check user button input 

    // Button 1 - triggers the pre receive (RX) state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->rx = SET_BIT; 
        mtbdl->data_select = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_led7_1); 
    }
    
    // Button 2 - triggers the pre send (TX) state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        // Before going to the device search state the code first check if there are any log 
        // files saved in the system. If there are none then the data select bit is not set 
        // which will ultimately aborts the TX state and tells the user there are no files 
        // available for sending. 
        if (mtbdl_tx_check())
        {
            // Files exist - go to the device search state 
            mtbdl->data_select = SET_BIT; 
        }
        mtbdl->tx = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_6, mtbdl_led6_1); 
    }
    
    // Button 3 - triggers the idle state - cancels data selection 
    else if (debounce_pressed(mtbdl->user_btn_3) && !(mtbdl->user_btn_3_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_3_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_5, mtbdl_led5_1); 
    }
    
    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->data_select || mtbdl->tx || mtbdl->idle || mtbdl->fault_code)
    {
        // Clear the data select state message 
        hd44780u_set_clear_flag(); 
    }

    //==================================================
}


// Search for Bluetooth connection 
void mtbdl_dev_search_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->data_select)
    {
        // Display the device connection search state message 
        hd44780u_set_msg(mtbdl_dev_search_msg, MTBDL_MSG_LEN_2_LINE); 

        // Take the HC-05 out of low power mode 
        hc05_on(); 
        
        mtbdl->data_select = CLEAR_BIT; 
    }

    //==================================================

    //==================================================
    // Checks 

    // Button 1 - triggers the idle state - cancels device search 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_led7_1); 
    }

    // If the HC-05 is connected to a device then move to the next state 
    if (hc05_status())
    {
        mtbdl->data_select = SET_BIT; 
    }

    //==================================================

    //===================================================
    // LED update 

    // Update the state of the LED 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_FAST, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->led_state = 1 - mtbdl->led_state; 

        // Toggle Bluetooth LED quickly while not connected 
        if (mtbdl->led_state)
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led2_1); 
        }
        else 
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 
        }
    }

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->idle || mtbdl->data_select || mtbdl->fault_code)
    {
        // Clear the device connection search state message 
        hd44780u_set_clear_flag(); 

        // Make sure Bluetooth LED is off 
        mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 

        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
    }

    //==================================================
}


// Pre RX state 
void mtbdl_prerx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->rx)
    {
        // Display the pre rx state message 
        hd44780u_set_msg(mtbdl_prerx_msg, MTBDL_MSG_LEN_3_LINE); 
        
        mtbdl->rx = CLEAR_BIT; 
        mtbdl->data_select = CLEAR_BIT; 
    }
    
    //==================================================

    //==================================================
    // Checks 

    // Button 1 - triggers the rx state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->rx = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_led7_1); 
    }
    
    // Button 2 - cancels the rx state --> triggers idle state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_6, mtbdl_led6_1); 
    }

    // If the HC-05 gets disconnected then abort the potential transfer 
    if (!hc05_status())
    {
        mtbdl->rx = SET_BIT; 
        mtbdl->noncrit_fault = SET_BIT; 
        mtbdl->msg = mtbdl_ncf_bt_con_lost; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
    }
    
    //==================================================

    //===================================================
    // LED update 

    // Update the state of the LED 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_NORM, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->led_state = 1 - mtbdl->led_state; 

        // Toggle Bluetooth LED slowly while connected 
        if (mtbdl->led_state)
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led2_1); 
        }
        else 
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 
        }
    }

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->rx || mtbdl->idle || mtbdl->fault_code)
    {
        // Clear the pre rx state message 
        hd44780u_set_clear_flag(); 

        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
    }

    //==================================================
}


// RX state 
void mtbdl_rx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->rx)
    {
        // Display the rx state message 
        hd44780u_set_msg(mtbdl_rx_msg, MTBDL_MSG_LEN_2_LINE); 

        // Begin the RX user interface 
        mtbdl_rx_prep(); 
        
        mtbdl->rx = CLEAR_BIT; 
    }

    //==================================================

    //==================================================
    // Checks 

    // Button 1 - stops the rx state --> triggers the post rx state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->rx = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_led7_1); 

        mtbdl->msg = mtbdl_postrx_msg; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
    }

    // If the HC-05 gets disconnected then abort the transfer 
    // Update the non-critical fault message 
    if (!hc05_status())
    {
        mtbdl->rx = SET_BIT; 
        mtbdl->noncrit_fault = SET_BIT; 
        mtbdl->msg = mtbdl_ncf_bt_con_lost; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
    }
    
    //==================================================

    //===================================================
    // LED update 

    // Update the state of the LED 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_NORM, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->led_state = 1 - mtbdl->led_state; 

        // Toggle Bluetooth LED slowly while connected 
        if (mtbdl->led_state)
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led2_1); 
        }
        else 
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 
        }
    }

    //==================================================

    //==================================================
    // Data transfer 

    // Read the device data 
    mtbdl_rx(); 

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->rx || mtbdl->fault_code)
    {
        // Clear the rx state message 
        hd44780u_set_clear_flag(); 

        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
    }

    //==================================================
}


// Post RX state 
void mtbdl_postrx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->rx)
    {
        // Update the screen message 
        hd44780u_set_msg(mtbdl->msg, mtbdl->msg_len); 

        // Turn on the Bluetooth LED 
        mtbdl_led_update(WS2812_LED_2, mtbdl_led2_1); 

        // Save the parameters to file and update tracking info 
        mtbdl_write_bike_params(HW125_MODE_OEW); 
        mtbdl->noncrit_fault = CLEAR_BIT; 
        mtbdl->rx = CLEAR_BIT; 
    }

    //==================================================

    //==================================================
    // State exit 

    // Wait for a short period of time before leaving the post rx state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_SLOW, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        // Clear the post rx state message 
        hd44780u_set_clear_flag(); 

        // Turn off the Bluetooth LED 
        mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 

        // Update tracking info 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->idle = SET_BIT; 
    }
    
    //==================================================
}


// Pre TX state 
void mtbdl_pretx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->tx)
    {
        // Prepare the log file to send 
        if (mtbdl_tx_prep())
        {
            // File ready - display the pre tx state message 
            mtbdl_set_pretx_msg(); 
            mtbdl->tx = CLEAR_BIT; 
        }
        else 
        {
            // File does not exist - update the message to send and abort the state 
            mtbdl->noncrit_fault = SET_BIT; 
            mtbdl->msg = mtbdl_ncf_no_files_msg; 
            mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
        }

        mtbdl->data_select = CLEAR_BIT; 
    }
    
    //==================================================

    //==================================================
    // Checks 

    // Button 1 - triggers the tx state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->tx = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_led7_1); 
    }
    
    // Button 2 - cancels the tx state --> triggers idle state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_6, mtbdl_led6_1); 

        // End the data transfer 
        mtbdl_tx_end(); 
    }

    // If the HC-05 gets disconnected then abort the potential transfer 
    // This check uses the tx bit to reflect the existance of log files saved in the system. 
    // In the state entry (above), if there are no log files to send then the tx bit will 
    // not be set which then prevents this Bluetooth device connection check from 
    // overwriting the non-critical fault message. 
    if (!hc05_status() && !mtbdl->tx)
    {
        mtbdl->tx = SET_BIT; 
        mtbdl->noncrit_fault = SET_BIT; 
        mtbdl->msg = mtbdl_ncf_bt_con_lost; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
    }

    //==================================================

    //===================================================
    // LED update 

    // Update the state of the LED 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_NORM, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->led_state = 1 - mtbdl->led_state; 

        // Toggle Bluetooth LED slowly while connected 
        if (mtbdl->led_state)
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led2_1); 
        }
        else 
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 
        }
    }

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->tx || mtbdl->idle || mtbdl->fault_code)
    {
        // Clear the pre rx state message 
        hd44780u_set_clear_flag(); 

        // Clear the Bluetooth LED 
        mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 

        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
    }

    //==================================================
}


// TX state 
void mtbdl_tx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->tx)
    {
        // Display the tx state message 
        hd44780u_set_msg(mtbdl_tx_msg, MTBDL_MSG_LEN_2_LINE); 
        mtbdl->msg = mtbdl_posttx_msg; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
        mtbdl->tx = CLEAR_BIT; 
    }

    //==================================================

    //==================================================
    // Checks 

    // Button 1 - stops the tx state --> triggers the post tx state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->tx = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_led7_1); 
    }

    // Transfer data log contents and set the tx bit if the transfer finishes 
    if (mtbdl_tx())
    {
        mtbdl->tx = SET_BIT; 
    }

    // If the HC-05 gets disconnected then abort the transfer 
    if (!hc05_status())
    {
        mtbdl->tx = SET_BIT; 
        mtbdl->noncrit_fault = SET_BIT; 
        mtbdl->msg = mtbdl_ncf_bt_con_lost; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
    }
    
    //==================================================

    //===================================================
    // LED update 

    // Update the state of the LED 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_NORM, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->led_state = 1 - mtbdl->led_state; 

        // Toggle Bluetooth LED slowly while connected 
        if (mtbdl->led_state)
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led2_1); 
        }
        else 
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 
        }
    }

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->tx || mtbdl->fault_code)
    {
        // Clear the tx state message 
        hd44780u_set_clear_flag(); 

        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
    }

    //==================================================
}


// Post TX state 
void mtbdl_posttx_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 
    
    if (mtbdl->tx)
    {
        // Set the post TX state message 
        hd44780u_set_msg(mtbdl->msg, mtbdl->msg_len); 

        // Turn on the Bluetooth LED 
        mtbdl_led_update(WS2812_LED_2, mtbdl_led2_1); 

        // End the transaction 
        mtbdl_tx_end(); 
        mtbdl->tx = CLEAR_BIT; 
    }

    //==================================================

    //==================================================
    // State exit 

    // Wait for a short period of time before leaving the post tx state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_SLOW, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Clear the post tx state message 
        hd44780u_set_clear_flag(); 

        // Turn off the Bluetooth LED 
        mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 

        // Go back to the pre-tx state if the connection was not lost 
        if (mtbdl->noncrit_fault)
        {
            mtbdl->idle = SET_BIT; 
        }
        else 
        {
            mtbdl->tx = SET_BIT; 
        }

        mtbdl->noncrit_fault = CLEAR_BIT; 
    }
    
    //==================================================
}


// Pre calibration state 
void mtbdl_precalibrate_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->calibrate)
    {
        // Display the pre calibration state message 
        hd44780u_set_msg(mtbdl_precal_msg, MTBDL_MSG_LEN_4_LINE); 
        
        mtbdl->calibrate = CLEAR_BIT; 
    }
    
    //==================================================

    //==================================================
    // Checks 

    // Button 1 - triggers the calibration state 
    if (debounce_pressed(mtbdl->user_btn_1) && !(mtbdl->user_btn_1_block))
    {
        mtbdl->calibrate = SET_BIT; 
        mtbdl->user_btn_1_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_led7_1); 

        // Take the MPU-6050 out of low power mode 
        mpu6050_clear_low_power(DEVICE_ONE); 
    }
    
    // Button 2 - cancels the calibration state --> triggers idle state 
    else if (debounce_pressed(mtbdl->user_btn_2) && !(mtbdl->user_btn_2_block))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->user_btn_2_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_6, mtbdl_led6_1); 
    }
    
    //==================================================

    //===================================================
    // LED update 

    // Update the state of the LED 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_NORM, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->led_state++; 

        if (mtbdl->led_state == MTBDL_STATE_CHECK_CNT_1)
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led2_2); 
        }
        else if (mtbdl->led_state >= MTBDL_STATE_CHECK_CNT_2)
        {
            mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 
            mtbdl->led_state = CLEAR; 
        }
    }

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->calibrate || mtbdl->idle || mtbdl->fault_code)
    {
        // Clear the pre calibration state message 
        hd44780u_set_clear_flag(); 

        // Make sure the calibration LED is off 
        mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 

        // Update the system tracking info 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
    }

    //==================================================
}


// Calibration state 
void mtbdl_calibrate_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->calibrate)
    {
        // Display the calibration state message 
        hd44780u_set_msg(mtbdl_cal_msg, MTBDL_MSG_LEN_1_LINE); 

        // Prepare to record data for calibration 
        mtbdl_cal_prep(); 

        // Turn on the calibration LED 
        mtbdl_led_update(WS2812_LED_2, mtbdl_led2_2); 
        
        mtbdl->calibrate = CLEAR_BIT; 
    }
    
    //==================================================

    //==================================================
    // Sample data 

    // This function records data that gets used to calculate the calibration values 
    mtbdl_calibrate(); 

    //==================================================

    //==================================================
    // State exit 

    // Wait until calibration is done before leaving the state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_SLOW, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        // Update the tracking information 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->calibrate = SET_BIT; 

        // Calculate the calibration values 
        mtbdl_cal_calc(); 

        // Clear the calibration state message and turn off the calibration LED 
        hd44780u_set_clear_flag(); 
        mtbdl_led_update(WS2812_LED_2, mtbdl_led_clear); 
    }
    
    //==================================================
}


// Post calibration state 
void mtbdl_postcalibrate_state(
    mtbdl_trackers_t *mtbdl)
{
    //===================================================
    // State entry 

    if (mtbdl->calibrate)
    {
        // Display the post calibration message and record the calibration data
        hd44780u_set_msg(mtbdl_postcal_msg, MTBDL_MSG_LEN_1_LINE); 
        mtbdl_write_sys_params(HW125_MODE_OEW); 
        mtbdl->calibrate = CLEAR_BIT; 
    }

    //===================================================

    //===================================================
    // State exit 

    // Wait for a period of time before returning to the idle state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_SLOW, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        // Update the tracking information 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->idle = SET_BIT; 

        // Clear the post calibration message 
        hd44780u_set_clear_flag(); 
    }

    //===================================================
}


// Low power state 
void mtbdl_lowpwr_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (mtbdl->low_pwr)
    {
        // Display the low power state message 
        hd44780u_set_msg(mtbdl_low_pwr_msg, MTBDL_MSG_LEN_2_LINE); 

        // Put the screen into power save mode 
        hd44780u_set_pwr_save_flag(); 
        hd44780u_set_sleep_time(MTBDL_LCD_LP_SLEEP); 

        // Put the GPS into low power mode 
        m8q_set_low_pwr_flag(); 

        // Make sure the MPU-6050 and HC-05 are in low power mode 
        mpu6050_set_low_power(DEVICE_ONE); 
        hc05_off(); 
        
        mtbdl->low_pwr = CLEAR_BIT; 
    }
    
    //==================================================

    //==================================================
    // Checks 
    
    // Button 4 - Turns the screen backlight on 
    if (debounce_pressed(mtbdl->user_btn_4) && !(mtbdl->user_btn_4_block))
    {
        mtbdl->user_btn_4_block = SET_BIT; 
        hd44780u_wake_up(); 
        mtbdl_led_update(WS2812_LED_4, mtbdl_led4_1); 
    }

    // If SOC is above the minimum threshold then we can exit low power state 
    // TODO replace this with a check on the SOC 
    // Button 3 - triggers the idle state 
    if (debounce_pressed(mtbdl->user_btn_3) && !(mtbdl->user_btn_3_block))
    {
        mtbdl->user_btn_3_block = SET_BIT; 
        mtbdl->low_pwr = SET_BIT; 
    }

    //===================================================
    
    //===================================================
    // LED update 

    // Update the state of the LED 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    MTBDL_STATE_CHECK_NORM, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        mtbdl->led_state++; 

        if (mtbdl->led_state == MTBDL_STATE_CHECK_CNT_2)
        {
            mtbdl_led_update(WS2812_LED_3, mtbdl_led3_1); 
        }
        else if (mtbdl->led_state >= MTBDL_STATE_CHECK_CNT_3)
        {
            mtbdl_led_update(WS2812_LED_3, mtbdl_led_clear); 
            mtbdl->led_state = CLEAR; 
        }
    }

    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->low_pwr)
    {
        // Update system tracking info 
        mtbdl->low_pwr = CLEAR_BIT; 
        mtbdl->idle = SET_BIT; 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 

        // Clear the idle state message and take devices out of low power mode 
        hd44780u_set_clear_flag(); 
        hd44780u_clear_pwr_save_flag(); 
        m8q_clear_low_pwr_flag(); 

        // Make sure the low power LED if off 
        mtbdl_led_update(WS2812_LED_3, mtbdl_led_clear); 
    }

    //==================================================
}


// Fault state 
void mtbdl_fault_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    if (!mtbdl->fault)
    {
        // Display the fault state message and take screen out of power save mode 
        hd44780u_set_msg(mtbdl_fault_msg, MTBDL_MSG_LEN_2_LINE); 
        hd44780u_clear_pwr_save_flag(); 

        // Turn on the fault status LED 
        mtbdl_led_update(WS2812_LED_3, mtbdl_led3_1); 

        // Record the fault code in a log 

        // Stop any existing processes - assess each state case 
        
        mtbdl->fault = SET_BIT; 
    }

    //==================================================

    //==================================================
    // Check user button input 

    // Button 4 - triggers a system reset (reset state) 
    if (debounce_pressed(mtbdl->user_btn_4) && !(mtbdl->user_btn_4_block))
    {
        mtbdl->reset = SET_BIT; 
        mtbdl->user_btn_4_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_4, mtbdl_led4_1); 
    }
    
    //==================================================

    //==================================================
    // State exit 

    if (mtbdl->reset)
    {
        mtbdl->fault_code = CLEAR; 
        mtbdl->fault = CLEAR_BIT; 

        // Turn off the fault status LED 
        mtbdl_led_update(WS2812_LED_3, mtbdl_led_clear); 

        // Clear the fault state message 
        hd44780u_set_clear_flag(); 
    }

    //==================================================
}


// reset state 
void mtbdl_reset_state(
    mtbdl_trackers_t *mtbdl)
{
    //==================================================
    // State entry 

    mtbdl->reset = CLEAR_BIT; 
    
    //==================================================

    //==================================================
    // State exit 

    // Set the init state flag once ready 
    mtbdl->init = SET_BIT; 

    // TODO Reset devices 
    hw125_set_reset_flag(); 
    
    //==================================================
}

//=======================================================================================
