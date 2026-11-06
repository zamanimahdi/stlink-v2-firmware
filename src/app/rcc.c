/*
 * app/rcc.c - Clock control (RCC)
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * RCC_DeInit
 *   ST RCC_DeInit: reset RCC CR/CFGR/CIR and peripheral clock/reset registers
 *   address 0x0800B6A0, 64 bytes
 *   called from: rcc_set_system_clock
 *   prototype: void RCC_DeInit(void)
 */
/* ST RCC_DeInit: reset RCC CR/CFGR/CIR and peripheral clock/reset registers */

void RCC_DeInit(void)

{
  uint uVar1;
  
  RCC_APB2RSTR = 0;
  RCC_APB1RSTR = 0;
  RCC_AHBENR = 0x14;
  RCC_APB2ENR = 0;
  RCC_APB1ENR = 0;
  uVar1 = RCC_CR;
  RCC_CR = uVar1 | 1;
  uVar1 = Ram40021004;
  Ram40021004 = uVar1 & 0xf8ff0000;
  uVar1 = RCC_CR;
  RCC_CR = uVar1 & 0xfef6ffff;
  uVar1 = RCC_CR;
  RCC_CR = uVar1 & 0xfffbffff;
  uVar1 = Ram40021004;
  Ram40021004 = uVar1 & 0xff80ffff;
  RCC_CIR = 0;
  return;
}

/**
 * RCC_HSEConfig
 *   ST RCC_HSEConfig: turn HSE on/off/bypass (CR HSEON/HSEBYP)
 *   address 0x0800B6E0, 50 bytes
 *   called from: rcc_set_system_clock
 *   prototype: void RCC_HSEConfig(int state)
 */
/* ST RCC_HSEConfig: turn HSE on/off/bypass (CR HSEON/HSEBYP) */

void RCC_HSEConfig(int param_1)

{
  uint uVar1;
  
  uVar1 = RCC_CR;
  RCC_CR = uVar1 & 0xfffeffff;
  uVar1 = RCC_CR;
  RCC_CR = uVar1 & 0xfffbffff;
  if (param_1 == 0x10000) {
    uVar1 = RCC_CR;
    uVar1 = uVar1 | 0x10000;
  }
  else {
    if (param_1 != 0x40000) {
      return;
    }
    uVar1 = RCC_CR;
    uVar1 = uVar1 | 0x50000;
  }
  RCC_CR = uVar1;
  return;
}

/**
 * RCC_WaitForHSEStartUp
 *   ST RCC_WaitForHSEStartUp: poll CR HSERDY with timeout, returns SUCCESS/ERROR
 *   address 0x0800B712, 46 bytes
 *   called from: rcc_set_system_clock
 *   prototype: undefined4 RCC_WaitForHSEStartUp(void)
 */
/* ST RCC_WaitForHSEStartUp: poll CR HSERDY with timeout, returns SUCCESS/ERROR */

undefined4 RCC_WaitForHSEStartUp(void)

{
  uint uVar1;
  int iVar2;
  
  do {
    uVar1 = RCC_CR;
    DAT_200030dc = (byte)(uVar1 >> 0x11) & 1;
    DAT_200030e0 = DAT_200030e0 + 1;
    if ((uVar1 >> 0x11 & 1) != 0) break;
  } while (DAT_200030e0 != 0x10000);
  iVar2 = RCC_CR;
  if (iVar2 << 0xe < 0) {
    return 1;
  }
  return 0;
}

/**
 * RCC_PLLConfig
 *   ST RCC_PLLConfig: set CFGR PLLSRC and PLLMUL
 *   address 0x0800B740, 16 bytes
 *   called from: rcc_set_system_clock
 *   prototype: void RCC_PLLConfig(uint src,uint mul)
 */
/* ST RCC_PLLConfig: set CFGR PLLSRC and PLLMUL */

void RCC_PLLConfig(uint param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = Ram40021004;
  Ram40021004 = param_1 | uVar1 & 0xffc0ffff | param_2;
  return;
}

/**
 * RCC_PLLCmd
 *   ST RCC_PLLCmd: set/clear CR PLLON (bit24 via bitband 0x42420060)
 *   address 0x0800B750, 6 bytes
 *   called from: rcc_set_system_clock
 *   prototype: void RCC_PLLCmd(undefined4 enable)
 */
/* ST RCC_PLLCmd: set/clear CR PLLON (bit24 via bitband 0x42420060) */

void RCC_PLLCmd(undefined4 param_1)

{
  DAT_42420060 = param_1;
  return;
}

/**
 * RCC_SYSCLKConfig
 *   ST RCC_SYSCLKConfig: set CFGR SW
 *   address 0x0800B756, 14 bytes
 *   called from: rcc_set_system_clock
 *   prototype: void RCC_SYSCLKConfig(uint sw)
 */
