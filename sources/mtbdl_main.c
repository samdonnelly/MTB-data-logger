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
#define MTBDL_STATE_EXIT_TIMER 5000000   // (us) Standard state exit time count 
#define MTBDL_STATE_EXIT_WAIT 30000000   // (us) State exit wait timer count 

// Battery Voltage 
#define MTBDL_SOC_CUTOFF 15              // Low power enter cutoff battery SOC 
#define MTBDL_SOC_THRESHOLD 20           // Low power exit min threshold battery SOC 

//=======================================================================================


//=======================================================================================
// Function prototypes 

/**
 * @brief System state machine function pointer 
 */
typedef void (*mtbdl_func_ptr_t)(mtbdl_trackers_t *mtbdl); 


/**
 * @brief System status checks 
 * 
 * @details Checks for faults in the system. This includes issues with reading from 
 *          devices and low battery voltage. 
 */
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
void mtbdl_init_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_init_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_init_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_idle_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_idle_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_idle_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_run_prep_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_run_prep_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_run_prep_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_run_countdown_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_run_countdown_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_run_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_run_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_run_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_postrun_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_postrun_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_data_select_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_data_select_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_data_select_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_dev_search_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_dev_search_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_dev_search_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_prerx_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_prerx_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_prerx_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_rx_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_rx_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_rx_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_postrx_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_postrx_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_pretx_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_pretx_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_pretx_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_tx_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_tx_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_tx_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_posttx_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_posttx_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_precalibrate_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_precalibrate_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_precalibrate_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_calibrate_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_lowpwr_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_calibrate_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_postcalibrate_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_postcalibrate_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_lowpwr_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_lowpwr_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_fault_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_fault_user_input_check(mtbdl_trackers_t *mtbdl); 
void mtbdl_fault_state_exit(mtbdl_trackers_t *mtbdl); 


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
void mtbdl_reset_state_entry(mtbdl_trackers_t *mtbdl); 
void mtbdl_reset_state_exit(mtbdl_trackers_t *mtbdl); 


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
            else if (mtbdl_trackers.low_pwr)
            {
                next_state = MTBDL_LOWPWR_STATE; 
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
            if (mtbdl_trackers.noncrit_fault || 
                mtbdl_trackers.fault_code || 
                mtbdl_trackers.low_pwr)
            {
                next_state = MTBDL_POSTRUN_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
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
            if (mtbdl_trackers.run || 
                mtbdl_trackers.noncrit_fault || 
                mtbdl_trackers.fault_code || 
                mtbdl_trackers.low_pwr)
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
            if (mtbdl_trackers.idle || 
                mtbdl_trackers.fault_code || 
                mtbdl_trackers.low_pwr)
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
            if (mtbdl_trackers.idle || 
                mtbdl_trackers.fault_code || 
                mtbdl_trackers.low_pwr)
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
            if (mtbdl_trackers.noncrit_fault || 
                mtbdl_trackers.fault_code || 
                mtbdl_trackers.low_pwr)
            {
                next_state = MTBDL_POSTRX_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.rx)
            {
                next_state = MTBDL_RX_STATE; 
            }
            break; 

        case MTBDL_RX_STATE: 
            if (mtbdl_trackers.rx || 
                mtbdl_trackers.noncrit_fault || 
                mtbdl_trackers.fault_code || 
                mtbdl_trackers.low_pwr)
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
            if (mtbdl_trackers.noncrit_fault || 
                mtbdl_trackers.fault_code || 
                mtbdl_trackers.low_pwr)
            {
                next_state = MTBDL_POSTTX_STATE; 
            }
            else if (mtbdl_trackers.idle)
            {
                next_state = MTBDL_IDLE_STATE; 
            }
            else if (mtbdl_trackers.tx)
            {
                next_state = MTBDL_TX_STATE; 
            }
            break; 

        case MTBDL_TX_STATE: 
            if (mtbdl_trackers.tx || 
                mtbdl_trackers.noncrit_fault || 
                mtbdl_trackers.fault_code || 
                mtbdl_trackers.low_pwr)
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
            if (mtbdl_trackers.fault_code || 
                mtbdl_trackers.low_pwr)
            {
                next_state = MTBDL_POSTCALIBRATE_STATE; 
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
            if (mtbdl_trackers.low_pwr)
            {
                next_state = MTBDL_LOWPWR_STATE; 
            }
            else if (mtbdl_trackers.reset)
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

    // Update the user interface and check the system status. System status checks are 
    // done specifically after the state machine and before the execution of the state. 
    // This is done so that if a fault or low power event occurs, these flags can be set 
    // without changing state before the current state has a chance to run its exit code. 
    mtbdl_trackers.btn_press = ui_status_update(); 
    system_status_checks(); 

    // Execute the state function then update the record 
    mtbdl_state_table[next_state](&mtbdl_trackers); 
    mtbdl_trackers.state = next_state; 

    // Call device controllers 
    hd44780u_controller(); 
    fatfs_controller(); 
    mpu6050_controller(DEVICE_ONE); 
    m8q_controller(); 
}


// System status checks 
void system_status_checks(void)
{
    // Low power check 
    if ((mtbdl_trackers.state != MTBDL_LOWPWR_STATE) && (ui_get_soc() <= MTBDL_SOC_CUTOFF))
    {
        mtbdl_trackers.low_pwr = SET_BIT; 

        // Only the first line of the low power message is used until the low power state 
        // is entered. 
        mtbdl_trackers.msg = mtbdl_low_pwr_msg; 
        mtbdl_trackers.msg_len = MTBDL_MSG_LEN_1_LINE; 
    } 

    // Fault checks 
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
    if (fatfs_get_fault_code())
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_3); 
    }
    if (hc05_get_status())
    {
        mtbdl_trackers.fault_code |= (SET_BIT << SHIFT_4); 
    }

    // Update screen message if there is a fault 
    if (mtbdl_trackers.fault_code)
    {
        // Only the first line of the fault message is used until the fault state is 
        // entered. 
        mtbdl_trackers.msg = mtbdl_fault_msg; 
        mtbdl_trackers.msg_len = MTBDL_MSG_LEN_1_LINE; 
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
        mtbdl_init_state_entry(mtbdl); 
    }

    // State operations: 
    // - Check for user button input 
    // - Wait for the SD card to be mounted before accessing the file system. Once 
    //   mounted, set the SD card check flag and set up the file structure and system 
    //   info. 

    mtbdl_init_user_input_check(mtbdl); 

    if (fatfs_get_state() == FATFS_ACCESS_STATE)
    {
        fatfs_set_check_flag(); 
        param_file_sys_setup(); 
    }

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_EXIT_TIMER))
    {
        mtbdl_init_state_exit(mtbdl); 
    }
}


