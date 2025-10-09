/**
 * @file user_interface.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief UI (user interface) module interface 
 * 
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _USER_INTERFACE_H_ 
#define _USER_INTERFACE_H_ 

//=======================================================================================
// Includes 

#include "includes_drivers.h" 
#include "string_config.h" 

//=======================================================================================


//=======================================================================================
// Macos 

// Timing 
#define UI_LED_DUTY_SHORT 20        // 5ms interrupt * 20 == 100ms duty cycle 
#define UI_LED_DUTY_LONG 100        // 5ms interrupt * 100 == 500ms duty cycle 

// Buffer sizes 
#define UI_HC05_BUFF_SIZE 200 

//=======================================================================================


//=======================================================================================
// Enums 

// User button number 
typedef enum {
    UI_BTN_NONE, 
    UI_BTN_1, 
    UI_BTN_2, 
    UI_BTN_3, 
    UI_BTN_4 
} ui_btn_num_t; 


// These screen messages contain data that changes (such as SOC and GPS position lock) 
typedef enum {
    UI_MSG_IDLE,       // Idle state message 
    UI_MSG_RUN_PREP,   // Run prep state message 
    UI_MSG_NUM         // Number of messages in the index list 
} ui_msg_update_index_t; 

//=======================================================================================


//=======================================================================================
// Structures 

// LED blink record 
typedef struct mtbdl_ui_led_blink_s 
{
    ws2812_led_index_t led_num; 
    uint16_t duty_cycle; 
    uint8_t update_blocker; 
}
mtbdl_ui_led_blink_t; 


// User interface data record 
typedef struct mtbdl_ui_s 
{
    // Peripherals 
    GPIO_TypeDef *user_btn_port;                // GPIO port for user buttons 

    // System info 
    uint16_t navstat;                           // Navigation status of GPS module 
    uint8_t soc;                                // Battery state of charge 

    // User buttons 
    uint8_t user_btn_1;                         // User button 1 pin number 
    uint8_t user_btn_2;                         // User button 2 pin number 
    uint8_t user_btn_3;                         // User button 3 pin number 
    uint8_t user_btn_4;                         // User button 4 pin number 
    uint8_t user_btn_1_block : 1;               // Stops repeated actions on btn 1 press 
    uint8_t user_btn_2_block : 1;               // Stops repeated actions on btn 2 press 
    uint8_t user_btn_3_block : 1;               // Stops repeated actions on btn 3 press 
    uint8_t user_btn_4_block : 1;               // Stops repeated actions on btn 4 press 

    // LEDs - Green bits: 16-23, Red bits: 8-15, Blue bits: 0-7 
    uint32_t led_colours[WS2812_LED_NUM];       // LED colours 
    uint32_t led_write_data[WS2812_LED_NUM];    // LED write buffer 
    uint16_t led_counter; 
    mtbdl_ui_led_blink_t led_state[WS2812_LED_NUM]; 

    // Screen 
    uint16_t msg_counter;                       // Screen message write counter 

    // Bluetooth data 
    USART_TypeDef *uart; 
    DMA_Stream_TypeDef *dma_stream; 
    uint8_t cb[UI_HC05_BUFF_SIZE];              // Circular buffer populated by DMA 
    cb_index_t cb_index;                        // Circular buffer indexing info 
    dma_index_t dma_index;                      // DMA transfer indexing info 
    uint8_t data_in_buff[UI_HC05_BUFF_SIZE];    // Buffer that stores latest UART input 

    // TX mode 
    uint8_t tx_send_status : 1;                 // TX log file send status 
    uint8_t tx_hs_status   : 1;                 // TX log file handshake status 

    // SD Card 
    char data_buff[MTBDL_MAX_STR_LEN];          // Buffer for reading and writing 
    char filename[MTBDL_MAX_STR_LEN];           // Buffer for storing a file name 
}
mtbdl_ui_t; 

//=======================================================================================


//=======================================================================================
// Initialization 

/**
 * @brief User interface init 
 * 
 * @details Sets module data to its default value, sets up the user buttons and 
 *          initializes button debouncing. 
 *          
 *          NOTE: The button pins must be pins 0-7. Anything higher will be truncated. 
 *                This happens due to the switch debounce driver data size. 
 * 
 * @param btn_port : GPIO port of the buttons 
 * @param btn1 : button 1 pin 
 * @param btn2 : button 2 pin 
 * @param btn3 : button 3 pin 
 * @param btn4 : button 4 pin 
 * @param uart : UART port of HC-05 
 * @param dma_stream : DMA stream address of HC-05 
 */
