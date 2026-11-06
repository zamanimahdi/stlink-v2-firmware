/*
 * app/led.c - Status LED
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * led_set_result
 *   Store the last command result byte (0x80=OK) in DAT_2000152C, consumed by the LED blink IRQ.
 *   address 0x08009438, 6 bytes
 *   called from: swim_scheduler_set_mode, swim_command_dispatch, stlink_cmd_rw_dap_reg, stlink_dispatch_command, jtag_shift_idcode, stlink_cmd_jtag_raw, stlink_cmd_process
 *   prototype: void led_set_result(undefined1 ok)
 */
/* Store the last command result byte (0x80=OK) in DAT_2000152C, consumed by the LED blink IRQ. */

void led_set_result(undefined1 param_1)

{
  led_cmd_result = param_1;
  return;
}

/**
 * led_get_result
 *   Return the last command result byte DAT_2000152C (read by APP_TIM1_UP_IRQHandler).
 *   address 0x0800943E, 6 bytes
 *   called from: APP_TIM1_UP_IRQHandler
 *   prototype: undefined1 led_get_result(void)
 */
/* Return the last command result byte DAT_2000152C (read by APP_TIM1_UP_IRQHandler). */

undefined1 led_get_result(void)

{
  return led_cmd_result;
}

/**
 * led_tim1_init
 *   Initialise TIM1 (0x40012C00) in PWM mode driving the status LED on PA9 (TIM1 CH2).
 *   address 0x08009444, 84 bytes
 *   called from: app_hw_init, _uncertain_target_io_reinit
 *   prototype: void led_tim1_init(void)
 */
/* Initialise TIM1 (0x40012C00) in PWM mode driving the status LED on PA9 (TIM1 CH2). */

void led_tim1_init(void)

{
  undefined2 local_18;
  undefined2 local_16;
  undefined2 local_14;
  undefined2 local_10;
  
  RCC_APB2PeriphClockCmd(0x800,1);
  RCC_APB2PeriphClockCmd(4,1);
  TIM1_reset();
  DAT_40012c00 = 0x84;
  TIM_OCStructInit(&local_18);
  local_18 = 0x60;
  local_10 = 2;
  local_16 = 1;
  local_14 = 0;
  tim1_oc2_load_config(&local_18);
  tim1_oc2_preload(1);
  TIM1_CtrlPWMOutputs(1);
  return;
}

/**
 * led_set_mode
 *   Status-LED/TIM1 control state machine: reconfigure PA9 and TIM1 channels/period for the requested blink pattern (param_1 0..6).
 *   address 0x08009498, 364 bytes
 *   called from: APP_main, stlink_cmd_set_led_ack, led_blink_activity, usb_class_reset, APP_TIM1_UP_IRQHandler, stlink_cmd_debug_connect, stlink_dispatch_command, stlink_cmd_process
 *   prototype: uint led_set_mode(uint mode, uint arg, int count)
 */
/* Status-LED/TIM1 control state machine: reconfigure PA9 and TIM1 channels/period for the requested
   blink pattern (param_1 0..6). */

uint led_set_mode(uint param_1,uint param_2,int param_3)

