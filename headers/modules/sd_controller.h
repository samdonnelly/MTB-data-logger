/**
 * @file sd_controller.h
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief SD card controller interface 
 * 
 * @version 0.1
 * @date 2023-01-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _SD_CARD_CONTROLLER_H_ 
#define _SD_CARD_CONTROLLER_H_ 

#ifdef __cplusplus
extern "C" {
#endif

//=======================================================================================
// Includes 

#include "sd_driver.h" 
#include "ff.h"

//=======================================================================================


//=======================================================================================
// Macros 

// State machine 
#define SD_NUM_STATES 7             // Number of possible states for the controller 

// Controller tracker 
#define SD_PATH_SIZE 50             // Volume path max length 
#define SD_INFO_SIZE 30             // Device info buffer size 
#define SD_FREE_THRESH 0x0000C350   // Free space threshold before disk full fault (KB) 

//=======================================================================================


//=======================================================================================
// Enums 

/**
 * @brief SD card controller states 
 */
typedef enum {
    SD_INIT_STATE, 
    SD_NOT_READY_STATE, 
    SD_ACCESS_STATE, 
    SD_ACCESS_CHECK_STATE, 
    SD_EJECT_STATE, 
    SD_FAULT_STATE, 
    SD_RESET_STATE 
} sd_states_t; 


/**
 * @brief SD card fault code bit indexes 
 */
typedef enum {
    SD_FAULT_DIR,          // Directory access - make or delete (unlink) 
    SD_FAULT_OPEN,         // Open 
    SD_FAULT_CLOSE,        // Close 
    SD_FAULT_WRITE,        // Write 
    SD_FAULT_READ,         // Read 
    SD_FAULT_SEEK,         // Seek 
    SD_FAULT_FREE,         // Free space 
    SD_FAULT_COMMS         // Comms 
} sd_fault_codes_t; 

//=======================================================================================


//=======================================================================================
// Structures 

// SD card controller trackers 
typedef struct sd_trackers_s 
{
    // Controller information 
    sd_states_t state;                        // State of the controller 
    uint16_t fault_code;                         // Fault code 
    DWORD fault_mode;                            // Fault mode - based on FRESULT 

    // File system information 
    FATFS file_sys;                              // File system object 
    FIL file;                                    // File object 
    FRESULT fresult;                             // Store result of FatFs operation 
    UINT br, bw;                                 // Read and write counters 
    TCHAR path[SD_PATH_SIZE];                    // Path to project directory 
    TCHAR dir[SD_PATH_SIZE];                     // Sub-directory in project directory 

    // Card capacity 
    FATFS *pfs;                                  // Pointer to file system object 
    DWORD fre_clust;                             // Free clusters 
    DWORD total, free_space;                     // Volume total and free space 
    
    // Volume tracking 
    TCHAR vol_label[SD_INFO_SIZE];               // Volume label 
    DWORD serial_num;                            // Volume serial number 

    // State trackers 
    uint8_t mount      : 1;                      // Volume mount flag 
    uint8_t not_ready  : 1;                      // Not ready flag 
    uint8_t check      : 1;                      // Check flag 
    uint8_t eject      : 1;                      // Eject flag 
    uint8_t open_file  : 1;                      // Open file flag 
    uint8_t reset      : 1;                      // Reset state trigger 
    uint8_t startup    : 1;                      // Ensures the init state is run 
}
sd_trackers_t; 

//=======================================================================================


//=======================================================================================
// Datatypes 

typedef sd_states_t SD_STATE; 
typedef uint16_t SD_FAULT_CODE; 
typedef DWORD SD_FAULT_MODE; 
typedef uint8_t SD_FILE_STATUS; 
typedef int8_t SD_EOF; 

//=======================================================================================


//=======================================================================================
// Function pointers 

/**
 * @brief SD card state machine function pointer 
 * 
 * @param sd_device : device tracker that defines control characteristics 
 */
typedef void (*sd_state_functions_t)(sd_trackers_t *sd_device); 

//=======================================================================================


//=======================================================================================
// Control functions 

/**
 * @brief SD card controller initialization 
 * 
 * @details Initializes the controller tracker information. The 'path' argument specifies 
 *          the directory where files will be saved on the volume. This directory is the 
 *          applications root directory and all subsequent folders and files will be saved 
 *          here. This allows for files from different applications to be easily separated. 
 *          Note that the path length must be less than SD_PATH_SIZE to prevent overrun. 
 *          If the path length is too long then the project/applications directory will not 
 *          be set. 
 * 
 * @param path : path to directory to use on the volume 
 */
