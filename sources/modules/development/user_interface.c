/**
 * @file user_interface.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief User interface 
 * 
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "user_interface.h" 
#include "system_parameters.h" 

#include "ws2812_config.h" 
#include "hd44780u_config.h" 
#include "stm32f4xx_it.h" 

//=======================================================================================


//=======================================================================================
// Macros 

#define UI_LED_COUNTER_PERIOD 200      // 5ms interrupt * 200 == 1s counter period 
#define UI_LED_WRITE_PERIOD 10         // 5ms interrupt * 10 == 50ms write period 

#define UI_LOG_INDEX_OFFSET 1 
#define UI_SCREEN_LINE_CHAR_OFFSET 1   // Prevents NULL from being the last line character 

//=======================================================================================


//=======================================================================================
// Variables 

static mtbdl_ui_t mtbdl_ui; 

//=======================================================================================


//=======================================================================================
// Prototypes 

/**
 * @brief Button press check 
 * 
 * @return ui_btn_num_t 
 */
ui_btn_num_t ui_button_press(void); 


/**
 * @brief Button release check 
 */
void ui_button_release(void); 


/**
 * @brief Update the LED output 
 */
void ui_led_update(void); 


/**
 * @brief Update the SOC calculation 
 */
void ui_soc_update(void); 


/**
 * @brief Update the screen output 
 */
void ui_screen_update(void); 

//=======================================================================================


//=======================================================================================
// Initialization 

// User interface init 
void ui_init(
    GPIO_TypeDef *btn_port, 
    pin_selector_t btn1, 
    pin_selector_t btn2, 
    pin_selector_t btn3, 
    pin_selector_t btn4)
{
    // GPIO port for the buttons 
    mtbdl_ui.user_btn_port = btn_port; 

    // User button pin numbers 
    mtbdl_ui.user_btn_1 = (uint8_t)(SET_BIT << btn1); 
    mtbdl_ui.user_btn_2 = (uint8_t)(SET_BIT << btn2); 
    mtbdl_ui.user_btn_3 = (uint8_t)(SET_BIT << btn3); 
    mtbdl_ui.user_btn_4 = (uint8_t)(SET_BIT << btn4); 

    // User button repeat blockers 
    mtbdl_ui.user_btn_1_block = CLEAR_BIT; 
    mtbdl_ui.user_btn_2_block = CLEAR_BIT; 
    mtbdl_ui.user_btn_3_block = CLEAR_BIT; 
    mtbdl_ui.user_btn_4_block = CLEAR_BIT; 

    // Configure the GPIO inputs for each user button 
    gpio_pin_init(mtbdl_ui.user_btn_port, mtbdl_ui.user_btn_1, 
                  MODER_INPUT, OTYPER_PP, OSPEEDR_HIGH, PUPDR_PU); 
    gpio_pin_init(mtbdl_ui.user_btn_port, mtbdl_ui.user_btn_2, 
                  MODER_INPUT, OTYPER_PP, OSPEEDR_HIGH, PUPDR_PU); 
    gpio_pin_init(mtbdl_ui.user_btn_port, mtbdl_ui.user_btn_3, 
                  MODER_INPUT, OTYPER_PP, OSPEEDR_HIGH, PUPDR_PU); 
    gpio_pin_init(mtbdl_ui.user_btn_port, mtbdl_ui.user_btn_4, 
                  MODER_INPUT, OTYPER_PP, OSPEEDR_HIGH, PUPDR_PU); 

    // Initialize the button debouncer 
    debounce_init(mtbdl_ui.user_btn_1 | mtbdl_ui.user_btn_2 | 
                  mtbdl_ui.user_btn_3 | mtbdl_ui.user_btn_4); 

    // LED colour data 
    memset((void *)mtbdl_ui.led_colours, CLEAR, sizeof(mtbdl_ui.led_colours)); 
    memset((void *)mtbdl_ui.led_write_data, CLEAR, sizeof(mtbdl_ui.led_write_data)); 
    mtbdl_ui.led_counter = CLEAR; 

    // Initialize blinking LED info 
    mtbdl_ui.led_state[WS2812_LED_0] = 
        (mtbdl_ui_led_blink_t){ WS2812_LED_0, UI_LED_DUTY_SHORT, CLEAR }; 
    mtbdl_ui.led_state[WS2812_LED_1] = 
        (mtbdl_ui_led_blink_t){ WS2812_LED_1, UI_LED_DUTY_SHORT, CLEAR }; 
    mtbdl_ui.led_state[WS2812_LED_2] = 
        (mtbdl_ui_led_blink_t){ WS2812_LED_2, UI_LED_DUTY_SHORT, CLEAR }; 
    mtbdl_ui.led_state[WS2812_LED_3] = 
        (mtbdl_ui_led_blink_t){ WS2812_LED_3, UI_LED_DUTY_SHORT, CLEAR }; 

    // Initialize system info 
    mtbdl_ui.navstat = M8Q_NAVSTAT_NF; 

    // Initialize SD card info 
    memset((void *)mtbdl_ui.data_buff, CLEAR, sizeof(mtbdl_ui.data_buff)); 
    memset((void *)mtbdl_ui.filename, CLEAR, sizeof(mtbdl_ui.filename)); 
    mtbdl_ui.tx_status = CLEAR; 
}