{
  undefined2 uVar1;
  ushort uVar2;
  undefined4 uVar3;
  uint uVar4;
  bool bVar5;
  uint local_20;
  undefined4 uStack_1c;
  
  if (param_1 == DAT_2000150d) {
    if ((param_1 < 2) || (param_1 == 2 || param_1 == 3)) {
      return local_20;
    }
    bVar5 = param_1 == 4 || param_1 == 5;
    if (param_1 == 4 || param_1 == 5) {
      bVar5 = DAT_20001510 == param_2;
    }
    if (bVar5) {
      return local_20;
    }
    uVar4 = (uint)DAT_2000150d;
    if (param_1 == 6) {
      uVar4 = DAT_20001510;
    }
    if (param_1 == 6 && uVar4 == param_2) {
      return local_20;
    }
  }
  else {
    tim1_dier_config(4,0);
    tim1_dier_config(1,0);
    TIM1_Cmd(0);
    TIM1_OnePulseMode(0);
  }
  uVar1 = (undefined2)param_2;
  switch(param_1) {
  case 0:
    uVar4 = GPIOA_CRH;
    local_20 = uVar4 & 0xffffff0f;
    GPIOA_CRH = local_20 | 0x40;
    break;
  case 1:
    uVar3 = 0x50;
    goto LAB_08009522;
  case 2:
    uVar3 = 0x40;
LAB_08009522:
    tim1_oc2_mode(uVar3);
    break;
  case 3:
    DAT_40012c2c = 100;
    uVar2 = DAT_40012c2c;
    DAT_40012c38 = uVar2 >> 1;
    DAT_40012c28 = 100;
    uVar1 = DAT_40012c18;
    local_20 = (byte)uVar1 | 0x6800;
    DAT_40012c18 = (short)local_20;
    break;
  case 4:
    tim1_oc2_mode(0x50);
    DAT_40012c2c = uVar1;
    DAT_40012c30 = 2;
    DAT_40012c28 = 36000;
    DAT_2000150c = 0;
    uVar3 = 4;
    goto LAB_080095ca;
  case 5:
    tim1_oc2_mode(0x40);
    DAT_40012c2c = uVar1;
    DAT_40012c30 = 2;
    DAT_40012c28 = 36000;
    DAT_2000150c = 0;
    uVar3 = 4;
    goto LAB_080095ca;
  case 6:
    DAT_40012c2c = uVar1;
    DAT_40012c38 = (short)(param_2 >> 1);
    DAT_40012c28 = 0xffff;
    uVar1 = DAT_40012c18;
    local_20 = (byte)uVar1 | 0x6800;
    DAT_40012c18 = (short)local_20;
    if (0xfe < param_3 - 1U) break;
    DAT_40012c30 = (short)param_3;
    DAT_2000150c = 1;
    TIM1_OnePulseMode();
    uVar2 = DAT_40012c10;
    DAT_40012c10 = uVar2 & 0xfffe;
    uVar3 = 1;
LAB_080095ca:
    tim1_dier_config(uVar3,1);
  }
  DAT_42258280 = 1;
  if (((DAT_2000150d == 0 || DAT_2000150d == 4) || (DAT_2000150d == 5 || DAT_2000150d == 7)) &&
     (param_1 != 0)) {
    uVar4 = GPIOA_CRH;
    local_20 = uVar4 & 0xffffff0f;
    GPIOA_CRH = local_20 | 0xb0;
  }
  DAT_2000150d = (byte)param_1;
  DAT_20001510 = param_2;
  uVar4 = TIM1_Cmd(1,local_20,uStack_1c);
  return uVar4;
}

/**
 * led_blink_activity
 *   Trigger the default LED activity blink (led_set_mode(6,0xB4,7)); called on each command.
 *   address 0x08009624, 8 bytes
 *   called from: cortexm_mem_ap_access_retry, fpb_set_breakpoint, cortexm_halt, cortexm_write_dhcsr_word, cortexm_write_reg_verify, cortexm_reset_and_halt, mem_write_16, mem_write_32_noinc ...
 *   prototype: void led_blink_activity(void)
 */
/* Trigger the default LED activity blink (led_set_mode(6,0xB4,7)); called on each command. */

void led_blink_activity(void)

{
  led_set_mode(6,0xb4,7);
  return;
}

/**
 * tim1_oc2_load_config
 *   Load TIM1 OC/CCER/CCR config (bitband 0x4225xxxx) incl CCR2 (0x40012C38) for the LED PWM on PA9 (TIM1 CH2)
 *   address 0x0800C53E, 66 bytes
 *   called from: led_tim1_init
 *   prototype: void tim1_oc2_load_config(undefined2 *cfg)
 */
/* Load TIM1 OC/CCER/CCR config (bitband 0x4225xxxx) incl CCR2 (0x40012C38) for the LED PWM on PA9
   (TIM1 CH2) */

void tim1_oc2_load_config(undefined2 *param_1)

