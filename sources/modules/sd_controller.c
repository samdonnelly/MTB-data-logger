/**
 * @file sd_controller.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief SD card controller 
 * 
 * @version 0.1
 * @date 2023-01-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//=======================================================================================
// Includes 

#include "sd_controller.h"

//=======================================================================================


//=======================================================================================
// Function prototypes 

/**
 * @brief SD card controller initialization state 
 * 
 * @details Attempts to mount the volume. If successful, the project directory saved in 
 *          sd_trackers_t will be made if it does not already exist and the state 
 *          machine will move to the "access" state. If unsuccessful, the "not ready" state
 *          will be entered. The init state is called on startup and controller reset as 
 *          well as from the "not ready" state once a device is seen. If there is a fault 
 *          during one of the volume read operations, excluding the mounting procedure, 
 *          then the fault flag will be set and the "fault" state will be entered. 
 * 
 * @param sd_device : device tracker that defines control characteristics 
 */
void sd_init_state(sd_trackers_t *sd_device); 


/**
 * @brief SD card controller not ready state 
 * 
 * @details Continuously checks for the presence of the volume to see if it shows up. This 
 *          state indicates that the volume is not seen by the system (not ready flag set) 
 *          or the user has intentions to remove the volume (eject flag set). This state
 *          is entered from the "init" state if volume mounting is unsuccessful or from
 *          the "eject" state. This state is left when both the not ready flag (which 
 *          indicates a device is present) and the eject flag (which indicates the user is 
 *          ready to use the volume) are cleared at which point the "init" state is 
 *          entered. This state can also be left if the reset flag gets set. The fault 
 *          flag cannot be set here as it assumes the volume is not present so no volume 
 *          faults can occur. 
 *          
 *          Note that being in this state will render the use of the read/write getters/
 *          setters useless. If you're in this state then either the disk is not present 
 *          so these reads/writes won't work or the eject flag is set which will unmount 
 *          the device and therefore make the reads/writes not possible. 
 * 
 * @param sd_device : device tracker that defines control characteristics 
 */
void sd_not_ready_state(sd_trackers_t *sd_device); 


/**
 * @brief SD card controller access state 
 * 
 * @param sd_device : data record 
 */
void sd_access_state(sd_trackers_t *sd_device); 


/**
 * @brief SD card controller access check state 
 * 
 * @details Continuously checks for the presence of the volume to see if it goes missing. If 
 *          it is missing the not ready flag will be set. This state indicates that the volume 
 *          is seen by the system and normal read/write operations can be performed (using the 
 *          getters and setters). This state is entered from the "init" state when mounting is 
 *          successful. It is left when the fault or reset flags are set, or if either of the 
 *          not ready or eject flags are set. 
 * 
 * @param sd_device : device tracker that defines control characteristics 
 */
void sd_access_check_state(sd_trackers_t *sd_device); 


/**
 * @brief SD card controller eject state 
 * 
 * @details If there is an open file this state closes it then unmounts the volume. This state 
 *          is triggered by the eject flag being set which is set by the application through 
 *          the sd_set_eject_flag setter. This can be used to safely remove the disk while 
 *          the system is still powered on. After this state is run it immediately goes to 
 *          the "not ready" state. The eject flag is cleared through the application code using 
 *          the sd_clear_eject_flag setter. 
 * 
 * @see sd_set_eject_flag 
 * @see sd_clear_eject_flag 
 * 
 * @param sd_device : device tracker that defines control characteristics 
 */
void sd_eject_state(sd_trackers_t *sd_device); 


/**
 * @brief SD card controller fault state 
 * 
 * @details Currently this state does nothing until the reset flag is set. This state can be 
 *          entered from the "init" or "access" states after the fault flag has been set. The 
 *          fault flag can be set in the getter/setter functions when a FATFS file operation 
 *          is performed if one of these operations is not successful. To leave this state, the 
 *          reset flag has to be set using the sd_set_reset_flag setter at which point the 
 *          "reset" state will be entered. 
 * 
 * @see sd_set_reset_flag 
 * 
 * @param sd_device : device tracker that defines control characteristics 
 */
