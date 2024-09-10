/**
 * @file mtbdl_main.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief MTB data logger main application 
 * 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

//=======================================================================================
// Includes 

#include "mtbdl.h" 

//=======================================================================================


//=======================================================================================
// Macros 

// Timing info 
#define MTBDL_LCD_SLEEP 10000000         // (us) Inactive time before screen backlight off 
#define MTBDL_LCD_LP_SLEEP 3000000       // (us) Low power state message display time 
#define MTBDL_STATE_CHECK_SLOW 5000000   // (us) Long period time for non-blocking timer 

//=======================================================================================


//=======================================================================================
// Function prototypes 

/**
 * @brief System state machine function pointer 
 */
typedef void (*mtbdl_func_ptr_t)(mtbdl_trackers_t *mtbdl); 



// System status checks 
void system_status_checks(void); 



/**
 * @brief Initialization state 
 * 
 * @details First state to run on system startup and after the main controller resets. 
 *          Used to set up or reset any devices or data. Defaults to the idle state when 
 *          done. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_init_state(mtbdl_trackers_t *mtbdl); 

// Init state helper functions 
void mtbdl_init_state_entry(void); 
void mtbdl_init_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_init_state_exit(void); 


/**
 * @brief Idle state 
 * 
 * @details Display general state information and system options on the screen and wait 
 *          for the user to choose an option using the buttons. Update the GPS connection 
 *          status on the screen and with the LEDs. 
 *          
 *          Enter the run prep, data selection or calibration prep state depending on the 
 *          button pressed. This is the default state for the system meaning it's where 
 *          states revert to when features of the system were finished. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_idle_state(mtbdl_trackers_t *mtbdl); 

// Idle state helper functions 
void mtbdl_idle_state_entry(void); 
void mtbdl_idle_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_idle_state_exit(void); 


/**
 * @brief Run prep state 
 * 
 * @details Checks if there's room for a new log file to be created. If the number of log 
 *          files it at its capacity then the state is aborted. If there is room then 
 *          the run prep message is displayed and the state waits for the user to choose 
 *          between proceeding to log data or returning to the idle state. The GPS 
 *          connection status will be indicated on the screen and through the LEDs but 
 *          will not prevent the user from starting a data log. The run prep state LED 
 *          will also flash. 
 *          
 *          This state can only be entered from the idle state. From here, the state can 
 *          enter the run countdown, post run or idle state. Post run state is entered if 
 *          there was an issue creating a new log file. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_run_prep_state(mtbdl_trackers_t *mtbdl); 

// Run prep state helper functions 
void mtbdl_run_prep_state_entry(void); 
void mtbdl_run_prep_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_run_prep_state_exit(void); 


/**
 * @brief Run countdown state 
 * 
 * @details Waits a short period of time before proceeding to the run state to log data. 
 *          This state is meant to provide the user with a chance to start riding before 
 *          data logging begins. The screen and LEDs will indicate that data logging 
 *          is about to begin. 
 *          
 *          Entered from the run prep state only. Exits to the run state only. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_run_countdown_state(mtbdl_trackers_t *mtbdl); 

// Run countdown state helper functions 
void mtbdl_run_countdown_state_entry(void); 
void mtbdl_run_countdown_state_exit(void); 


/**
 * @brief Run state 
 * 
 * @details Continuously logs data from the bike and system including the suspension 
 *          potentiometers, GPS, IMU, wheel speed hall effect sensor and user trail 
 *          markers. The buttons give the user to option to stop a data log or set a 
 *          trail marker which gets recorded in the log file. Data is logged at an 
 *          interval triggered by an interrupt and there is a scheduler that controls 
 *          what data gets logged at each interval. All the data gets saved to the 
 *          file created before entering this state. The screen is shut off during this 
 *          state as it's not needed. The data logging LED will flash to indicate that 
 *          data logging is progress. 
 *          
 *          Entered from the run prep state only. Exits to the post run state if the 
 *          user stops the data log with a button push. Can also exit to the fault state 
 *          if a fault occurs in the system. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_run_state(mtbdl_trackers_t *mtbdl); 

// Run state helper functions 
void mtbdl_run_state_entry(void); 
void mtbdl_run_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_run_state_exit(void); 


/**
 * @brief Post run state 
 * 
 * @details Stops logging data then saves and closes the log file. The post run state 
 *          message is displayed to the screen and that state waits a short period of 
 *          time before exiting. LEDs will indicate the end of data logging. 
 *          
 *          Entered from the run state state after a data log or from the run prep state 
 *          if data log file creation was unsuccessful. Exits to the idle state when 
 *          done. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_postrun_state(mtbdl_trackers_t *mtbdl); 

// Post run state helper functions 
void mtbdl_postrun_state_entry(void); 
void mtbdl_postrun_state_exit(void); 


/**
 * @brief Data transfer selection state 
 * 
 * @details Displays the data selection state message which gives the user the option 
 *          between sending or receiving data from the system which the user can choose 
 *          with the buttons. Sending data involves offloading data logs to an external 
 *          device and receiving data involves taking input from an external device to 
 *          update system settings. Alternatively, the user can cancel and go back to the 
 *          idle state with a different button press. 
 *          
 *          Entered from the idle state only. Exits to the idle state or the device 
 *          search state depending on the user button input. If the user chooses to send 
 *          data and there are no log files to send then the system will exit to the TX 
 *          prep state which will end up moving the system back to the idle state. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_data_select_state(mtbdl_trackers_t *mtbdl); 

// Data transfer selection state helper functions 
void mtbdl_data_select_state_entry(void); 
void mtbdl_data_select_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_data_select_state_exit(void); 


/**
 * @brief Search for Bluetooth connection state 
 * 
 * @details Turns the Bluetooth module on and waits for an external device to connect to 
 *          it. A connection is needed in order to exchange data logs and system 
 *          settings. The Bluetooth LED will toggle quickly to indicate a searching 
 *          state. The user can choose to cancel the search with a button press. 
 *          
 *          Enters from the data selection state only. Exits to either the idle state 
 *          if the user cancels the search, or pre TX or pre RX states if a connection 
 *          is made. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_dev_search_state(mtbdl_trackers_t *mtbdl); 

// Device search state helper functions 
void mtbdl_dev_search_state_entry(void); 
void mtbdl_dev_search_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_dev_search_state_exit(void); 


/**
 * @brief Pre RX state 
 * 
 * @details Displays the pre RX state message and waits for the user to start the RX 
 *          state or abort to operation. The Bluetooth LED will blink slowly to indicate 
 *          a connection but the system will continuously monitor the connection and 
 *          abort the operation if a connection is lost. 
 *          
 *          Enters from the device search state only once a Bluetooth connection is made 
 *          to an external device. Exits to the idle state if aborted, the RX state if 
 *          the user chooses to proceed, or the post RX state if connection is lost. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_prerx_state(mtbdl_trackers_t *mtbdl); 

// Pre-RX state helper functions 
void mtbdl_prerx_state_entry(void); 
void mtbdl_prerx_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_prerx_state_exit(void); 


/**
 * @brief RX state 
 * 
 * @details Continuously poles the Bluetooth device and processes any new data received. 
 *          This data is used to update system settings/information and any data that is 
 *          not of a valid format will be discarded. If Bluetooth connection is lost 
 *          then the operation will be aborted. The RX state message is displayed to the 
 *          screen which will give the user the option to stop the transaction with a 
 *          button press. 
 *          
 *          Entered from pre RX state only. Exits to the post RX state when done or if 
 *          a connection is lost. Can exit to the fault state if the system detects a 
 *          fault. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_rx_state(mtbdl_trackers_t *mtbdl); 

// RX state helper functions 
void mtbdl_rx_state_entry(void); 
void mtbdl_rx_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_rx_state_exit(void); 


/**
 * @brief Post RX state 
 * 
 * @details Saves the new data received during the RX state to the parameter files then 
 *          waits a short period of time before returning to the idle state. Once done 
 *          the Bluetooth will be shut off. A post RX state message will be displayed 
 *          and the Bluetooth LED will indicate that the transaction is over. 
 *          
 *          Entered from the RX state after a transaction or from either the RX or pre 
 *          RX state if Bluetooth connection is lost. Exits to the idle state only. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_postrx_state(mtbdl_trackers_t *mtbdl); 

// Post-RX state helper functions 
void mtbdl_postrx_state_entry(void); 
void mtbdl_postrx_state_exit(void); 


/**
 * @brief Pre TX state 
 * 
 * @details Prepares a log file for sending to an external device via Bluetooth, and if 
 *          successful then displays the pre TX message and waits for the user to start 
 *          the TX state or exit to idle with a button press. If no log file exists then 
 *          abort the state. Continuously checks the Bluetooth connection status and 
 *          aborts the operation if connection is lost. 
 *          
 *          Enters from the device search state once a Bluetooth connection is made, the 
 *          data selection state if no log file exists for sending, or from the post TX 
 *          state if the user chooses to send another log file. Exits to the idle state 
 *          if the user cancels, the TX state if the user proceeds with a transfer, or 
 *          to the post TX state if connection is lost. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_pretx_state(mtbdl_trackers_t *mtbdl); 

// Pre-TX state helper functions 
void mtbdl_pretx_state_entry(void); 
void mtbdl_pretx_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_pretx_state_exit(void); 


/**
 * @brief TX state 
 * 
 * @details Sends a data log file to a connected external device via Bluetooth until the 
 *          whole file has been sent. The user has the option to cancel the transaction 
 *          before it's finished with a button press. If Bluetooth connection is lost 
 *          before the end of the transaction then the operation will be aborted. 
 *          
 *          Enters from the pre TX state only. Exits to the post TX state when the 
 *          transaction is finished or the operation is aborted. It can exit to the 
 *          fault state if a fauult occurs in the system. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_tx_state(mtbdl_trackers_t *mtbdl); 

// TX state helper functions 
void mtbdl_tx_state_entry(void); 
void mtbdl_tx_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_tx_state_exit(void); 


/**
 * @brief Post TX state 
 * 
 * @details If the transaction is successful then the log file is closed and deleted from 
 *          the system. Waits for a period of time before exiting. 
 *          
 *          Enters from the TX state after a transaction or from the TX and pre TX states 
 *          if the operation is aborted. Defaults back to the pre TX state if there are 
 *          more log files available to send and there is still a connection. Otherwise 
 *          return to the idle state. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_posttx_state(mtbdl_trackers_t *mtbdl); 

// Post-TX state helper functions 
void mtbdl_posttx_state_entry(void); 
void mtbdl_posttx_state_exit(void); 


/**
 * @brief Pre calibration state 
 * 
 * @details Displays the pre calibration state message to prompt the user to prepare the 
 *          system for calibration. The user can choose to proceed to calibration or 
 *          return to idle with a button push. 
 *          
 *          Enters from the idle state only. Exits to the idle state if canceled or to 
 *          the calibration state if proceeding. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_precalibrate_state(mtbdl_trackers_t *mtbdl); 

// Pre-calibration state helper functions 
void mtbdl_precalibrate_state_entry(void); 
void mtbdl_precalibrate_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_precalibrate_state_exit(void); 


/**
 * @brief Calibrate state 
 * 
 * @details Records potentiometer and IMU data continously for a short period of time 
 *          which gets used to determine the resting value of each sensor. The system 
 *          should be held still during this time. The screen and LEDs will indicate 
 *          to the user that calibration is taking place. Once finished, the recorded 
 *          values will be averaged to get the new calibration values for these 
 *          sensors. These values are is used to correct for sensor errors and it 
 *          gets reflected in the data log values. 
 *          
 *          Entered from the pre calibration state only. Exits to the post calibration 
 *          state only. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_calibrate_state(mtbdl_trackers_t *mtbdl); 

// Calibration state helper functions 
void mtbdl_calibrate_state_entry(void); 
void mtbdl_lowpwr_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_calibrate_state_exit(void); 


/**
 * @brief Post calibration state 
 * 
 * @details Saves the new calibration values to the parameter files and displays the 
 *          post calibration state message to indicate that calibration is complete. 
 *          
 *          Entered from the calibration state only. Exits to the idle state only. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_postcalibrate_state(mtbdl_trackers_t *mtbdl); 

// Post-calibration state helper functions 
void mtbdl_postcalibrate_state_entry(void); 
void mtbdl_postcalibrate_state_exit(void); 


/**
 * @brief Low power state 
 * 
 * @details This state occurs in response to the battery SOC dropping too low. This state 
 *          puts all devices into low power mode to save power and waits for the battery 
 *          SOC to rise above a threshold. Although the state looks for the SOC to change, 
 *          once in this state the user should power down the system. There is a user 
 *          button available to temporarily light the screen to show that the system is 
 *          in low power mode. A low power LED will also flash. 
 *          
 *          Enters whenever the battery SOC is too low. Exits only when the SOC is high 
 *          enough. It is not recommended to charge the battery while the system is 
 *          running. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_lowpwr_state(mtbdl_trackers_t *mtbdl); 

// Low power state helper functions 
void mtbdl_lowpwr_state_entry(void); 
void mtbdl_lowpwr_state_exit(void); 


/**
 * @brief Fault state 
 * 
 * @details Occurs when the system sees a fault. Stops any existing operation, displays 
 *          a fault message to the screen and waits for the user to trigger a system 
 *          reset. 
 *          
 *          Entered from any continuous state when there is a fault. Exits to the reset 
 *          state when the user triggers a reset with a button press. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_fault_state(mtbdl_trackers_t *mtbdl); 

// Fault state helper functions 
void mtbdl_fault_state_entry(void); 
void mtbdl_fault_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_fault_state_exit(void); 


/**
 * @brief Reset state 
 * 
 * @details Resets any devices or data as needed during a system reset. Immediately goes 
 *          to the idle state when done. 
 *          
 *          Entered from the fault state only. Exits to the idle state only. 
 * 
 * @param mtbdl : main controller tracking info 
 */
