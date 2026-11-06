/*
 * app/flash.c - Internal flash programming
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * FLASH_SetLatency
 *   ST FLASH_SetLatency: set FLASH_ACR LATENCY
 *   address 0x0800BD50, 22 bytes
 *   called from: rcc_set_system_clock
 *   prototype: void FLASH_SetLatency(uint latency)
 */
/* ST FLASH_SetLatency: set FLASH_ACR LATENCY */

void FLASH_SetLatency(uint param_1)

{
  uint uVar1;
  
  uVar1 = FLASH_ACR;
  FLASH_ACR = uVar1 & 0x38;
  uVar1 = FLASH_ACR;
  FLASH_ACR = param_1 | uVar1;
  return;
}

/**
 * FLASH_PrefetchBufferCmd
 *   ST FLASH_PrefetchBufferCmd: set/clear FLASH_ACR PRFTBE (bit4)
 *   address 0x0800BD66, 12 bytes
 *   called from: rcc_set_system_clock
 *   prototype: void FLASH_PrefetchBufferCmd(uint state)
 */
/* ST FLASH_PrefetchBufferCmd: set/clear FLASH_ACR PRFTBE (bit4) */

void FLASH_PrefetchBufferCmd(uint param_1)

{
  uint uVar1;
  
  uVar1 = FLASH_ACR;
  FLASH_ACR = uVar1 & 0xffffffef;
  uVar1 = FLASH_ACR;
  FLASH_ACR = param_1 | uVar1;
  return;
}

/**
 * FLASH_Unlock
 *   ST FLASH_Unlock: write KEY1/KEY2 to FLASH_KEYR
 *   address 0x0800BD72, 18 bytes
 *   called from: anticlone_reflash_signature, anticlone_background_write
 *   prototype: void FLASH_Unlock(void)
 */
/* ST FLASH_Unlock: write KEY1/KEY2 to FLASH_KEYR */

void FLASH_Unlock(void)

{
  FLASH_KEYR = 0x45670123;
  FLASH_KEYR = 0xcdef89ab;
  return;
}

/**
 * FLASH_ErasePage
 *   ST FLASH_ErasePage: CR PER, AR=addr, STRT, wait
 *   address 0x0800BD84, 62 bytes
 *   called from: anticlone_reflash_signature, anticlone_background_write
 *   prototype: void FLASH_ErasePage(undefined4 addr)
 */
/* ST FLASH_ErasePage: CR PER, AR=addr, STRT, wait */

void FLASH_ErasePage(undefined4 param_1)

{
  uint uVar1;
  int iVar2;
  
  iVar2 = FLASH_WaitForLastOperation(0xfff);
  if (iVar2 == 4) {
    uVar1 = FLASH_CR;
    FLASH_CR = uVar1 | 2;
    FLASH_AR = param_1;
    uVar1 = FLASH_CR;
    FLASH_CR = uVar1 | 0x40;
    iVar2 = FLASH_WaitForLastOperation(0xfff);
    if (iVar2 != 1) {
      uVar1 = FLASH_CR;
      FLASH_CR = uVar1 & 0x1ffd;
    }
  }
  return;
}

/**
 * FLASH_ProgramWord
 *   ST FLASH_ProgramWord: program 32-bit value as two halfwords with status waits
 *   address 0x0800BDC2, 64 bytes
 *   called from: anticlone_reflash_signature
 *   prototype: void FLASH_ProgramWord(undefined2 *addr,undefined4 data)
 */
/* ST FLASH_ProgramWord: program 32-bit value as two halfwords with status waits */

void FLASH_ProgramWord(undefined2 *param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  
  iVar2 = FLASH_WaitForLastOperation(0xf);
  if (iVar2 == 4) {
    uVar1 = FLASH_CR;
    FLASH_CR = uVar1 | 1;
    *param_1 = (short)param_2;
    iVar2 = FLASH_WaitForLastOperation(0xf);
    if (iVar2 == 4) {
      param_1[1] = (short)((uint)param_2 >> 0x10);
      iVar2 = FLASH_WaitForLastOperation(0xf);
    }
    if (iVar2 != 1) {
      uVar1 = FLASH_CR;
      FLASH_CR = uVar1 & 0x1ffe;
    }
  }
  return;
}

/**
 * FLASH_ProgramHalfWord
 *   ST FLASH_ProgramHalfWord: CR PG, write halfword, wait
 *   address 0x0800BE02, 50 bytes
 *   called from: anticlone_background_write
 *   prototype: void FLASH_ProgramHalfWord(undefined2 *addr,undefined2 data)
 */
/* ST FLASH_ProgramHalfWord: CR PG, write halfword, wait */

void FLASH_ProgramHalfWord(undefined2 *param_1,undefined2 param_2)

{
  uint uVar1;
  int iVar2;
  
  iVar2 = FLASH_WaitForLastOperation(0xf);
  if (iVar2 == 4) {
    uVar1 = FLASH_CR;
    FLASH_CR = uVar1 | 1;
    *param_1 = param_2;
    iVar2 = FLASH_WaitForLastOperation(0xf);
    if (iVar2 != 1) {
      uVar1 = FLASH_CR;
      FLASH_CR = uVar1 & 0x1ffe;
    }
  }
  return;
}

/**
 * FLASH_WaitForLastOperation
 *   ST FLASH_GetStatus/WaitForLastOperation: poll FLASH_SR BSY/errors with timeout
 *   address 0x0800BE34, 106 bytes
 *   called from: FLASH_ErasePage, FLASH_ProgramWord, FLASH_ProgramHalfWord
 *   prototype: undefined8 FLASH_WaitForLastOperation(int timeout,undefined4,undefined4,int)
 */
/* ST FLASH_GetStatus/WaitForLastOperation: poll FLASH_SR BSY/errors with timeout */

undefined8 FLASH_WaitForLastOperation(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int iVar2;
  undefined4 local_8;
  
  iVar1 = FLASH_SR;
  iVar2 = 4;
  local_8 = param_4;
  if (iVar1 << 0x1f < 0) {
    iVar2 = 1;
  }
  else {
    iVar1 = FLASH_SR;
    if (iVar1 << 0x1d < 0) {
      iVar2 = 2;
    }
    else {
      iVar1 = FLASH_SR;
      if (iVar1 << 0x1b < 0) {
        iVar2 = 3;
      }
    }
  }
  while (iVar2 == 1) {
    if (param_1 == 0) goto LAB_0800be96;
    for (local_8 = 0xff; local_8 != 0; local_8 = local_8 + -1) {
    }
    iVar1 = FLASH_SR;
    iVar2 = 4;
    if (iVar1 << 0x1f < 0) {
      iVar2 = 1;
    }
    else {
      iVar1 = FLASH_SR;
      if (iVar1 << 0x1d < 0) {
        iVar2 = 2;
      }
      else {
        iVar1 = FLASH_SR;
        if (iVar1 << 0x1b < 0) {
          iVar2 = 3;
        }
      }
    }
    param_1 = param_1 + -1;
  }
  if (param_1 == 0) {
LAB_0800be96:
    iVar2 = 5;
  }
  return CONCAT44(local_8,iVar2);
}

