/**
 * @file stm32f4xx_it.c
 * 
 * @author Sam Donnelly (samueldonnelly11@gmail.com)
 * 
 * @brief Interrupt Service Routines (ISRs) implementation 
 * 
 * @version 0.1
 * @date 2024-03-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//=======================================================================================
// Includes 

#include "stm32f4xx_it.h" 
#include "stm32f4xx_hal.h" 

#include "data_logging.h" 

//=======================================================================================


//=======================================================================================
// Globals 

// Instance of interrupt flag data record 
int_handle_flags_t handler_flags; 

//=======================================================================================


//=======================================================================================
// Initialization 

// Interrupt handler flag initialization 
void int_handler_init(void)
{
    // Clear all flags 
    memset((void *)&handler_flags, CLEAR, sizeof(int_handle_flags_t)); 
}

//=======================================================================================


//=======================================================================================
// Cortex-M4 Processor Interruption and Exception Handlers 

// This function handles Non maskable interrupt 
void NMI_Handler(void)
{
    while (1) {}
}


// This function handles Hard fault interrupt 
void HardFault_Handler(void)
{
    while (1) {}
}


// This function handles Memory management fault 
void MemManage_Handler(void)
{
    while (1) {}
}


// This function handles Pre-fetch fault, memory access fault 
void BusFault_Handler(void)
{
    while (1) {}
}


// This function handles Undefined instruction or illegal state 
void UsageFault_Handler(void)
{
    while (1) {}
}


// This function handles Debug monitor.
void DebugMon_Handler(void)
{
    // 
}


// This function handles System service call via SWI instruction 
void SVC_Handler(void)
{
    // 
}


// This function handles Pendable request for system service 
void PendSV_Handler(void)
{
    // 
}


// This function handles System tick timer.
void SysTick_Handler(void)
{
    HAL_IncTick();
}

//=======================================================================================


//=======================================================================================
// STM32F4xx Peripheral Interrupt Handlers 

// Interrupt handler names are defined in startup_stm32f411xe.s 

// EXTI Line 0 
void EXTI0_IRQHandler(void)
{
    handler_flags.exti0_flag = SET_BIT; 
    exti_pr_clear(EXTI_L0); 
}


// EXTI Line 1 
void EXTI1_IRQHandler(void)
{
    handler_flags.exti1_flag = SET_BIT; 
    exti_pr_clear(EXTI_L1);  
}


// EXTI Line 2 
void EXTI2_IRQHandler(void)
{
    handler_flags.exti2_flag = SET_BIT; 
    exti_pr_clear(EXTI_L2); 
}


// EXTI Line 3 
void EXTI3_IRQHandler(void)
{
    handler_flags.exti3_flag = SET_BIT; 
    exti_pr_clear(EXTI_L3); 
}


// EXTI Line 4 
void EXTI4_IRQHandler(void)
{
    handler_flags.exti4_flag = SET_BIT; 
    exti_pr_clear(EXTI_L4); 
}


// EXTI lines 5-9 
void EXTI9_5_IRQHandler(void)
{
    handler_flags.exti5_9_flag = SET_BIT; 
    exti_pr_clear(EXTI_L5 | EXTI_L6 | EXTI_L7 | EXTI_L8 | EXTI_L9); 
}


// EXTI lines 10-15 
void EXTI15_10_IRQHandler(void)
{
    handler_flags.exti10_15_flag = SET_BIT; 
    exti_pr_clear(EXTI_L10 | EXTI_L11 | EXTI_L12 | EXTI_L13 | EXTI_L14 | EXTI_L15); 
}


// DMA1 Stream 0 
void DMA1_Stream0_IRQHandler(void)
{
    handler_flags.dma1_0_flag = SET_BIT; 
    dma_clear_int_flags(DMA1); 
}


// DMA1 Stream 1 
void DMA1_Stream1_IRQHandler(void)
{
    handler_flags.dma1_1_flag = SET_BIT; 
    dma_clear_int_flags(DMA1); 
}


// DMA1 Stream 2 
void DMA1_Stream2_IRQHandler(void)
{
    handler_flags.dma1_2_flag = SET_BIT; 
    dma_clear_int_flags(DMA1); 
}


// DMA1 Stream 3 
void DMA1_Stream3_IRQHandler(void)
{
    handler_flags.dma1_3_flag = SET_BIT; 
    dma_clear_int_flags(DMA1); 
}


// DMA1 Stream 4 
void DMA1_Stream4_IRQHandler(void)
{
    handler_flags.dma1_4_flag = SET_BIT; 
    dma_clear_int_flags(DMA1); 
}


// DMA1 Stream 5 
void DMA1_Stream5_IRQHandler(void)
{
    handler_flags.dma1_5_flag = SET_BIT; 
    dma_clear_int_flags(DMA1); 
}


// DMA1 Stream 6 
void DMA1_Stream6_IRQHandler(void)
{
    handler_flags.dma1_6_flag = SET_BIT; 
    dma_clear_int_flags(DMA1); 
}


// DMA1 Stream 7 
void DMA1_Stream7_IRQHandler(void)
{
    handler_flags.dma1_7_flag = SET_BIT; 
    dma_clear_int_flags(DMA1); 
}


// DMA2 Stream 0 
void DMA2_Stream0_IRQHandler(void)
{
    handler_flags.dma2_0_flag = SET_BIT; 
    dma_clear_int_flags(DMA2); 
}


// DMA2 Stream 1 
void DMA2_Stream1_IRQHandler(void)
{
    handler_flags.dma2_1_flag = SET_BIT; 
    dma_clear_int_flags(DMA2); 
}


// DMA2 Stream 2 
void DMA2_Stream2_IRQHandler(void)
{
    handler_flags.dma2_2_flag = SET_BIT; 
    dma_clear_int_flags(DMA2); 
}


// DMA2 Stream 3 
void DMA2_Stream3_IRQHandler(void)
{
    handler_flags.dma2_3_flag = SET_BIT; 
    dma_clear_int_flags(DMA2); 
}


// DMA2 Stream 4 
void DMA2_Stream4_IRQHandler(void)
{
    handler_flags.dma2_4_flag = SET_BIT; 
    dma_clear_int_flags(DMA2); 
}


// DMA2 Stream 5 
void DMA2_Stream5_IRQHandler(void)
{
    handler_flags.dma2_5_flag = SET_BIT; 
    dma_clear_int_flags(DMA2); 
}


// DMA2 Stream 6 
void DMA2_Stream6_IRQHandler(void)
{
    handler_flags.dma2_6_flag = SET_BIT; 
    dma_clear_int_flags(DMA2); 
}


// DMA2 Stream 7 
void DMA2_Stream7_IRQHandler(void)
{
    handler_flags.dma2_7_flag = SET_BIT; 
    dma_clear_int_flags(DMA2); 
}


// Timer 1 break + timer 9 global 
__weak void TIM1_BRK_TIM9_IRQHandler(void)
{
    handler_flags.tim1_brk_tim9_glbl_flag = SET_BIT; 
    tim_uif_clear(TIM1); 
    tim_uif_clear(TIM9); 
}


// Timer 1 update + timer 10 global 
void TIM1_UP_TIM10_IRQHandler(void)
{
    handler_flags.tim1_up_tim10_glbl_flag = SET_BIT; 
    tim_uif_clear(TIM1); 
    tim_uif_clear(TIM10); 
}


// Timer 1 trigger and communication + timer 11 global interrupts 
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
    handler_flags.tim1_trg_tim11_glbl_flag = SET_BIT; 

    // Start the ADC conversion so new data is available for each data logging interval. 
    log_data_adc_handler(); 

    tim_uif_clear(TIM1); 
    tim_uif_clear(TIM11); 
}


// Timer 1 capture compare 
void TIM1_CC_IRQHandler(void)
{
    handler_flags.tim1_cc_flag = SET_BIT; 
    tim_uif_clear(TIM1); 
}


// Timer 2 
void TIM2_IRQHandler(void)
{
    handler_flags.tim2_glbl_flag = SET_BIT; 
    tim_uif_clear(TIM2); 
}


// Timer 3
void TIM3_IRQHandler(void)
{
    handler_flags.tim3_glbl_flag = SET_BIT; 
    tim_uif_clear(TIM3); 
}


// Timer 4
void TIM4_IRQHandler(void)
{
    handler_flags.tim4_glbl_flag = SET_BIT; 
    tim_uif_clear(TIM4); 
}


// Timer 5
void TIM5_IRQHandler(void)
{
    handler_flags.tim5_glbl_flag = SET_BIT; 
    tim_uif_clear(TIM5); 
}


// ADC1 
void ADC_IRQHandler(void)
{
    handler_flags.adc_flag = SET_BIT;  
}


// USART1 
void USART1_IRQHandler(void)
{
    handler_flags.usart1_flag = SET_BIT; 
    dummy_read(USART1->SR); 
    dummy_read(USART1->DR); 
}


// USART2 
void USART2_IRQHandler(void)
{
    handler_flags.usart2_flag = SET_BIT; 
    dummy_read(USART2->SR); 
    dummy_read(USART2->DR); 
}


// USART6 
void USART6_IRQHandler(void)
{
    handler_flags.usart6_flag = SET_BIT; 
    dummy_read(USART6->SR); 
    dummy_read(USART6->DR); 
}

//=======================================================================================