// Init state entry 
void mtbdl_init_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->init = CLEAR_BIT; 

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
void mtbdl_init_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->idle = SET_BIT; 
    mtbdl->delay_timer.time_start = SET_BIT; 
    
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
        mtbdl_idle_state_entry(mtbdl); 
    }

    // State operations: 
    // - Check for user button input 
    // - Update GPS status and feedback 
    // - Update screen message contents 

    mtbdl_idle_user_input_check(mtbdl); 
    ui_gps_led_status_update(); 
    ui_msg_update(UI_MSG_IDLE); 

    // State exit 
    if (mtbdl->run || 
        mtbdl->data_select || 
        mtbdl->calibrate || 
        mtbdl->fault_code || 
        mtbdl->low_pwr)
    {
        mtbdl_idle_state_exit(mtbdl); 
    }
}


// Idle state entry 
void mtbdl_idle_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->idle = CLEAR_BIT; 

    // Display the idle state message 
    ui_set_idle_msg(); 

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
void mtbdl_idle_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->delay_timer.time_start = SET_BIT; 

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
        mtbdl_run_prep_state_entry(mtbdl); 
    }

    // State operations: 
    // - Check for user button input 
    // - Update the data logging LED 
    // - Update the GPS status and feedback 
    // - Update screen message contents 

    mtbdl_run_prep_user_input_check(mtbdl); 
    ui_led_state_update(WS2812_LED_0); 
    ui_gps_led_status_update(); 
    ui_msg_update(UI_MSG_RUN_PREP); 

    // State exit 
    if (mtbdl->run || 
        mtbdl->idle || 
        mtbdl->noncrit_fault || 
        mtbdl->fault_code || 
        mtbdl->low_pwr)
    {
        mtbdl_run_prep_state_exit(mtbdl); 
    }
}