//=======================================================================================


//=======================================================================================
// Device update 

// Periodic UI update 
ui_btn_num_t ui_status_update(void)
{
    ui_btn_num_t btn_num = UI_BTN_NONE; 

    // 5ms interrupt 
    if (handler_flags.tim1_up_tim10_glbl_flag)
    {
        handler_flags.tim1_up_tim10_glbl_flag = CLEAR; 

        // Update user button input status 
        debounce((uint8_t)gpio_port_read(mtbdl_ui.user_btn_port)); 
        btn_num = ui_button_press(); 
        ui_button_release(); 

        // Update LED timing and output 
        ui_led_update(); 

        // Update the battery SOC 
        ui_soc_update(); 

        // Update screen message 
        ui_screen_update(); 
    }

    return btn_num; 
}


// Button press check 
ui_btn_num_t ui_button_press(void)
{
    ui_btn_num_t btn_num = UI_BTN_NONE; 

    // Button 1 
    if (debounce_pressed(mtbdl_ui.user_btn_1) && !mtbdl_ui.user_btn_1_block)
    {
        mtbdl_ui.user_btn_1_block = SET_BIT; 
        ui_led_colour_change(WS2812_LED_7, mtbdl_ui.led_colours[WS2812_LED_7]); 
        btn_num = UI_BTN_1; 
    }
    
    // Button 2 
    else if (debounce_pressed(mtbdl_ui.user_btn_2) && !mtbdl_ui.user_btn_2_block)
    {
        mtbdl_ui.user_btn_2_block = SET_BIT; 
        ui_led_colour_change(WS2812_LED_6, mtbdl_ui.led_colours[WS2812_LED_6]); 
        btn_num = UI_BTN_2; 
    }
    
    // Button 3 
    else if (debounce_pressed(mtbdl_ui.user_btn_3) && !mtbdl_ui.user_btn_3_block)
    {
        mtbdl_ui.user_btn_3_block = SET_BIT; 
        ui_led_colour_change(WS2812_LED_5, mtbdl_ui.led_colours[WS2812_LED_5]); 
        btn_num = UI_BTN_3; 
    }
    
    // Button 4 
    else if (debounce_pressed(mtbdl_ui.user_btn_4) && !mtbdl_ui.user_btn_4_block)
    {
        mtbdl_ui.user_btn_4_block = SET_BIT; 
        ui_led_colour_change(WS2812_LED_4, mtbdl_ui.led_colours[WS2812_LED_4]); 
        btn_num = UI_BTN_4; 
    }

    return btn_num; 
}


