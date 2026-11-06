/*
 * app/adc.c - ADC (target voltage)
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * adc_init_measure
 *   Configure ADC1 (0x40012400): set channel/sample time, run calibration, start conversion and store the result in DAT_200030EC (target-voltage sampling).
 *   address 0x080090F8, 168 bytes
 *   called from: APP_main
 */
/* Configure ADC1 (0x40012400): set channel/sample time, run calibration, start conversion and store
   the result in DAT_200030EC (target-voltage sampling). */

void adc_init_measure(void)

{
  int iVar1;
  undefined4 local_20;
  undefined1 local_1c;
  undefined1 local_1b;
  undefined4 local_18;
  undefined4 local_14;
  undefined1 local_10;
  
  RCC_APB2PeriphClockCmd(0x200,1);
  local_20 = 0;
  local_1c = 1;
  local_1b = 0;
  local_18 = 0xe0000;
  local_14 = 0;
  local_10 = 1;
  ADC_Init(&DAT_40012400,&local_20);
  ADC_Cmd(&DAT_40012400,0);
  ADC_TempSensorVrefintCmd(1);
  ADC_RegularChannelConfig(&DAT_40012400,0x11,1,1);
  ADC_Cmd(&DAT_40012400,1);
  ADC_ResetCalibration(&DAT_40012400);
  do {
    iVar1 = ADC_GetResetCalibrationStatus(&DAT_40012400);
  } while (iVar1 != 0);
  ADC_StartCalibration(&DAT_40012400);
  do {
    iVar1 = ADC_GetCalibrationStatus(&DAT_40012400);
  } while (iVar1 != 0);
  ADC_SoftwareStartConvCmd(&DAT_40012400,1);
  do {
    iVar1 = ADC_GetFlagStatus(&DAT_40012400,2);
  } while (iVar1 == 0);
  adc_voltage_sample = ADC_GetConversionValue(&DAT_40012400);
  ADC_RegularChannelConfig(&DAT_40012400,0,1,1);
  return;
}

/**
 * adc_check_voltage
 *   Trigger an ADC1 conversion, wait, read it and return 1 when the sample exceeds 0x400 (target-present/voltage threshold).
 *   address 0x0800938A, 44 bytes
 *   called from: swim_state_read_start
 *   prototype: undefined4 adc_check_voltage(void)
 */
/* Trigger an ADC1 conversion, wait, read it and return 1 when the sample exceeds 0x400
   (target-present/voltage threshold). */

undefined4 adc_check_voltage(void)

{
  int iVar1;
  uint uVar2;
  
  ADC_SoftwareStartConvCmd(&DAT_40012400,1);
  do {
    iVar1 = ADC_GetFlagStatus(&DAT_40012400,2);
  } while (iVar1 == 0);
  uVar2 = ADC_GetConversionValue(&DAT_40012400);
  if (0x400 < uVar2) {
    return 1;
  }
  return 0;
}

/**
 * ADC_Init
 *   ST ADC_Init: program ADC_CR1, ADC_CR2, and NbrOfChannel in SQR1 (reg+0x2C)
 *   address 0x0800BF0C, 60 bytes
 *   called from: adc_init_measure
 *   prototype: void ADC_Init(int adc,uint *init)
 */
/* ST ADC_Init: program ADC_CR1, ADC_CR2, and NbrOfChannel in SQR1 (reg+0x2C) */

void ADC_Init(int param_1,uint *param_2)

{
  *(uint *)(param_1 + 4) =
       *(uint *)(param_1 + 4) & 0xfff0feff | *param_2 | (uint)(byte)param_2[1] << 8;
  *(uint *)(param_1 + 8) =
       *(uint *)(param_1 + 8) & 0xfff1f7fd | param_2[3] | param_2[2] |
       (uint)*(byte *)((int)param_2 + 5) << 1;
  *(uint *)(param_1 + 0x2c) =
       *(uint *)(param_1 + 0x2c) & 0xff0fffff | ((byte)param_2[4] - 1 & 0xff) << 0x14;
  return;
}

