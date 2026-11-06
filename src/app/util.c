/*
 * app/util.c - Miscellaneous helpers
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * nop_function
 *   Empty function (returns immediately); likely an inlined barrier/placeholder.
 *   address 0x08009234, 4 bytes
 *   called from: stlink_cmd_process
 *   prototype: void nop_function(void)
 */
/* Empty function (returns immediately); likely an inlined barrier/placeholder. */

void nop_function(void)

{
  return;
}

/**
 * util_time_diff
 *   Return param_2 - param_1 (elapsed-tick / pointer difference helper).
 *   address 0x080093FC, 4 bytes
 *   called from: APP_main, anticlone_background_write
 *   prototype: int util_time_diff(int a, int b)
 */
/* Return param_2 - param_1 (elapsed-tick / pointer difference helper). */

int util_time_diff(int param_1,int param_2)

{
  return param_2 - param_1;
}

/**
 * RTC_GetCounter
 *   Read 32-bit RTC counter (CNTH:CNTL at 0x40002818/1C) - used as timestamp/entropy
 *   address 0x0800BEB8, 12 bytes
 *   called from: anticlone_start, APP_main, timer_delay_ticks, anticlone_background_write
 *   prototype: undefined4 RTC_GetCounter(void)
 */
/* Read 32-bit RTC counter (CNTH:CNTL at 0x40002818/1C) - used as timestamp/entropy */

undefined4 RTC_GetCounter(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = DAT_4000281c;
  uVar1 = DAT_40002818;
  return CONCAT22(uVar1,uVar2);
}

/**
 * RTC_SetPrescaler
 *   ST RTC_SetPrescaler: enter config (CRL CNF), write PRLH/PRLL
 *   address 0x0800BEC4, 32 bytes
 *   called from: rtc_lse_init
 *   prototype: void RTC_SetPrescaler(undefined4 presc)
 */
/* ST RTC_SetPrescaler: enter config (CRL CNF), write PRLH/PRLL */

void RTC_SetPrescaler(undefined4 param_1)

{
  ushort uVar1;
  
  uVar1 = DAT_40002804;
  DAT_40002804 = uVar1 | 0x10;
  DAT_40002808 = (ushort)((uint)param_1 >> 0x10) & 0xf;
  DAT_4000280c = (short)param_1;
  uVar1 = DAT_40002804;
  DAT_40002804 = uVar1 & 0xffef;
  return;
}

/**
 * RTC_WaitForLastTask
 *   ST RTC_WaitForLastTask: poll CRL RTOFF
 *   address 0x0800BEE4, 10 bytes
 *   called from: rtc_lse_init
 *   prototype: void RTC_WaitForLastTask(void)
 */
/* ST RTC_WaitForLastTask: poll CRL RTOFF */

void RTC_WaitForLastTask(void)

{
  ushort uVar1;
  
  do {
    uVar1 = DAT_40002804;
  } while (-1 < (int)((uint)uVar1 << 0x1a));
  return;
}

/**
 * RTC_WaitForSynchro
 *   ST RTC_WaitForSynchro: clear CRL RSF, wait for resync
 *   address 0x0800BEEE, 20 bytes
 *   called from: rtc_lse_init
 *   prototype: void RTC_WaitForSynchro(void)
 */
/* ST RTC_WaitForSynchro: clear CRL RSF, wait for resync */

void RTC_WaitForSynchro(void)

{
  ushort uVar1;
  
  uVar1 = DAT_40002804;
  DAT_40002804 = uVar1 & 0xfff7;
  do {
    uVar1 = DAT_40002804;
  } while (-1 < (int)((uint)uVar1 << 0x1c));
  return;
}

/**
 * rtc_lse_init
 *   Enable PWR/BKP clocks + backup access, select LSE/RTC via BDCR, set prescaler
 *   address 0x0800C0D2, 58 bytes
 *   called from: usb_class_reset, rcc_set_system_clock
 *   prototype: void rtc_lse_init(void)
 */