void ui_init(
    GPIO_TypeDef *btn_port, 
    pin_selector_t btn1, 
    pin_selector_t btn2, 
    pin_selector_t btn3, 
    pin_selector_t btn4,
    USART_TypeDef *uart,
    DMA_Stream_TypeDef *dma_stream); 

//=======================================================================================


//=======================================================================================
// Device update 

/**
 * @brief Periodic UI update 
 * 
 * @details Periodically checks user button press/release, updates an LED flashing 
 *          counter, calculates battery SOC and updates a screen message refresh counter. 
 *          The period of the check is controlled by a periodic interrupt and the checks 
 *          called from this function may have their own counter threshold to divide the 
 *          period further. This function is called continuously regardless of the system 
 *          state. 
 * 
 * @return ui_btn_num_t : button currently being pressed 
 */
ui_btn_num_t ui_status_update(void); 

//=======================================================================================


//=======================================================================================
// LED control 

/**
 * @brief Change the state of the LED 
 * 
 * @details Used to toggle an LED in a flashing/strobing/blinking pattern. This function 
 *          must be repeatedly called in order for the pattern to maintain consistant 
 *          timing. The LED will be alternated between the colour set using 
 *          ui_led_colour_set and off with a duty cycle that can be set using 
 *          ui_led_duty_set. The function checks the state of a counter to know when 
 *          and what colour to update the LED to and this counter is controlled by the 
 *          ui_status_update function. 
 * 
 * @see ui_status_update 
 * @see ui_led_duty_set 
 * @see ui_led_colour_set 
 * 
 * @param led : LED to update 
 */
void ui_led_state_update(ws2812_led_index_t led); 


/**
 * @brief Update GPS position status LED 
 * 
 * @details Checks for a GPS position lock and updates the GPS LED accordingly. If there 
 *          is a position lock then ui_led_state_update will be used to strobe the GPS 
 *          LED. If there is no position lock then the LED will be forced off. This 
 *          function must be called continuously for the LED to accurately reflect the 
 *          GPS status. 
 */
void ui_gps_led_status_update(void); 

//=======================================================================================


//=======================================================================================
// Screen control 

/**
 * @brief Update screen message output 
 * 
 * @details This function is used to update screen messages that have dynamic information 
 *          (i.e. info that changes within a given screen message). It uses the periodic 
 *          interrpt for user interface updates to control the frequency at which the 
 *          specified message gets refreshed. This function is used in states such as the 
 *          idle state which contains info about GPS lock and battery SOC. This function 
 *          must be repeatedly called for updates to happen on schedule. States that don't 
 *          have dynamic messages (i.e. states not available as arguments) don't need 
 *          and can't use this function. 
 * 
 * @see ui_status_update 
 * 
 * @param msg_index : message to update 
 */
void ui_msg_update(ui_msg_update_index_t msg_index); 


/**
 * @brief Format the idle state message 
 * 
 * @details The idle state message contains information relevant to the user that can 
 *          change based on the state of the system. This function updates the values of 
 *          the idle state message and triggers a write of this message to the screen. 
 *          Information in the idle state message includes: 
 *          - Fork settings & travel (set manually by the user) 
 *          - Shock settings & travel (set manually by the user) 
 *          - Wheel size (manually set by the user) 
 *          - Battery SOC 
 *          - GPS position lock status 
 */
void ui_set_idle_msg(void); 


/**
 * @brief Format the run prep state message 
 * 
 * @details The run prep state message contains the GPS position lock status. This 
 *          information is displayed to the user before entering the run mode and allows 
 *          the user to know if they have GPS lock before beginning to record data. This 
 *          function updates GPS status information and triggers a write of this message 
 *          to the screen. 
 */
void ui_set_run_prep_msg(void); 


/**
 * @brief Format the pre TX state message 
 * 
 * @details The pre TX state message contains the current log file index which tells the 
 *          user how many log files are available to be sent. This function adds the log 
 *          index to the message before writing it to the screen. 
 */
void ui_set_pretx_msg(void); 

//=======================================================================================


//=======================================================================================
// RX mode 