// Button release check 
void ui_button_release(void)
{
    // Free the button pressed status as soon as possible & turn the LEDs off 
    
    // Button 1 
    if (debounce_released(mtbdl_ui.user_btn_1) && mtbdl_ui.user_btn_1_block)
    {
        mtbdl_ui.user_btn_1_block = CLEAR_BIT; 
        ui_led_colour_change(WS2812_LED_7, mtbdl_led_clear); 
    }
    
    // Button 2 
    if (debounce_released(mtbdl_ui.user_btn_2) && mtbdl_ui.user_btn_2_block)
    {
        mtbdl_ui.user_btn_2_block = CLEAR_BIT; 
        ui_led_colour_change(WS2812_LED_6, mtbdl_led_clear); 
    }
    
    // Button 3 
    if (debounce_released(mtbdl_ui.user_btn_3) && mtbdl_ui.user_btn_3_block)
    {
        mtbdl_ui.user_btn_3_block = CLEAR_BIT; 
        ui_led_colour_change(WS2812_LED_5, mtbdl_led_clear); 
    }
    
    // Button 4 
    if (debounce_released(mtbdl_ui.user_btn_4) && mtbdl_ui.user_btn_4_block)
    {
        mtbdl_ui.user_btn_4_block = CLEAR_BIT; 
        ui_led_colour_change(WS2812_LED_4, mtbdl_led_clear); 
    }
}


// Update the LED output 
void ui_led_update(void)
{
    static uint8_t led_write_counter = CLEAR; 

    // Update LED timing counter 
    if (mtbdl_ui.led_counter++ > UI_LED_COUNTER_PERIOD)
    {
        mtbdl_ui.led_counter = CLEAR; 
    }

    // Update the LED output periodically 
    if (led_write_counter++ == UI_LED_WRITE_PERIOD)
    {
        ws2812_send(DEVICE_ONE, mtbdl_ui.led_write_data); 
        led_write_counter = CLEAR; 
    }
}


// Update the SOC calculation 
void ui_soc_update(void)
{
    // 
}


// Update screen message 
// - Kind of like a refresh function where each message function will retrieve updated 
//   data. 
// - Have a way to ignore message update requests (for example if the GPS status changes 
//   but we're in data logging mode where the screen is off). 
// Update the screen output 
void ui_screen_update(void)
{
    // - Counter to control how often to write the screen message. 
    // - Message gets updated by a setter? 
    // - Messages that require formatting will need continuous calling like the LED blink 
    //   function. 
}

//=======================================================================================


//=======================================================================================
// LED control 

// Change the state of the LED 
void ui_led_state_update(ws2812_led_index_t led_num)
{
    if (led_num > WS2812_LED_3)
    {
        return; 
    }

    mtbdl_ui_led_blink_t *led = &(mtbdl_ui.led_state[led_num]); 

    if ((mtbdl_ui.led_counter < led->duty_cycle) && !led->update_blocker)
    {
        // Set the LED colour 
        ui_led_colour_change(led->led_num, mtbdl_ui.led_colours[led->led_num]); 
        led->update_blocker = SET_BIT; 
    }
    else if ((mtbdl_ui.led_counter >= led->duty_cycle) && led->update_blocker)
    {
        // Clear the LED colour 
        ui_led_colour_change(led->led_num, mtbdl_led_clear); 
        led->update_blocker = CLEAR_BIT; 
    }
}


