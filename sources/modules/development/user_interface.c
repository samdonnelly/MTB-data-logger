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

//=======================================================================================


//=======================================================================================
// Variables 

static mtbdl_ui_t mtbdl_ui; 

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
}

//=======================================================================================


//=======================================================================================
// Input - buttons, RX mode 

// Button press check 
ui_btn_num_t ui_button_press(void)
{
    ui_btn_num_t btn_num = UI_BTN_NONE; 

    // Button 1 
    if (debounce_pressed(mtbdl_ui.user_btn_1) && !mtbdl_ui.user_btn_1_block)
    {
        mtbdl_ui.user_btn_1_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_7, mtbdl_ui.user_led_7); 
        btn_num = UI_BTN_1; 
    }
    
    // Button 2 
    else if (debounce_pressed(mtbdl_ui.user_btn_2) && !mtbdl_ui.user_btn_2_block)
    {
        mtbdl_ui.user_btn_2_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_6, mtbdl_ui.user_led_6); 
        btn_num = UI_BTN_2; 
    }
    
    // Button 3 
    else if (debounce_pressed(mtbdl_ui.user_btn_3) && !mtbdl_ui.user_btn_3_block)
    {
        mtbdl_ui.user_btn_3_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_5, mtbdl_ui.user_led_5); 
        btn_num = UI_BTN_3; 
    }
    
    // Button 4 
    else if (debounce_pressed(mtbdl_ui.user_btn_4) && !mtbdl_ui.user_btn_4_block)
    {
        mtbdl_ui.user_btn_4_block = SET_BIT; 
        mtbdl_led_update(WS2812_LED_4, mtbdl_ui.user_led_4); 
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
        mtbdl_ui.user_btn_1_block = CLEAR; 
        // mtbdl_led_update(WS2812_LED_7, mtbdl_led_clear); 
    }
    
    // Button 2 
    if (debounce_released(mtbdl_ui.user_btn_2) && mtbdl_ui.user_btn_2_block)
    {
        mtbdl_ui.user_btn_2_block = CLEAR; 
        // mtbdl_led_update(WS2812_LED_6, mtbdl_led_clear); 
    }
    
    // Button 3 
    if (debounce_released(mtbdl_ui.user_btn_3) && mtbdl_ui.user_btn_3_block)
    {
        mtbdl_ui.user_btn_3_block = CLEAR; 
        // mtbdl_led_update(WS2812_LED_5, mtbdl_led_clear); 
    }
    
    // Button 4 
    if (debounce_released(mtbdl_ui.user_btn_4) && mtbdl_ui.user_btn_4_block)
    {
        mtbdl_ui.user_btn_4_block = CLEAR; 
        // mtbdl_led_update(WS2812_LED_4, mtbdl_led_clear); 
    }
}

//=======================================================================================


//=======================================================================================
// Output - screen, TX mode, LEDs 
//=======================================================================================


//=======================================================================================
// Setters 

// LED colour update 
void ui_led_update(
    uint8_t led, 
    uint32_t colour)
{
    // 
}

//=======================================================================================
