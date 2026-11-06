/*
 * app/nvic.c - Interrupt controller
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * NVIC_SetPriorityGrouping
 *   Write SCB_AIRCR PRIGROUP with VECTKEY (0x05FA)
 *   address 0x0800CD20, 10 bytes
 *   called from: usb_interrupt_config, swim_init
 *   prototype: void NVIC_SetPriorityGrouping(uint group)
 */
/* Write SCB_AIRCR PRIGROUP with VECTKEY (0x05FA) */

void NVIC_SetPriorityGrouping(uint param_1)

{
  SCB_AIRCR = param_1 | 0x5fa0000;
  return;
}

/**
 * NVIC_Init
 *   ST NVIC_Init: program interrupt priority (IPR) and enable (ISER) or disable from an init struct
 *   address 0x0800CD2A, 128 bytes
 *   called from: swim_deinit, usart_bridge_init, usb_interrupt_config, swd_connect, swim_init
 *   prototype: void NVIC_Init(byte *init)
 */
/* ST NVIC_Init: program interrupt priority (IPR) and enable (ISER) or disable from an init struct
    */

void NVIC_Init(byte *param_1)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  
  uVar2 = (uint)*param_1;
  if (param_1[3] != 0) {
    uVar1 = SCB_AIRCR;
    uVar1 = 0x700 - (uVar1 & 0x700) >> 8;
    iVar3 = (uVar2 & 3) << 3;
    uVar4 = 0xff << iVar3;
    *(uint *)(&DAT_e000e400 + (uVar2 & 0xfffffffc)) =
         ((0xfU >> (uVar1 & 0xff) & (uint)param_1[2] | (uint)param_1[1] << (4 - uVar1 & 0xff)) << 4)
         << iVar3 & uVar4 | *(uint *)(&DAT_e000e400 + (uVar2 & 0xfffffffc)) & ~uVar4;
    (&DAT_e000e100)[*param_1 >> 5] = 1 << (*param_1 & 0x1f);
    return;
  }
  (&DAT_e000e180)[*param_1 >> 5] = 1 << (uVar2 & 0x1f);
  return;
}

/**
 * __disable_irq
 *   Disable interrupts (CPSID i)
 *   address 0x0800CDAA, 4 bytes
 *   called from: swim_read_bits, swim_scheduler_set_mode, usb_interrupt_config, stlink_cmd_process, swim_command_dispatch
 *   prototype: void __disable_irq(void)
 */
/* Disable interrupts (CPSID i) */

void __disable_irq(void)

{
  disableIRQinterrupts();
  return;
}

/**
 * __enable_irq
 *   Enable interrupts (CPSIE i)
 *   address 0x0800CDAE, 4 bytes
 *   called from: swim_read_bits, swim_scheduler_set_mode, usb_interrupt_config, stlink_cmd_process, swim_command_dispatch
 *   prototype: void __enable_irq(void)
 */
/* Enable interrupts (CPSIE i) */

void __enable_irq(void)

{
  enableIRQinterrupts();
  return;
}

/**
 * set_basepri
 *   Set BASEPRI (priority mask) if currently privileged
 *   address 0x0800CDB2, 12 bytes
 *   called from: APP_main
 *   prototype: int set_basepri(int prio)
 */
/* Set BASEPRI (priority mask) if currently privileged */

int set_basepri(int param_1)

{
  bool bVar1;
  
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    setBasePriority(param_1 << 4);
  }
  return param_1 << 4;
}

/**
 * NVIC_TriggerSWInterrupt
 *   Software-trigger an interrupt via NVIC STIR (0xE000EF00) - used to kick SWIM EXTI0 state machine
 *   address 0x0800CDB8, 6 bytes
 *   called from: swim_state_write_advance, swim_dma_ch1_isr, _uncertain_swim_state_wait_sync, swim_dma_ch7_err_isr, swim_abort_to_idle
 *   prototype: void NVIC_TriggerSWInterrupt(undefined4 irqn)
 */