// Update GPS position status 
void ui_gps_led_status_update(void)
{
    static uint8_t gps_status_block = CLEAR_BIT; 

    // Notes: 
    // - Make sure to refresh/update the navstat for each screen message that requires it. 
    // - How do we specify a screen message to update? What if we don't want to update 
    //   a screen message at all? 

    // Monitor the GPS position lock status and update the LED and screen message 
    // for feedback. 
    if (m8q_get_position_navstat_lock())
    {
        ui_led_state_update(WS2812_LED_1); 

        if (!gps_status_block)
        {
            gps_status_block = SET_BIT; 
        }
    }
    else if (gps_status_block)
    {
        gps_status_block = CLEAR_BIT; 
        
        // Turn the GPS LED off 
        ui_led_colour_change(WS2812_LED_1, mtbdl_led_clear); 
    }
}

//=======================================================================================


//=======================================================================================
// Screen control 

// Format the idle state message 
void ui_set_idle_msg(void)
{
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_4_LINE]; 

    // Create an editable copy of the message 
    for (uint8_t i = CLEAR; i < MTBDL_MSG_LEN_4_LINE; i++) 
    {
        msg[i] = mtbdl_idle_msg[i]; 
    }

    mtbdl_ui.navstat = m8q_get_position_navstat(); 

    // Format the messages with data 
    // snprintf will NULL terminate the string at the screen line length so in order to use 
    // the last spot on the screen line the message length must be indexed up by one 

    snprintf(msg[HD44780U_L1].msg, 
             (HD44780U_LINE_LEN + UI_SCREEN_LINE_CHAR_OFFSET), 
             mtbdl_idle_msg[HD44780U_L1].msg, 
             param_get_bike_setting(PARAM_BIKE_SET_FPSI), 
             param_get_bike_setting(PARAM_BIKE_SET_FC), 
             param_get_bike_setting(PARAM_BIKE_SET_FR)); 
    
    snprintf(msg[HD44780U_L2].msg, 
             (HD44780U_LINE_LEN + UI_SCREEN_LINE_CHAR_OFFSET), 
             mtbdl_idle_msg[HD44780U_L2].msg, 
             param_get_bike_setting(PARAM_BIKE_SET_SPSI), 
             param_get_bike_setting(PARAM_BIKE_SET_SL), 
             param_get_bike_setting(PARAM_BIKE_SET_SR)); 
    
    snprintf(msg[HD44780U_L3].msg, 
             (HD44780U_LINE_LEN + UI_SCREEN_LINE_CHAR_OFFSET), 
             mtbdl_idle_msg[HD44780U_L3].msg, 
             // mtbdl_data.adc_buff[MTBDL_ADC_SOC], 
             1, 
             (char)(mtbdl_ui.navstat >> SHIFT_8), 
             (char)(mtbdl_ui.navstat)); 

    hd44780u_set_msg(msg, MTBDL_MSG_LEN_4_LINE); 
}


// Format the run prep state message 
void ui_set_run_prep_msg(void)
{
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_3_LINE]; 

    // Create an editable copy of the message 
    for (uint8_t i = CLEAR; i < MTBDL_MSG_LEN_3_LINE; i++) 
    {
        msg[i] = mtbdl_run_prep_msg[i]; 
    }

    mtbdl_ui.navstat = m8q_get_position_navstat(); 

    // Format the message with data 
    snprintf(msg[HD44780U_L1].msg, 
             HD44780U_LINE_LEN, 
             mtbdl_run_prep_msg[HD44780U_L1].msg, 
             (char)(mtbdl_ui.navstat >> SHIFT_8), 
             (char)(mtbdl_ui.navstat)); 

    hd44780u_set_msg(msg, MTBDL_MSG_LEN_3_LINE); 
}


// Format the pre TX state message 
void ui_set_pretx_msg(void)
{
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_4_LINE]; 

    // Create an editable copy of the message 
    for (uint8_t i = CLEAR; i < MTBDL_MSG_LEN_4_LINE; i++) 
    {
        msg[i] = mtbdl_pretx_msg[i]; 
    }

    // Format the message with data 
    // The log index is adjusted because it will be one ahead of the most recent log 
    // file number after the most recent log has been created 
    snprintf(msg[HD44780U_L2].msg, 
             HD44780U_LINE_LEN, 
             mtbdl_pretx_msg[HD44780U_L2].msg, 
             (param_get_log_index() - UI_LOG_INDEX_OFFSET)); 

    hd44780u_set_msg(msg, MTBDL_MSG_LEN_4_LINE); 
}

