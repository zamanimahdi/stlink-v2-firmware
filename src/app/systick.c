/*
 * app/systick.c - SysTick / delays
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * SysTick_SetReload
 *   Write SysTick LOAD (reload value)
 *   address 0x0800CE00, 6 bytes
 *   called from: swim_state_write_resync, swim_state_read_start, swim_read_bits, swim_state_write_start, _uncertain_swim_state_entry_seq, swim_state_read_prep, swim_state_write_second, swim_state_write_advance ...
 *   prototype: void SysTick_SetReload(undefined4 reload)
 */
/* Write SysTick LOAD (reload value) */

void SysTick_SetReload(undefined4 param_1)

{
  SysTick_LOAD = param_1;
  return;
}

/**
 * SysTick_CounterCmd
 *   Enable/disable or clear SysTick (CTRL ENABLE / VAL=0)
 *   address 0x0800CE06, 26 bytes
 *   called from: _uncertain_swim_state_sync_done, swim_state_write_resync, swim_state_read_start, swim_read_bits, swim_state_write_start, _uncertain_swim_state_entry_seq, swim_rx_complete_dispatch, swim_state_read_prep ...
 *   prototype: void SysTick_CounterCmd(int cmd)
 */
/* Enable/disable or clear SysTick (CTRL ENABLE / VAL=0) */

void SysTick_CounterCmd(int param_1)

{
  uint uVar1;
  
  if (param_1 != 0) {
    uVar1 = SysTick_CTRL;
    if (param_1 == 1) {
      uVar1 = uVar1 | 1;
    }
    else {
      uVar1 = uVar1 & 0xfffffffe;
    }
    SysTick_CTRL = uVar1;
    return;
  }
  SysTick_VAL = 0;
  return;
}

/**
 * SysTick_ITConfig
 *   Set/clear SysTick CTRL TICKINT (interrupt enable)
 *   address 0x0800CE20, 20 bytes
 *   called from: swim_init
 *   prototype: void SysTick_ITConfig(int enable)
 */
/* Set/clear SysTick CTRL TICKINT (interrupt enable) */

void SysTick_ITConfig(int param_1)

{
  uint uVar1;
  
  uVar1 = SysTick_CTRL;
  if (param_1 == 0) {
    uVar1 = uVar1 & 0xfffffffd;
  }
  else {
    uVar1 = uVar1 | 2;
  }
  SysTick_CTRL = uVar1;
  return;
}

/**
 * APP_SysTick_Handler
 *   SysTick exception: timekeeping/LED tick, schedules task state (DAT_200011b5=4)
 *   address 0x0800E080, 4 bytes
 *   prototype: void APP_SysTick_Handler(void)
 */
/* SysTick exception: timekeeping/LED tick, schedules task state (DAT_200011b5=4) */

void APP_SysTick_Handler(void)

{
  undefined4 unaff_r7;
  
  SysTick_CounterCmd(0xfffffffe);
  SysTick_CounterCmd(0);
  swim_status = 4;
  NVIC_TriggerSWInterrupt(6,unaff_r7);
  return;
}