void sd_fault_state(sd_trackers_t *sd_device); 


/**
 * @brief SD card controller reset state 
 * 
 * @details Closes any open file, resets the projects subdirectory, unmounts the volumes and 
 *          resets controller trackers as needed. This state is triggered by setting the reset 
 *          flag using sd_set_reset_flag and can be entered from the "not ready", "access" or 
 *          "fault" states. This state is called typically when there is an issue in the system 
 *          and it needs to re-initialize itself. After this state is run it will go directly to 
 *          the "init" state and the reset flag will be automatically cleared. 
 * 
 * @see sd_set_reset_flag 
 * 
 * @param sd_device : device tracker that defines control characteristics 
 */
void sd_reset_state(sd_trackers_t *sd_device); 


/**
 * @brief Mount the volume 
 * 
 * @details Attempts to mount the volume and returns the file operation status. If the mount 
 *          is successful then the mount flag will be set. This function is called by the "init" 
 *          state and is needed before read/write operations can be performed on the volume. 
 * 
 * @return FRESULT : FATFS file function return code 
 */
FRESULT sd_mount(sd_trackers_t *sd_device); 


/**
 * @brief Unmount the volume 
 * 
 * @details This function unmounts the volume and clears the mount flag. This function is 
 *          called buy the "init" state if mounting fails, and also by the "eject" and "reset" 
 *          states. If power remains on in the system, then unmounting should be done before 
 *          removing the volume. 
 *          
 *          Note that this controller/driver only support a single volume right now so this 
 *          function is hard coded to unmount logical drive 0 which is the default volume 
 *          number and number assigned to the volumes used. 
 * 
 * @return FRESULT : FATFS file function return code 
 */
FRESULT sd_unmount(sd_trackers_t *sd_device); 


/**
 * @brief Get the volume label 
 * 
 * @details Reads the volume label and stores it in the controller tracker data structure if 
 *          needed. This function is called during the "init" state if mounting was 
 *          successful and will update the fault code in the process if something goes wrong 
 *          while reading the label. 
 * 
 * @param sd_device : data record 
 * @return FRESULT : FATFS file function return code 
 */
FRESULT sd_getlabel(sd_trackers_t *sd_device); 


/**
 * @brief Get free space 
 * 
 * @details Checks the free space of the volume. This function is called after successful 
 *          mounting of the volume in the "init" state. The free space is checked against 
 *          a threshold to ensure there is sufficient space for the system to record data. 
 *          If the free space is below the threshold then the fault flag is set. 
 * 
 * @return FRESULT : FATFS file function return code 
 */
FRESULT sd_getfree(sd_trackers_t *sd_device); 

//=======================================================================================


//=======================================================================================
// Variables 

// Device tracker record instance 
static sd_trackers_t sd_device_trackers; 

// Function pointers to controller states 
static sd_state_functions_t state_table[SD_NUM_STATES] = 
{
    &sd_init_state, 
    &sd_not_ready_state, 
    &sd_access_state, 
    &sd_access_check_state, 
    &sd_eject_state, 
    &sd_fault_state, 
    &sd_reset_state 
}; 

//=======================================================================================


//=======================================================================================
// Control functions 

// SD card controller initialization 
void sd_controller_init(const char *path)
{
    // Check that the path length is not too long 

    // Controller information 
    sd_device_trackers.state = SD_INIT_STATE; 
    sd_device_trackers.fault_code = CLEAR; 
    sd_device_trackers.fault_mode = CLEAR; 

    // File system information 
    memset((void *)sd_device_trackers.path, CLEAR, SD_PATH_SIZE); 
    strcpy(sd_device_trackers.path, path); 
    memset((void *)sd_device_trackers.dir, CLEAR, SD_PATH_SIZE); 
    
    // State trackers 
    sd_device_trackers.mount = CLEAR_BIT; 
    sd_device_trackers.not_ready = CLEAR_BIT; 
    sd_device_trackers.check = CLEAR_BIT; 
    sd_device_trackers.eject = CLEAR_BIT; 
    sd_device_trackers.open_file = CLEAR_BIT; 
    sd_device_trackers.startup = SET_BIT; 
}


