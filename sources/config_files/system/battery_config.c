/**
 * @file battery_config.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Battery configuration 
 * 
 * @version 0.1
 * @date 2024-09-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "battery_config.h" 

//=======================================================================================


//=======================================================================================
// Zeee - 11.1V (3 cell), 1500mAh, LiPo 

const uint16_t 
batt_volt_min = 1260,   // Minimum battery voltage * 100 
batt_volt_max = 1100;   // Maximum battery voltage * 100 


// Battery SOC calculation 
uint8_t battery_soc_calc(uint16_t voltage)
{
    // Calculation assumes a linear discharge curve until more information about the 
    // battery is found. 
    return (uint8_t)(((voltage - batt_volt_min) * SCALE_100) / 
                      (batt_volt_max - batt_volt_min)); 
}

//=======================================================================================