/**
 * @brief RX user interface start 
 * 
 * @details Passes a user prompt to the Bluetooth module which gets sent to an external 
 *          device. Note that the Bluetooth module must be connected to a device that can 
 *          display the sent messages for this to have an effect. The prompt tells the 
 *          user that the system is ready to receive its input. 
 */
void ui_rx_prep(void); 


/**
 * @brief Read user input 
 * 
 * @details Poles the Bluetooth module for new data, then once new data is available it's 
 *          read and checked against valid commands. If a match is found then system 
 *          settings/parameters are updated. If the user input does not match one of the 
 *          available commands then nothing will change. This function should be called 
 *          continuously to check for data and provide a new user prompt after data is 
 *          input. 
 */
void ui_rx(void); 

//=======================================================================================


//=======================================================================================
// TX mode 

/**
 * @brief Prepare to send a data log file 
 * 
 * @details If a log file exists that matches the most recent data log index then this 
 *          function will open that file and return true. Otherwise it will return false. 
 * 
 * @return uint8_t : status of the file check 
 */
uint8_t ui_tx_prep(void); 


/**
 * @brief Transfer data log contents 
 * 
 * @details Reads a single line from an open data log file and passes the info to the 
 *          Bluetooth module for sending out to an external device. The end of the file 
 *          is checked for after each line. Once the end of the file is seen the function 
 *          will return false. This function does not loop so it needs to be repeatedly 
 *          called. 
 * 
 *          Note that this function does not check for a valid open file. The TX prep 
 *          function should be called before this function to make sure there is a file 
 *          open before trying to read from the SD card. 
 * 
 * @see ui_tx_prep 
 * 
 * @return uint8_t : end of file status 
 */
uint8_t ui_tx(void); 


/**
 * @brief Close the log file and delete it 
 * 
 * @details Closes the open data log file and waits for a confirmation response 
 *          (handshake) from the device connected to the Bluetooth module. If the 
 *          transaction was completed successfully and a confirmation is received then 
 *          the log file that was sent gets deleted and the log file index updated. Note 
 *          that this function should only be called after 'ui_tx' is done being called. 
 * 
 * @return uint8_t : handshake status 
 */
uint8_t ui_tx_end(void); 

//=======================================================================================


//=======================================================================================
// Setters 

/**
 * @brief Store the LED colour to use 
 * 
 * @details This function does not write the new colour to the LED. Instead it saves the 
 *          colour so an LED can be changed to this colour in future write events. This is 
 *          used for cases where an LED changes between on and off states based on other 
 *          events. This includes user buttons LEDs that light up when buttons are pressed 
 *          and strobe/flashing LEDs that change on a timer. This colour is saved until 
 *          updated next. 
 * 
 * @param led_num : LED to set 
 * @param colour : LED colour to set 
 */
void ui_led_colour_set(
    ws2812_led_index_t led_num, 
    uint32_t colour); 


/**
 * @brief Change the LED colour to write next 
 * 
 * @details This function effectively writes the new colout to the LED. The state of all 
 *          LEDs gets updated at a fixed period and the colour set in this function will 
 *          be reflected in the next write event. The period at which the LEDs are 
 *          updated is fast enough so that a user would not notice a delay between 
 *          this function call and a colour change. 
 * 
 * @param led_num : LED to change 
 * @param colour : LED colour to set 
 */
void ui_led_colour_change(
    ws2812_led_index_t led_num, 
    uint32_t colour); 


/**
 * @brief Update the blinking LED duty cycle 
 * 
 * @details Some LEDs in the system are set to blink/flash/stobe and this function 
 *          changes the amount of time they spend lit up. The period at which they 
 *          flash is fixed. 
 *          
 *          Note that only system LEDs 0-3 (non-user button LEDs) are able to flash. 
 *          Specifying an LED number greater than this will be ignored. 
 * 
 * @param led_num : LED to set 
 * @param duty_cycle : on time of the LED 
 */
void ui_led_duty_set(
    ws2812_led_index_t led_num, 
    uint8_t duty_cycle); 

//=======================================================================================


//=======================================================================================
// Getters 

/**
 * @brief Get battery SOC 
 * 
 * @details The battery SOC gets updated using the ui_status_update function. 
 * 
 * @see ui_status_update 
 * 
 * @return uint8_t : battery SOC 
 */
uint8_t ui_get_soc(void); 

//=======================================================================================

#endif   // _USER_INTERFACE_H_ 