void mtbdl_reset_state(mtbdl_trackers_t *mtbdl); 

// Reset state helper functions 
void mtbdl_reset_state_entry(void); 
void mtbdl_reset_state_exit(void); 


/**
 * @brief Non-blocking delay 
 * 
 * @param mtbdl : main controller tracking info 
 * @param delay_time : amount of time to delay (non-blocking) 
 * @return uint8_t : true when 'delay_time' has elapsed, false otherwise 
 */
uint8_t mtbdl_nonblocking_delay(
    mtbdl_trackers_t *mtbdl, 
    uint32_t delay_time); 

//=======================================================================================


//=======================================================================================
// Global variables 

// Instance of the system data trackers 
mtbdl_trackers_t mtbdl_trackers; 


// Function pointers to system controller states 
static mtbdl_func_ptr_t mtbdl_state_table[MTBDL_NUM_STATES] = 
{
    &mtbdl_init_state,                  // State 0  : Startup 
    &mtbdl_idle_state,                  // State 1  : Idle 
    &mtbdl_run_prep_state,              // State 2  : Run prep 
    &mtbdl_run_countdown_state,         // State 3  : Run countdown 
    &mtbdl_run_state,                   // State 4  : Run 
    &mtbdl_postrun_state,               // State 5  : Post run 
    &mtbdl_data_select_state,           // State 6  : Data transfer selection 
    &mtbdl_dev_search_state,            // State 7  : Search for Bluetooth connection 
    &mtbdl_prerx_state,                 // State 8  : Pre data receive 
    &mtbdl_rx_state,                    // State 9  : Data receive 
    &mtbdl_postrx_state,                // State 10 : Post data receive 
    &mtbdl_pretx_state,                 // State 11 : Pre data send 
    &mtbdl_tx_state,                    // State 12 : Data send 
    &mtbdl_posttx_state,                // State 13 : Post data send 
    &mtbdl_precalibrate_state,          // State 14 : Pre calibration 
    &mtbdl_calibrate_state,             // State 15 : Calibration 
    &mtbdl_postcalibrate_state,         // State 16 : Post calibration 
    &mtbdl_lowpwr_state,                // State 17 : Low power mode 
    &mtbdl_fault_state,                 // State 18 : Fault 
    &mtbdl_reset_state                  // State 19 : Reset 
}; 