/* ST RCC_SYSCLKConfig: set CFGR SW */

void RCC_SYSCLKConfig(uint param_1)

{
  uint uVar1;
  
  uVar1 = Ram40021004;
  Ram40021004 = param_1 | uVar1 & 0xfffffffc;
  return;
}

/**
 * RCC_GetSYSCLKSource
 *   ST RCC_GetSYSCLKSource: read CFGR SWS (bits2-3)
 *   address 0x0800B764, 10 bytes
 *   called from: rcc_set_system_clock
 *   prototype: uint RCC_GetSYSCLKSource(void)
 */
/* ST RCC_GetSYSCLKSource: read CFGR SWS (bits2-3) */

uint RCC_GetSYSCLKSource(void)

{
  uint uVar1;
  
  uVar1 = Ram40021004;
  return uVar1 & 0xc;
}

/**
 * RCC_HCLKConfig
 *   ST RCC_HCLKConfig: set CFGR HPRE (AHB prescaler)
 *   address 0x0800B76E, 10 bytes
 *   called from: rcc_set_system_clock
 *   prototype: void RCC_HCLKConfig(uint hpre)
 */
/* ST RCC_HCLKConfig: set CFGR HPRE (AHB prescaler) */

void RCC_HCLKConfig(uint param_1)

{
  uint uVar1;
  
  uVar1 = Ram40021004;
  Ram40021004 = param_1 | uVar1 & 0xffffff0f;
  return;
}

/**
 * RCC_PCLK1Config
 *   ST RCC_PCLK1Config: set CFGR PPRE1 (APB1 prescaler bits8-10)
 *   address 0x0800B778, 14 bytes
 *   called from: rcc_set_system_clock
 *   prototype: void RCC_PCLK1Config(uint ppre1)
 */
/* ST RCC_PCLK1Config: set CFGR PPRE1 (APB1 prescaler bits8-10) */

void RCC_PCLK1Config(uint param_1)

{
  uint uVar1;
  
  uVar1 = Ram40021004;
  Ram40021004 = param_1 | uVar1 & 0xfffff8ff;
  return;
}

/**
 * RCC_PCLK2Config
 *   ST RCC_PCLK2Config: set CFGR PPRE2 (APB2 prescaler bits11-13)
 *   address 0x0800B786, 16 bytes
 *   called from: rcc_set_system_clock
 *   prototype: void RCC_PCLK2Config(int ppre2)
 */
/* ST RCC_PCLK2Config: set CFGR PPRE2 (APB2 prescaler bits11-13) */

void RCC_PCLK2Config(int param_1)

{
  uint uVar1;
  
  uVar1 = Ram40021004;
  Ram40021004 = uVar1 & 0xffffc7ff | param_1 << 3;
  return;
}

/**
 * RCC_USBCLKConfig
 *   ST RCC_USBCLKConfig: set CFGR USBPRE (bit22 via bitband 0x424200D8)
 *   address 0x0800B796, 6 bytes
 *   called from: rcc_set_system_clock
 *   prototype: void RCC_USBCLKConfig(undefined4 div)
 */
/* ST RCC_USBCLKConfig: set CFGR USBPRE (bit22 via bitband 0x424200D8) */

void RCC_USBCLKConfig(undefined4 param_1)

{
  DAT_424200d8 = param_1;
  return;
}

/**
 * RCC_LSEConfig
 *   OR bits into RCC_BDCR (LSE/RTC source select)
 *   address 0x0800B79C, 6 bytes
 *   called from: rtc_lse_init
 *   prototype: void RCC_LSEConfig(uint bits)
 */
/* OR bits into RCC_BDCR (LSE/RTC source select) */

void RCC_LSEConfig(uint param_1)

{
  uint uVar1;
  
  uVar1 = RCC_BDCR;
  RCC_BDCR = param_1 | uVar1;
  return;
}

/**
 * RCC_RTCCLKCmd
 *   ST RCC_RTCCLKCmd: set BDCR RTCEN (bit15 via bitband 0x4242043C)
 *   address 0x0800B7A2, 6 bytes
 *   called from: rtc_lse_init
 *   prototype: void RCC_RTCCLKCmd(undefined4 enable)
 */
/* ST RCC_RTCCLKCmd: set BDCR RTCEN (bit15 via bitband 0x4242043C) */

void RCC_RTCCLKCmd(undefined4 param_1)

{
  DAT_4242043c = param_1;
  return;
}

/**
 * RCC_GetClocksFreq
 *   ST RCC_GetClocksFreq: compute SYSCLK/HCLK/PCLK1/PCLK2/ADCCLK into struct using presc tables DAT_0800B8F8/B908
 *   address 0x0800B7A8, 126 bytes
 *   called from: usart_init
 *   prototype: void RCC_GetClocksFreq(uint *clocks)
 */