/* Software-trigger an interrupt via NVIC STIR (0xE000EF00) - used to kick SWIM EXTI0 state machine
    */

void NVIC_TriggerSWInterrupt(undefined4 param_1)

{
  DAT_e000ef00 = param_1;
  return;
}

/**
 * NVIC_ClearPendingIRQ
 *   Clear a pending interrupt via NVIC ICPR (0xE000E280)
 *   address 0x0800CDBE, 18 bytes
 *   called from: APP_TIM1_CC_IRQHandler, swim_rx_complete_dispatch, usb_interrupt_config
 *   prototype: void NVIC_ClearPendingIRQ(uint irqn)
 */
/* Clear a pending interrupt via NVIC ICPR (0xE000E280) */

void NVIC_ClearPendingIRQ(uint param_1)

{
  *(int *)(&DAT_e000e280 + (param_1 >> 5) * 4) = 1 << (param_1 & 0x1f);
  return;
}

/**
 * NVIC_SetVectorTable
 *   Write SCB_VTOR (vector table offset)
 *   address 0x0800CDD0, 12 bytes
 *   called from: usb_interrupt_config
 *   prototype: void NVIC_SetVectorTable(uint base,uint off)
 */
/* Write SCB_VTOR (vector table offset) */

void NVIC_SetVectorTable(uint param_1,uint param_2)

{
  SCB_VTOR = param_1 | param_2 & 0x1fffff80;
  return;
}

/**
 * nvic_enable_irq
 *   Thin wrapper around the CPSIE i intrinsic (enable global IRQ)
 *   address 0x0800D09A, 4 bytes
 *   prototype: void nvic_enable_irq(void)
 */
/* Thin wrapper around the CPSIE i intrinsic (enable global IRQ) */

void nvic_enable_irq(void)

{
  enableIRQinterrupts();
  return;
}

/**
 * nvic_disable_irq
 *   Thin wrapper around the CPSID i intrinsic (disable global IRQ)
 *   address 0x0800D09E, 4 bytes
 *   prototype: void nvic_disable_irq(void)
 */
/* Thin wrapper around the CPSID i intrinsic (disable global IRQ) */

void nvic_disable_irq(void)

{
  disableIRQinterrupts();
  return;
}

/**
 * APP_WWDG_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E084, 2 bytes
 *   prototype: void APP_WWDG_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_WWDG_IRQHandler(void)

{
  return;
}

/**
 * APP_PVD_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E086, 2 bytes
 *   prototype: void APP_PVD_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_PVD_IRQHandler(void)

{
  return;
}

/**
 * APP_TAMPER_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E088, 2 bytes
 *   prototype: void APP_TAMPER_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_TAMPER_IRQHandler(void)

{
  return;
}

/**
 * APP_RTC_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E08A, 2 bytes
 *   prototype: void APP_RTC_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_RTC_IRQHandler(void)

{
  return;
}

/**
 * APP_FLASH_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E08C, 2 bytes
 *   prototype: void APP_FLASH_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_FLASH_IRQHandler(void)

{
  return;
}

/**
 * APP_RCC_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E08E, 2 bytes
 *   prototype: void APP_RCC_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_RCC_IRQHandler(void)

{
  return;
}

/**
 * APP_EXTI0_IRQHandler
 *   EXTI line0 handler: core SWIM/debug bit-timing state machine; clears EXTI, advances DAT_200011b5 state, resets timers and dispatches via sub_0800C808
 *   address 0x0800E090, 4 bytes
 *   prototype: void APP_EXTI0_IRQHandler(void)
 */
/* EXTI line0 handler: core SWIM/debug bit-timing state machine; clears EXTI, advances DAT_200011b5
   state, resets timers and dispatches via sub_0800C808 */

void APP_EXTI0_IRQHandler(void)

