/**
 * @file user_init.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB DL main application code initialization header 
 * 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _MTBDL_INIT_H_
#define _MTBDL_INIT_H_

//=======================================================================================
// Includes 

#include "includes_app.h"
#include "includes_drivers.h"

//=======================================================================================


//=======================================================================================
// Macros 

// MPU-6050 config 
#define MPU6050_STBY_MASK 0x00           // Axis standby status mask 
#define MPU6050_SMPLRT_DIVIDER 0         // Sample Rate Divider 
#define MPU6050_RATE 250000              // Time between reading new data (us) 

//=======================================================================================


//=======================================================================================
// Function Prototypes

/**
 * @brief Function that gets called once. Initialization function calls go here.
 */
void mtbdl_init(void);

//=======================================================================================

#endif  // _MTBDL_INIT_H_
