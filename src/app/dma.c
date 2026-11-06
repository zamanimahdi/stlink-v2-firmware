/*
 * app/dma.c - DMA
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * DMA_DeInit
 *   ST DMA_DeInit: disable channel, clear CCR/CNDTR/CPAR/CMAR, clear matching IFCR flag group
 *   address 0x0800B3E8, 122 bytes
 *   called from: jtag_disconnect, swim_deinit, usart_bridge_init, jtag_spi_dma_init, swd_connect, swim_dma_init
 *   prototype: void DMA_DeInit(uint *chan)
 */
/* ST DMA_DeInit: disable channel, clear CCR/CNDTR/CPAR/CMAR, clear matching IFCR flag group */

void DMA_DeInit(uint *param_1)

{
  uint uVar1;
  
  *param_1 = *param_1 & 0xfffffffe;
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  if (param_1 == &DAT_40020008) {
    uVar1 = DAT_40020004;
    uVar1 = uVar1 | 0xf;
  }
  else if (param_1 == &DAT_4002001c) {
    uVar1 = DAT_40020004;
    uVar1 = uVar1 | 0xf0;
  }
  else if (param_1 == &DAT_40020030) {
    uVar1 = DAT_40020004;
    uVar1 = uVar1 | 0xf00;
  }
  else if (param_1 == (uint *)&UNK_40020044) {
    uVar1 = DAT_40020004;
    uVar1 = uVar1 | 0xf000;
  }
  else if (param_1 == &DAT_40020058) {
    uVar1 = DAT_40020004;
    uVar1 = uVar1 | 0xf0000;
  }
  else if (param_1 == (uint *)&UNK_4002006c) {
    uVar1 = DAT_40020004;
    uVar1 = uVar1 | 0xf00000;
  }
  else {
    if (param_1 != &DAT_40020080) {
      return;
    }
    uVar1 = DAT_40020004;
    uVar1 = uVar1 | 0xf000000;
  }
  DAT_40020004 = uVar1;
  return;
}

/**
 * DMA_Init
 *   ST DMA_Init: program CCR from init-struct, CNDTR, CPAR, CMAR
 *   address 0x0800B462, 54 bytes
 *   called from: usart_bridge_init, jtag_spi_dma_init, jtag_spi_dma_transfer, swim_dma_init
 *   prototype: void DMA_Init(uint *chan,uint *init)
 */
/* ST DMA_Init: program CCR from init-struct, CNDTR, CPAR, CMAR */

void DMA_Init(uint *param_1,uint *param_2)

{
  *param_1 = *param_1 & 0xffff800f | param_2[2] | param_2[8] | param_2[4] | param_2[5] | param_2[6]
             | param_2[7] | param_2[9] | param_2[10];
  param_1[1] = param_2[3];
  param_1[2] = *param_2;
  param_1[3] = param_2[1];
  return;
}

/**
 * DMA_Cmd
 *   ST DMA_Cmd: set/clear channel EN (CCR bit0)
 *   address 0x0800B498, 18 bytes
 *   called from: usart_bridge_close, jtag_spi_dma_transfer, usart_bridge_set_baud, swim_dma_init
 *   prototype: void DMA_Cmd(uint *chan,int enable)
 */
/* ST DMA_Cmd: set/clear channel EN (CCR bit0) */

void DMA_Cmd(uint *param_1,int param_2)

{
  uint uVar1;
  
  if (param_2 == 0) {
    uVar1 = *param_1 & 0xfffffffe;
  }
  else {
    uVar1 = *param_1 | 1;
  }
  *param_1 = uVar1;
  return;
}

/**
 * DMA_ITConfig
 *   ST DMA_ITConfig: set/clear interrupt enable bits in CCR
 *   address 0x0800B4AA, 16 bytes
 *   called from: usart_bridge_init, swd_connect, swim_dma_init
 *   prototype: void DMA_ITConfig(uint *chan,uint it,int enable)
 */
/* ST DMA_ITConfig: set/clear interrupt enable bits in CCR */

void DMA_ITConfig(uint *param_1,uint param_2,int param_3)

{
  if (param_3 == 0) {
    param_2 = *param_1 & ~param_2;
  }
  else {
    param_2 = param_2 | *param_1;
  }
  *param_1 = param_2;
  return;
}

/**
 * DMA_GetCurrDataCounter
 *   ST DMA_GetCurrDataCounter: read CNDTR (reg+4) low 16 bits
 *   address 0x0800B4BA, 6 bytes
 *   called from: usart_bridge_poll_rx
 *   prototype: uint DMA_GetCurrDataCounter(int chan)
 */
/* ST DMA_GetCurrDataCounter: read CNDTR (reg+4) low 16 bits */

uint DMA_GetCurrDataCounter(int param_1)

{
  return *(uint *)(param_1 + 4) & 0xffff;
}

/**
 * APP_DMA1_Channel1_IRQHandler
 *   DMA1 Ch1 ISR: on TC/HT clears flag, stops timers, sets state 0x0B, schedules task
 *   address 0x0800E09C, 4 bytes
 *   prototype: void APP_DMA1_Channel1_IRQHandler(void)
 */
/* DMA1 Ch1 ISR: on TC/HT clears flag, stops timers, sets state 0x0B, schedules task */

void APP_DMA1_Channel1_IRQHandler(void)