//=======================================================================================


//=======================================================================================
// Main controller 

void mtbdl_app(void)
{
    mtbdl_states_t next_state = mtbdl_trackers.state; 

    // System status checks 
    system_status_checks(); 

    // UI update 
    mtbdl_trackers.btn_press = ui_status_update(); 

    //===================================================
    // System state machine 

    switch (next_state)
    {
        case MTBDL_INIT_STATE: 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            break; 

        case MTBDL_IDLE_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.run)
            {
                next_state = MTBDL_RUN_PREP_STATE; 
            }
            else if (mtbdl_trackers.data_select)
            {
                next_state = MTBDL_DATA_SELECT_STATE; 
            }
            else if (mtbdl_trackers.calibrate)
            {
                next_state = MTBDL_PRECALIBRATE_STATE;  
            }
            break; 

        case MTBDL_RUN_PREP_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_POSTRUN_STATE; 
            }
            else if (mtbdl_trackers.run)
            {
                next_state = MTBDL_RUN_COUNTDOWN_STATE; 
            }
            break; 

        case MTBDL_RUN_COUNTDOWN_STATE: 
            if (mtbdl_trackers.run)
            {
                next_state = MTBDL_RUN_STATE; 
            }

            break; 

        case MTBDL_RUN_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.run)
            {
                next_state = MTBDL_POSTRUN_STATE; 
            }
            break; 

        case MTBDL_POSTRUN_STATE: 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }

            break; 

        case MTBDL_DATA_SELECT_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.data_select)
            {
                next_state = MTBDL_DEV_SEARCH_STATE; 
            }
            else if (mtbdl_trackers.tx)
            {
                next_state = MTBDL_PRETX_STATE; 
            }
            break; 

        case MTBDL_DEV_SEARCH_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.data_select && mtbdl_trackers.rx)
            {
                next_state = MTBDL_PRERX_STATE; 
            }
            else if (mtbdl_trackers.data_select && mtbdl_trackers.tx)
            {
                next_state = MTBDL_PRETX_STATE; 
            }
            break; 

        case MTBDL_PRERX_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_POSTRX_STATE; 
            }
            else if (mtbdl_trackers.rx)
            {
                next_state = MTBDL_RX_STATE; 
            }
            break; 

        case MTBDL_RX_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.rx || mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_POSTRX_STATE; 
            }
            break; 

        case MTBDL_POSTRX_STATE: 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            break; 

        case MTBDL_PRETX_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_POSTTX_STATE; 
            }
            else if (mtbdl_trackers.tx)
            {
                next_state = MTBDL_TX_STATE; 
            }
            break; 

        case MTBDL_TX_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.tx || mtbdl_trackers.noncrit_fault)
            {
                next_state = MTBDL_POSTTX_STATE; 
            }
            break; 

        case MTBDL_POSTTX_STATE: 
            if (mtbdl_trackers.tx)
            {
                next_state = MTBDL_PRETX_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            break; 

        case MTBDL_PRECALIBRATE_STATE: 
            if (mtbdl_trackers.fault_code)
            {
                next_state = MTBDL_FAULT_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.calibrate)
            {
                next_state = MTBDL_CALIBRATE_STATE;  
            }
            break; 
        
        case MTBDL_CALIBRATE_STATE: 
            if (mtbdl_trackers.calibrate)
            {
                next_state = MTBDL_POSTCALIBRATE_STATE;  
            }
            break; 

        case MTBDL_POSTCALIBRATE_STATE: 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            break; 

        case MTBDL_LOWPWR_STATE: 
            if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            break; 

        case MTBDL_FAULT_STATE: 
            if (mtbdl_trackers.reset)
            {
                next_state = MTBDL_RESET_STATE; 
            }
            break; 

        case MTBDL_RESET_STATE: 
            if (mtbdl_trackers.init)
            {
                next_state = MTBDL_INIT_STATE; 
            }
            break; 

        default: 
            next_state = MTBDL_INIT_STATE; 
            break; 
    }

    //===================================================

    // Execute the state function then update the record 
    mtbdl_state_table[next_state](&mtbdl_trackers); 
    mtbdl_trackers.state = next_state; 

    // Call device controllers 
    hd44780u_controller(); 
    hw125_controller(); 
    mpu6050_controller(DEVICE_ONE); 
    m8q_controller(); 
}