/* Enable PWR/BKP clocks + backup access, select LSE/RTC via BDCR, set prescaler */

void rtc_lse_init(void)

{
  undefined4 unaff_r7;
  
  RCC_APB1PeriphClockCmd(0x18000000,1);
  util_bitband_write(1);
  util_toggle_pulse();
  RCC_LSEConfig(0x300);
  RCC_RTCCLKCmd(1);
  RTC_WaitForSynchro();
  RTC_WaitForLastTask();
  RTC_SetPrescaler(0x3e);
  RTC_WaitForLastTask(unaff_r7);
  return;
}

/**
 * stub_return_neg1
 *   Stub returning 0xFFFFFFFF (unreachable tail removed)
 *   address 0x0800C294, 16 bytes
 *   called from: APP_main
 *   prototype: undefined4 stub_return_neg1(void)
 */
/* WARNING: Removing unreachable block (ram,0x0800c2a0) */
/* Stub returning 0xFFFFFFFF (unreachable tail removed) */

undefined4 stub_return_neg1(void)

{
  return 0xffffffff;
}

/**
 * swap_bytes_u16
 *   Byte-swap a 16-bit value
 *   address 0x0800C7FC, 10 bytes
 *   called from: Setup0_Process
 *   prototype: uint swap_bytes_u16(uint v)
 */
/* Byte-swap a 16-bit value */

uint swap_bytes_u16(uint param_1)

{
  return (param_1 & 0xff) << 8 | param_1 >> 8;
}

/**
 * USART_DeInit
 *   Reset a USART/UART (USART1 via APB2RSTR, USART2/3/UART4/5 via APB1RSTR) - bridge init helper
 *   address 0x0800CEB0, 136 bytes
 *   called from: usart_bridge_dma_init, swd_connect
 *   prototype: undefined4 USART_DeInit(undefined2 *usart)
 */
/* Reset a USART/UART (USART1 via APB2RSTR, USART2/3/UART4/5 via APB1RSTR) - bridge init helper */

undefined4 USART_DeInit(undefined2 *param_1)

{
  undefined4 uVar1;
  undefined4 unaff_r7;
  
  if (param_1 == (undefined2 *)&UNK_40004400) {
    RCC_APB1PeriphResetCmd(0x20000,1);
    uVar1 = 0x20000;
  }
  else if (param_1 == (undefined2 *)&UNK_40004800) {
    RCC_APB1PeriphResetCmd(0x40000,1);
    uVar1 = 0x40000;
  }
  else if (param_1 == (undefined2 *)&UNK_40004c00) {
    RCC_APB1PeriphResetCmd(0x80000,1);
    uVar1 = 0x80000;
  }
  else {
    if (param_1 != (undefined2 *)&UNK_40005000) {
      if (param_1 == &DAT_40013800) {
        RCC_APB2PeriphResetCmd(0x4000,1);
        uVar1 = RCC_APB2PeriphResetCmd(0x4000,0,unaff_r7);
        return uVar1;
      }
      return unaff_r7;
    }
    RCC_APB1PeriphResetCmd(0x100000,1);
    uVar1 = 0x100000;
  }
  uVar1 = RCC_APB1PeriphResetCmd(uVar1,0,unaff_r7);
  return uVar1;
}

/**
 * usart_init
 *   ST StdPeriph USART_Init: programs CR1/CR2/CR3 and computes BRR from PCLK (PCLK2 for USART1 base 0x40013800, else PCLK1 via RCC_GetClocksFreq)
 *   address 0x0800CF38, 122 bytes
 *   called from: usart_bridge_dma_init
 *   prototype: void usart_init(uint16_t *USARTx, USART_InitTypeDef *init)
 */
