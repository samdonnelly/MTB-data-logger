/**
 * @file fatfs_controller_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief FATFS controller mock 
 * 
 * @version 0.1
 * @date 2024-10-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "fatfs_controller.h" 
#include "fatfs_controller_mock.h" 

//=======================================================================================


//=======================================================================================
// Macros 

#define FATFS_MOCK_NUM_STRS 10 

//=======================================================================================


//=======================================================================================
// Mock data 

typedef struct fatfs_controller_mock_data_s 
{
    uint8_t write_index; 
    uint8_t read_index; 
    char data_buff[FATFS_MOCK_NUM_STRS][FATFS_MOCK_STR_SIZE]; 
}
fatfs_controller_mock_data_t; 

fatfs_controller_mock_data_t mock_data; 

//=======================================================================================


//=======================================================================================
// Controller functions 

// FATFS controller initialization 
void fatfs_controller_init(const char *path)
{
    // 
}


// FATFS controller 
void fatfs_controller(void)
{
    // 
}


// Set the check flag 
void fatfs_set_check_flag(void)
{
    // 
}


// Clear the check flag 
void fatfs_clear_check_flag(void)
{
    // 
}


// Set the eject flag 
void fatfs_set_eject_flag(void)
{
    // 
}


// Clear the eject flag 
void fatfs_clear_eject_flag(void)
{
    // 
}


// Set reset flag 
void fatfs_set_reset_flag(void)
{
    // 
}


// Set directory 
void fatfs_set_dir(const TCHAR *dir)
{
    if (dir == NULL)
    {
        return; 
    }
}


// Make a new directory in the project directory 
FRESULT fatfs_mkdir(const TCHAR *dir)
{
    return FR_OK; 
}


// Open a file 
FRESULT fatfs_open(
    const TCHAR *file_name, 
    uint8_t mode)
{
    if (file_name == NULL)
    {
        return FR_INVALID_OBJECT; 
    }

    return FR_OK; 
}


// Close an open file 
FRESULT fatfs_close(void)
{
    return FR_OK; 
}


// Write data to the open file 
FRESULT fatfs_f_write(
    const void *buff, 
    UINT btw)
{
    return FR_OK; 
}


// Write a string to the open file 
int16_t fatfs_puts(const TCHAR *str)
{
    if (str == NULL)
    {
        return FALSE; 
    }

    // The data logging module will write multiple lines at a time while logging data 
    // so instead of doing a memcpy of "str", the data is manually copied until the 
    // end of a line is seen. This will allow each line of a single data log write 
    // to be separated and looked at individually. 

    while ((*str != NULL_CHAR) && (mock_data.write_index < FATFS_MOCK_NUM_STRS))
    {
        int16_t index = CLEAR; 

        while (*str != NL_CHAR)
        {
            mock_data.data_buff[mock_data.write_index][index++] = *str++; 
        }

        mock_data.data_buff[mock_data.write_index++][index] = *str++; 
    }

    return TRUE; 
}


// Write a formatted string to the open file 
int8_t fatfs_printf(
    const TCHAR *fmt_str, 
    uint16_t fmt_value)
{
    return 0; 
}


// Select read/write pointer within an open file 
FRESULT fatfs_lseek(FSIZE_t offset)
{
    return FR_OK; 
}


// Delete a file 
FRESULT fatfs_unlink(const TCHAR* filename)
{
    return FR_OK; 
}


// Get controller state 
FATFS_STATE fatfs_get_state(void)
{
    return FATFS_INIT_STATE; 
}


// Get fault code 
FATFS_FAULT_CODE fatfs_get_fault_code(void)
{
    return 0; 
}


// Get fault mode 
FATFS_FAULT_MODE fatfs_get_fault_mode(void)
{
    return 0; 
}


// Get open file flag 
FATFS_FILE_STATUS fatfs_get_file_status(void)
{
    return 0; 
}


// Check for the existance of a file or directory 
FRESULT fatfs_get_exists(const TCHAR *str)
{
    return FR_OK; 
}


// Read data from an open file 
FRESULT fatfs_f_read(
    void *buff, 
    UINT btr)
{
    return FR_OK; 
}


// Reads a string from an open file 
TCHAR* fatfs_gets(
    TCHAR *buff, 
    uint16_t len)
{
    return NULL; 
}


// Check for end of file on an open file 
FATFS_EOF fatfs_eof(void)
{
    return 0; 
}

//=======================================================================================


//=======================================================================================
// Mock functions 

// FATFS Controller Mock: Init 
void fatfs_controller_mock_init(void)
{
    mock_data.write_index = CLEAR; 
    mock_data.read_index = CLEAR; 
    memset((void *)mock_data.data_buff, CLEAR, sizeof(mock_data.data_buff)); 
}


// FATFS Controller Mock: Get String 
void fatfs_controller_mock_get_str(
    char *buff, 
    uint8_t buff_len)
{
    if ((buff != NULL) && 
        (buff_len <= FATFS_MOCK_STR_SIZE) && 
        (mock_data.read_index < FATFS_MOCK_NUM_STRS))
    {
        memcpy((void *)buff, 
               (void *)mock_data.data_buff[mock_data.read_index++], 
               (size_t)buff_len); 
    }

}

//=======================================================================================