/* ST RCC_GetClocksFreq: compute SYSCLK/HCLK/PCLK1/PCLK2/ADCCLK into struct using presc tables
   DAT_0800B8F8/B908 */

void RCC_GetClocksFreq(uint *param_1)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = Ram40021004;
  if ((uVar1 & 0xc) == 8) {
    uVar1 = Ram40021004;
    iVar2 = Ram40021004;
    if (iVar2 << 0xf < 0) {
      iVar2 = Ram40021004;
      if (iVar2 << 0xe < 0) {
        iVar2 = 4000000;
      }
      else {
        iVar2 = 8000000;
      }
    }
    else {
      iVar2 = 4000000;
    }
    uVar1 = iVar2 * (((uVar1 & 0x3fffff) >> 0x12) + 2);
  }
  else {
    uVar1 = 8000000;
  }
  *param_1 = uVar1;
  uVar1 = Ram40021004;
  param_1[1] = *param_1 >> (&DAT_0800b8f8)[(uVar1 & 0xff) >> 4];
  uVar1 = Ram40021004;
  param_1[2] = param_1[1] >> (&DAT_0800b8f8)[(uVar1 & 0x7ff) >> 8];
  uVar1 = Ram40021004;
  param_1[3] = param_1[1] >> (&DAT_0800b8f8)[(uVar1 & 0x3fff) >> 0xb];
  uVar1 = Ram40021004;
  param_1[4] = param_1[3] / (uint)(byte)(&DAT_0800b908)[(uVar1 & 0xffff) >> 0xe];
  return;
}

/**
 * RCC_AHBPeriphClockCmd
 *   ST RCC_AHBPeriphClockCmd: set/clear bits in RCC_AHBENR
 *   address 0x0800B826, 18 bytes
 *   called from: jtag_connect, jtag_disconnect, usart_bridge_init, swd_connect, swim_init
 *   prototype: void RCC_AHBPeriphClockCmd(uint p,int en)
 */
/* ST RCC_AHBPeriphClockCmd: set/clear bits in RCC_AHBENR */

void RCC_AHBPeriphClockCmd(uint param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = RCC_AHBENR;
  if (param_2 == 0) {
    param_1 = uVar1 & ~param_1;
  }
  else {
    param_1 = param_1 | uVar1;
  }
  RCC_AHBENR = param_1;
  return;
}

/**
 * RCC_APB2PeriphClockCmd
 *   ST RCC_APB2PeriphClockCmd: set/clear bits in RCC_APB2ENR
 *   address 0x0800B838, 18 bytes
 *   called from: adc_init_measure, rcc_set_system_clock, _uncertain_stlink_target_mode_apply, usart_bridge_close, jtag_connect, jtag_disconnect, swim_deinit, usart_bridge_init ...
 *   prototype: void RCC_APB2PeriphClockCmd(uint p,int en)
 */
/* ST RCC_APB2PeriphClockCmd: set/clear bits in RCC_APB2ENR */

void RCC_APB2PeriphClockCmd(uint param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = RCC_APB2ENR;
  if (param_2 == 0) {
    param_1 = uVar1 & ~param_1;
  }
  else {
    param_1 = param_1 | uVar1;
  }
  RCC_APB2ENR = param_1;
  return;
}

/**
 * RCC_APB1PeriphClockCmd
 *   ST RCC_APB1PeriphClockCmd: set/clear bits in RCC_APB1ENR
 *   address 0x0800B84A, 18 bytes
 *   called from: rtc_lse_init, jtag_connect, jtag_disconnect, swim_deinit, swim_init, rcc_set_system_clock
 *   prototype: void RCC_APB1PeriphClockCmd(uint p,int en)
 */
/* ST RCC_APB1PeriphClockCmd: set/clear bits in RCC_APB1ENR */

void RCC_APB1PeriphClockCmd(uint param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = RCC_APB1ENR;
  if (param_2 == 0) {
    param_1 = uVar1 & ~param_1;
  }
  else {
    param_1 = param_1 | uVar1;
  }
  RCC_APB1ENR = param_1;
  return;
}

/**
 * RCC_APB2PeriphResetCmd
 *   ST RCC_APB2PeriphResetCmd: set/clear bits in RCC_APB2RSTR
 *   address 0x0800B85C, 18 bytes
 *   called from: jtag_spi_reset_pulse, USART_DeInit, spi_reset_pulse, TIM1_reset
 *   prototype: void RCC_APB2PeriphResetCmd(uint p,int en)
 */