// Run prep state entry 
void mtbdl_run_prep_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->run = CLEAR_BIT; 

    // Check the log file name 
    if (log_data_name_prep()) 
    {
        // New file name created - display the run prep state message 
        ui_set_run_prep_msg(); 

        // Make sure the M8Q is out of low power mode 
        m8q_clear_low_pwr_flag(); 

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
    else 
    {
        // Too many log files saved - abort the state 
        mtbdl->noncrit_fault = SET_BIT; 
        mtbdl->msg = mtbdl_ncf_excess_files_msg; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
    }
}


// Run prep state user button input check 
void mtbdl_run_prep_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 1 - triggers the run state 
        case UI_BTN_1: 
            mtbdl->run = SET_BIT; 
            break; 

        // Button 2 - cancels the run state --> triggers idle state 
        case UI_BTN_2: 
            mtbdl->idle = SET_BIT; 
            break; 

        default: 
            break; 
    }
}


// Run prep state exit 
void mtbdl_run_prep_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->delay_timer.time_start = SET_BIT; 

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
        mtbdl_run_countdown_state_entry(mtbdl); 
    }

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_EXIT_TIMER))
    {
        mtbdl_run_countdown_state_exit(mtbdl); 
    }
}


// Run countdown state entry 
void mtbdl_run_countdown_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->run = CLEAR_BIT; 

    // Display the run countdown state message 
    hd44780u_set_msg(mtbdl_run_countdown_msg, MTBDL_MSG_LEN_1_LINE); 

    // Prepare the log file 
    log_data_file_prep(); 

    // SD card will be written to constantly so no need for the check state 
    fatfs_clear_check_flag(); 

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
void mtbdl_run_countdown_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->run = SET_BIT; 
    mtbdl->delay_timer.time_start = SET_BIT; 

    // Put the screen in low power mode 
    hd44780u_set_low_pwr_flag(); 

    // Set the M8Q to idle state so the time of reading can be controlled. This setter 
    // is placed here specifically so that the M8Q does not go too long without being 
    // read from. Originally it was in the state entry function but the state takes ~5s 
    // which means the M8Q then doesn't get read from for at least 5 seconds until the 
    // system starts data logging. The device will consider itself not needed and go into 
    // an idle state after a couple seconds if it's not read from which means once we 
    // got to data logging, it would take a bit longer to get the first read of data 
    // from the device which was casing a data overrun. Putting the setter here prevents 
    // this from happening. 
    m8q_set_idle_flag(); 

    // Turn off the data logging LED 
    ui_led_colour_change(WS2812_LED_0, mtbdl_led_clear); 
}

//=======================================================================================


//=======================================================================================
// Run state 

void mtbdl_run_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->run)
    {
        mtbdl_run_state_entry(mtbdl); 
    }

    // State operations: 
    // - Check for the user button input 
    // - Log system data 
    // - Update the data logging LED 
    // - Update the GPS status and feedback 

    mtbdl_run_user_input_check(mtbdl); 
    log_data(); 
    ui_led_state_update(WS2812_LED_0); 
    ui_gps_led_status_update(); 

    // State exit 
    if (mtbdl->run || mtbdl->fault_code || mtbdl->low_pwr)
    {
        mtbdl_run_state_exit(mtbdl); 
    }
}