// System status checks 
void system_status_checks(void)
{
    if (hd44780u_get_fault_code())
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_0); 
    }
    if (mpu6050_get_fault_code(DEVICE_ONE))
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_1); 
    }
    if (m8q_get_fault_code())
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_2); 
    }
    if (hw125_get_fault_code())
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_3); 
    }
    if (hc05_get_status())
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_4); 
    }
}

//=======================================================================================


//=======================================================================================
// Init state 

void mtbdl_init_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->init)
    {
        mtbdl->init = CLEAR_BIT; 
        mtbdl_init_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    // - Wait for the SD card to be mounted before accessing the file system. Once 
    //   mounted, set the SD card check flag and set up the file structure and system 
    //   info. 

    mtbdl_init_user_input_check(mtbdl); 

    if (hw125_get_state() == HW125_ACCESS_STATE)
    {
        hw125_set_check_flag(); 
        // mtbdl_file_sys_setup(); 
        param_file_sys_setup(); 
    }

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_CHECK_SLOW))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl_init_state_exit(); 
    }
}


// Init state entry 
void mtbdl_init_state_entry(void)
{
    // Display the startup message 
    hd44780u_set_msg(mtbdl_welcome_msg, MTBDL_MSG_LEN_1_LINE); 

    // Turn on the init state LED 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led2_3); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led4_1); 
}


// Init state user button input check 
void mtbdl_init_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 4 - Re-initialize the screen if it doesn't work 
        case UI_BTN_4: 
            hd44780u_set_reset_flag(); 
            mtbdl->delay_timer.time_start = SET_BIT; 
            mtbdl->init = SET_BIT; 
            break; 

        default: 
            break; 
    }
}


// Init state exit 
void mtbdl_init_state_exit(void)
{
    // Clear the screen startup message 
    hd44780u_set_clear_flag(); 

    // Turn off the init state LED 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Idle state 

void mtbdl_idle_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->idle)
    {
        mtbdl->idle = CLEAR_BIT; 
        mtbdl_idle_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    // - Update GPS status and feedback 

    mtbdl_idle_user_input_check(mtbdl); 
    ui_gps_led_status_update(); 

    // State exit 
    if (mtbdl->run || mtbdl->data_select || mtbdl->calibrate || mtbdl->fault_code)
    {
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl_idle_state_exit(); 
    }
}


// Idle state entry 
void mtbdl_idle_state_entry(void)
{
    // Display the idle state message 
    mtbdl_set_idle_msg(); 

    // Set the screen to power save mode 
    hd44780u_set_pwr_save_flag(); 
    hd44780u_set_sleep_time(MTBDL_LCD_SLEEP); 

    // Put the HC-05 into low power mode 
    hc05_off(); 

    // Put the MPU-6050 into low power mode 
    mpu6050_set_low_power(DEVICE_ONE); 

    // Set the colour and blink rate of the GPS LED 
    ui_led_colour_set(WS2812_LED_1, mtbdl_led1_1); 
    ui_led_duty_set(WS2812_LED_1, UI_LED_DUTY_SHORT); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led7_1); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led6_1); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led5_1); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led4_1); 
}


// Idle state user button input check 
void mtbdl_idle_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 1 - triggers the pre run state 
        case UI_BTN_1: 
            mtbdl->run = SET_BIT; 
            break; 

        // Button 2 - triggers the data transfer selection state 
        case UI_BTN_2: 
            mtbdl->data_select = SET_BIT; 
            break; 

        // Button 3 - triggers the alternate functions state 
        case UI_BTN_3: 
            mtbdl->calibrate = SET_BIT; 
            break; 

        // Button 4 - Turns the screen backlight on 
        case UI_BTN_4: 
            hd44780u_wake_up(); 
            break; 

        default: 
            break; 
    }
}


// Idle state exit 
void mtbdl_idle_state_exit(void)
{
    // Clear the idle state message and take the screen out of power save mode 
    hd44780u_set_clear_flag(); 
    hd44780u_clear_pwr_save_flag(); 

    // Turn off the GPS LED 
    ui_led_colour_change(WS2812_LED_1, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Run prep state 

void mtbdl_run_prep_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->run)
    {
        // Check the log file name 
        if (mtbdl_log_name_prep()) 
        {
            // New file name created - display the run prep state message 
            mtbdl_set_run_prep_msg(); 
            mtbdl->run = CLEAR_BIT; 

            // Make sure the M8Q is out of low power mode 
            m8q_clear_low_pwr_flag(); 
        }
        else 
        {
            // Too many log files saved - abort 
            mtbdl->noncrit_fault = SET_BIT; 
            mtbdl->msg = mtbdl_ncf_excess_files_msg; 
            mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
        }

        mtbdl_run_prep_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    // - Update the data logging LED 
    // - Update the GPS status and feedback 

    mtbdl_run_prep_user_input_check(mtbdl); 
    ui_led_state_update(WS2812_LED_0); 
    ui_gps_led_status_update(); 

    // State exit 
    if (mtbdl->run || mtbdl->idle || mtbdl->fault_code)
    {
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl_run_prep_state_exit(); 
    }
}


// Pre run (run prep) state entry 
void mtbdl_run_prep_state_entry(void)
{
    // Set the colour and blink rate of the data logging and GPS LEDs 
    ui_led_colour_set(WS2812_LED_0, mtbdl_led0_1); 
    ui_led_colour_set(WS2812_LED_1, mtbdl_led1_1); 
    ui_led_duty_set(WS2812_LED_0, UI_LED_DUTY_LONG); 
    ui_led_duty_set(WS2812_LED_1, UI_LED_DUTY_SHORT); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led7_1); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led6_1); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Pre-run state user button input check 
void mtbdl_run_prep_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 1 - triggers the run state 
        case UI_BTN_1: 
            mtbdl->run = SET_BIT; 
            mtbdl_log_file_prep(); 
            break; 

        // Button 2 - cancels the run state --> triggers idle state 
        case UI_BTN_2: 
            mtbdl->idle = SET_BIT; 
            break; 

        default: 
            break; 
    }
}


// Pre run (run prep) state exit 
void mtbdl_run_prep_state_exit(void)
{
    // Clear the run prep state message 
    hd44780u_set_clear_flag(); 

    // Make sure the data logging and GPS LEDs are off 
    ui_led_colour_change(WS2812_LED_0, mtbdl_led_clear); 
    ui_led_colour_change(WS2812_LED_1, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Run countdown state 

void mtbdl_run_countdown_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->run)
    {
        mtbdl->run = CLEAR_BIT; 
        mtbdl_run_countdown_state_entry(); 
    }

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_CHECK_SLOW))
    {
        mtbdl->run = SET_BIT; 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl_run_countdown_state_exit(); 
    }
}