//=======================================================================================


//=======================================================================================
// RX mode 

// RX user interface start 
void ui_rx_prep(void)
{
    hc05_send(mtbdl_rx_prompt); 
    hc05_clear(); 
}


// Read user input 
void ui_rx(void)
{
    unsigned int param_index, setting_data; 

    // Read Bluetooth data if available 
    if (hc05_data_status())
    {
        // Read and parse the data from the HC-05 
        hc05_read(mtbdl_ui.data_buff, MTBDL_MAX_STR_LEN); 
        sscanf(mtbdl_ui.data_buff, 
               mtbdl_rx_input, 
               &param_index, 
               &setting_data); 

        // Check for a data match if a valid parameter index is provided 
        if (param_index < PARAM_BIKE_SET_SR)
        {
            param_update_bike_setting((param_bike_set_index_t)param_index, setting_data); 
        }

        // Provide a user prompt 
        ui_rx_prep(); 
    }
}

//=======================================================================================


//=======================================================================================
// TX mode 

// Prepare to send a data log file 
uint8_t ui_tx_prep(void)
{
    // Check if there are no log files 
    if (!param_get_log_index())
    {
        return FALSE; 
    }

    // Log files exist. Move to the data directory. 
    hw125_set_dir(mtbdl_data_dir); 

    // Generate a log file name. The log index is adjusted because it will be one ahead 
    // of the most recent log file number after the most recent log has been created 
    snprintf(mtbdl_ui.filename, 
             MTBDL_MAX_STR_LEN, 
             mtbdl_log_file, 
             (param_get_log_index() - UI_LOG_INDEX_OFFSET)); 

    // Check for the existance of the specified file number 
    if (hw125_get_exists(mtbdl_ui.filename) == FR_NO_FILE)
    {
        return FALSE; 
    }

    // Open the file 
    hw125_open(mtbdl_ui.filename, HW125_MODE_OAWR); 

    return TRUE; 
}


// Transfer data log contents 
uint8_t ui_tx(void)
{
    // Read a line from the data log and send it out via the Bluetooth module. 
    hw125_gets(mtbdl_ui.data_buff, MTBDL_MAX_STR_LEN); 
    hc05_send(mtbdl_ui.data_buff); 

    // Check for end of file - if true we can stop the transaction 
    if (hw125_eof())
    {
        mtbdl_ui.tx_status = SET_BIT; 
        return TRUE; 
    }

    return FALSE; 
}


// End the transmission 
void ui_tx_end(void)
{
    hw125_close(); 

    if (mtbdl_ui.tx_status)
    {
        // Transaction completed - delete the file and update the log index 
        hw125_unlink(mtbdl_ui.filename); 
        mtbdl_ui.tx_status = CLEAR_BIT; 
        param_update_log_index(PARAM_LOG_INDEX_DEC); 
    }
}

//=======================================================================================


//=======================================================================================
// Setters 

// Store the LED colour to use 
void ui_led_colour_set(
    ws2812_led_index_t led_num, 
    uint32_t colour)
{
    mtbdl_ui.led_colours[led_num] = colour; 
}


// Change the LED colour to write next 
void ui_led_colour_change(
    ws2812_led_index_t led_num, 
    uint32_t colour)
{
    mtbdl_ui.led_write_data[led_num] = colour; 
}


// Update the blinking LED duty cycle 
void ui_led_duty_set(
    ws2812_led_index_t led_num, 
    uint8_t duty_cycle)
{
    if (led_num <= WS2812_LED_3)
    {
        mtbdl_ui.led_state[led_num].duty_cycle = duty_cycle; 
    }
}

//=======================================================================================