// Run state entry 
void mtbdl_run_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->run = CLEAR_BIT; 

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

    // Prep the logging data and start interrupts 
    log_data_prep(); 
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
            log_set_trailmark(); 
            break; 

        default: 
            break; 
    }
}


// Run state exit 
void mtbdl_run_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->msg = mtbdl_postrun_msg; 
    mtbdl->msg_len = MTBDL_MSG_LEN_2_LINE; 
    
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
        mtbdl_postrun_state_entry(mtbdl); 
    }
    
    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_EXIT_TIMER))
    {
        mtbdl_postrun_state_exit(mtbdl); 
    }
}


// Post run state entry 
void mtbdl_postrun_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->run = CLEAR_BIT; 
    mtbdl->noncrit_fault = CLEAR_BIT; 

    // Terminate a possible open log file and update the screen message with the 
    // system status. 
    log_data_end(); 
    hd44780u_set_msg(mtbdl->msg, mtbdl->msg_len); 
    
    // Put the M8Q back into a continuous read state 
    m8q_set_read_flag(); 

    // Turn on the data logging LED 
    ui_led_colour_change(WS2812_LED_0, mtbdl_led0_1); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Post run state exit 
void mtbdl_postrun_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->idle = SET_BIT; 
    mtbdl->delay_timer.time_start = SET_BIT; 
    
    // Clear the post run state message 
    hd44780u_set_clear_flag(); 

    // Set the SD card controller check flag 
    fatfs_set_check_flag(); 

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
        mtbdl_data_select_state_entry(mtbdl); 
    }

    // State operations: 
    // - Check for user button input 
    mtbdl_data_select_user_input_check(mtbdl); 

    // State exit 
    if (mtbdl->data_select || 
        mtbdl->tx || 
        mtbdl->idle || 
        mtbdl->fault_code || 
        mtbdl->low_pwr)
    {
        mtbdl_data_select_state_exit(mtbdl); 
    }
}


// Data selection state entry 
void mtbdl_data_select_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->data_select = CLEAR_BIT; 

    // Display the data select state message 
    hd44780u_set_msg(mtbdl_data_select_msg, MTBDL_MSG_LEN_3_LINE); 

    // Take the HC-05 out of low power mode. The device is turned on here to prevent any 
    // possible read errors. 
    hc05_on(); 

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
void mtbdl_data_select_state_exit(mtbdl_trackers_t *mtbdl)
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
        mtbdl_dev_search_state_entry(mtbdl); 
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
    if (mtbdl->idle || 
        mtbdl->data_select || 
        mtbdl->fault_code || 
        mtbdl->low_pwr)
    {
        mtbdl_dev_search_state_exit(mtbdl); 
    }
}


// Device search state entry 
void mtbdl_dev_search_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->data_select = CLEAR_BIT; 

    // Display the device connection search state message 
    hd44780u_set_msg(mtbdl_dev_search_msg, MTBDL_MSG_LEN_2_LINE); 

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
void mtbdl_dev_search_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->delay_timer.time_start = SET_BIT; 
    mtbdl->led_state = CLEAR; 
    
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
        mtbdl_prerx_state_entry(mtbdl); 
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
    if (mtbdl->rx || 
        mtbdl->noncrit_fault || 
        mtbdl->idle || 
        mtbdl->fault_code || 
        mtbdl->low_pwr)
    {
        mtbdl_prerx_state_exit(mtbdl); 
    }
}


// Pre RX state entry 
void mtbdl_prerx_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->rx = CLEAR_BIT; 
    mtbdl->data_select = CLEAR_BIT; 

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
void mtbdl_prerx_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->delay_timer.time_start = SET_BIT; 
    mtbdl->led_state = CLEAR; 

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
        mtbdl_rx_state_entry(mtbdl); 
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
    if (mtbdl->rx || 
        mtbdl->noncrit_fault || 
        mtbdl->fault_code || 
        mtbdl->low_pwr)
    {
        mtbdl_rx_state_exit(mtbdl); 
    }
}


