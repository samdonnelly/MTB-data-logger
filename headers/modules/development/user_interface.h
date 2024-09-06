/**
 * @file user_interface.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief User interface header 
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

//=======================================================================================


//=======================================================================================
// Macos 

#define UI_LED_DUTY_SHORT 20        // 5ms interrupt * 20 == 100ms duty cycle 
#define UI_LED_DUTY_LONG 100        // 5ms interrupt * 100 == 500ms duty cycle 

//=======================================================================================


//=======================================================================================
// Enums 

typedef enum {
    UI_BTN_NONE, 
    UI_BTN_1, 
    UI_BTN_2, 
    UI_BTN_3, 
    UI_BTN_4 
} ui_btn_num_t; 

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
    GPIO_TypeDef *user_btn_port;                // GPIO port for user buttons 

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
}
mtbdl_ui_t; 

//=======================================================================================


//=======================================================================================
// Initialization 

/**
 * @brief User interface init 
 * 
 * @details 
 *          
 *          The button pins must be pins 0-7. Anything higher will be truncated. This 
 *          happens due to the switch debounce driver data size. 
 * 
 * @param btn_port 
 * @param btn1 
 * @param btn2 
 * @param btn3 
 * @param btn4 
 */
void ui_init(
    GPIO_TypeDef *btn_port, 
    pin_selector_t btn1, 
    pin_selector_t btn2, 
    pin_selector_t btn3, 
    pin_selector_t btn4); 

//=======================================================================================


//=======================================================================================
// Device update 

/**
 * @brief Button status update 
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
 * @param led : LED to update 
 */
void ui_led_state_update(ws2812_led_index_t led); 


/**
 * @brief Update GPS position status 
 */
void ui_gps_led_status_update(void); 

//=======================================================================================


//=======================================================================================
// Screen control 
//=======================================================================================


//=======================================================================================
// RX mode 
//=======================================================================================


//=======================================================================================
// TX mode 
//=======================================================================================


//=======================================================================================
// Setters 

/**
 * @brief Store the LED colour to use 
 * 
 * @param led_num 
 * @param colour 
 */
void ui_led_colour_set(
    ws2812_led_index_t led_num, 
    uint32_t colour); 


/**
 * @brief Change the LED colour to write next 
 * 
 * @param led_num 
 * @param colour 
 */
void ui_led_colour_change(
    ws2812_led_index_t led_num, 
    uint32_t colour); 


/**
 * @brief Update the blinking LED duty cycle 
 * 
 * @param led_num 
 * @param duty_cycle 
 */
void ui_led_duty_set(
    ws2812_led_index_t led_num, 
    uint8_t duty_cycle); 

//=======================================================================================

#endif   // _USER_INTERFACE_H_ 
