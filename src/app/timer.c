/*
 * app/timer.c - Timers
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * timer_delay_ticks
 *   Busy-wait until the free-running counter (sub_0800BEB8) advances by param_1 ticks.
 *   address 0x08009400, 22 bytes
 *   called from: APP_main, _uncertain_stlink_mode_enter, _uncertain_stlink_target_mode_apply, target_drive_nrst
 *   prototype: undefined4 timer_delay_ticks(uint ticks)
 */
/* Busy-wait until the free-running counter (sub_0800BEB8) advances by param_1 ticks. */

undefined4 timer_delay_ticks(uint param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  
  iVar1 = RTC_GetCounter();
  do {
    iVar2 = RTC_GetCounter();
  } while ((uint)(iVar2 - iVar1) < param_1);
  return param_4;
}

/**
 * TIM1_reset
 *   Pulse TIM1 reset via APB2RSTR bit (0x800)
 *   address 0x0800C524, 26 bytes
 *   called from: led_tim1_init
 *   prototype: void TIM1_reset(void)
 */
/* Pulse TIM1 reset via APB2RSTR bit (0x800) */

void TIM1_reset(void)

{
  undefined4 unaff_r7;
  
  RCC_APB2PeriphResetCmd(0x800,1);
  RCC_APB2PeriphResetCmd(0x800,0,unaff_r7);
  return;
}

/**
 * TIM_OCStructInit
 *   Zero an 8-halfword TIM output-compare init struct
 *   address 0x0800C580, 20 bytes
 *   called from: led_tim1_init
 *   prototype: void TIM_OCStructInit(undefined2 *oc)
 */
/* Zero an 8-halfword TIM output-compare init struct */

void TIM_OCStructInit(undefined2 *param_1)

{
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  return;
}

/**
 * TIM_DeInit
 *   Reset TIM2/TIM3/TIM4 via RCC_APB1PeriphResetCmd based on timer base
 *   address 0x0800CE3C, 70 bytes
 *   called from: swim_deinit, swim_init
 *   prototype: undefined4 TIM_DeInit(undefined2 *tim)
 */
/* Reset TIM2/TIM3/TIM4 via RCC_APB1PeriphResetCmd based on timer base */

undefined4 TIM_DeInit(undefined2 *param_1)

{
  undefined4 uVar1;
  undefined4 unaff_r7;
  
  if (param_1 == (undefined2 *)0x40000000) {
    RCC_APB1PeriphResetCmd(1,1);
    uVar1 = 1;
  }
  else if (param_1 == (undefined2 *)&UNK_40000400) {
    RCC_APB1PeriphResetCmd(2,1);
    uVar1 = 2;
  }
  else {
    if (param_1 != &DAT_40000800) {
      return unaff_r7;
    }
    RCC_APB1PeriphResetCmd(4,1);
    uVar1 = 4;
  }
  uVar1 = RCC_APB1PeriphResetCmd(uVar1,0,unaff_r7);
  return uVar1;
}

/**
 * TIM_TimeBaseInit
 *   ST TIM_TimeBaseInit: program ARR (reg+0x2C), PSC (reg+0x28), CR1 (clock div/CMS) for SWIM TIM2/TIM4
 *   address 0x0800CE8C, 32 bytes
 *   called from: swim_init
 *   prototype: void TIM_TimeBaseInit(ushort *tim,ushort *init)
 */
/* ST TIM_TimeBaseInit: program ARR (reg+0x2C), PSC (reg+0x28), CR1 (clock div/CMS) for SWIM
   TIM2/TIM4 */

void TIM_TimeBaseInit(ushort *param_1,ushort *param_2)

{
  param_1[0x16] = *param_2;
  param_1[0x14] = param_2[1];
  *param_1 = *param_1 & 0x8f;
  *param_1 = param_2[3] | param_2[2] | *param_1;
  return;
}

