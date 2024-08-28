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
// 

static mtbdl_ui_t mtbdl_ui; 

//=======================================================================================


//=======================================================================================
// Initialization 

// User interface init 
void ui_init(
    GPIO_TypeDef *btn_port, 
    gpio_pin_num_t btn1, 
    gpio_pin_num_t btn2, 
    gpio_pin_num_t btn3, 
    gpio_pin_num_t btn4)
{
    // User button pin numbers 
    mtbdl_ui.user_btn_1 = (uint8_t)btn1; 
    mtbdl_ui.user_btn_2 = (uint8_t)btn2; 
    mtbdl_ui.user_btn_3 = (uint8_t)btn3; 
    mtbdl_ui.user_btn_4 = (uint8_t)btn4; 

    // 
    mtbdl_ui.user_btn_1_block = CLEAR_BIT; 
    mtbdl_ui.user_btn_2_block = CLEAR_BIT; 
    mtbdl_ui.user_btn_3_block = CLEAR_BIT; 
    mtbdl_ui.user_btn_4_block = CLEAR_BIT; 
}

//=======================================================================================


//=======================================================================================
// Input - buttons, RX mode 

// Button press check 
void ui_button_press(void)
{
    // Button 1 
    if (debounce_pressed(mtbdl_ui.user_btn_1) && !mtbdl_ui.user_btn_1_block)
    {
        mtbdl_ui.user_btn_1_block = SET_BIT; 
    }
    
    // Button 2 
    else if (debounce_pressed(mtbdl_ui.user_btn_2) && !mtbdl_ui.user_btn_2_block)
    {
        mtbdl_ui.user_btn_2_block = SET_BIT; 
    }
    
    // Button 3 
    else if (debounce_pressed(mtbdl_ui.user_btn_3) && !mtbdl_ui.user_btn_3_block)
    {
        mtbdl_ui.user_btn_3_block = SET_BIT; 
    }
    
    // Button 4 
    else if (debounce_pressed(mtbdl_ui.user_btn_4) && !mtbdl_ui.user_btn_4_block)
    {
        mtbdl_ui.user_btn_4_block = SET_BIT; 
    }
}


// Button release check 
void ui_button_release(void)
{
    // 
}

//=======================================================================================


//=======================================================================================
// Output - screen, TX mode 
//=======================================================================================
