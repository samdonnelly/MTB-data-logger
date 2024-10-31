/**
 * @file user_interface.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief UI (user interface) module 
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
#include "data_logging.h" 
#include "system_parameters.h" 

#include "ws2812_config.h" 
#include "hd44780u_config.h" 
#include "battery_config.h" 
#include "stm32f4xx_it.h" 

//=======================================================================================


//=======================================================================================
// Macros 

// Initialization 
#define UI_SOC_INIT 100                // Initial SOC value - ensures not low battery 

// Timing 
#define UI_LED_COUNTER_PERIOD 200      // 5ms interrupt * 200 == 1s counter period 
#define UI_LED_WRITE_PERIOD 10         // 5ms interrupt * 10 == 50ms write period 
#define UI_SOC_CALC_PERIOD 2000        // 5ms interrupt * 2000 == 10s calculation period 
#define UI_MSG_COUNTER_PERIOD 2000     // 5ms interrupt * 2000 == 10s counter period 

// Data offsets 
#define UI_LOG_INDEX_OFFSET 1          // Difference between log index and number 
#define UI_SCREEN_LINE_CHAR_OFFSET 1   // Prevents NULL from being the last line character 

//=======================================================================================


//=======================================================================================
// Prototypes 

/**
 * @brief Screen message formatting function pointer 
 */
typedef void (*ui_screen_msg_func_ptr)(void); 


/**
 * @brief Button press check 
 * 
 * @details Called by the ui_status_update function using a periodic interrupt. 
 *          Checks the state of each of the user buttons using the switch debouncer to 
 *          see if the button is pressed. If a button is seen to be pressed then the 
 *          button pressed will be returned and the LED corresponding to the button 
 *          will light up. Only one button press will be acknowledged at a given time 
 *          meaning pressing two or more buttons at once will not register all the 
 *          inputs. 
 * 
 * @see ui_status_update 
 * 
 * @return ui_btn_num_t : button currently being pressed 
 */
ui_btn_num_t ui_button_press(void); 


/**
 * @brief Button release check 
 * 
 * @details Called by the ui_status_update function using a periodic interrupt. 
 *          Similar to ui_button_press but checks for buttons being newly released. If 
 *          a button was just released then the LED for the button will be shut off. 
 * 
 * @see ui_status_update 
 */
void ui_button_release(void); 


/**
 * @brief Update the LED output 
 * 
 * @details Called by the ui_status_update function using a periodic interrupt. 
 *          Increments an LED counter used for flashing/strobing/blinking LED that are 
 *          controlled by the ui_led_state_update function. Also uses an internal counter 
 *          to control when to write to the LEDs to update their colour as needed. 
 * 
 * @see ui_status_update 
 * @see ui_led_state_update 
 */
void ui_led_update(void); 


/**
 * @brief Update the SOC calculation 
 * 
 * @details Called by the ui_status_update function using a periodic interrupt. 
 *          Uses an internal counter to control when to re-calculate the battery SOC. The 
 *          SOC can be fetched using the ui_get_soc getter. The SOC is used for user 
 *          feedback and to put the system into low power mode if needed. 
 * 
 * @see ui_status_update 
 * @see ui_get_soc 
 */
void ui_soc_update(void); 


/**
 * @brief Update screen message timer 
 * 
 * @details Called by the ui_status_update function using a periodic interrupt. 
 *          Increments a counter used by the ui_msg_update function to control when 
 *          screen messages get updated. 
 * 
 * @see ui_status_update 
 * @see ui_msg_update 
 */
void ui_msg_timer_update(void); 

//=======================================================================================


//=======================================================================================
// Variables 

static mtbdl_ui_t mtbdl_ui; 

// Function pointers to screen message formatting functions 
static ui_screen_msg_func_ptr msg_table[UI_MSG_NUM] = 
{
    &ui_set_idle_msg,       // Idle state message 
    &ui_set_run_prep_msg    // Run prep state message 
}; 

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
    // Peripheral initialization 
    mtbdl_ui.user_btn_port = btn_port; 

    // Initialize system info 
    mtbdl_ui.navstat = M8Q_NAVSTAT_NF; 
    mtbdl_ui.soc = UI_SOC_INIT; 

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

    // Initialize screen info 
    mtbdl_ui.msg_counter = CLEAR; 

    // TX mode info 
    mtbdl_ui.tx_send_status = CLEAR_BIT; 
    mtbdl_ui.tx_hs_status = CLEAR_BIT; 

    // Initialize SD card info 
    memset((void *)mtbdl_ui.data_buff, CLEAR, sizeof(mtbdl_ui.data_buff)); 
    memset((void *)mtbdl_ui.filename, CLEAR, sizeof(mtbdl_ui.filename)); 
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

        // Update screen message timer 
        ui_msg_timer_update(); 
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
    if (led_write_counter++ >= UI_LED_WRITE_PERIOD)
    {
        ws2812_send(DEVICE_ONE, mtbdl_ui.led_write_data); 
        led_write_counter = CLEAR; 
    }
}