void sd_controller_init(const char *path); 


/**
 * @brief SD card controller 
 * 
 * @details Contains the state machine to control the flow of the controller. Should be 
 *          called continuously by the application while the device is in use. 
 */
void sd_controller(void); 

//=======================================================================================


//=======================================================================================
// Setters 

/**
 * @brief Set the check flag 
 * 
 * @details The check flag triggers the access check state. In this state the controller will 
 *          continuously check for the presence of the volume to ensure it is still there. 
 *          This state is best for when the volume is not consistently in direct use but want 
 *          to make sure it wasm't removed. Normal read and write operations can still be 
 *          performed in this state, however there will be the added overhead of checking 
 *          for the volume presence on each pass. 
 */
void sd_set_check_flag(void); 


/**
 * @brief Clear the check flag 
 * 
 * @details Clearing the check flag will make the controller idle in the access state where 
 *          it does nothing and lets the SD card be accessed through the setters. This state 
 *          is best when the volume is being accessed consistently so a check is not needed 
 *          and when you don't want to waste cycle on a ping of the volume. 
 */
void sd_clear_check_flag(void); 


/**
 * @brief Set the eject flag 
 * 
 * @details The eject flag is intended to put the controller in the "eject" state which 
 *          preps the volume for removal. This flag is set by the application if the user 
 *          wants to remove the volume while the system still has power. 
 */
void sd_set_eject_flag(void); 


/**
 * @brief Clear the eject flag 
 * 
 * @details The eject flag must be cleared in order for the volume to be properly mounted 
 *          and used. This setter is only needed after sd_set_eject_flag has been called. 
 */
void sd_clear_eject_flag(void); 


/**
 * @brief Set reset flag 
 * 
 * @details The reset flag triggers a controller reset. This flag will be cleared 
 *          automatically after being set. 
 */
void sd_set_reset_flag(void); 


/**
 * @brief Set directory 
 * 
 * @details Updated (overwrites) the directry in the data record. 
 * 
 * @param dir : project directory to access 
 */
void sd_set_dir(const TCHAR *dir); 


/**
 * @brief Make a new directory in the project directory 
 * 
 * @details Wrapper function for the FATFS function f_mkdir. 
 *          
 *          Concatenates the argument 'dir' onto the project/application 'path' (defined in 
 *          the controller init function), checks for the existance of the new directory and 
 *          creates the directory if it does not already exist. If there is an error creating 
 *          the directory then the fault code will be set accordingly. 
 *          
 *          Since 'dir' is added onto the project path with each function call, passing 
 *          different directories will create new folders in parallel to one another. If 
 *          further sub-directories are desired then 'dir' must contain an existing directory 
 *          with the sub-directory added to the end and separated by a "/". 
 *          
 *          The length of 'dir' and 'path' together should be less than twice the length of 
 *          SD_PATH_SIZE. 
 *          
 *          If 'dir' is an invalid pointer then the function will return before attempting to 
 *          create a directory. 
 * 
 * @param dir : sub directory to creae within the project directory 
 * @return FRESULT : FATFS file function return code 
 */
FRESULT sd_mkdir(const TCHAR *dir); 


/**
 * @brief Open a file 
 * 
 * @details Wrapper function for the FATFS function f_open. 
 *          
 *          Concatenates the file name ('file_name') onto the project directory and attempts to 
 *          open the specified file. If there is an error opening the file then the fault code 
 *          will be updated accordingly. Note that if a subdirectory for the project has been 
 *          created using sd_mkdir then the file will be made in that directory. If you 
 *          want the file in a different directory then use sd_mkdir to update the 
 *          subdirectory accordingly (can specify 'dir' as "" in sd_mkdir to go to the 
 *          project root directory). 
 *          
 *          When the function attempts to open the specified file it will use the method 
 *          specified by 'mode' to do so. For example, if you specify sd_MODE_W as the mode 
 *          then the function will create a file if it does not already exist and open it in 
 *          write mode. See the SD card driver header for possible modes. 
 *           
 *          If a file is already open then there will be no attempt to open another. The result 
 *          can be observed in the return value. 
 * 
 * @param file_name : name of the file to open 
 * @param mode : mode to open the file in (read, write, etc.) 
 * @return FRESULT : FATFS file function return code 
 */