// SD card controller 
void sd_controller(void)
{
    sd_states_t next_state = sd_device_trackers.state; 

    //==================================================
    // State machine 

    switch (next_state)
    {
        case SD_INIT_STATE: 
            // Make sure the init state runs at least once 
            if (!sd_device_trackers.startup)
            {
                // Fault during drive access 
                if (sd_device_trackers.fault_code) 
                {
                    next_state = SD_FAULT_STATE; 
                }

                // Device successfully mounted and access check requested 
                else if (sd_device_trackers.mount && sd_device_trackers.check) 
                {
                    next_state = SD_ACCESS_CHECK_STATE; 
                }

                // Device successfully mounted 
                else if (sd_device_trackers.mount) 
                {
                    next_state = SD_ACCESS_STATE; 
                }

                // Default to the not ready state if not mounted 
                else 
                {
                    next_state = SD_NOT_READY_STATE; 
                }
            }

            break; 

        case SD_NOT_READY_STATE: 
            // Reset flag set 
            if (sd_device_trackers.reset) 
            {
                next_state = SD_RESET_STATE; 
            }

            // Drive accessible and application code clears eject flag 
            else if (!sd_device_trackers.not_ready && !sd_device_trackers.eject) 
            {
                next_state = SD_INIT_STATE; 
            }

            break; 

        case SD_ACCESS_STATE: 
            // File access fault 
            if (sd_device_trackers.fault_code) 
            {
                next_state = SD_FAULT_STATE; 
            }

            // Reset flag set 
            else if (sd_device_trackers.reset) 
            {
                next_state = SD_RESET_STATE; 
            }

            // Eject flag set 
            else if (sd_device_trackers.eject)
            {
                next_state = SD_EJECT_STATE; 
            }

            // Check flag set 
            else if (sd_device_trackers.check)
            {
                next_state = SD_ACCESS_CHECK_STATE; 
            }

            break; 

        case SD_ACCESS_CHECK_STATE: 
            // File access fault 
            if (sd_device_trackers.fault_code) 
            {
                next_state = SD_FAULT_STATE; 
            }

            // Reset flag set 
            else if (sd_device_trackers.reset) 
            {
                next_state = SD_RESET_STATE; 
            }

            // Volume not seen or eject flag set 
            else if (sd_device_trackers.not_ready || sd_device_trackers.eject)
            {
                next_state = SD_EJECT_STATE; 
            }

            // Check flag cleared 
            else if (!sd_device_trackers.check)
            {
                next_state = SD_ACCESS_STATE; 
            }

            break; 

        case SD_EJECT_STATE: 
            // Default to the not ready state if the eject flag has been set 
            next_state = SD_NOT_READY_STATE; 
            break; 

        case SD_FAULT_STATE: 
            // Wait for reset flag to set 
            if (sd_device_trackers.reset) 
            {
                next_state = SD_RESET_STATE; 
            }

            // Eject flag set 
            if (sd_device_trackers.eject) 
            {
                next_state = SD_EJECT_STATE; 
            }

            break; 

        case SD_RESET_STATE: 
            next_state = SD_INIT_STATE; 
            break; 

        default: 
            // Default back to the init state 
            next_state = SD_INIT_STATE; 
            break; 
    }

    //==================================================

    // Go to state function 
    (state_table[next_state])(&sd_device_trackers); 

    // Update the state 
    sd_device_trackers.state = next_state; 
}

//=======================================================================================


//=======================================================================================
// State functions 