// RX state entry 
void mtbdl_rx_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->rx = CLEAR_BIT; 

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
void mtbdl_rx_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->delay_timer.time_start = SET_BIT; 
    mtbdl->led_state = CLEAR; 

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
        mtbdl_postrx_state_entry(mtbdl); 
    }

    // State operations: 
    // - Update the Bluetooth transfer LED 
    ui_led_state_update(WS2812_LED_2); 

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_EXIT_TIMER))
    {
        mtbdl_postrx_state_exit(mtbdl); 
    }
}


// Post RX state entry 
void mtbdl_postrx_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->noncrit_fault = CLEAR_BIT; 
    mtbdl->rx = CLEAR_BIT; 
    
    // Update the screen message 
    hd44780u_set_msg(mtbdl->msg, mtbdl->msg_len); 
    
    // Save the parameters to file and update tracking info 
    param_write_bike_params(FATFS_MODE_OEW); 

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
void mtbdl_postrx_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->idle = SET_BIT; 
    mtbdl->delay_timer.time_start = SET_BIT; 

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
        mtbdl_pretx_state_entry(mtbdl); 
    }

    // State operations: 
    // - Check for user button input 
    // - Update the Bluetooth LED 
    // - Check for a loss of Bluetooth connection or the lack of a data log file to send - 
    //   if true then abort the transfer 

    mtbdl_pretx_user_input_check(mtbdl); 
    ui_led_state_update(WS2812_LED_2); 

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

    // State exit 
    if (mtbdl->tx || 
        mtbdl->idle || 
        mtbdl->noncrit_fault || 
        mtbdl->fault_code || 
        mtbdl->low_pwr)
    {
        mtbdl_pretx_state_exit(mtbdl); 
    }
}


// Pre TX state entry 
void mtbdl_pretx_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->data_select = CLEAR_BIT; 

    // Prepare the log file to send 
    if (ui_tx_prep())
    {
        // File ready - display the pre tx state message 
        ui_set_pretx_msg(); 
        mtbdl->tx = CLEAR_BIT; 

        // Set the Bluetooth LED colour and blink rate 
        ui_led_colour_set(WS2812_LED_2, mtbdl_led2_1); 
        ui_led_duty_set(WS2812_LED_2, UI_LED_DUTY_SHORT); 

        // Set user button LED colours 
        ui_led_colour_set(WS2812_LED_7, mtbdl_led7_1); 
        ui_led_colour_set(WS2812_LED_6, mtbdl_led6_1); 
        ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
        ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
    }
    else 
    {
        // File does not exist - update the message and abort the state 
        mtbdl->noncrit_fault = SET_BIT; 
        mtbdl->msg = mtbdl_ncf_no_files_msg; 
        mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 
    }
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
void mtbdl_pretx_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->delay_timer.time_start = SET_BIT; 
    mtbdl->led_state = CLEAR; 

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
        mtbdl_tx_state_entry(mtbdl); 
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
    if (mtbdl->tx || 
        mtbdl->noncrit_fault || 
        mtbdl->fault_code || 
        mtbdl->low_pwr)
    {
        mtbdl_tx_state_exit(mtbdl); 
    }
}


// TX state entry 
void mtbdl_tx_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->tx = CLEAR_BIT; 
    mtbdl->msg = mtbdl_posttx_msg; 
    mtbdl->msg_len = MTBDL_MSG_LEN_3_LINE; 

    // Display the tx state message 
    hd44780u_set_msg(mtbdl_tx_msg, MTBDL_MSG_LEN_2_LINE); 
    
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
void mtbdl_tx_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->delay_timer.time_start = SET_BIT; 
    mtbdl->led_state = CLEAR; 

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
    static uint32_t exit_timer = CLEAR; 

    // State entry 
    if (mtbdl->tx)
    {
        exit_timer = mtbdl->noncrit_fault ? MTBDL_STATE_EXIT_TIMER : MTBDL_STATE_EXIT_WAIT; 
        mtbdl_posttx_state_entry(mtbdl); 
    }

    // State operations: 
    // - Update the Bluetooth LED 
    // - Continuously call the TX mode end function while waiting for the state end timer 
    //   to expire. This checks for a response from the user after a log file has been 
    //   fully sent. 

    ui_led_state_update(WS2812_LED_2); 

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, exit_timer) || ui_tx_end())
    {
        mtbdl_posttx_state_exit(mtbdl); 
    }
}