/* ST RCC_APB2PeriphResetCmd: set/clear bits in RCC_APB2RSTR */

void RCC_APB2PeriphResetCmd(uint param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = RCC_APB2RSTR;
  if (param_2 == 0) {
    param_1 = uVar1 & ~param_1;
  }
  else {
    param_1 = param_1 | uVar1;
  }
  RCC_APB2RSTR = param_1;
  return;
}

/**
 * RCC_APB1PeriphResetCmd
 *   ST RCC_APB1PeriphResetCmd: set/clear bits in RCC_APB1RSTR
 *   address 0x0800B86E, 18 bytes
 *   called from: USART_DeInit, spi_reset_pulse, TIM_DeInit
 *   prototype: void RCC_APB1PeriphResetCmd(uint p,int en)
 */
/* ST RCC_APB1PeriphResetCmd: set/clear bits in RCC_APB1RSTR */

void RCC_APB1PeriphResetCmd(uint param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = RCC_APB1RSTR;
  if (param_2 == 0) {
    param_1 = uVar1 & ~param_1;
  }
  else {
    param_1 = param_1 | uVar1;
  }
  RCC_APB1RSTR = param_1;
  return;
}

/**
 * RCC_BackupResetCmd
 *   ST RCC_BackupResetCmd: set BDCR BDRST (bit16 via bitband 0x42420440)
 *   address 0x0800B880, 6 bytes
 *   called from: util_toggle_pulse
 *   prototype: void RCC_BackupResetCmd(undefined4 enable)
 */
/* ST RCC_BackupResetCmd: set BDCR BDRST (bit16 via bitband 0x42420440) */

void RCC_BackupResetCmd(undefined4 param_1)

{
  DAT_42420440 = param_1;
  return;
}

/**
 * RCC_MCOConfig
 *   Set MCO output source (CFGR byte at 0x40021007)
 *   address 0x0800B886, 6 bytes
 *   called from: _uncertain_stlink_mode_enter, _uncertain_target_mco_io_config, stlink_cmd_process, _uncertain_stlink_target_mode_apply
 *   prototype: void RCC_MCOConfig(undefined1 mco)
 */
/* Set MCO output source (CFGR byte at 0x40021007) */

void RCC_MCOConfig(undefined1 param_1)

{
  DAT_40021007 = param_1;
  return;
}

/**
 * RCC_GetFlagStatus
 *   ST RCC_GetFlagStatus: read a flag bit from CR/BDCR/CSR selected by flag id
 *   address 0x0800B88C, 34 bytes
 *   called from: rcc_set_system_clock
 *   prototype: uint RCC_GetFlagStatus(uint flag)
 */
/* ST RCC_GetFlagStatus: read a flag bit from CR/BDCR/CSR selected by flag id */

uint RCC_GetFlagStatus(uint param_1)

{
  uint uVar1;
  
  if (param_1 >> 5 == 1) {
    uVar1 = RCC_CR;
  }
  else if (param_1 >> 5 == 2) {
    uVar1 = RCC_BDCR;
  }
  else {
    uVar1 = RCC_CSR;
  }
  return uVar1 >> (param_1 & 0x1f) & 1;
}

/**
 * rcc_set_system_clock
 *   SetSysClock: RCC_DeInit, HSE on, PLL config, flash latency/prefetch, switch SYSCLK to PLL, enable USB/peripheral clocks
 *   address 0x0800C10C, 130 bytes
 *   called from: app_hw_init
 *   prototype: void rcc_set_system_clock(void)
 */
/* SetSysClock: RCC_DeInit, HSE on, PLL config, flash latency/prefetch, switch SYSCLK to PLL, enable
   USB/peripheral clocks */

void rcc_set_system_clock(void)

{
  int iVar1;
  undefined4 unaff_r7;
  
  RCC_DeInit();
  RCC_HSEConfig(0x10000);
  iVar1 = RCC_WaitForHSEStartUp();
  if (iVar1 == 1) {
    FLASH_PrefetchBufferCmd(0x10);
    FLASH_SetLatency(2);
    RCC_HCLKConfig(0);
    RCC_PCLK2Config(0x400);
    RCC_PCLK1Config(0x400);
    RCC_USBCLKConfig(0);
    RCC_PLLConfig(0x10000,0x1c0000);
    RCC_PLLCmd(1);
    do {
      iVar1 = RCC_GetFlagStatus(0x39);
    } while (iVar1 == 0);
    RCC_SYSCLKConfig(2);
    do {
      iVar1 = RCC_GetSYSCLKSource();
    } while (iVar1 != 8);
  }
  RCC_APB1PeriphClockCmd(0x800000,1);
  RCC_APB2PeriphClockCmd(0xa0c,1);
  rtc_lse_init(unaff_r7);
  return;
}

