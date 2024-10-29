/**
 * @file adc_driver_mock.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Analog driver mock 
 * 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Include 

#include "analog_driver_mock.h" 
#include "analog_driver.h" 

//=======================================================================================


//=======================================================================================
// Driver functions 

// ADC1 clock enable 
ADC_STATUS adc1_clock_enable(RCC_TypeDef *rcc)
{
    if (rcc == NULL)
    {
        return ADC_INVALID_PTR; 
    }

    return ADC_OK; 
}


// ADC port init - called once for each ADC port used 
ADC_STATUS adc_port_init(
    ADC_TypeDef *adc, 
    ADC_Common_TypeDef *adc_common, 
    adc_prescalar_t prescalar, 
    adc_res_t resolution, 
    adc_param_config_t eoc, 
    adc_param_config_t eocie, 
    adc_param_config_t scan, 
    adc_param_config_t cont, 
    adc_param_config_t dma, 
    adc_param_config_t dds, 
    adc_param_config_t ovrie)
{
    if ((adc == NULL) || (adc_common == NULL))
    {
        return ADC_INVALID_PTR; 
    }

    return ADC_OK; 
}


// ADC pin init - called for each ADC pin used 
ADC_STATUS adc_pin_init(
    ADC_TypeDef *adc, 
    GPIO_TypeDef *gpio, 
    pin_selector_t adc_pin, 
    adc_channel_t adc_channel, 
    adc_smp_cycles_t smp)
{
    if ((adc == NULL) || (gpio == NULL))
    {
        return ADC_INVALID_PTR; 
    }

    return ADC_OK; 
}


// ADC watchdog setup 
ADC_STATUS adc_wd_init(
    ADC_TypeDef *adc, 
    adc_param_config_t wd, 
    adc_param_config_t wdsc, 
    adc_channel_t channel, 
    uint16_t hi_thresh, 
    uint16_t lo_thresh, 
    adc_param_config_t awdie)
{
    if (adc == NULL)
    {
        return ADC_INVALID_PTR; 
    }

    return ADC_OK; 
}


// Channel sequence - called for each ADC input (pin) in the sequence 
ADC_STATUS adc_seq(
    ADC_TypeDef *adc, 
    adc_channel_t channel, 
    adc_seq_num_t seq_num)
{
    if (adc == NULL)
    {
        return ADC_INVALID_PTR; 
    }

    return ADC_OK; 
} 


// Regular channel sequence length setter - called once if a sequence is used 
ADC_STATUS adc_seq_len_set(
    ADC_TypeDef *adc, 
    adc_seq_num_t seq_len) 
{
    if (adc == NULL)
    {
        return ADC_INVALID_PTR; 
    }

    return ADC_OK; 
}


// Turn ADC on 
ADC_STATUS adc_on(ADC_TypeDef *adc)
{
    if (adc == NULL)
    {
        return ADC_INVALID_PTR; 
    }
    return ADC_OK; 
}


// Turn ADC off 
ADC_STATUS adc_off(ADC_TypeDef *adc)
{
    if (adc == NULL)
    {
        return ADC_INVALID_PTR; 
    }

    adc->CR2 &= ~(SET_BIT << SHIFT_0); 
    return ADC_OK; 
}


// Start an ADC conversion 
ADC_STATUS adc_start(ADC_TypeDef *adc)
{
    if (adc == NULL)
    {
        return ADC_INVALID_PTR; 
    }

    return ADC_OK; 
}


// Read a select single ADC conversion 
uint16_t adc_read_single(
    ADC_TypeDef *adc, 
    adc_channel_t channel)
{
    if (adc == NULL)
    {
        return ADC_INVALID_PTR; 
    }

    return NONE; 
}


// Scan all ADC conversion in the sequence 
ADC_STATUS adc_scan_seq(
    ADC_TypeDef *adc, 
    adc_seq_num_t seq_len, 
    uint16_t *adc_data)
{
    if (adc == NULL)
    {
        return ADC_INVALID_PTR; 
    }

    return ADC_OK; 
}

//=======================================================================================
