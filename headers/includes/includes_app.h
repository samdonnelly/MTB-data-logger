/**
 * @file includes_app.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Includes file for the application code 
 * 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _INCLUDES_APP_H_
#define _INCLUDES_APP_H_

//=======================================================================================
// Includes 

#include "stm32f4xx_it.h"

// Modules 
#include "user_interface.h"
#include "data_logging.h"
#include "system_parameters.h"
#include "hd44780u_controller.h"
#include "sd_controller.h"
#include "m8q_controller.h"
#include "mpu6050_controller.h"

// Config files 
#include "battery_config.h"
#include "string_config.h"
#include "hd44780u_config.h"
#include "m8q_config.h"
#include "ws2812_config.h"

//=======================================================================================

#endif  // _INCLUDES_APP_H_