// Update the SOC calculation 
void ui_soc_update(void)
{
    // The SOC counter is set to trigger a calculation right away so the SOC is updated 
    // for the first screen message shown to the user. This isn't handled in the init 
    // function because DMA is not yet enabled so we can't read the ADC. 
    static uint16_t soc_calc_counter = UI_SOC_CALC_PERIOD; 

    if (soc_calc_counter++ >= UI_SOC_CALC_PERIOD)
    {
        // Read the battery voltage and calculate the current SOC using battery specific 
        // information. 
        soc_calc_counter = CLEAR; 
        mtbdl_ui.soc = battery_soc_calc(log_get_batt_voltage()); 
    }
}


// Update screen message timer 
void ui_msg_timer_update(void)
{
    mtbdl_ui.msg_counter++; 
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

// Update screen message output 
void ui_msg_update(ui_msg_update_index_t msg_index)
{
    if (msg_index >= UI_MSG_NUM)
    {
        return; 
    }

    if (mtbdl_ui.msg_counter >= UI_MSG_COUNTER_PERIOD)
    {
        mtbdl_ui.msg_counter = CLEAR; 
        msg_table[msg_index](); 
    }
}


// Format the idle state message 
void ui_set_idle_msg(void)
{
    hd44780u_msgs_t msg[MTBDL_MSG_LEN_4_LINE]; 
    uint8_t str_len = HD44780U_LINE_LEN + UI_SCREEN_LINE_CHAR_OFFSET; 
    char line1[str_len], line2[str_len], line3[str_len]; 

    // Create an editable copy of the message 
    for (uint8_t i = CLEAR; i < MTBDL_MSG_LEN_4_LINE; i++) 
    {
        msg[i] = mtbdl_idle_msg[i]; 
    }

    mtbdl_ui.navstat = m8q_get_position_navstat(); 

    // Format the messages with data 
    // snprintf will NULL terminate the string at the screen line length so in order to use 
    // the last spot on the screen line the message length must be indexed up by one. 
    // But since indexing up by one would go over the screen line length, the data must 
    // then be recopied to the message buffer to write to the screen. 

    snprintf(line1, 
             str_len, 
             mtbdl_idle_msg[HD44780U_L1].msg, 
             param_get_bike_setting(PARAM_BIKE_SET_FPSI), 
             param_get_bike_setting(PARAM_BIKE_SET_FC), 
             param_get_bike_setting(PARAM_BIKE_SET_FR), 
             param_get_bike_setting(PARAM_BIKE_SET_FT)); 
    memcpy((void *)msg[HD44780U_L1].msg, (void *)line1, HD44780U_LINE_LEN); 
    
    snprintf(line2, 
             str_len, 
             mtbdl_idle_msg[HD44780U_L2].msg, 
             param_get_bike_setting(PARAM_BIKE_SET_SPSI), 
             param_get_bike_setting(PARAM_BIKE_SET_SL), 
             param_get_bike_setting(PARAM_BIKE_SET_SR), 
             param_get_bike_setting(PARAM_BIKE_SET_ST)); 
    memcpy((void *)msg[HD44780U_L1].msg, (void *)line1, HD44780U_LINE_LEN); 
    
    snprintf(line3, 
             str_len, 
             mtbdl_idle_msg[HD44780U_L3].msg, 
             param_get_bike_setting(PARAM_BIKE_SET_WS), 
             mtbdl_ui.soc, 
             (char)(mtbdl_ui.navstat >> SHIFT_8), 
             (char)(mtbdl_ui.navstat)); 
    memcpy((void *)msg[HD44780U_L1].msg, (void *)line1, HD44780U_LINE_LEN); 

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
    memset((void *)mtbdl_ui.data_buff, CLEAR, sizeof(mtbdl_ui.data_buff)); 
    hc05_send(mtbdl_rx_prompt); 
    hc05_clear(); 
}


// Read user input 
void ui_rx(void)
{
    // These are set to their max value so they won't accidentally update any of the 
    // parameters. They are also unsigned ints because other data types were causing 
    // scanning issues. 
    unsigned int param_index = ~CLEAR, setting_data = ~CLEAR; 

    // Check if Bluetooth data is available 
    if (hc05_data_status())
    {
        // Read the data from the HC-05 
        hc05_read(mtbdl_ui.data_buff, MTBDL_MAX_STR_LEN); 

        // Check if the transmission was successful. If so then parse the input and 
        // check if the input provided by the user is valid. If it is then send a 
        // confirmation message back to the user. If the transmission was not 
        // successful or the input is not valid then ignore the input. If the 
        // transaction is not successful, we ignore the input and reset the Bluetooth 
        // module status so the system doesn't enter the fault state as this is not a 
        // critical fault. 
        if (!hc05_get_status())
        {
            sscanf(mtbdl_ui.data_buff, 
                   mtbdl_rx_input, 
                   &param_index, 
                   &setting_data); 

            if (param_index < PARAM_BIKE_SET_NONE)
            {
                if (param_update_bike_setting((param_bike_set_index_t)param_index, 
                                              (uint16_t)setting_data))
                {
                    hc05_send(mtbdl_rx_confirm); 
                }
            }
        }

        hc05_clear_status(); 
        ui_rx_prep(); 
    }
}

//=======================================================================================


//=======================================================================================
// TX mode 

// Prepare to send a data log file 
uint8_t ui_tx_prep(void)
{
    uint8_t log_index = param_get_log_index(); 

    // Clear any leftover data that might be in the Bluetooth devices UART register. 
    hc05_clear(); 

    // Check if there are no log files 
    if (!log_index)
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
             (log_index - UI_LOG_INDEX_OFFSET)); 

    // Check for the existance of the specified file number 
    if (hw125_get_exists(mtbdl_ui.filename) == FR_NO_FILE)
    {
        // If the file does not exist then decrement the file index. At the top of this 
        // function we already check if the index is 0 which means if we get to this 
        // point then the index is greater than 0. 
        param_update_log_index(PARAM_LOG_INDEX_DEC); 
        return FALSE; 
    }

    hw125_open(mtbdl_ui.filename, HW125_MODE_OAWR); 

    // Initialize the user interface for sending a log file. This is helpful when 
    // multiple log files are sent and they need seperation between one another. 
    hc05_send(mtbdl_tx_ui_init); 

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
        mtbdl_ui.tx_send_status = SET_BIT; 
        hc05_send(mtbdl_tx_prompt); 
        return TRUE; 
    }

    return FALSE; 
}