// Post TX state entry 
void mtbdl_posttx_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->tx = CLEAR_BIT; 

    // Set the post TX state message 
    hd44780u_set_msg(mtbdl->msg, mtbdl->msg_len); 

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
void mtbdl_posttx_state_exit(mtbdl_trackers_t *mtbdl)
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
        mtbdl_precalibrate_state_entry(mtbdl); 
    }

    // State operations: 
    // - Check for user button input 
    // - Update the calibration LED 

    mtbdl_precalibrate_user_input_check(mtbdl); 
    ui_led_state_update(WS2812_LED_2); 

    // State exit 
    if (mtbdl->calibrate || 
        mtbdl->idle || 
        mtbdl->fault_code || 
        mtbdl->low_pwr)
    {
        mtbdl_precalibrate_state_exit(mtbdl); 
    }
}


// Pre-calibration state entry 
void mtbdl_precalibrate_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->calibrate = CLEAR_BIT; 

    // Display the calibration state message 
    hd44780u_set_msg(mtbdl_precal_msg, MTBDL_MSG_LEN_4_LINE); 

    // Take the MPU-6050 out of low power mode 
    mpu6050_clear_low_power(DEVICE_ONE); 

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
void mtbdl_precalibrate_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->delay_timer.time_start = SET_BIT; 
    mtbdl->led_state = CLEAR; 

    // Turn the calibration LED off 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led_clear); 

    // Clear the pre-calibration state message 
    hd44780u_set_clear_flag(); 
}

//=======================================================================================


//=======================================================================================
// Calibration state 

void mtbdl_calibrate_state(mtbdl_trackers_t *mtbdl)
{
    // State entry 
    if (mtbdl->calibrate)
    {
        mtbdl_calibrate_state_entry(mtbdl); 
    }

    // State operations: 
    // - Sample data that can be used for calculating the calibration values 
    log_calibration(); 

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_EXIT_TIMER))
    {
        mtbdl_calibrate_state_exit(mtbdl); 
    }
}


// Calibration state entry 
void mtbdl_calibrate_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->calibrate = CLEAR_BIT; 
    mtbdl->msg = mtbdl_postcal_msg; 
    mtbdl->msg_len = MTBDL_MSG_LEN_1_LINE; 

    // Display the calibration state message 
    hd44780u_set_msg(mtbdl_cal_msg, MTBDL_MSG_LEN_1_LINE); 

    // Prepare to record data for calibration 
    log_calibration_prep(); 

    // Turn the calibration LED on 
    ui_led_colour_change(WS2812_LED_2, mtbdl_led2_2); 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Calibration state exit 
void mtbdl_calibrate_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->calibrate = SET_BIT; 
    mtbdl->delay_timer.time_start = SET_BIT; 

    // Calculate the calibration values 
    log_calibration_calculation(); 

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
        mtbdl_postcalibrate_state_entry(mtbdl); 
    }

    // State operations: 
    // - Update the calibration LED 
    ui_led_state_update(WS2812_LED_2); 

    // State exit 
    if (mtbdl_nonblocking_delay(mtbdl, MTBDL_STATE_EXIT_TIMER))
    {
        mtbdl_postcalibrate_state_exit(mtbdl); 
    }
}