// Run countdown state entry 
void mtbdl_run_countdown_state_entry(void)
{
    // Display the run countdown state message 
    hd44780u_set_msg(mtbdl_run_countdown_msg, MTBDL_MSG_LEN_1_LINE); 

    // SD card will be written to constantly so no need for the check state 
    hw125_clear_check_flag(); 

    // Take the MPU-6050 out of low power mode 
    mpu6050_clear_low_power(DEVICE_ONE); 

    // Turn on the data logging LED 
    ui_led_colour_change(WS2812_LED_0, mtbdl_led0_1); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Run countdown state exit 
void mtbdl_run_countdown_state_exit(void)
{
    // Put the screen in low power mode 
    hd44780u_set_low_pwr_flag(); 

    // Turn off the data logging LED 
    ui_led_colour_change(WS2812_LED_0, mtbdl_led_clear); 

    // Prep the logging data 
    mtbdl_log_data_prep(); 
}

//=======================================================================================


//=======================================================================================
// Run state 

void mtbdl_run_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->run)
    {
        mtbdl->run = CLEAR_BIT; 
        mtbdl_run_state_entry(); 
    }

    // State operations: 
    // - Check for the user button input 
    // - Log system data 
    // - Update the data logging LED 
    // - Update the GPS status and feedback 

    mtbdl_run_user_input_check(mtbdl); 
    mtbdl_logging(); 
    ui_led_state_update(WS2812_LED_0); 
    ui_gps_led_status_update(); 

    // State exit 
    if (mtbdl->run || mtbdl->fault_code)
    {
        mtbdl->msg = mtbdl_postrun_msg; 
        mtbdl->msg_len = MTBDL_MSG_LEN_2_LINE; 
        mtbdl_run_state_exit(); 
    }
}


// Run state entry 
void mtbdl_run_state_entry(void)
{
    // Set the colour and blink rate of the data logging and GPS LEDs 
    ui_led_colour_set(WS2812_LED_0, mtbdl_led0_1); 
    ui_led_colour_set(WS2812_LED_1, mtbdl_led1_1); 
    ui_led_duty_set(WS2812_LED_0, UI_LED_DUTY_SHORT); 
    ui_led_duty_set(WS2812_LED_1, UI_LED_DUTY_SHORT); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led7_1); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led6_1); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Run state user button input check 
void mtbdl_run_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 1 - stops the run state --> triggers the post run state 
        case UI_BTN_1: 
            mtbdl->run = SET_BIT; 
            break; 

        // Button 2 - Sets a marker 
        case UI_BTN_2: 
            mtbdl_set_trailmark(); 
            break; 

        default: 
            break; 
    }
}


// Run state exit 
void mtbdl_run_state_exit(void)
{
    // Take the screen out of low power mode 
    hd44780u_clear_low_pwr_flag(); 

    // Turn the data logging and GPS LEDs off 
    ui_led_colour_change(WS2812_LED_0, mtbdl_led_clear); 
    ui_led_colour_change(WS2812_LED_1, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Post run state 

void mtbdl_postrun_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->run)
    {
        // Close the open data log file if there was no non-critical faults 
        if (!mtbdl->noncrit_fault)
        {
            mtbdl_log_end(); 
        }

        hd44780u_set_msg(mtbdl->msg, mtbdl->msg_len); 
        mtbdl->noncrit_fault = CLEAR_BIT; 
        mtbdl->run = CLEAR_BIT; 

        mtbdl_postrun_state_entry(); 
    }
    
    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_CHECK_SLOW))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl_postrun_state_exit(); 
    }
}


// Post run state entry 
void mtbdl_postrun_state_entry(void)
{
    // Turn on the data logging LED 
    ui_led_colour_change(WS2812_LED_0, mtbdl_led0_1); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Post run state exit 
void mtbdl_postrun_state_exit(void)
{
    // Clear the post run state message 
    hd44780u_set_clear_flag(); 

    // Set the SD card controller check flag 
    hw125_set_check_flag(); 

    // Turn off the data logging and GPS LEDs 
    ui_led_colour_change(WS2812_LED_0, mtbdl_led_clear); 
    ui_led_colour_change(WS2812_LED_1, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Data transfer selection state 

void mtbdl_data_select_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->data_select)
    {
        mtbdl->data_select = CLEAR_BIT; 
        mtbdl_data_select_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    mtbdl_data_select_user_input_check(mtbdl); 

    // State exit 
    if (mtbdl->data_select || mtbdl->tx || mtbdl->idle || mtbdl->fault_code)
    {
        mtbdl_data_select_state_exit(); 
    }
}


// Data selection state entry 
void mtbdl_data_select_state_entry(void)
{
    // Display the data select state message 
    hd44780u_set_msg(mtbdl_data_select_msg, MTBDL_MSG_LEN_3_LINE); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led7_1); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led6_1); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led5_1); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Data selection state user button input check 
void mtbdl_data_select_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 1 - triggers the pre receive (RX) state 
        case UI_BTN_1: 
            mtbdl->rx = SET_BIT; 
            mtbdl->data_select = SET_BIT; 
            break; 

        // Button 2 - triggers the pre send (TX) state 
        case UI_BTN_2: 
            // Before going to the device search state the code first checks if there are any log 
            // files saved in the system. If there are none then the data select bit is not set 
            // which will ultimately abort the TX state and tell the user there are no files 
            // available for sending. 
            if (param_get_log_index())
            {
                // Files exist - go to the device search state 
                mtbdl->data_select = SET_BIT; 
            }
            mtbdl->tx = SET_BIT; 
            break; 

        // Button 3 - triggers the idle state - cancels data selection 
        case UI_BTN_3: 
            mtbdl->idle = SET_BIT; 
            break; 

        default: 
            break; 
    }
}


// Data selection state exit 
void mtbdl_data_select_state_exit(void)
{
    // Clear the data select state message 
    hd44780u_set_clear_flag(); 
}

//=======================================================================================


//=======================================================================================
// Device search state (search for a Bluetooth connection) 

void mtbdl_dev_search_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->data_select)
    {
        mtbdl->data_select = CLEAR_BIT; 
        mtbdl_dev_search_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    // - Update the Bluetooth device search LED 
    // - Check for a Bluetooth connection before moving to the next state 

    mtbdl_dev_search_user_input_check(mtbdl); 
    ui_led_state_update(WS2812_LED_2); 

    if (hc05_status())
    {
        mtbdl->data_select = SET_BIT; 
    }

    // State exit 
    if (mtbdl->idle || mtbdl->data_select || mtbdl->fault_code)
    {
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
        mtbdl_dev_search_state_exit(); 
    }
}


