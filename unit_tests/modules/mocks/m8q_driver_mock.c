/**
 * @file m8q_driver_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief M8Q driver mock 
 * 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "m8q_driver.h" 
#include "m8q_driver_mock.h" 

//=======================================================================================


//=======================================================================================
// Global data 

typedef struct m8q_mock_data_s 
{
    // The M8Q data buffers are made 1-byte larger than their driver size because here 
    // they act as independent strings whereas in the driver the data is pullec from 
    // one larger string. 

    // Time info 
    uint8_t time [BYTE_9 + BYTE_1];    // UTC time 
    uint8_t date [BYTE_6 + BYTE_1];    // UTC date 

    // Position info 
    uint8_t lat [BYTE_11 + BYTE_1];    // Latitude 
    uint8_t NS  [BYTE_1 + BYTE_1];     // North/South indicator 
    uint8_t lon [BYTE_12 + BYTE_1];    // Longitude 
    uint8_t EW  [BYTE_1 + BYTE_1];     // East/West indicator 
    uint8_t SOG [BYTE_9 + BYTE_1];     // Speed over ground 
}
m8q_mock_data_t; 

m8q_mock_data_t m8q_mock_data; 

//=======================================================================================


//=======================================================================================
// Driver functions 

// Device initialization 
M8Q_STATUS m8q_init(
    I2C_TypeDef *i2c, 
    const char *config_msgs, 
    uint8_t msg_num, 
    uint8_t max_msg_size, 
    uint16_t data_buff_limit)
{
    if ((i2c == NULL) || (config_msgs == NULL))
    {
        return M8Q_INVALID_PTR; 
    }

    return M8Q_OK; 
}


// Low power pin initialization 
M8Q_STATUS m8q_pwr_pin_init(
    GPIO_TypeDef *gpio, 
    pin_selector_t pwr_save_pin)
{
    if (gpio == NULL)
    {
        return M8Q_INVALID_PTR; 
    }

    return M8Q_OK; 
}


// TX ready pin initialization 
M8Q_STATUS m8q_txr_pin_init(
    GPIO_TypeDef *gpio, 
    pin_selector_t tx_ready_pin)
{
    if (gpio == NULL)
    {
        return M8Q_INVALID_PTR; 
    }

    return M8Q_OK; 
}


// Read and store relevant message data 
M8Q_STATUS m8q_read_data(void)
{
    return M8Q_OK; 
}


// Read and return the data stream contents 
M8Q_STATUS m8q_read_ds(
    uint8_t *data_buff, 
    uint16_t buff_size)
{
    if (data_buff == NULL)
    {
        return M8Q_INVALID_PTR; 
    }

    return M8Q_OK; 
}


// Read data stream size 
M8Q_STATUS m8q_read_ds_size(uint16_t *data_size)
{
    if (data_size == NULL)
    {
        return M8Q_INVALID_PTR; 
    }

    return M8Q_OK; 
}


// Return the ACK/NAK message counter status 
uint16_t m8q_get_ack_status(void)
{
    return NONE; 
}


// Send a message to the device 
M8Q_STATUS m8q_send_msg(
    const char *write_msg, 
    uint8_t max_msg_size)
{
    if (write_msg == NULL)
    {
        return M8Q_INVALID_PTR; 
    }

    return M8Q_OK; 
}


// Get TX ready status 
GPIO_STATE m8q_get_tx_ready(void)
{
    return GPIO_HIGH; 
}


// Enter low power mode 
void m8q_set_low_pwr(void)
{
    // 
}


// Exit low power mode 
void m8q_clear_low_pwr(void)
{
    // 
}


// Get latitude coordinate 
double m8q_get_position_lat(void)
{
    return NONE; 
}


// Get latitude coordinate string 
M8Q_STATUS m8q_get_position_lat_str(
    uint8_t *lat_str, 
    uint8_t lat_str_len)
{
    if (lat_str == NULL)
    {
        return M8Q_INVALID_PTR; 
    }

    memcpy((void *)lat_str, (void *)m8q_mock_data.lat, BYTE_11); 
    return M8Q_OK; 
}


// Get North/South hemisphere 
uint8_t m8q_get_position_NS(void)
{
    return m8q_mock_data.NS[BYTE_0]; 
}


// Get longitude coordinate 
double m8q_get_position_lon(void)
{
    return NONE; 
}


// Get longitude coordinate string 
M8Q_STATUS m8q_get_position_lon_str(
    uint8_t *lon_str, 
    uint8_t lon_str_len)
{
    if (lon_str == NULL)
    {
        return M8Q_INVALID_PTR; 
    }

    memcpy((void *)lon_str, (void *)m8q_mock_data.lon, BYTE_12); 
    return M8Q_OK; 
}


// Get East/West hemisphere 
uint8_t m8q_get_position_EW(void)
{
    return m8q_mock_data.EW[BYTE_0]; 
}


// Get navigation status 
uint16_t m8q_get_position_navstat(void)
{
    return TRUE; 
}


// Get acceptable navigation status 
uint8_t m8q_get_position_navstat_lock(void)
{
    return TRUE; 
}


// Get speed over ground (SOG) value 
uint32_t m8q_get_position_sog(void)
{
    return NONE; 
}


// Get speed over ground (SOG) string 
M8Q_STATUS m8q_get_position_sog_str(
    uint8_t *sog_str, 
    uint8_t sog_str_len)
{
    if (sog_str == NULL)
    {
        return M8Q_INVALID_PTR; 
    }

    memcpy((void *)sog_str, (void *)m8q_mock_data.SOG, BYTE_9); 
    return M8Q_OK; 
}


// Get UTC time 
M8Q_STATUS m8q_get_time_utc_time(
    uint8_t *utc_time, 
    uint8_t utc_time_len)
{
    if (utc_time == NULL)
    {
        return M8Q_INVALID_PTR; 
    }

    memcpy((void *)utc_time, (void *)m8q_mock_data.time, BYTE_9); 
    return M8Q_OK; 
}


// Get UTC date 
M8Q_STATUS m8q_get_time_utc_date(
    uint8_t *utc_date, 
    uint8_t utc_date_len)
{
    if (utc_date == NULL)
    {
        return M8Q_INVALID_PTR; 
    }

    memcpy((void *)utc_date, (void *)m8q_mock_data.date, BYTE_6); 
    return M8Q_OK; 
}

//=======================================================================================


//=======================================================================================
// Mock functions 

// M8Q driver mock init 
void m8q_mock_init(void)
{
    memset((void *)m8q_mock_data.time, CLEAR, sizeof(m8q_mock_data.time)); 
    memset((void *)m8q_mock_data.date, CLEAR, sizeof(m8q_mock_data.date)); 
    memset((void *)m8q_mock_data.lat, CLEAR, sizeof(m8q_mock_data.lat)); 
    memset((void *)m8q_mock_data.NS, CLEAR, sizeof(m8q_mock_data.NS)); 
    memset((void *)m8q_mock_data.lon, CLEAR, sizeof(m8q_mock_data.lon)); 
    memset((void *)m8q_mock_data.EW, CLEAR, sizeof(m8q_mock_data.EW)); 
    memset((void *)m8q_mock_data.SOG, CLEAR, sizeof(m8q_mock_data.SOG)); 
}


// Set UTC time 
void m8q_mock_set_time_utc_time(
    char *time_str, 
    uint8_t time_str_len)
{
    if ((time_str == NULL) || (sizeof(m8q_mock_data.time) < time_str_len))
    {
        return; 
    }

    memcpy((void *)m8q_mock_data.time, (void *)time_str, time_str_len); 
}


// Set UTC date 
void m8q_mock_set_time_utc_date(
    char *date_str, 
    uint8_t date_str_len)
{
    if ((date_str == NULL) || (sizeof(m8q_mock_data.date) < date_str_len))
    {
        return; 
    }

    memcpy((void *)m8q_mock_data.date, (void *)date_str, date_str_len); 
}


// Set latitude string 
void m8q_mock_set_position_lat(
    char *lat_str, 
    uint8_t lat_str_len)
{
    if ((lat_str == NULL) || (sizeof(m8q_mock_data.lat) < lat_str_len))
    {
        return; 
    }

    memcpy((void *)m8q_mock_data.lat, (void *)lat_str, lat_str_len); 
}


// Set NS string 
void m8q_mock_set_position_ns(
    char *ns_str, 
    uint8_t ns_str_len)
{
    if ((ns_str == NULL) || (sizeof(m8q_mock_data.NS) < ns_str_len))
    {
        return; 
    }

    memcpy((void *)m8q_mock_data.NS, (void *)ns_str, ns_str_len); 
}


// Set longitude string 
void m8q_mock_set_position_lon(
    char *lon_str, 
    uint8_t lon_str_len)
{
    if ((lon_str == NULL) || (sizeof(m8q_mock_data.lon) < lon_str_len))
    {
        return; 
    }

    memcpy((void *)m8q_mock_data.lon, (void *)lon_str, lon_str_len); 
}


// Set EW string 
void m8q_mock_set_position_ew(
    char *ew_str, 
    uint8_t ew_str_len)
{
    if ((ew_str == NULL) || (sizeof(m8q_mock_data.EW) < ew_str_len))
    {
        return; 
    }

    memcpy((void *)m8q_mock_data.EW, (void *)ew_str, ew_str_len); 
}


// Set SOG string 
void m8q_mock_set_position_sog(
    char *sog_str, 
    uint8_t sog_str_len)
{
    if ((sog_str == NULL) || (sizeof(m8q_mock_data.SOG) < sog_str_len))
    {
        return; 
    }

    memcpy((void *)m8q_mock_data.SOG, (void *)sog_str, sog_str_len); 
}

//=======================================================================================