FRESULT sd_open(
    const TCHAR *file_name, 
    uint8_t mode); 


/**
 * @brief Close an open file 
 * 
 * @details Wrapper function for the FATFS function f_close. 
 *          
 *          If there is an open file then it gets closed, the fault code gets updated if there 
 *          is an issue closing the file and then the volume free space gets updated in the 
 *          controller tracker. If there is no file open then the function will bypass the 
 *          above steps and return FR_OK. 
 * 
 * @return FRESULT : FATFS file function return code 
 */
FRESULT sd_close(void); 


/**
 * @brief Write data to the open file 
 * 
 * @details Wrapper function for the FATFS function f_write. 
 *          
 *          Attempts to write the specified data to the open file and updates the fault code 
 *          if there's a write issue. If no file is open then no data will be written and the 
 *          fault code won't be updated. 
 * 
 * @param buff : void pointer to data to write 
 * @param btw : number of bytes to write 
 * @return FRESULT : FATFS file function return code 
 */
FRESULT sd_f_write(
    const void *buff, 
    UINT btw); 


/**
 * @brief Write a string to the open file 
 * 
 * @details Wrapper function for the FATFS function f_puts. 
 *          
 *          Attempts to write a string to the open file and updates the fault code if there's 
 *          a write issue. If no file is open then no data will be written and the fault code 
 *          won't be updated. The function returns the number of character encoding units 
 *          written to the file. If the write fails then a negtive number will be returned. 
 *          
 *          If there is a fault, the fault mode will always read FR_DISK_ERR. f_puts is a 
 *          wrapper of f_write and if there is an error of any kind in f_write then the 
 *          return of f_puts is negative. There is no (known) way of distinguishing 
 *          fault/error types using f_puts. 
 * 
 * @param str : pointer to string to write 
 * @return int8_t : number of character encoding units written to the file 
 */
int16_t sd_puts(const TCHAR *str); 


/**
 * @brief Write a formatted string to the open file 
 * 
 * @details Wrapper function for the FATFS function f_printf. 
 *          
 *          This function attempts to write a formatted string to the open file and updates 
 *          the fault code if there's a write issue. If no file is open then no data will be 
 *          written and the fault code will not be updated. The formatted string and data 
 *          type (in this case an unsigned 16-bit integer) must match for this function to 
 *          work as expected. 
 *          
 *          The function returns the number of character encoding units written to the file. 
 *          If the write fails then a negtive number will be returned. 
 *          
 *          f_printf has optional arguments (see the FATFS documentation for details), however 
 *          this function uses f_printf specifically for writing unsigned integers because 
 *          the embedded applications used by this driver (as of now) don't need different data 
 *          types so it simplifies the function. 
 *          
 *          If there is a fault, the fault mode will always read FR_DISK_ERR. f_printf is a 
 *          wrapper of f_write and if there is an error of any kind in f_write then the 
 *          return of f_printf is negative. There is no (known) way of distinguishing 
 *          fault/error types using f_puts. 
 * 
 * @param fmt_str : pointer to formatted string to write 
 * @param fmt_value : unsigned integer to write with the formatted string 
 * @return int8_t : number of character encoding units written to the file 
 */
int8_t sd_printf(
    const TCHAR *fmt_str, 
    uint16_t fmt_value); 


/**
 * @brief Select read/write pointer within an open file 
 * 
 * @details Wrapper function for the FATFS function f_lseek. 
 *          
 *          Moves to the specified position within an open file and updates the fault code 
 *          if there are issues doing so. If no file is open then nothing will happen. This 
 *          position/offset within the file is indexed from 0 which is the beginning of the 
 *          file. 
 *          
 *          If an offset beyond the open file size is specified and the file is opened in 
 *          write mode then the file will be expanded to the specified offset. If the file 
 *          position pointer is not pointer where expected it could be due to being at the 
 *          end of a file while in read mode, or from the volume being full and therefore 
 *          the file cannot be expanded. For these reasons the pointer position should be 
 *          checked after changing it. 
 *          
 *          The macro function f_rewind can be used to point the file position pointer back 
 *          to the beginning of the file (position 0). 
 * 
 * @param offset : byte position in the file to point to 
 * @return FRESULT : FATFS file function return code 
 */
FRESULT sd_lseek(FSIZE_t offset); 


/**
 * @brief Delete a file 
 * 
 * @details Attempt to delete the specified file in the path. The status of the operation 
 *          is returned. 
 * 
 * @param filename : name of file 
 * @return FRESULT : status of the delete operation 
 */