// Device search state entry 
void mtbdl_dev_search_state_entry(void)
{
    // Display the device connection search state message 
    hd44780u_set_msg(mtbdl_dev_search_msg, MTBDL_MSG_LEN_2_LINE); 

    // Take the HC-05 out of low power mode 
    hc05_on(); 

    // Set the Bluetooth LED colour and blink rate 
    ui_led_colour_set(WS2812_LED_2, mtbdl_led2_1); 
    ui_led_duty_set(WS2812_LED_2, UI_LED_DUTY_LONG); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led7_1); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Device search state user button input check 
void mtbdl_dev_search_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 1 - triggers the idle state - cancels device search 
        case UI_BTN_1: 
            mtbdl->idle = SET_BIT; 
            break; 

        default: 
            break; 
    }
}


// Device search state exit 
void mtbdl_dev_search_state_exit(void)
{
    // Clear the device connection search state message 
    hd44780u_set_clear_flag(); 

    // Make sure the Bluetooth LED is off 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Pre data receive (RX) state 

void mtbdl_prerx_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->rx)
    {
        mtbdl->rx = CLEAR_BIT; 
        mtbdl->data_select = CLEAR_BIT; 
        mtbdl_prerx_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    // - Update the Bluetooth connection LED 
    // - Check for a loss of Bluetooth connection - if true then abort the operation 

    mtbdl_prerx_user_input_check(mtbdl); 
    ui_led_state_update(WS2812_LED_2); 

    if (!hc05_status())
    {
        mtbdl->rx = SET_BIT; 
        mtbdl->noncrit_fault = SET_BIT; 
        mtbdl->msg = mtbdl_ncf_bt_con_lost; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
    }

    // State exit 
    if (mtbdl->rx || mtbdl->idle || mtbdl->fault_code)
    {
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
        mtbdl_prerx_state_exit(); 
    }
}


// Pre RX state entry 
void mtbdl_prerx_state_entry(void)
{
    // Display the pre rx state message 
    hd44780u_set_msg(mtbdl_prerx_msg, MTBDL_MSG_LEN_3_LINE); 

    // Set the Bluetooth LED colour and blink rate 
    ui_led_colour_set(WS2812_LED_2, mtbdl_led2_1); 
    ui_led_duty_set(WS2812_LED_2, UI_LED_DUTY_SHORT); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led7_1); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led6_1); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Pre-RX state user button input check 
void mtbdl_prerx_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 1 - triggers the rx state 
        case UI_BTN_1: 
            mtbdl->rx = SET_BIT; 
            break; 

        // Button 2 - cancels the rx state --> triggers idle state 
        case UI_BTN_2: 
            mtbdl->idle = SET_BIT; 
            break; 

        default: 
            break; 
    }
}


// Pre RX state exit 
void mtbdl_prerx_state_exit(void)
{
    // Clear the pre rx state message 
    hd44780u_set_clear_flag(); 

    // Make sure the Bluetooth LED is off 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Data receive (RX) state 

void mtbdl_rx_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->rx)
    {
        mtbdl->rx = CLEAR_BIT; 
        mtbdl_rx_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    // - Check for a loss of Bluetooth connection - if true then abort the transfer and 
    //   update the non-critical fault message 
    // - Read device data 

    mtbdl_rx_user_input_check(mtbdl); 

    if (!hc05_status())
    {
        mtbdl->rx = SET_BIT; 
        mtbdl->noncrit_fault = SET_BIT; 
        mtbdl->msg = mtbdl_ncf_bt_con_lost; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
    }

    ui_rx(); 

    // State exit 
    if (mtbdl->rx || mtbdl->fault_code)
    {
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
        mtbdl_rx_state_exit(); 
    }
}


// RX state entry 
void mtbdl_rx_state_entry(void)
{
    // Display the rx state message 
    hd44780u_set_msg(mtbdl_rx_msg, MTBDL_MSG_LEN_2_LINE); 

    // Begin the RX user interface 
    ui_rx_prep(); 

    // Turn the Bluetooth LED on 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led2_1); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led7_1); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// RX state user button input check 
void mtbdl_rx_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 1 - stops the rx state --> triggers the post rx state 
        case UI_BTN_1: 
            mtbdl->rx = SET_BIT; 
            mtbdl->msg = mtbdl_postrx_msg; 
            mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
            break; 

        default: 
            break; 
    }
}


// RX state exit 
void mtbdl_rx_state_exit(void)
{
    // Clear the rx state message 
    hd44780u_set_clear_flag(); 

    // Turn the Bluetooth LED off 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Post data receive (RX) state 

void mtbdl_postrx_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->rx)
    {
        // Update the screen message 
        hd44780u_set_msg(mtbdl->msg, mtbdl->msg_len); 

        mtbdl->noncrit_fault = CLEAR_BIT; 
        mtbdl->rx = CLEAR_BIT; 
        mtbdl_postrx_state_entry(); 
    }

    // State operations: 
    // - Update the Bluetooth transfer LED 
    ui_led_state_update(WS2812_LED_2); 

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_CHECK_SLOW))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl_postrx_state_exit(); 
    }
}


// Post RX state entry 
void mtbdl_postrx_state_entry(void)
{
    // Save the parameters to file and update tracking info 
    // mtbdl_write_bike_params(HW125_MODE_OEW); 
    param_write_bike_params(HW125_MODE_OEW); 

    // Set the Bluetooth LED colour and blink rate 
    ui_led_colour_set(WS2812_LED_2, mtbdl_led2_1); 
    ui_led_duty_set(WS2812_LED_2, UI_LED_DUTY_LONG); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Post RX state exit 
void mtbdl_postrx_state_exit(void)
{
    // Clear the post rx state message 
    hd44780u_set_clear_flag(); 

    // Turn the Bluetooth LED off 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Pre data send (TX) state 

void mtbdl_pretx_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->tx)
    {
        // Prepare the log file to send 
        if (ui_tx_prep())
        {
            // File ready - display the pre tx state message 
            mtbdl_set_pretx_msg(); 
            mtbdl->tx = CLEAR_BIT; 
        }
        else 
        {
            // File does not exist - update the message to send and abort the state 
            mtbdl->noncrit_fault = SET_BIT; 
            mtbdl->msg = mtbdl_ncf_no_files_msg; 
            mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
        }

        mtbdl->data_select = CLEAR_BIT; 

        mtbdl_pretx_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    // - Check for a loss of Bluetooth connection or the lack of a data log file to send - 
    //   if true then abort the transfer 
    // - Update the Bluetooth LED 

    mtbdl_pretx_user_input_check(mtbdl); 

    if (!hc05_status() && !mtbdl->tx)
    {
        // This check uses the tx bit to reflect the existance of log files saved in the 
        // system. In the state entry, if there are no log files to send then the tx bit 
        // will not be set which then prevents this Bluetooth device connection check 
        // from overwriting the non-critical fault message. 

        mtbdl->tx = SET_BIT; 
        mtbdl->noncrit_fault = SET_BIT; 
        mtbdl->msg = mtbdl_ncf_bt_con_lost; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
    }

    ui_led_state_update(WS2812_LED_2); 

    // State exit 
    if (mtbdl->tx || mtbdl->idle || mtbdl->fault_code)
    {
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
        mtbdl_pretx_state_exit(); 
    }
}


// Pre TX state entry 
void mtbdl_pretx_state_entry(void)
{
    // Set the Bluetooth LED colour and blink rate 
    ui_led_colour_set(WS2812_LED_2, mtbdl_led2_1); 
    ui_led_duty_set(WS2812_LED_2, UI_LED_DUTY_SHORT); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led7_1); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led6_1); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Pre-TX state user button input check 