{
  undefined2 uVar1;
  
  uVar1 = DAT_40012c18;
  DAT_42258410 = 0;
  DAT_40012c18 = (short)CONCAT21(*param_1,(char)uVar1);
  DAT_42258410 = (uint)(ushort)param_1[1];
  DAT_42258418 = (uint)(ushort)param_1[2];
  DAT_42258414 = (uint)(ushort)param_1[4];
  DAT_4225841c = (uint)(ushort)param_1[5];
  DAT_422580a8 = (uint)(ushort)param_1[6];
  DAT_422580ac = (uint)(ushort)param_1[7];
  DAT_40012c38 = param_1[3];
  return;
}

/**
 * TIM1_Cmd
 *   Set TIM1 CR1 CEN (counter enable, bitband 0x42258000) - LED timer run/stop
 *   address 0x0800C594, 8 bytes
 *   called from: led_set_mode
 *   prototype: void TIM1_Cmd(undefined4 enable)
 */
/* Set TIM1 CR1 CEN (counter enable, bitband 0x42258000) - LED timer run/stop */

void TIM1_Cmd(undefined4 param_1)

{
  DAT_42258000 = param_1;
  return;
}

/**
 * TIM1_CtrlPWMOutputs
 *   Set TIM1 BDTR MOE (main output enable, bitband 0x422588BC) for LED PWM
 *   address 0x0800C59C, 8 bytes
 *   called from: led_tim1_init
 *   prototype: void TIM1_CtrlPWMOutputs(undefined4 enable)
 */
/* Set TIM1 BDTR MOE (main output enable, bitband 0x422588BC) for LED PWM */

void TIM1_CtrlPWMOutputs(undefined4 param_1)

{
  DAT_422588bc = param_1;
  return;
}

/**
 * tim1_dier_config
 *   Set/clear bits in TIM1_DIER (0x40012C0C) - update/CC interrupt or DMA enable for LED blink timing
 *   address 0x0800C5A4, 20 bytes
 *   called from: led_set_mode
 *   prototype: void tim1_dier_config(ushort bits,int enable)
 */
/* Set/clear bits in TIM1_DIER (0x40012C0C) - update/CC interrupt or DMA enable for LED blink timing
    */

void tim1_dier_config(ushort param_1,int param_2)

{
  ushort uVar1;
  
  uVar1 = DAT_40012c0c;
  if (param_2 == 1) {
    param_1 = param_1 | uVar1;
  }
  else {
    param_1 = uVar1 & ~param_1;
  }
  DAT_40012c0c = param_1;
  return;
}

/**
 * TIM1_OnePulseMode
 *   Set TIM1 CR1 OPM (one-pulse mode, bitband 0x4225800C) - LED one-shot flash mode
 *   address 0x0800C5B8, 6 bytes
 *   called from: led_set_mode
 *   prototype: void TIM1_OnePulseMode(undefined4 enable)
 */
/* Set TIM1 CR1 OPM (one-pulse mode, bitband 0x4225800C) - LED one-shot flash mode */

void TIM1_OnePulseMode(undefined4 param_1)

{
  DAT_4225800c = param_1;
  return;
}

/**
 * tim1_oc2_mode
 *   Set TIM1 CCMR1 OC2M field (0x40012C18 bits8-10) - LED PWM output-compare mode
 *   address 0x0800C5BE, 20 bytes
 *   called from: led_set_mode
 *   prototype: void tim1_oc2_mode(short mode)
 */
/* Set TIM1 CCMR1 OC2M field (0x40012C18 bits8-10) - LED PWM output-compare mode */

void tim1_oc2_mode(short param_1)

{
  ushort uVar1;
  
  uVar1 = DAT_40012c18;
  DAT_40012c18 = uVar1 & 0xf7f | param_1 << 8;
  return;
}

/**
 * tim1_oc2_preload
 *   Enable TIM1 CCMR1 OC2PE preload (bitband 0x4225832C)
 *   address 0x0800C5C8, 6 bytes
 *   called from: led_tim1_init
 *   prototype: void tim1_oc2_preload(undefined4 enable)
 */
/* Enable TIM1 CCMR1 OC2PE preload (bitband 0x4225832C) */

void tim1_oc2_preload(undefined4 param_1)

{
  DAT_4225832c = param_1;
  return;
}