// Post calibration state entry 
void mtbdl_postcalibrate_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->calibrate = CLEAR_BIT; 

    // Display the post calibration message 
    hd44780u_set_msg(mtbdl->msg, mtbdl->msg_len); 

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
void mtbdl_postcalibrate_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->idle = SET_BIT; 
    mtbdl->delay_timer.time_start = SET_BIT; 

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
        mtbdl_lowpwr_state_entry(mtbdl); 
    }

    // State operations: 
    // - Check for user button input 
    // - Update the low power LED 
    // - Check battery SOC 

    mtbdl_lowpwr_user_input_check(mtbdl); 
    ui_led_state_update(WS2812_LED_3); 

    if (ui_get_soc() >= MTBDL_SOC_THRESHOLD)
    {
        mtbdl->low_pwr = SET_BIT; 
    }

    // State exit 
    if (mtbdl->low_pwr)
    {
        mtbdl_lowpwr_state_exit(mtbdl); 
    }
}


// Low power state entry 
void mtbdl_lowpwr_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->low_pwr = CLEAR_BIT; 

    // Display the low power state message 
    hd44780u_set_msg(mtbdl_low_pwr_msg, MTBDL_MSG_LEN_3_LINE); 

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
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led4_1); 
}


// Low power state user button input check 
void mtbdl_lowpwr_user_input_check(mtbdl_trackers_t *mtbdl)
{
    switch (mtbdl->btn_press)
    {
        // Button 4 - Turns the screen backlight on 
        case UI_BTN_4: 
            hd44780u_wake_up(); 
            break; 

        default: 
            break; 
    }
}


// Low power state exit 
void mtbdl_lowpwr_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->low_pwr = CLEAR_BIT; 
    mtbdl->idle = SET_BIT; 
    mtbdl->delay_timer.time_start = SET_BIT; 
    mtbdl->led_state = CLEAR; 

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
        mtbdl_fault_state_entry(mtbdl); 
    }

    // State operations: 
    // - Check for user button input 
    mtbdl_fault_user_input_check(mtbdl); 

    // State exit 
    if (mtbdl->reset || mtbdl->low_pwr)
    {
        mtbdl_fault_state_exit(mtbdl); 
    }
}


// Fault state entry 
void mtbdl_fault_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->fault = SET_BIT; 

    // Display the fault state message and take screen out of power save mode 
    hd44780u_set_msg(mtbdl_fault_msg, MTBDL_MSG_LEN_2_LINE); 
    hd44780u_clear_pwr_save_flag(); 

    // Turn the fault LED on 
    ui_led_colour_change(WS2812_LED_3, mtbdl_led3_1); 

    // Close any file that may be open 
    fatfs_close(); 

    // Write the fault code to a file on the SD card 
    char fault_str_buff[MTBDL_MAX_STR_LEN]; 
    fatfs_set_dir(mtbdl_fault_dir); 
    fatfs_open(mtbdl_bike_param_file, FATFS_MODE_W); 
    snprintf(fault_str_buff, MTBDL_MAX_STR_LEN, mtbdl_fault_info, mtbdl->fault_code); 
    fatfs_puts(fault_str_buff); 
    fatfs_close(); 

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
void mtbdl_fault_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->fault_code = CLEAR; 
    mtbdl->fault = CLEAR_BIT; 
        
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
        mtbdl_reset_state_entry(mtbdl); 
    }

    // State exit 
    mtbdl_reset_state_exit(mtbdl); 
}


// Reset state entry 
void mtbdl_reset_state_entry(mtbdl_trackers_t *mtbdl)
{
    mtbdl->reset = CLEAR_BIT; 

    // Set user button LED colours 
    ui_led_colour_set(WS2812_LED_7, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_6, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_5, mtbdl_led_clear); 
    ui_led_colour_set(WS2812_LED_4, mtbdl_led_clear); 
}


// Reset state exit 
void mtbdl_reset_state_exit(mtbdl_trackers_t *mtbdl)
{
    mtbdl->init = SET_BIT; 
    mtbdl->fault_code = CLEAR; 

    // Reset devices 
    fatfs_set_reset_flag(); 
    hc05_clear_status(); 
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
