/**
 * @file dma_driver_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief DMA driver mock 
 * 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "dma_driver_mock.h" 
#include "dma_driver.h" 

//=======================================================================================


//=======================================================================================
// Driver functions 

// Initialize the DMA stream 
void dma_stream_init(
    DMA_TypeDef *dma, 
    DMA_Stream_TypeDef *dma_stream, 
    dma_channel_t channel, 
    dma_direction_t dir, 
    dma_cm_t cm, 
    dma_priority_t priority, 
    dma_dbm_t dbm, 
    dma_addr_inc_mode_t minc, 
    dma_addr_inc_mode_t pinc, 
    dma_data_size_t msize, 
    dma_data_size_t psize)
{
    // 
} 


// Configure the DMA stream 
void dma_stream_config(
    DMA_Stream_TypeDef *dma_stream, 
    uint32_t per_addr, 
    uint32_t mem0_addr, 
    uint32_t mem1_addr, 
    uint16_t data_items)
{
    // 
}


// Configure the FIFO usage 
void dma_fifo_config(
    DMA_Stream_TypeDef *dma_stream, 
    dma_fifo_mode_t mode, 
    dma_fifo_threshold_t fth, 
    dma_feie_t feie)
{
    // 
}


// Configure the DMA interrupts 
void dma_int_config(
    DMA_Stream_TypeDef *dma_stream,  
    dma_tcie_t tcie, 
    dma_htie_t htie, 
    dma_teie_t teie, 
    dma_dmeie_t dmeie)
{
    // 
}


// Clear all the stream interrupt flags LISR and HISR registers 
void dma_clear_int_flags(DMA_TypeDef *dma)
{
    // 
}


// Read the stream interrupt flags 
void dma_int_flags(
    DMA_TypeDef *dma, 
    uint32_t *lo_streams, 
    uint32_t *hi_streams)
{
    // 
}


// Get the transfer complete status 
uint8_t dma_get_tc_status(
    DMA_TypeDef *dma, 
    DMA_Stream_TypeDef* dma_stream)
{
    return TRUE; 
}

//=======================================================================================


//=======================================================================================
// Mock functions 
//=======================================================================================