// End the transmission 
uint8_t ui_tx_end(void)
{
    uint8_t hs_status = FALSE; 

    // Close the log file then check for a response from the connected device that the 
    // data log file was successfully received or not. Responses that don't match the 
    // predefined confirmations will be ignored. The log file must have been fully sent 
    // without interruption and a positive confirmation must be received for the log file 
    // to be deleted and the log index updated. A negative confirmation will return the 
    // same status as a positive one but the log file will not be deleted. Special cases 
    // such as a lost Bluetooth connection or a fault will simply close the log file and 
    // ignore the feedback from the connected device. 

    hw125_close(); 

    if (hc05_data_status())
    {
        hc05_read(mtbdl_ui.data_buff, MTBDL_MAX_STR_LEN); 
        const char *user_msg = mtbdl_rx_confirm; 

        if (!strcmp(mtbdl_tx_complete, mtbdl_ui.data_buff))
        {
            mtbdl_ui.tx_hs_status = SET_BIT; 
        }
        else if (!strcmp(mtbdl_tx_not_complete, mtbdl_ui.data_buff))
        {
            hs_status = TRUE; 
        }
        else 
        {
            user_msg = mtbdl_tx_prompt; 
        }

        hc05_send(user_msg); 
    }

    if (mtbdl_ui.tx_send_status && mtbdl_ui.tx_hs_status)
    {
        mtbdl_ui.tx_send_status = CLEAR_BIT; 
        mtbdl_ui.tx_hs_status = CLEAR_BIT; 

        // Transaction completed - delete the file and update the log index 
        hw125_unlink(mtbdl_ui.filename); 
        param_update_log_index(PARAM_LOG_INDEX_DEC); 

        hs_status = TRUE; 
    }

    return hs_status; 
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


//=======================================================================================
// Getters 

// Get battery SOC 
uint8_t ui_get_soc(void)
{
    return mtbdl_ui.soc; 
}

//=======================================================================================