FRESULT sd_unlink(const TCHAR* filename); 

//=======================================================================================


//=======================================================================================
// Getters 

/**
 * @brief Get controller state 
 * 
 * @details Returns the current state of the controllers state machine. 
 * 
 * @return SD_STATE : state machine state 
 */
SD_STATE sd_get_state(void); 


/**
 * @brief Get fault code 
 * 
 * @details Returns the controllers fault code. The fault code indicates the FATFS file 
 *          system function that caused a fault. Each bit of the code corresponds to a file 
 *          operation which is defined by sd_fault_codes_t. When one of these operations 
 *          is unsuccessful on a valid file then the fault code will be set. The fault code 
 *          is used by the state machine to determine whether to enter the fault state. 
 *          The fault code is cleared on a controller reset. 
 * 
 * @see sd_fault_codes_t 
 * 
 * @return SD_FAULT_CODE : controller fault code 
 */
SD_FAULT_CODE sd_get_fault_code(void); 


/**
 * @brief Get fault mode 
 * 
 * @details Returns the controllers fault mode. The fault mode provides the FATFS file 
 *          system function return value defined by the FRESULT enum (FATFS code). The 
 *          return code in the FRESULT enum corresponds to the bit of the fault mode so 
 *          value of the the fault mode can be identified. When there is a fault in the 
 *          controller, this flag will be set and can be used to identify the cause of 
 *          the problem along with the fault code. 
 * 
 * @return SD_FAULT_MODE : controller fault mode 
 */
SD_FAULT_MODE sd_get_fault_mode(void); 


/**
 * @brief Get open file flag 
 * 
 * @details Returns the open file flag state. 
 * 
 * @return SD_FILE_STATUS : open file flag state 
 */
SD_FILE_STATUS sd_get_file_status(void); 


/**
 * @brief Check for the existance of a file or directory 
 *       
 * NOTE: The root directory is set during the controller init and the sub-directory 
 *       is set by the sd_set_dir function. 'str', passed as an argument to this 
 *       function, is concatenated onto the root + sub-directory that is already 
 *       defined so do not include those in 'str'. 
 * 
 * @param str : string to file or directory to check for 
 * @return FRESULT : FATFS file function return code 
 */
FRESULT sd_get_exists(const TCHAR *str); 


/**
 * @brief Read data from an open file 
 * 
 * @details Wrapper function for the FATFS function f_read. 
 *          
 *          Attempts to read data from an open file and updates the fault code if there 
 *          is an error during the read process. If there is no file open then nothing 
 *          will happen. Note that the read will start at the read/write pointer which 
 *          can be changed using sd_lseek. There is no data type during the read 
 *          process so a void pointer type buffer is used. 
 * 
 * @see sd_lseek 
 * 
 * @param buff : void pointer to buffer to store read data 
 * @param btr : number of bytes to read 
 * @return FRESULT : FATFS file function return code 
 */
FRESULT sd_f_read(
    void *buff, 
    UINT btr); 


/**
 * @brief Reads a string from an open file 
 * 
 * @details Wrapper function for the FATFS function f_gets. 
 *          
 *          Attempts to read a string from an open file then updates the fault code if 
 *          it's unsuccessful. If no file is open then nothing will happen. A string 
 *          will be read until an end of line character is seen ('\n'), the end of the 
 *          file is reached or the string length has been reached. The read string is 
 *          terminated with '\0'. If the read is unsuccessful then a NULL pointer is 
 *          returned. 
 * 
 * @param buff : pointer to character buffer to store the read string 
 * @param len : lengh of string to read (bytes) 
 * @return TCHAR : pointer to buff (if read successful) 
 */
TCHAR* sd_gets(
    TCHAR *buff, 
    uint16_t len); 


/**
 * @brief Check for end of file on an open file 
 * 
 * @details Wrapper function for the FATFS macro function f_eof. 
 *          
 *          f_eof is a macro function that checks the file read/write pointer against the 
 *          file object size to determine if the read/write pointer is at the end of the 
 *          open file. If at the end of the file then this function will return a non-zero 
 *          value and zero otherwise. 
 * 
 * @return SD_EOF : end of file status 
 */
SD_EOF sd_eof(void); 

//=======================================================================================

#ifdef __cplusplus
}
#endif

#endif   // _SD_CARD_CONTROLLER_H_