void mtbdl_pretx_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 1 - triggers the tx state 
        case UI_BTN_1: 
            mtbdl->tx = SET_BIT; 
            break; 

        // Button 2 - cancels the tx state --> triggers idle state 
        case UI_BTN_2: 
            mtbdl->idle = SET_BIT; 
            break; 

        default: 
            break; 
    }
}


// Pre TX state exit 
void mtbdl_pretx_state_exit(void)
{
    // Clear the pre rx state message 
    hd44780u_set_clear_flag(); 

    // Make sure the Bluetooth LED is off 
    ui_led_colour_set(WS2812_LED_2, mtbdl_led_clear); 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Data send (TX) state 

void mtbdl_tx_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->tx)
    {
        // Display the tx state message 
        hd44780u_set_msg(mtbdl_tx_msg, MTBDL_MSG_LEN_2_LINE); 
        mtbdl->msg = mtbdl_posttx_msg; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
        mtbdl->tx = CLEAR_BIT; 

        mtbdl_tx_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    // - Transfer data log contents and set the tx bit if the transfer finishes so the 
    //   state can exit. 
    // - Check for a loss of Bluetooth connection - if true then abort the transfer 

    mtbdl_tx_user_input_check(mtbdl); 

    if (ui_tx())
    {
        mtbdl->tx = SET_BIT; 
    }

    if (!hc05_status())
    {
        mtbdl->tx = SET_BIT; 
        mtbdl->noncrit_fault = SET_BIT; 
        mtbdl->msg = mtbdl_ncf_bt_con_lost; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
    }

    // State exit 
    if (mtbdl->tx || mtbdl->fault_code)
    {
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
        mtbdl_tx_state_exit(); 
    }
}


// TX state entry 
void mtbdl_tx_state_entry(void)
{
    // Turn the Bluetooth LED on 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led2_1); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led7_1); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// TX state user button input check 
void mtbdl_tx_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 1 - stops the tx state --> triggers the post tx state 
        case UI_BTN_1: 
            mtbdl->tx = SET_BIT; 
            break; 

        default: 
            break; 
    }
}


// TX state exit 
void mtbdl_tx_state_exit(void)
{
    // Clear the tx state message 
    hd44780u_set_clear_flag(); 

    // Turn the Bluetooth LED off 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Post data send (TX) state 

void mtbdl_posttx_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->tx)
    {
        // Set the post TX state message 
        hd44780u_set_msg(mtbdl->msg, mtbdl->msg_len); 

        mtbdl->tx = CLEAR_BIT; 
        mtbdl_posttx_state_entry(); 
    }

    // State operations: 
    // - Update the Bluetooth LED 
    ui_led_state_update(WS2812_LED_2); 

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_CHECK_SLOW))
    {
        mtbdl->delay_timer.time_start = SET_BIT; 

        // Go back to the pre-tx state if the connection was not lost 
        if (mtbdl->noncrit_fault)
        {
            mtbdl->idle = SET_BIT; 
        }
        else 
        {
            mtbdl->tx = SET_BIT; 
        }

        mtbdl->noncrit_fault = CLEAR_BIT; 
        mtbdl_posttx_state_exit(); 
    }
}


// Post TX state entry 
void mtbdl_posttx_state_entry(void)
{
    // End the transaction 
    ui_tx_end(); 

    // Set the Bluetooth LED colour and blink rate 
    ui_led_colour_set(WS2812_LED_2, mtbdl_led2_1); 
    ui_led_duty_set(WS2812_LED_2, UI_LED_DUTY_LONG); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Post TX state exit 
void mtbdl_posttx_state_exit(void)
{
    // Clear the post tx state message 
    hd44780u_set_clear_flag(); 

    // Turn the Bluetooth LED off 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Pre calibration state 

void mtbdl_precalibrate_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->calibrate)
    {
        mtbdl->calibrate = CLEAR_BIT; 
        mtbdl_precalibrate_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    // - Update the calibration LED 

    mtbdl_precalibrate_user_input_check(mtbdl); 
    ui_led_state_update(WS2812_LED_2); 

    // State exit 
    if (mtbdl->calibrate || mtbdl->idle || mtbdl->fault_code)
    {
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
        mtbdl_precalibrate_state_exit(); 
    }
}


// Pre-calibration state entry 
void mtbdl_precalibrate_state_entry(void)
{
    // Display the calibration state message 
    hd44780u_set_msg(mtbdl_precal_msg, MTBDL_MSG_LEN_4_LINE); 

    // Set the calibration LED colour and blink rate 
    ui_led_colour_set(WS2812_LED_2, mtbdl_led2_2); 
    ui_led_duty_set(WS2812_LED_2, UI_LED_DUTY_SHORT); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led7_1); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led6_1); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Pre-calibration state user button input check 
void mtbdl_precalibrate_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 1 - triggers the calibration state 
        case UI_BTN_1: 
            mtbdl->calibrate = SET_BIT; 
            mpu6050_clear_low_power(DEVICE_ONE);   // Take MPU-6050 out of low power mode 
            break; 

        // Button 2 - cancels the calibration state --> triggers idle state 
        case UI_BTN_2: 
            mtbdl->idle = SET_BIT; 
            break; 

        default: 
            break; 
    }
}


// Pre-calibration state exit 
void mtbdl_precalibrate_state_exit(void)
{
    // Turn the calibration LED off 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Calibration state 

void mtbdl_calibrate_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->calibrate)
    {
        mtbdl->calibrate = CLEAR_BIT; 
        mtbdl_calibrate_state_entry(); 
    }

    // State operations: 
    // - Sample data that can be used for calculating the calibration values 
    mtbdl_calibrate(); 

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_CHECK_SLOW))
    {
        mtbdl->calibrate = SET_BIT; 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl_calibrate_state_exit(); 
    }
}


