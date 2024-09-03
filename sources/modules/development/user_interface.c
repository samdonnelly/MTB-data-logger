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
#include "ws2812_config.h" 
#include "stm32f4xx_it.h" 

//=======================================================================================


//=======================================================================================
// Macros 

#define UI_LED_COUNTER_PERIOD 200   // 5ms interrupt * 200 == 1s counter period 
#define UI_LED_WRITE_PERIOD 10      // 5ms interrupt * 10 == 50ms write period 

#define UI_LED_DUTY_SHORT 20        // 5ms interrupt * 20 == 100ms duty cycle 
#define UI_LED_DUTY_LONG 100        // 5ms interrupt * 100 == 500ms duty cycle 

//=======================================================================================


//=======================================================================================
// Variables 

static mtbdl_ui_t mtbdl_ui; 

//=======================================================================================


//=======================================================================================
// Prototypes 

// Update the LED output 
void ui_led_update(void); 

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
// Device update 

// User interface update 
void ui_status_update(void)
{
    // 5ms interrupt 
    if (handler_flags.tim1_up_tim10_glbl_flag)
    {
        handler_flags.tim1_up_tim10_glbl_flag = CLEAR; 

        // Update user button input status 
        debounce((uint8_t)gpio_port_read(mtbdl_ui.user_btn_port)); 

        // Update LED timing and output 
        ui_led_update(); 
    }
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
        mtbdl_ui.user_btn_1_block = CLEAR; 
        ui_led_colour_change(WS2812_LED_7, mtbdl_led_clear); 
    }
    
    // Button 2 
    if (debounce_released(mtbdl_ui.user_btn_2) && mtbdl_ui.user_btn_2_block)
    {
        mtbdl_ui.user_btn_2_block = CLEAR; 
        ui_led_colour_change(WS2812_LED_6, mtbdl_led_clear); 
    }
    
    // Button 3 
    if (debounce_released(mtbdl_ui.user_btn_3) && mtbdl_ui.user_btn_3_block)
    {
        mtbdl_ui.user_btn_3_block = CLEAR; 
        ui_led_colour_change(WS2812_LED_5, mtbdl_led_clear); 
    }
    
    // Button 4 
    if (debounce_released(mtbdl_ui.user_btn_4) && mtbdl_ui.user_btn_4_block)
    {
        mtbdl_ui.user_btn_4_block = CLEAR; 
        ui_led_colour_change(WS2812_LED_4, mtbdl_led_clear); 
    }
}

//=======================================================================================


//=======================================================================================
// Output - LEDs, screen, TX mode 

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


// Change the state of the LED 
void ui_led_state_update(ws2812_led_index_t led_num)
{
    if (led_num > WS2812_LED_3)
    {
        return; 
    }

    mtbdl_ui_led_blink_t led = mtbdl_ui.led_state[led_num]; 

    if ((mtbdl_ui.led_counter < led.duty_cycle) && !led.update_blocker)
    {
        // Set the LED colour 
        ui_led_colour_change(led.led_num, mtbdl_ui.led_colours[led.led_num]); 
        led.update_blocker = SET_BIT; 
    }
    else if ((mtbdl_ui.led_counter >= led.duty_cycle) && led.update_blocker)
    {
        // Clear the LED colour 
        ui_led_colour_change(led.led_num, mtbdl_led_clear); 
        led.update_blocker = CLEAR_BIT; 
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