// SD card controller initialization state 
void sd_init_state(sd_trackers_t *sd_device) 
{
    // Clear startup flag 
    sd_device->startup = CLEAR_BIT; 

    // Clear reset flag 
    sd_device->reset = CLEAR_BIT; 

    // Attempt to mount the volume 
    if (sd_mount(sd_device) == FR_OK) 
    {
        // Mounting successful 
        // Read the volume, serial number, free space and make the directory specified by 
        // "path" if it does not exist 
        sd_getlabel(sd_device); 
        sd_getfree(sd_device); 
        sd_mkdir(""); 
    }
    else 
    {
        // Mounting unsuccessful 
        // Go to the not ready state and unmount the volume 
        sd_device->not_ready = SET_BIT; 
        sd_unmount(sd_device); 
    }
}


// SD card controller not ready state 
void sd_not_ready_state(sd_trackers_t *sd_device)
{
    // Check if the volume is present 
    if (sd_get_existance() == (DISK_STATUS)TRUE) 
    {
        // Present - clear the not ready flag so we can try remounting 
        sd_device->not_ready = CLEAR_BIT; 
    }
}


// SD card controller access state 
void sd_access_state(sd_trackers_t *sd_device)
{
    // Do nothing while the volume is accessed 
}


// SD card controller access file state 
void sd_access_check_state(sd_trackers_t *sd_device) 
{
    // Check for the presence of the volume 
    if (sd_ready_rec() == (DISK_STATUS)FALSE)
    {
        // If not seen then set the not_ready flag 
        sd_device->not_ready = SET_BIT; 
    }
}


// SD card controller eject state 
void sd_eject_state(sd_trackers_t *sd_device)
{
    // Attempt to close the open file 
    sd_close(); 

    // Unmount the volume 
    sd_unmount(sd_device); 
}


// SD card controller fault state 
void sd_fault_state(sd_trackers_t *sd_device) 
{
    // Idle until the reset flag is set 
}


// SD card controller reset state 
void sd_reset_state(sd_trackers_t *sd_device) 
{
    // Attempt to close a file 
    sd_close(); 

    // Reset sub directory 
    memset((void *)sd_device_trackers.dir, CLEAR, SD_PATH_SIZE); 

    // Unmount the volume 
    sd_unmount(sd_device); 

    // Clear device trackers 
    sd_device->fault_code = CLEAR; 
    sd_device->fault_mode = CLEAR; 
    sd_device->not_ready = CLEAR_BIT; 
    sd_device->eject = CLEAR_BIT; 
}

//=======================================================================================


//=======================================================================================
// Controller volume access functions 

// Mount the volume 
FRESULT sd_mount(sd_trackers_t *sd_device) 
{
    sd_device->fresult = f_mount(&sd_device->file_sys, "", SD_MOUNT_NOW); 

    if (sd_device->fresult == FR_OK) 
    {
        sd_device->mount = SET_BIT; 
    }

    return sd_device_trackers.fresult; 
}


// Unmount the volume 
FRESULT sd_unmount(sd_trackers_t *sd_device) 
{
    // Unmount, clear the init status so it can be re-mounted, and clear the mount bit 
    f_unmount(""); 
    sd_device->mount = CLEAR_BIT; 

    return FR_OK; 
}


// Get the volume label 
FRESULT sd_getlabel(sd_trackers_t *sd_device) 
{
    sd_device->fresult = f_getlabel("", sd_device->vol_label, &sd_device->serial_num); 

    if (sd_device->fresult) 
    {
        sd_device->fault_mode |= (SET_BIT << sd_device->fresult); 
        sd_device->fault_code |= (SET_BIT << SD_FAULT_COMMS); 
    }

    return sd_device->fresult; 
}


// Get free space 
FRESULT sd_getfree(sd_trackers_t *sd_device) 
{
    sd_device->fresult = f_getfree("", &sd_device->fre_clust, &sd_device->pfs); 

    if (sd_device->fresult == FR_OK) 
    {
        // Calculate the total space 
        // (n_fatent - 2) * csize / 2
        sd_device->total = (uint32_t)(((sd_device->pfs->n_fatent - 2) * 
                                           sd_device->pfs->csize) >> SHIFT_1);
        
        // Calculate the free space 
        // fre_clust * csize / 2 
        sd_device->free_space = (uint32_t)((sd_device->fre_clust * 
                                               sd_device->pfs->csize) >> SHIFT_1); 

        // Check if there is sufficient disk space 
        if (sd_device->free_space < SD_FREE_THRESH) 
        {
            sd_device->fault_mode |= (SET_BIT << FR_DENIED); 
            sd_device->fault_code |= (SET_BIT << SD_FAULT_FREE); 
        }

    }
    else   // Communication fault 
    {
        sd_device->fault_mode |= (SET_BIT << sd_device->fresult); 
        sd_device->fault_code |= (SET_BIT << SD_FAULT_COMMS); 
    }

    return sd_device->fresult; 
}