/* ST StdPeriph USART_Init: programs CR1/CR2/CR3 and computes BRR from PCLK (PCLK2 for USART1 base
   0x40013800, else PCLK1 via RCC_GetClocksFreq) */

void usart_init(undefined2 *param_1,int *param_2)

{
  uint uVar1;
  undefined1 auStack_20 [8];
  int local_18;
  int local_14;
  
  param_1[8] = param_1[8] & 0xcfff | *(ushort *)((int)param_2 + 6);
  param_1[6] = param_1[6] & 0xe9f3 | *(ushort *)(param_2 + 1) | *(ushort *)(param_2 + 2) |
               *(ushort *)((int)param_2 + 10);
  param_1[10] = param_1[10] & 0xfcff | *(ushort *)(param_2 + 3);
  RCC_GetClocksFreq(auStack_20);
  if (param_1 != &DAT_40013800) {
    local_14 = local_18;
  }
  uVar1 = (uint)(local_14 * 0x19) / (uint)(*param_2 << 2);
  param_1[4] = (ushort)(((uVar1 % 100) * 0x10 + 0x32) / 100) & 0xf | (ushort)(uVar1 / 100 << 4);
  return;
}

/**
 * usart_clock_config
 *   ST USART_ClockInit: ORs clock/stop bits (CPOL/CPHA/LBCL/LastBit) into USART CR2 (base+0x10)
 *   address 0x0800CFC8, 30 bytes
 *   called from: usart_bridge_dma_init
 *   prototype: void usart_clock_config(uint32_t USARTx, uint16_t *clk_init)
 */
/* ST USART_ClockInit: ORs clock/stop bits (CPOL/CPHA/LBCL/LastBit) into USART CR2 (base+0x10) */

void usart_clock_config(int param_1,ushort *param_2)

{
  *(ushort *)(param_1 + 0x10) =
       param_2[3] | *(ushort *)(param_1 + 0x10) & 0xf0ff | *param_2 | param_2[1] | param_2[2];
  return;
}

/**
 * usart_cr3_bit_config
 *   Sets or clears a bit-mask in USART CR3 (base+0x14) depending on enable flag (DMA/flow-control cmd)
 *   address 0x0800CFE0, 16 bytes
 *   called from: usart_bridge_dma_init
 *   prototype: void usart_cr3_bit_config(uint32_t USARTx, uint16_t mask, int enable)
 */
/* Sets or clears a bit-mask in USART CR3 (base+0x14) depending on enable flag (DMA/flow-control
   cmd) */

void usart_cr3_bit_config(int param_1,ushort param_2,int param_3)

{
  if (param_3 == 0) {
    param_2 = *(ushort *)(param_1 + 0x14) & ~param_2;
  }
  else {
    param_2 = param_2 | *(ushort *)(param_1 + 0x14);
  }
  *(ushort *)(param_1 + 0x14) = param_2;
  return;
}

/**
 * util_bitband_write
 *   Writes a value to a peripheral bit-band alias (0x420E0020 -> bit0 of 0x40007000 region)
 *   address 0x0800D030, 6 bytes
 *   called from: rtc_lse_init
 *   prototype: void util_bitband_write(uint32_t val)
 */
/* Writes a value to a peripheral bit-band alias (0x420E0020 -> bit0 of 0x40007000 region) */

void util_bitband_write(undefined4 param_1)

{
  DAT_420e0020 = param_1;
  return;
}

/**
 * util_toggle_pulse
 *   Pulses a line: calls helper(1) then helper(0) (sub_0800B880)
 *   address 0x0800D03C, 18 bytes
 *   called from: rtc_lse_init
 *   prototype: void util_toggle_pulse(void)
 */
/* Pulses a line: calls helper(1) then helper(0) (sub_0800B880) */

void util_toggle_pulse(void)

{
  undefined4 unaff_r7;
  
  RCC_BackupResetCmd(1);
  RCC_BackupResetCmd(0,unaff_r7);
  return;
}