/**
 * ADC_Cmd
 *   ST ADC_Cmd: set/clear ADC_CR2 ADON (bit0)
 *   address 0x0800BF48, 18 bytes
 *   called from: adc_init_measure
 *   prototype: void ADC_Cmd(int adc,int enable)
 */
/* ST ADC_Cmd: set/clear ADC_CR2 ADON (bit0) */

void ADC_Cmd(int param_1,int param_2)

{
  uint uVar1;
  
  if (param_2 == 0) {
    uVar1 = *(uint *)(param_1 + 8) & 0xfffffffe;
  }
  else {
    uVar1 = *(uint *)(param_1 + 8) | 1;
  }
  *(uint *)(param_1 + 8) = uVar1;
  return;
}

/**
 * ADC_ResetCalibration
 *   ST ADC_ResetCalibration: set ADC_CR2 RSTCAL (bit3)
 *   address 0x0800BF5A, 10 bytes
 *   called from: adc_init_measure
 *   prototype: void ADC_ResetCalibration(int adc)
 */
/* ST ADC_ResetCalibration: set ADC_CR2 RSTCAL (bit3) */

void ADC_ResetCalibration(int param_1)

{
  *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) | 8;
  return;
}

/**
 * ADC_GetResetCalibrationStatus
 *   ST ADC_GetResetCalibrationStatus: read ADC_CR2 RSTCAL
 *   address 0x0800BF64, 6 bytes
 *   called from: adc_init_measure
 *   prototype: uint ADC_GetResetCalibrationStatus(int adc)
 */
/* ST ADC_GetResetCalibrationStatus: read ADC_CR2 RSTCAL */

uint ADC_GetResetCalibrationStatus(int param_1)

{
  return *(uint *)(param_1 + 8) >> 3 & 1;
}

/**
 * ADC_StartCalibration
 *   ST ADC_StartCalibration: set ADC_CR2 CAL (bit2)
 *   address 0x0800BF6A, 10 bytes
 *   called from: adc_init_measure
 *   prototype: void ADC_StartCalibration(int adc)
 */
/* ST ADC_StartCalibration: set ADC_CR2 CAL (bit2) */

void ADC_StartCalibration(int param_1)

{
  *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) | 4;
  return;
}

/**
 * ADC_GetCalibrationStatus
 *   ST ADC_GetCalibrationStatus: read ADC_CR2 CAL
 *   address 0x0800BF74, 10 bytes
 *   called from: adc_init_measure
 *   prototype: uint ADC_GetCalibrationStatus(int adc)
 */
/* ST ADC_GetCalibrationStatus: read ADC_CR2 CAL */

uint ADC_GetCalibrationStatus(int param_1)

{
  return *(uint *)(param_1 + 8) >> 2 & 1;
}

/**
 * ADC_SoftwareStartConvCmd
 *   ST ADC_SoftwareStartConvCmd: set/clear ADC_CR2 EXTTRIG+SWSTART (0x500000)
 *   address 0x0800BF7E, 18 bytes
 *   called from: adc_check_voltage, adc_init_measure, stlink_cmd_process
 *   prototype: void ADC_SoftwareStartConvCmd(int adc,int enable)
 */
/* ST ADC_SoftwareStartConvCmd: set/clear ADC_CR2 EXTTRIG+SWSTART (0x500000) */

void ADC_SoftwareStartConvCmd(int param_1,int param_2)

{
  uint uVar1;
  
  if (param_2 == 0) {
    uVar1 = *(uint *)(param_1 + 8) & 0xffafffff;
  }
  else {
    uVar1 = *(uint *)(param_1 + 8) | 0x500000;
  }
  *(uint *)(param_1 + 8) = uVar1;
  return;
}