//=======================================================================================


//=======================================================================================
// Setters 

// Set the check flag 
void sd_set_check_flag(void)
{
    sd_device_trackers.check = SET_BIT; 
}


// Clear the check flag 
void sd_clear_check_flag(void)
{
    sd_device_trackers.check = CLEAR_BIT; 
}


// Set the eject flag 
void sd_set_eject_flag(void) 
{
    sd_device_trackers.eject = SET_BIT; 
}


// Clear the eject flag 
void sd_clear_eject_flag(void) 
{
    sd_device_trackers.eject = CLEAR_BIT; 
}


// Set reset flag 
void sd_set_reset_flag(void) 
{
    sd_device_trackers.reset = SET_BIT; 
}


// Set directory 
void sd_set_dir(const TCHAR *dir)
{
    // Reset the saved directory and set the new directory 
    memset((void *)sd_device_trackers.dir, CLEAR, SD_PATH_SIZE); 
    strcpy(sd_device_trackers.dir, dir); 
}


// Make a new directory in the project directory 
FRESULT sd_mkdir(const TCHAR *dir) 
{
    // Check for NULL pointer 
    if (dir == NULL) 
    {
        return FR_INVALID_OBJECT; 
    }
    
    TCHAR sub_dir[SD_PATH_SIZE*2]; 

    // Record 'dir' for future use and establish 'path' as the base of the sub directory 
    sd_set_dir(dir); 
    strcpy(sub_dir, sd_device_trackers.path); 

    // If 'dir' is not a null character then prepare the sub-directory to be concatenated. 
    // 'dir' will be a null character when it's empty such as in the "init" state. 
    if (*sd_device_trackers.dir != NULL_CHAR)
    {
        strcat(sub_dir, "/"); 
    }

    // Concatenate 'dir' to complete the sub directory string 
    strcat(sub_dir, sd_device_trackers.dir); 

    // Check for the existance of the directory 
    sd_device_trackers.fresult = f_stat(sub_dir, (FILINFO *)NULL); 

    // Only proceed to make the directory if it does not exist 
    if (sd_device_trackers.fresult)
    {
        sd_device_trackers.fresult = f_mkdir(sub_dir); 

        // Set fault code if there is an access error 
        if (sd_device_trackers.fresult) 
        {
            sd_device_trackers.fault_mode |= (SET_BIT << sd_device_trackers.fresult); 
            sd_device_trackers.fault_code |= (SET_BIT << SD_FAULT_DIR); 
        }
    }

    return sd_device_trackers.fresult; 
}


// Open file 
FRESULT sd_open(
    const TCHAR *file_name, 
    uint8_t mode) 
{
    // Check for NULL pointers and strings 
    if ((file_name == NULL) || (*file_name == NULL_CHAR)) 
    {
        return FR_INVALID_OBJECT; 
    }

    TCHAR file_dir[SD_PATH_SIZE*3]; 

    // Attempt to open file if a file is not already open 
    if (!sd_device_trackers.open_file) 
    {
        // Establish 'path' as the root of the file directory 
        strcpy(file_dir, sd_device_trackers.path); 

        // If 'dir' is not a null character then concatenate it to the file directory 
        if (*sd_device_trackers.dir != NULL_CHAR)
        {
            strcat(file_dir, "/"); 
            strcat(file_dir, sd_device_trackers.dir); 
        }

        strcat(file_dir, "/"); 
        strcat(file_dir, file_name); 
        
        sd_device_trackers.fresult = f_open(&sd_device_trackers.file, 
                                               file_dir, 
                                               mode); 

        if (sd_device_trackers.fresult == FR_OK) 
        {
            sd_device_trackers.open_file = SET_BIT; 
        }
        else   // Open fault - record the fault types 
        {
            sd_device_trackers.fault_mode |= (SET_BIT << sd_device_trackers.fresult); 
            sd_device_trackers.fault_code |= (SET_BIT << SD_FAULT_OPEN); 
        }

        return sd_device_trackers.fresult; 
    }

    return FR_TOO_MANY_OPEN_FILES; 
}