// Calibration state entry 
void mtbdl_calibrate_state_entry(void)
{
    // Display the calibration state message 
    hd44780u_set_msg(mtbdl_cal_msg, MTBDL_MSG_LEN_1_LINE); 

    // Prepare to record data for calibration 
    mtbdl_cal_prep(); 

    // Turn the calibration LED on 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led2_2); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Calibration state exit 
void mtbdl_calibrate_state_exit(void)
{
    // Calculate the calibration values 
    mtbdl_cal_calc(); 

    // Clear the calibration state message 
    hd44780u_set_clear_flag(); 

    // Turn the calibration LED off 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Post calibration state 

void mtbdl_postcalibrate_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->calibrate)
    {
        mtbdl->calibrate = CLEAR_BIT; 
        mtbdl_postcalibrate_state_entry(); 
    }

    // State operations: 
    // - Update the calibration LED 
    ui_led_state_update(WS2812_LED_2); 

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_CHECK_SLOW))
    {
        mtbdl->idle = SET_BIT; 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl_postcalibrate_state_exit(); 
    }
}


// Post calibration state entry 
void mtbdl_postcalibrate_state_entry(void)
{
    // Display the post calibration message and record the calibration data
    hd44780u_set_msg(mtbdl_postcal_msg, MTBDL_MSG_LEN_1_LINE); 
    // mtbdl_write_sys_params(HW125_MODE_OEW); 
    param_write_sys_params(HW125_MODE_OEW); 

    // Set the calibration LED colour and blink rate 
    ui_led_colour_set(WS2812_LED_2, mtbdl_led2_2); 
    ui_led_duty_set(WS2812_LED_2, UI_LED_DUTY_LONG); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Post calibration state exit 
void mtbdl_postcalibrate_state_exit(void)
{
    // Clear the post calibration message 
    hd44780u_set_clear_flag(); 

    // Turn the calibration LED off 
    ui_led_colour_set(WS2812_LED_2, mtbdl_led_clear); 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Low power state 

void mtbdl_lowpwr_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->low_pwr)
    {
        mtbdl->low_pwr = CLEAR_BIT; 
        mtbdl_lowpwr_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    // - Update the low power LED 

    mtbdl_lowpwr_user_input_check(mtbdl); 
    ui_led_state_update(WS2812_LED_3); 

    // State exit 
    if (mtbdl->low_pwr)
    {
        mtbdl->low_pwr = CLEAR_BIT; 
        mtbdl->idle = SET_BIT; 
        mtbdl->delay_timer.time_start = SET_BIT; 
        mtbdl->led_state = CLEAR; 
        mtbdl_lowpwr_state_exit(); 
    }
}


// Low power state entry 
void mtbdl_lowpwr_state_entry(void)
{
    // Display the low power state message 
    hd44780u_set_msg(mtbdl_low_pwr_msg, MTBDL_MSG_LEN_2_LINE); 

    // Put the screen into power save mode 
    hd44780u_set_pwr_save_flag(); 
    hd44780u_set_sleep_time(MTBDL_LCD_LP_SLEEP); 

    // Put the GPS into low power mode 
    m8q_set_low_pwr_flag(); 

    // Make sure the MPU-6050 and HC-05 are in low power mode 
    mpu6050_set_low_power(DEVICE_ONE); 
    hc05_off(); 

    // Set the low power mode LED colour and blink rate 
    ui_led_colour_set(WS2812_LED_3, mtbdl_led3_1); 
    ui_led_duty_set(WS2812_LED_3, UI_LED_DUTY_SHORT); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led5_1); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led4_1); 
}


// Low power state user button input check 
void mtbdl_lowpwr_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 3 - triggers the idle state 
        // If SOC is above the minimum threshold then we can exit low power state. 
        // A button press is put here instead temporarily umtil SOC calibration has 
        // been created. 
        case UI_BTN_3: 
            mtbdl->low_pwr = SET_BIT; 
            break; 

        // Button 4 - Turns the screen backlight on 
        case UI_BTN_4: 
            hd44780u_wake_up(); 
            break; 

        default: 
            break; 
    }
}


// Low power state exit 
void mtbdl_lowpwr_state_exit(void)
{
    // Clear the idle state message and take devices out of low power mode 
    hd44780u_set_clear_flag(); 
    hd44780u_clear_pwr_save_flag(); 
    m8q_clear_low_pwr_flag(); 

    // Make sure the low power LED if off 
    ui_led_colour_change(WS2812_LED_3, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Fault state 

void mtbdl_fault_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (!mtbdl->fault)
    {
        mtbdl->fault = SET_BIT; 
        mtbdl_fault_state_entry(); 
    }

    // State operations: 
    // - Check for user button input 
    mtbdl_fault_user_input_check(mtbdl); 

    // State exit 
    if (mtbdl->reset)
    {
        mtbdl->fault_code = CLEAR; 
        mtbdl->fault = CLEAR_BIT; 
        mtbdl_fault_state_exit(); 
    }
}


// Fault state entry 
void mtbdl_fault_state_entry(void)
{
    // Display the fault state message and take screen out of power save mode 
    hd44780u_set_msg(mtbdl_fault_msg, MTBDL_MSG_LEN_2_LINE); 
    hd44780u_clear_pwr_save_flag(); 

    // Record the fault code in a log 

    // Stop any existing processes - assess each state case 

    // Turn the fault LED on 
    ui_led_colour_change(WS2812_LED_3, mtbdl_led3_1); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led4_1); 
}


// Fault state user button input check 
void mtbdl_fault_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 4 - triggers a system reset (reset state) 
        case UI_BTN_4: 
            mtbdl->reset = SET_BIT; 
            break; 

        default: 
            break; 
    }
}


// Fault state exit 
void mtbdl_fault_state_exit(void)
{
    // Clear the fault state message 
    hd44780u_set_clear_flag(); 

    // Turn the fault LED off 
    ui_led_colour_change(WS2812_LED_3, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Reset state 

void mtbdl_reset_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->reset)
    {
        mtbdl->reset = CLEAR_BIT; 
        mtbdl_reset_state_entry(); 
    }

    // State exit 
    mtbdl->init = SET_BIT; 
    mtbdl_reset_state_exit(); 
}


// Reset state entry 
void mtbdl_reset_state_entry(void)
{
    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Reset state exit 
void mtbdl_reset_state_exit(void)
{
    // Reset devices 
    hw125_set_reset_flag(); 
}

//=======================================================================================


//=======================================================================================
// Timing 

// Non-blocking delay 
uint8_t mtbdl_nonblocking_delay(
    mtbdl_trackers_t *mtbdl, 
    uint32_t delay_time)
{
    // Wait for a short period of time before leaving the init state 
    if (tim_compare(mtbdl->timer_nonblocking, 
                    mtbdl->delay_timer.clk_freq, 
                    delay_time, 
                    &mtbdl->delay_timer.time_cnt_total, 
                    &mtbdl->delay_timer.time_cnt, 
                    &mtbdl->delay_timer.time_start))
    {
        return TRUE; 
    }

    return FALSE; 
}

//=======================================================================================