{
  ushort uVar1;
  undefined4 uVar2;
  
  swim_captured_period = 0;
  DAT_e000e100 = 0x100000;
  uVar1 = (ushort)swim_status;
  if (uVar1 == 4) {
    uVar2 = 4;
    uVar1 = swim_byte_index;
  }
  else if (uVar1 == 5) {
    if (swim_retry_count_a < 2) {
      swim_retry_count_a = swim_retry_count_a + 1;
      swim_status = 1;
      return;
    }
    uVar2 = 5;
    uVar1 = swim_byte_index;
  }
  else if (uVar1 == 6) {
    if (swim_retry_count_b < 2) {
      swim_status = 1;
      swim_captured_period = 0;
      swim_retry_count_b = swim_retry_count_b + 1;
      return;
    }
    uVar2 = 6;
    uVar1 = swim_byte_index;
  }
  else if (uVar1 == 7) {
    uVar2 = 7;
    uVar1 = swim_byte_index;
  }
  else {
    if (uVar1 == 9) {
      uVar2 = 9;
    }
    else {
      if (uVar1 != 0x10) {
        uVar2 = 10;
        goto LAB_0800a99e;
      }
      uVar2 = 0x10;
    }
    uVar1 = 0;
  }
LAB_0800a99e:
  NVIC_ClearPendingIRQ(6);
  DAT_40000800 = 0;
  DAT_40000000 = 0;
  DAT_4000001c = 0x5000;
  SysTick_CounterCmd(0xfffffffe);
  swim_state_fn = &DAT_08009d41;
  swim_status = 0;
  swim_scheduler_set_mode(uVar2,uVar1);
  return;
}

/**
 * APP_EXTI1_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E094, 2 bytes
 *   prototype: void APP_EXTI1_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_EXTI1_IRQHandler(void)

{
  return;
}

/**
 * APP_EXTI2_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E096, 2 bytes
 *   prototype: void APP_EXTI2_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_EXTI2_IRQHandler(void)

{
  return;
}

/**
 * APP_EXTI3_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E098, 2 bytes
 *   prototype: void APP_EXTI3_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_EXTI3_IRQHandler(void)

{
  return;
}

/**
 * APP_EXTI4_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E09A, 2 bytes
 *   prototype: void APP_EXTI4_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_EXTI4_IRQHandler(void)

{
  return;
}

/**
 * APP_EXTI9_5_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E0DC, 2 bytes
 *   prototype: void APP_EXTI9_5_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_EXTI9_5_IRQHandler(void)

{
  return;
}

/**
 * APP_I2C1_EV_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E178, 2 bytes
 *   prototype: void APP_I2C1_EV_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_I2C1_EV_IRQHandler(void)

{
  return;
}

/**
 * APP_I2C1_ER_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E17A, 2 bytes
 *   prototype: void APP_I2C1_ER_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_I2C1_ER_IRQHandler(void)

{
  return;
}

/**
 * APP_I2C2_EV_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E17C, 2 bytes
 *   prototype: void APP_I2C2_EV_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_I2C2_EV_IRQHandler(void)

{
  return;
}

/**
 * APP_I2C2_ER_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E17E, 2 bytes
 *   prototype: void APP_I2C2_ER_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_I2C2_ER_IRQHandler(void)

{
  return;
}

/**
 * APP_SPI1_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E180, 2 bytes
 *   prototype: void APP_SPI1_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_SPI1_IRQHandler(void)

{
  return;
}

/**
 * APP_SPI2_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E182, 2 bytes
 *   prototype: void APP_SPI2_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_SPI2_IRQHandler(void)

{
  return;
}

/**
 * APP_USART1_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E184, 2 bytes
 *   prototype: void APP_USART1_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_USART1_IRQHandler(void)

{
  return;
}

/**
 * APP_USART2_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E186, 2 bytes
 *   prototype: void APP_USART2_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_USART2_IRQHandler(void)

{
  return;
}

/**
 * APP_USART3_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E188, 2 bytes
 *   prototype: void APP_USART3_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_USART3_IRQHandler(void)

{
  return;
}

/**
 * APP_EXTI15_10_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E18A, 2 bytes
 *   prototype: void APP_EXTI15_10_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_EXTI15_10_IRQHandler(void)

{
  return;
}

/**
 * APP_RTC_Alarm_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E18C, 2 bytes
 *   prototype: void APP_RTC_Alarm_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_RTC_Alarm_IRQHandler(void)

{
  return;
}

/**
 * APP_USBWakeUp_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E18E, 2 bytes
 *   prototype: void APP_USBWakeUp_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_USBWakeUp_IRQHandler(void)

{
  return;
}

/**
 * APP_NMI_Handler
 *   NMI exception handler: infinite loop
 *   address 0x0800E460, 4 bytes
 *   prototype: void APP_NMI_Handler(void)
 */