// Close the open file 
FRESULT sd_close(void) 
{
    // Attempt to close a file if it's open 
    if (sd_device_trackers.open_file) 
    {
        sd_device_trackers.fresult = f_close(&sd_device_trackers.file); 

        if (sd_device_trackers.fresult) 
        {
            // Close file fault 
            sd_device_trackers.fault_mode |= (SET_BIT << sd_device_trackers.fresult); 
            sd_device_trackers.fault_code |= (SET_BIT << SD_FAULT_CLOSE); 
        }

        // Clear the open file flag regardless of the fault code 
        sd_device_trackers.open_file = CLEAR_BIT; 

        // Update the free space 
        sd_getfree(&sd_device_trackers); 

        return sd_device_trackers.fresult; 
    }

    return FR_OK; 
}


// Write to the open file 
FRESULT sd_f_write(
    const void *buff, 
    UINT btw) 
{
    // Check for void pointer? 
    // Check for open file? 

    // Write to the file 
    sd_device_trackers.fresult = f_write(&sd_device_trackers.file, 
                                            buff, 
                                            btw, 
                                            &sd_device_trackers.bw); 

    // Set fault code if there is an access error and a file is open 
    if (sd_device_trackers.fresult && sd_device_trackers.open_file) 
    {
        sd_device_trackers.fault_mode |= (SET_BIT << sd_device_trackers.fresult); 
        sd_device_trackers.fault_code |= (SET_BIT << SD_FAULT_WRITE); 
    }

    return sd_device_trackers.fresult; 
}


// Write a string to the open file 
int16_t sd_puts(const TCHAR *str) 
{
    // Check for void pointer? 
    // Check for open file? 

    // Writes a string to the file 
    int16_t puts_return = f_puts(str, &sd_device_trackers.file); 

    // Set fault code if there is a function error and a file is open 
    if ((puts_return < 0) && sd_device_trackers.open_file) 
    {
        sd_device_trackers.fault_mode |= (SET_BIT << FR_DISK_ERR); 
        sd_device_trackers.fault_code |= (SET_BIT << SD_FAULT_WRITE); 
    }

    return puts_return; 
}


// Write a formatted string to the open file 
int8_t sd_printf(
    const TCHAR *fmt_str, 
    uint16_t fmt_value) 
{
    // Check for void pointer? 
    // Check for open file? 

    // Writes a formatted string to the file 
    int8_t printf_return = f_printf(&sd_device_trackers.file, 
                                    fmt_str, 
                                    fmt_value); 

    // Set fault code if there is a function error and a file is open 
    if ((printf_return < 0) && sd_device_trackers.open_file) 
    {
        sd_device_trackers.fault_mode |= (SET_BIT << FR_DISK_ERR); 
        sd_device_trackers.fault_code |= (SET_BIT << SD_FAULT_WRITE); 
    }

    return printf_return; 
}


// Navigate within the open file 
FRESULT sd_lseek(FSIZE_t offset) 
{
    // Move to the specified position in the file 
    sd_device_trackers.fresult = f_lseek(&sd_device_trackers.file, offset); 

    // Set fault code if there is an access error and a file is open 
    if (sd_device_trackers.fresult && sd_device_trackers.open_file) 
    {
        sd_device_trackers.fault_mode |= (SET_BIT << sd_device_trackers.fresult); 
        sd_device_trackers.fault_code |= (SET_BIT << SD_FAULT_SEEK); 
    }

    return sd_device_trackers.fresult; 
}


