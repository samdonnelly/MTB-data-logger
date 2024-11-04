/**
 * @file hw125_controller_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief HW125 controller mock 
 * 
 * @version 0.1
 * @date 2024-10-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "hw125_controller.h" 
#include "hw125_controller_mock.h" 

//=======================================================================================


//=======================================================================================
// Macros 

#define HW125_MOCK_NUM_STRS 10 

//=======================================================================================


//=======================================================================================
// Mock data 

typedef struct hw125_controller_mock_data_s 
{
    uint8_t write_index; 
    uint8_t read_index; 
    char data_buff[HW125_MOCK_NUM_STRS][HW125_MOCK_STR_SIZE]; 
}
hw125_controller_mock_data_t; 

hw125_controller_mock_data_t mock_data; 

//=======================================================================================


//=======================================================================================
// Controller functions 

// HW125 controller initialization 
void hw125_controller_init(const char *path)
{
    // 
}


// HW125 controller 
void hw125_controller(void)
{
    // 
}


// Set the check flag 
void hw125_set_check_flag(void)
{
    // 
}


// Clear the check flag 
void hw125_clear_check_flag(void)
{
    // 
}


// Set the eject flag 
void hw125_set_eject_flag(void)
{
    // 
}


// Clear the eject flag 
void hw125_clear_eject_flag(void)
{
    // 
}


// Set reset flag 
void hw125_set_reset_flag(void)
{
    // 
}


// Set directory 
void hw125_set_dir(const TCHAR *dir)
{
    if (dir == NULL)
    {
        return; 
    }
}


// Make a new directory in the project directory 
FRESULT hw125_mkdir(const TCHAR *dir)
{
    return FR_OK; 
}


// Open a file 
FRESULT hw125_open(
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
FRESULT hw125_close(void)
{
    return FR_OK; 
}


// Write data to the open file 
FRESULT hw125_f_write(
    const void *buff, 
    UINT btw)
{
    return FR_OK; 
}


// Write a string to the open file 
int16_t hw125_puts(const TCHAR *str)
{
    if (mock_data.write_index < HW125_MOCK_NUM_STRS)
    {
        memcpy((void *)mock_data.data_buff[mock_data.write_index++], 
               (void *)str, 
               HW125_MOCK_STR_SIZE); 
    }

    return 0; 
}


// Write a formatted string to the open file 
int8_t hw125_printf(
    const TCHAR *fmt_str, 
    uint16_t fmt_value)
{
    return 0; 
}


// Select read/write pointer within an open file 
FRESULT hw125_lseek(FSIZE_t offset)
{
    return FR_OK; 
}


// Delete a file 
FRESULT hw125_unlink(const TCHAR* filename)
{
    return FR_OK; 
}


// Get controller state 
HW125_STATE hw125_get_state(void)
{
    return HW125_INIT_STATE; 
}


// Get fault code 
HW125_FAULT_CODE hw125_get_fault_code(void)
{
    return 0; 
}


// Get fault mode 
HW125_FAULT_MODE hw125_get_fault_mode(void)
{
    return 0; 
}


// Get open file flag 
HW125_FILE_STATUS hw125_get_file_status(void)
{
    return 0; 
}


// Check for the existance of a file or directory 
FRESULT hw125_get_exists(const TCHAR *str)
{
    return FR_OK; 
}


// Read data from an open file 
FRESULT hw125_f_read(
    void *buff, 
    UINT btr)
{
    return FR_OK; 
}


// Reads a string from an open file 
TCHAR* hw125_gets(
    TCHAR *buff, 
    uint16_t len)
{
    return NULL; 
}


// Check for end of file on an open file 
HW125_EOF hw125_eof(void)
{
    return 0; 
}

//=======================================================================================


//=======================================================================================
// Mock functions 

// HW125 Controller Mock: Init 
void hw125_controller_mock_init(void)
{
    mock_data.write_index = CLEAR; 
    mock_data.read_index = CLEAR; 
    memset((void *)mock_data.data_buff, CLEAR, sizeof(mock_data.data_buff)); 
}


// HW125 Controller Mock: Get String 
void hw125_controller_mock_get_str(
    char *buff, 
    uint8_t buff_len)
{
    if ((buff != NULL) && 
        (buff_len <= HW125_MOCK_STR_SIZE) && 
        (mock_data.read_index < HW125_MOCK_NUM_STRS))
    {
        memcpy((void *)buff, 
               (void *)mock_data.data_buff[mock_data.read_index++], 
               (size_t)buff_len); 
    }

}

//=======================================================================================