/* NMI exception handler: infinite loop */

void APP_NMI_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/**
 * APP_HardFault_Handler
 *   HardFault exception handler: infinite loop
 *   address 0x0800E464, 4 bytes
 *   prototype: void APP_HardFault_Handler(void)
 */
/* HardFault exception handler: infinite loop */

void APP_HardFault_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/**
 * APP_MemManage_Handler
 *   MemManage exception handler: infinite loop
 *   address 0x0800E468, 4 bytes
 *   prototype: void APP_MemManage_Handler(void)
 */
/* MemManage exception handler: infinite loop */

void APP_MemManage_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/**
 * APP_BusFault_Handler
 *   BusFault exception handler: infinite loop
 *   address 0x0800E46C, 4 bytes
 *   prototype: void APP_BusFault_Handler(void)
 */
/* BusFault exception handler: infinite loop */

void APP_BusFault_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/**
 * APP_UsageFault_Handler
 *   UsageFault exception handler: infinite loop
 *   address 0x0800E470, 4 bytes
 *   prototype: void APP_UsageFault_Handler(void)
 */
/* UsageFault exception handler: infinite loop */

void APP_UsageFault_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/**
 * APP_SVC_Handler
 *   SVCall exception handler: infinite loop
 *   address 0x0800E474, 4 bytes
 *   prototype: void APP_SVC_Handler(void)
 */
/* SVCall exception handler: infinite loop */

void APP_SVC_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/**
 * APP_DebugMon_Handler
 *   DebugMonitor exception handler: infinite loop
 *   address 0x0800E478, 4 bytes
 *   prototype: void APP_DebugMon_Handler(void)
 */
/* DebugMonitor exception handler: infinite loop */

void APP_DebugMon_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/**
 * APP_PendSV_Handler
 *   PendSV exception handler: infinite loop
 *   address 0x0800E47C, 4 bytes
 *   prototype: void APP_PendSV_Handler(void)
 */
/* PendSV exception handler: infinite loop */

void APP_PendSV_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/**
 * APP_ADC1_2_IRQHandler
 *   Unused ADC1_2 IRQ: infinite loop
 *   address 0x0800E480, 4 bytes
 *   prototype: void APP_ADC1_2_IRQHandler(void)
 */
/* Unused ADC1_2 IRQ: infinite loop */

void APP_ADC1_2_IRQHandler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/**
 * APP_CAN1_RX1_IRQHandler
 *   Unused CAN1_RX1 IRQ: infinite loop
 *   address 0x0800E484, 4 bytes
 *   prototype: void APP_CAN1_RX1_IRQHandler(void)
 */
/* Unused CAN1_RX1 IRQ: infinite loop */

void APP_CAN1_RX1_IRQHandler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/**
 * APP_CAN1_SCE_IRQHandler
 *   Unused CAN1_SCE IRQ: infinite loop
 *   address 0x0800E488, 4 bytes
 *   prototype: void APP_CAN1_SCE_IRQHandler(void)
 */
/* Unused CAN1_SCE IRQ: infinite loop */

void APP_CAN1_SCE_IRQHandler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