// Delete a file 
FRESULT sd_unlink(const TCHAR* filename)
{
    // Check that path validity 
    if (filename == NULL) 
    {
        return FR_INVALID_OBJECT; 
    }

    TCHAR file_dir[SD_PATH_SIZE*3]; 

    // Establish 'path' as the root of the file directory 
    strcpy(file_dir, sd_device_trackers.path); 

    // If 'dir' is not a null character then concatenate it to the file directory 
    if (*sd_device_trackers.dir != NULL_CHAR)
    {
        strcat(file_dir, "/"); 
        strcat(file_dir, sd_device_trackers.dir); 
    }

    strcat(file_dir, "/"); 
    strcat(file_dir, filename); 

    // Attempt to delete the specified file 
    sd_device_trackers.fresult = f_unlink(file_dir); 

    // Set the fault code if the file failed to be deleted 
    if (sd_device_trackers.fresult)
    {
        sd_device_trackers.fault_mode |= (SET_BIT << sd_device_trackers.fresult); 
        sd_device_trackers.fault_code |= (SET_BIT << SD_FAULT_DIR); 
    }

    return sd_device_trackers.fresult; 
}

//=======================================================================================


//=======================================================================================
// Getters 

// Get state 
SD_STATE sd_get_state(void) 
{
    return sd_device_trackers.state; 
}


// Get fault code 
SD_FAULT_CODE sd_get_fault_code(void) 
{
    return sd_device_trackers.fault_code; 
}


// Get fault mode 
SD_FAULT_MODE sd_get_fault_mode(void)
{
    return sd_device_trackers.fault_mode; 
}


// Get open file flag 
SD_FILE_STATUS sd_get_file_status(void)
{
    return sd_device_trackers.open_file; 
}


// Check for the existance of a file or directory 
FRESULT sd_get_exists(const TCHAR *str)
{
    // Check for a valid file name 
    if ((str == NULL) || (*str == NULL_CHAR)) 
    {
        return FR_INVALID_OBJECT; 
    }

    // Local variables 
    TCHAR directory[SD_PATH_SIZE*3]; 

    // Establish 'path' as the root of the file directory 
    strcpy(directory, sd_device_trackers.path); 

    // If 'dir' is not a null character then concatenate it to the file directory 
    if (*sd_device_trackers.dir != NULL_CHAR)
    {
        strcat(directory, "/"); 
        strcat(directory, sd_device_trackers.dir); 
    }

    strcat(directory, "/"); 
    strcat(directory, str); 

    // Check for the existance of the directory 
    return f_stat(directory, (FILINFO *)NULL); 
}


// Read data from open file 
FRESULT sd_f_read(
    void *buff, 
    UINT btr) 
{
    // Read from the file 
    sd_device_trackers.fresult = f_read(&sd_device_trackers.file, 
                                           buff, 
                                           btr, 
                                           &sd_device_trackers.br); 
    
    // Set fault code if there is an access error and a file is open 
    if (sd_device_trackers.fresult && sd_device_trackers.open_file) 
    {
        sd_device_trackers.fault_mode |= (SET_BIT << sd_device_trackers.fresult); 
        sd_device_trackers.fault_code |= SD_FAULT_READ; 
    }

    return sd_device_trackers.fresult; 
}


// Reads a string from open file 
TCHAR* sd_gets(
    TCHAR *buff, 
    uint16_t len)
{
    // Read a string from the file 
    TCHAR *gets_return = f_gets(buff, len, &sd_device_trackers.file); 

    // Set fault code if there was a read operation error and a file is open 
    if ((gets_return == NULL) && (!sd_eof()) && sd_device_trackers.open_file) 
    {
        sd_device_trackers.fault_mode |= (SET_BIT << FR_DISK_ERR); 
        sd_device_trackers.fault_code |= SD_FAULT_READ; 
    }

    return gets_return; 
}


// Test for end of file on open file 
SD_EOF sd_eof(void) 
{
    return (SD_EOF)f_eof(&sd_device_trackers.file); 
}

//=======================================================================================