/**
 * APP_TIM1_BRK_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E0DE, 2 bytes
 *   prototype: void APP_TIM1_BRK_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_TIM1_BRK_IRQHandler(void)

{
  return;
}

/**
 * APP_TIM1_UP_IRQHandler
 *   TIM1 update ISR: clears UIF, on active flag runs SWIM bit generator and schedules next task phase
 *   address 0x0800E0E0, 54 bytes
 *   prototype: void APP_TIM1_UP_IRQHandler(void)
 */
/* TIM1 update ISR: clears UIF, on active flag runs SWIM bit generator and schedules next task phase
    */

undefined4 APP_TIM1_UP_IRQHandler(void)

{
  ushort uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 unaff_r7;
  
  uVar1 = DAT_40012c10;
  DAT_40012c10 = uVar1 & 0xfffe;
  if (DAT_2000150c == '\x01') {
    iVar2 = led_get_result();
    if (iVar2 == 1) {
      uVar3 = 2;
    }
    else {
      uVar3 = 3;
    }
    uVar3 = led_set_mode(uVar3,0,0,unaff_r7);
    return uVar3;
  }
  return unaff_r7;
}

/**
 * APP_TIM1_TRG_COM_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E116, 2 bytes
 *   prototype: void APP_TIM1_TRG_COM_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_TIM1_TRG_COM_IRQHandler(void)

{
  return;
}

/**
 * APP_TIM1_CC_IRQHandler
 *   TIM1 capture/compare ISR: clears CC2IF, drives PA output mode (GPIOA_CRH) for SWIM edge, counts phases
 *   address 0x0800E118, 62 bytes
 *   prototype: uint32_t APP_TIM1_CC_IRQHandler(void)
 */
/* TIM1 capture/compare ISR: clears CC2IF, drives PA output mode (GPIOA_CRH) for SWIM edge, counts
   phases */

uint APP_TIM1_CC_IRQHandler(void)

{
  ushort uVar1;
  uint uVar2;
  uint unaff_r7;
  
  uVar1 = DAT_40012c10;
  DAT_40012c10 = uVar1 & 0xfffb;
  NVIC_ClearPendingIRQ(0x1b);
  if (DAT_2000150c == '\0') {
    DAT_200030f0 = DAT_200030f0 + 1;
    uVar2 = GPIOA_CRH;
    unaff_r7 = uVar2 & 0xffffff0f;
    if (DAT_200030f0 * -0x80000000 < 0) {
      uVar2 = unaff_r7 | 0x40;
    }
    else {
      uVar2 = unaff_r7 | 0xb0;
    }
    GPIOA_CRH = uVar2;
  }
  return unaff_r7;
}

/**
 * APP_TIM2_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E170, 2 bytes
 *   prototype: void APP_TIM2_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_TIM2_IRQHandler(void)

{
  return;
}

/**
 * APP_TIM3_IRQHandler
 *   Unused IRQ handler stub
 *   address 0x0800E172, 2 bytes
 *   prototype: void APP_TIM3_IRQHandler(void)
 */
/* Unused IRQ handler stub */

void APP_TIM3_IRQHandler(void)

{
  return;
}

/**
 * APP_TIM4_IRQHandler
 *   TIM4 ISR: captures CCR, disables timer and capture, flags measurement done, pends EXTI0
 *   address 0x0800E174, 4 bytes
 *   prototype: void APP_TIM4_IRQHandler(void)
 */
/* TIM4 ISR: captures CCR, disables timer and capture, flags measurement done, pends EXTI0 */

void APP_TIM4_IRQHandler(void)

{
  ushort uVar1;
  
  swim_captured_period = DAT_40000840;
  uVar1 = DAT_4000080c;
  DAT_4000080c = uVar1 & 0xffef;
  swim_capture_done_flag = 1;
  DAT_40000000 = 0;
  uVar1 = DAT_40000820;
  DAT_40000820 = uVar1 & 0xefff;
  DAT_e000e100 = 0x100000;
  return;
}