/**
 * ADC_RegularChannelConfig
 *   ST ADC_RegularChannelConfig: set sample time (SMPR1/2) and rank channel (SQR1/2/3) - used for F7 target-voltage (ch0/VREFINT)
 *   address 0x0800BF90, 126 bytes
 *   called from: adc_init_measure
 *   prototype: void ADC_RegularChannelConfig(int adc,int ch,int rank,int smp)
 */
/* ST ADC_RegularChannelConfig: set sample time (SMPR1/2) and rank channel (SQR1/2/3) - used for F7
   target-voltage (ch0/VREFINT) */

void ADC_RegularChannelConfig(int param_1,int param_2,int param_3,int param_4)

{
  uint uVar1;
  
  if (param_2 < 10) {
    *(uint *)(param_1 + 0x10) =
         param_4 << (param_2 * 3 & 0xffU) |
         *(uint *)(param_1 + 0x10) & ~(7 << (param_2 * 3 & 0xffU));
  }
  else {
    uVar1 = (param_2 + -10) * 3;
    *(uint *)(param_1 + 0xc) =
         param_4 << (uVar1 & 0xff) | *(uint *)(param_1 + 0xc) & ~(7 << (uVar1 & 0xff));
  }
  if (param_3 < 7) {
    uVar1 = (param_3 + -1) * 5;
    *(uint *)(param_1 + 0x34) =
         param_2 << (uVar1 & 0xff) | *(uint *)(param_1 + 0x34) & ~(0x1f << (uVar1 & 0xff));
    return;
  }
  if (param_3 < 0xd) {
    uVar1 = (param_3 + -7) * 5;
    *(uint *)(param_1 + 0x30) =
         param_2 << (uVar1 & 0xff) | *(uint *)(param_1 + 0x30) & ~(0x1f << (uVar1 & 0xff));
    return;
  }
  uVar1 = (param_3 + -0xd) * 5;
  *(uint *)(param_1 + 0x2c) =
       param_2 << (uVar1 & 0xff) | *(uint *)(param_1 + 0x2c) & ~(0x1f << (uVar1 & 0xff));
  return;
}

/**
 * ADC_GetConversionValue
 *   ST ADC_GetConversionValue: read ADC_DR (reg+0x4C) low 16 bits
 *   address 0x0800C00E, 6 bytes
 *   called from: adc_check_voltage, adc_init_measure, stlink_cmd_process
 *   prototype: uint ADC_GetConversionValue(int adc)
 */
/* ST ADC_GetConversionValue: read ADC_DR (reg+0x4C) low 16 bits */

uint ADC_GetConversionValue(int param_1)

{
  return *(uint *)(param_1 + 0x4c) & 0xffff;
}

/**
 * ADC_TempSensorVrefintCmd
 *   ST ADC_TempSensorVrefintCmd: set/clear ADC_CR2 TSVREFE (bit23) for VREFINT sensing
 *   address 0x0800C014, 20 bytes
 *   called from: adc_init_measure
 *   prototype: void ADC_TempSensorVrefintCmd(int enable)
 */
/* ST ADC_TempSensorVrefintCmd: set/clear ADC_CR2 TSVREFE (bit23) for VREFINT sensing */

void ADC_TempSensorVrefintCmd(int param_1)

{
  uint uVar1;
  
  uVar1 = DAT_40012408;
  if (param_1 == 0) {
    uVar1 = uVar1 & 0xff7fffff;
  }
  else {
    uVar1 = uVar1 | 0x800000;
  }
  DAT_40012408 = uVar1;
  return;
}

/**
 * ADC_GetFlagStatus
 *   ST ADC_GetFlagStatus: test flag bit in ADC_SR
 *   address 0x0800C038, 14 bytes
 *   called from: adc_check_voltage, adc_init_measure, stlink_cmd_process
 *   prototype: bool ADC_GetFlagStatus(uint *adc,uint flag)
 */
/* ST ADC_GetFlagStatus: test flag bit in ADC_SR */

bool ADC_GetFlagStatus(uint *param_1,uint param_2)

{
  return (*param_1 & param_2) != 0;
}