{
  int iVar1;
  
  iVar1 = DAT_40020000;
  if (iVar1 << 0x1c < 0) {
    DAT_40020004 = 8;
    DAT_40000800 = 0;
    DAT_40000000 = 0;
    swim_status = 0xb;
    NVIC_TriggerSWInterrupt(6);
  }
  iVar1 = DAT_40020000;
  if (iVar1 << 0x1d < 0) {
    DAT_40020004 = 4;
    swim_error_flag = 1;
  }
  return;
}

/**
 * APP_DMA1_Channel2_IRQHandler
 *   DMA1 Ch2 ISR: clears Ch2 global interrupt flag
 *   address 0x0800E0A0, 14 bytes
 *   prototype: void APP_DMA1_Channel2_IRQHandler(void)
 */
/* DMA1 Ch2 ISR: clears Ch2 global interrupt flag */

void APP_DMA1_Channel2_IRQHandler(void)

{
  DAT_40020004 = 0x20;
  return;
}

/**
 * APP_DMA1_Channel3_IRQHandler
 *   DMA1 Ch3 ISR: clears Ch3 global interrupt flag
 *   address 0x0800E0A6, 8 bytes
 *   prototype: void APP_DMA1_Channel3_IRQHandler(void)
 */
/* DMA1 Ch3 ISR: clears Ch3 global interrupt flag */

void APP_DMA1_Channel3_IRQHandler(void)

{
  DAT_40020004 = 0x200;
  return;
}

/**
 * APP_DMA1_Channel4_IRQHandler
 *   DMA1 Ch4 ISR: clears Ch4 global interrupt flag
 *   address 0x0800E0AE, 10 bytes
 *   prototype: void APP_DMA1_Channel4_IRQHandler(void)
 */
/* DMA1 Ch4 ISR: clears Ch4 global interrupt flag */

void APP_DMA1_Channel4_IRQHandler(void)

{
  DAT_40020004 = 0x2000;
  return;
}

/**
 * APP_DMA1_Channel5_IRQHandler
 *   DMA1 Ch5 ISR: double-buffer SWIM/USART bridge transfer handling, toggles ping-pong buffers and restarts channel
 *   address 0x0800E0B8, 16 bytes
 *   prototype: uint16_t APP_DMA1_Channel5_IRQHandler(void)
 */
/* DMA1 Ch5 ISR: double-buffer SWIM/USART bridge transfer handling, toggles ping-pong buffers and
   restarts channel */

undefined2 APP_DMA1_Channel5_IRQHandler(void)

{
  ushort uVar1;
  uint uVar2;
  undefined2 uVar3;
  undefined2 in_r3;
  int iVar4;
  
  if (g_connect_phase == '\x03') {
    iVar4 = DAT_40020000;
    if (-1 < iVar4 << 0xc) {
      return 0;
    }
    DAT_40020004 = 0x80000;
    DAT_40000800 = 0;
    DAT_40000000 = 0;
    swim_status = 0xb;
    uVar3 = NVIC_TriggerSWInterrupt(6);
    return uVar3;
  }
  iVar4 = DAT_40020000;
  if (iVar4 << 0xe < 0) {
    uVar2 = DAT_40020004;
    DAT_40020004 = uVar2 | 0x30000;
    if ((&DAT_2000000a)[-DAT_20000018] != (&DAT_20000006)[-DAT_20000018]) {
      DAT_20000001 = 1;
      uVar1 = DAT_4001380c;
      DAT_4001380c = uVar1 & 0xdfff;
      uVar1 = DAT_40013800;
      if ((int)((uint)uVar1 << 0x1c) < 0) {
        in_r3 = DAT_40013804;
      }
      return in_r3;
    }
    iVar4 = 1 - DAT_20000018;
    DAT_20000018 = iVar4;
    (&DAT_20000008)[iVar4] = 0;
    uVar2 = DAT_40020058;
    DAT_40020058 = uVar2 & 0xfffffffe;
    DAT_4002005c = (uint)(ushort)(&usart_bridge_rx_len)[iVar4];
    DAT_40020064 = &DAT_20000020 + iVar4 * 0x800;
    uVar2 = DAT_40020058;
    DAT_40020058 = uVar2 | 1;
    uVar1 = DAT_4001380c;
    DAT_4001380c = uVar1 | 0x2000;
    return in_r3;
  }
  return 0;
}

/**
 * APP_DMA1_Channel6_IRQHandler
 *   DMA1 Ch6 ISR: clears Ch6 global interrupt flag
 *   address 0x0800E0C8, 8 bytes
 *   prototype: void APP_DMA1_Channel6_IRQHandler(void)
 */
/* DMA1 Ch6 ISR: clears Ch6 global interrupt flag */

void APP_DMA1_Channel6_IRQHandler(void)

{
  DAT_40020004 = 0x200000;
  return;
}

/**
 * APP_DMA1_Channel7_IRQHandler
 *   DMA1 Ch7 ISR: on TC clears flag, stops timers, sets state 0x0B, schedules task
 *   address 0x0800E0D0, 4 bytes
 *   prototype: void APP_DMA1_Channel7_IRQHandler(void)
 */
/* DMA1 Ch7 ISR: on TC clears flag, stops timers, sets state 0x0B, schedules task */

void APP_DMA1_Channel7_IRQHandler(void)

{
  int iVar1;
  
  iVar1 = DAT_40020000;
  if (iVar1 << 4 < 0) {
    DAT_40020004 = 0x8000000;
    DAT_40000800 = 0;
    DAT_40000000 = 0;
    swim_status = 0xb;
    NVIC_TriggerSWInterrupt(6);
    return;
  }
  return;
}

