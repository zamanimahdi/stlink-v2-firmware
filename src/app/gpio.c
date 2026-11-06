/*
 * app/gpio.c - GPIO
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * GPIO_Init
 *   ST StdPeriph GPIO_Init: program CRL/CRH of the port from the init struct (pin mask + mode/speed nibble), handling pull-up(0x48)/pull-down(0x28) via BSRR/BRR.
 *   address 0x08008FE2, 188 bytes
 *   called from: jtag_init_pins, _uncertain_stlink_mode_enter, jtag_deinit_pins, _uncertain_stlink_mode_leave, app_hw_init, swd_init_gpio, _uncertain_target_io_reinit, _uncertain_stlink_target_mode_apply ...
 *   prototype: void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *init)
 */
/* ST StdPeriph GPIO_Init: program CRL/CRH of the port from the init struct (pin mask + mode/speed
   nibble), handling pull-up(0x48)/pull-down(0x28) via BSRR/BRR. */

void GPIO_Init(uint *param_1,ushort *param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  
  uVar1 = 0;
  uVar2 = *(byte *)((int)param_2 + 3) & 0xf;
  if ((int)((uint)*(byte *)((int)param_2 + 3) << 0x1b) < 0) {
    uVar2 = uVar2 | (byte)param_2[1];
  }
  uVar3 = (uint)*param_2;
  if ((char)*param_2 != '\0') {
    uVar5 = *param_1;
    do {
      uVar4 = 1 << (uVar1 & 0xff);
      if ((uVar4 & uVar3) == uVar4) {
        uVar5 = uVar2 << (uVar1 << 2 & 0xff) | uVar5 & ~(0xf << (uVar1 << 2 & 0xff));
        if (*(char *)((int)param_2 + 3) == '(') {
          param_1[5] = uVar4;
        }
        if (*(char *)((int)param_2 + 3) == 'H') {
          param_1[4] = uVar4;
        }
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 < 8);
    *param_1 = uVar5;
  }
  if (0xff < uVar3) {
    uVar5 = param_1[1];
    uVar1 = 0;
    do {
      uVar4 = 1 << (uVar1 + 8 & 0xff);
      if ((uVar4 & uVar3) == uVar4) {
        uVar5 = uVar2 << (uVar1 << 2 & 0xff) | uVar5 & ~(0xf << (uVar1 << 2 & 0xff));
        if (*(char *)((int)param_2 + 3) == '(') {
          param_1[5] = uVar4;
        }
        if (*(char *)((int)param_2 + 3) == 'H') {
          param_1[4] = uVar4;
        }
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 < 8);
    param_1[1] = uVar5;
  }
  return;
}

/**
 * GPIO_ReadInputDataBit
 *   ST StdPeriph GPIO_ReadInputDataBit: return nonzero if the IDR bit (param_2) of the port is set.
 *   address 0x0800909E, 14 bytes
 *   called from: swim_wait_line_high
 *   prototype: bool GPIO_ReadInputDataBit(int GPIOx, uint pin)
 */
/* ST StdPeriph GPIO_ReadInputDataBit: return nonzero if the IDR bit (param_2) of the port is set.
    */

bool GPIO_ReadInputDataBit(int param_1,uint param_2)

{
  return (*(uint *)(param_1 + 8) & param_2) != 0;
}

/**
 * GPIO_SetBits
 *   ST StdPeriph GPIO_SetBits: write param_2 to the port BSRR (offset 0x10).
 *   address 0x080090AC, 4 bytes
 *   called from: swim_init, app_hw_init, swim_command_dispatch, _uncertain_target_io_reinit, stlink_cmd_process
 *   prototype: void GPIO_SetBits(int GPIOx, undefined4 pins)
 */
/* ST StdPeriph GPIO_SetBits: write param_2 to the port BSRR (offset 0x10). */

void GPIO_SetBits(int param_1,undefined4 param_2)

{
  *(undefined4 *)(param_1 + 0x10) = param_2;
  return;
}

/**
 * GPIO_ResetBits
 *   ST StdPeriph GPIO_ResetBits: write param_2 to the port BRR (offset 0x14).
 *   address 0x080090B0, 4 bytes
 *   called from: gpio_pa15_write, swim_command_dispatch
 *   prototype: void GPIO_ResetBits(int GPIOx, undefined4 pins)
 */
/* ST StdPeriph GPIO_ResetBits: write param_2 to the port BRR (offset 0x14). */

void GPIO_ResetBits(int param_1,undefined4 param_2)

{
  *(undefined4 *)(param_1 + 0x14) = param_2;
  return;
}

/**
 * GPIO_WriteBit
 *   ST StdPeriph GPIO_WriteBit: set pins via BSRR when param_3!=0 else clear via BRR.
 *   address 0x080090B4, 10 bytes
 *   called from: jtag_init_pins, _uncertain_stlink_mode_enter, _uncertain_stlink_mode_leave, gpio_pb0_init, swd_init_gpio, stlink_dispatch_command, _uncertain_stlink_target_mode_apply, target_drive_nrst
 *   prototype: void GPIO_WriteBit(int GPIOx, undefined4 pins, int val)
 */
/* ST StdPeriph GPIO_WriteBit: set pins via BSRR when param_3!=0 else clear via BRR. */

void GPIO_WriteBit(int param_1,undefined4 param_2,int param_3)

{
  if (param_3 != 0) {
    *(undefined4 *)(param_1 + 0x10) = param_2;
    return;
  }
  *(undefined4 *)(param_1 + 0x14) = param_2;
  return;
}

/**
 * GPIO_PinRemapConfig
 *   ST StdPeriph GPIO_PinRemapConfig: read-modify-write AFIO_MAPR for the encoded remap field in param_1, enabling when param_2!=0.
 *   address 0x080090BE, 54 bytes
 *   called from: swim_init, _uncertain_stlink_target_mode_apply
 *   prototype: void GPIO_PinRemapConfig(uint remap, int newstate)
 */
/* ST StdPeriph GPIO_PinRemapConfig: read-modify-write AFIO_MAPR for the encoded remap field in
   param_1, enabling when param_2!=0. */

void GPIO_PinRemapConfig(uint param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = AFIO_MAPR;
  if ((int)(param_1 << 10) < 0) {
    uVar1 = uVar1 & 0xf8ffffff;
  }
  else if ((int)(param_1 << 0xb) < 0) {
    uVar1 = uVar1 & ~(3 << ((param_1 & 0xfffff) >> 0x10));
  }
  else {
    uVar1 = uVar1 & ~(param_1 & 0xffff);
  }
  if (param_2 != 0) {
    if ((int)(param_1 << 10) < 0) {
      uVar1 = uVar1 | param_1 << 0x10;
    }
    else {
      uVar1 = uVar1 | param_1 & 0xffff;
    }
  }
  AFIO_MAPR = uVar1;
  return;
}

/**
 * _uncertain_gpio_cfg_or_merge
 *   OR-merge 8 config halfwords into a GPIO CR register keeping fixed bits (0x3040), copy 9th word
 *   address 0x0800B380, 46 bytes
 *   called from: jtag_spi_dma_init
 *   prototype: void gpio_cfg_or_merge(ushort *dst,ushort *src)
 */
/* OR-merge 8 config halfwords into a GPIO CR register keeping fixed bits (0x3040), copy 9th word */

void _uncertain_gpio_cfg_or_merge(ushort *param_1,ushort *param_2)

{
  *param_1 = *param_1 & 0x3040 | *param_2 | param_2[1] | param_2[2] | param_2[3] | param_2[4] |
             param_2[5] | param_2[6] | param_2[7];
  param_1[8] = param_2[8];
  return;
}

/**
 * gpio_pa15_write
 *   Drive PA15 high (BSRR 0x8000) or call low-side helper sub_080090B0 based on arg
 *   address 0x0800C348, 20 bytes
 *   called from: usb_hw_power_on, usb_hw_power_off
 *   prototype: void gpio_pa15_write(int high)
 */
/* Drive PA15 high (BSRR 0x8000) or call low-side helper sub_080090B0 based on arg */

void gpio_pa15_write(int param_1)

{
  if (param_1 != 0) {
    GPIOA_BSRR = 0x8000;
    return;
  }
  GPIO_ResetBits();
  return;
}

/**
 * gpio_pb0_init
 *   Configure GPIOB pin0 as output and set it (enable helper)
 *   address 0x0800C444, 52 bytes
 *   called from: _uncertain_stlink_target_mode_apply
 *   prototype: undefined8 gpio_pb0_init(void)
 */
/* Configure GPIOB pin0 as output and set it (enable helper) */

undefined8 gpio_pb0_init(void)

{
  uint uVar1;
  undefined2 local_10;
  undefined1 local_e;
  undefined1 local_d;
  undefined4 uStack_c;
  
  uVar1 = GPIOB_CRL;
  if ((uVar1 & 3) == 0) {
    GPIO_WriteBit(&GPIOB_CRL,1,1);
  }
  local_10 = 1;
  local_d = 0x14;
  local_e = 1;
  GPIO_Init(&GPIOB_CRL,&local_10);
  return CONCAT44(uStack_c,CONCAT13(local_d,CONCAT12(local_e,local_10)));
}

