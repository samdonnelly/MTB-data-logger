/**
 * @file mtbdl_interface.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL interface 
 * 
 * @version 0.1
 * @date 2024-03-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _MTBDL_INTERFACE_H_ 
#define _MTBDL_INTERFACE_H_ 

#ifdef __cplusplus
extern "C" {
#endif

//=======================================================================================
// Includes 

#include "includes_drivers.h" 
#include "includes_app.h" 

//=======================================================================================


//=======================================================================================
// Functions 

/**
 * @brief Function that gets called once. Initialization function calls go here.
 */
void mtbdl_init(void); 


/**
 * @brief MTB DL controller init 
 * 
 * @details Initializes the main controller tracking information. Must be called during 
 *          setup of the system. 
 * 
 * @param timer_nonblocking : TIM port used for non-blocking delays 
 * @param user_btn_gpio : GPIO port used for user buttons 
 * @param user_btn_1 : user button 1 pin number 
 * @param user_btn_2 : user button 2 pin number 
 * @param user_btn_3 : user button 3 pin number 
 * @param user_btn_4 : user button 4 pin number 
 */
void mtbdl_app_init(
    TIM_TypeDef *timer_nonblocking, 
    GPIO_TypeDef *user_btn_gpio, 
    gpio_pin_num_t user_btn_1, 
    gpio_pin_num_t user_btn_2, 
    gpio_pin_num_t user_btn_3, 
    gpio_pin_num_t user_btn_4); 


/**
 * @brief MTB DL application 
 * 
 * @details Checks for system faults, updates the status of user buttons, executes the 
 *          systems state machine and calls devices controllers. 
 */
void mtbdl_app(void); 

//=======================================================================================

#ifdef __cplusplus
}
#endif

#endif   // _MTBDL_INTERFACE_H_ 
