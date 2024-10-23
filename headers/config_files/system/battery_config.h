/**
 * @file battery_config.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Battery config interface 
 * 
 * @version 0.1
 * @date 2024-09-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _BATTERY_CONFIG_H_ 
#define _BATTERY_CONFIG_H_ 

//=======================================================================================
// Includes 

#include "tools.h" 

//=======================================================================================


//=======================================================================================
// Battery: Zeee - 11.1V (3 cell), 1500mAh, LiPo 

/**
 * @brief Battery SOC (state of charge) calculation 
 * 
 * @param voltage : Battery voltage 
 * @return uint8_t : Battery SOC 
 */
uint8_t battery_soc_calc(uint16_t voltage); 

//=======================================================================================

#endif   // _BATTERY_CONFIG_H_ 
