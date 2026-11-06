/*
 * app/swim.c - SWIM (STM8 single wire)
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * swim_dma_init
 *   Configure DMA1 channels tied to TIM2/TIM4 for SWIM bit-stream transfer (capture-in buffer DAT_200011b8, pattern-out buffers); programs 3 channels at 0x40020008/0058/0080.
 *   address 0x08009A08, 262 bytes
 *   called from: swim_init
 *   prototype: void swim_dma_init(void)
 */
/* Configure DMA1 channels tied to TIM2/TIM4 for SWIM bit-stream transfer (capture-in buffer
   DAT_200011b8, pattern-out buffers); programs 3 channels at 0x40020008/0058/0080. */

void swim_dma_init(void)

{
  undefined4 in_r3;
  
  DMA_DeInit(&DAT_40020008);
  DAT_40020004 = 1;
  DAT_200011f0 = (undefined2 *)&DAT_40000834;
  DAT_20001208 = 0x100;
  DAT_200011f4 = &swim_capture_buf;
  DAT_200011f8 = 0;
  DAT_200011fc = 10;
  DAT_20001200 = 0;
  DAT_20001204 = 0x80;
  DAT_2000120c = 0x400;
  DAT_20001210 = 0;
  DAT_20001214 = 0;
  DAT_20001218 = 0;
  DMA_Init(&DAT_40020008,&DAT_200011f0);
  DMA_ITConfig(&DAT_40020008,0xc,1);
  DMA_DeInit(&DAT_40020058);
  DAT_40020004 = 0x10000;
  DAT_20001208 = 0x100;
  DAT_200011f0 = &DAT_40000820;
  DAT_200011f4 = (undefined2 *)&DAT_20001290;
  DAT_200011f8 = 0x10;
  DAT_200011fc = 1;
  DAT_20001200 = 0;
  DAT_20001204 = 0;
  DAT_2000120c = 0x400;
  DAT_20001210 = 0x20;
  DAT_20001214 = 0x2000;
  DAT_20001218 = 0;
  DMA_Init(&DAT_40020058,&DAT_200011f0);
  DMA_ITConfig(&DAT_40020058,8,1);
  DMA_Cmd(&DAT_40020058,1);
  DMA_DeInit(&DAT_40020080);
  DAT_40020004 = 0x1000000;
  DAT_20001208 = 0x100;
  DAT_200011f0 = &DAT_40000040;
  DAT_200011f4 = (undefined2 *)0x0;
  DAT_200011f8 = 0x10;
  DAT_200011fc = 0;
  DAT_20001200 = 0;
  DAT_20001204 = 0x80;
  DAT_2000120c = 0x400;
  DAT_20001210 = 0;
  DAT_20001218 = 0;
  DAT_20001214 = 0x2000;
  DMA_Init(&DAT_40020080,&DAT_200011f0);
  DMA_ITConfig(&DAT_40020080,8,1,in_r3);
  return;
}

/**
 * swim_init
 *   Enter/initialize SWIM interface: set up GPIOB SWIM pin, TIM2+TIM4 (sub_0800CD2A channels), DMA (swim_dma_init), EXTI/NVIC, SCB_SHPR3 priority, and install initial state handler pointer DAT_2000121c.
 *   address 0x08009B0E, 560 bytes
 *   called from: stlink_cmd_process
 *   prototype: void swim_init(void)
 */
/* Enter/initialize SWIM interface: set up GPIOB SWIM pin, TIM2+TIM4 (sub_0800CD2A channels), DMA
   (swim_dma_init), EXTI/NVIC, SCB_SHPR3 priority, and install initial state handler pointer
   DAT_2000121c. */

void swim_init(void)

{
  undefined1 local_28;
  undefined1 local_27;
  undefined1 local_26;
  undefined1 local_25;
  undefined2 local_24;
  undefined1 local_22;
  undefined1 local_21;
  undefined2 local_20;
  undefined1 local_1e;
  undefined1 local_1d;
  
  RCC_APB1PeriphClockCmd(5,1);
  RCC_APB2PeriphClockCmd(1,1);
  RCC_APB2PeriphClockCmd(0x10,1);
  RCC_AHBPeriphClockCmd(1);
  local_20 = 0x80;
  local_1d = 4;
  local_1e = 3;
  GPIO_Init(&GPIOB_CRL,&local_20);
  local_20 = 0x200;
  local_1d = 4;
  local_1e = 3;
  GPIO_Init(&GPIOB_CRL,&local_20);
  local_20 = 0x20;
  local_1d = 4;
  local_1e = 3;
  GPIO_Init(&GPIOB_CRL,&local_20);
  GPIO_PinRemapConfig(0x180200,1);
  GPIO_SetBits(&GPIOC_CRL,0x8000);
  GPIO_SetBits(&GPIOB_CRL,0x40);
  NVIC_SetPriorityGrouping(0x500);
  local_28 = 0xb;
  local_27 = 0;
  local_26 = 1;
  local_25 = 1;
  NVIC_Init(&local_28);
  local_28 = 0xf;
  local_27 = 0;
  local_26 = 1;
  local_25 = 1;
  NVIC_Init(&local_28);
  local_28 = 0x11;
  local_27 = 0;
  local_26 = 1;
  local_25 = 1;
  NVIC_Init(&local_28);
  local_28 = 0x1e;
  local_27 = 0;
  local_26 = 2;
  local_25 = 1;
  NVIC_Init(&local_28);
  local_28 = 6;
  local_27 = 1;
  local_26 = 1;
  local_25 = 1;
  NVIC_Init(&local_28);
  SCB_SHPR3 = 0x100000;
  swim_dma_init();
  SysTick_CounterCmd(0xfffffffe);
  SysTick_SetReload(0x34bc);
  SysTick_CounterCmd(0);
  SysTick_ITConfig(1);
  TIM_DeInit(0x40000000);
  TIM_DeInit(&DAT_40000800);
  DAT_200030e4 = 0xffff;
  DAT_200030e6 = 0x11;
  DAT_200030e8 = 0;
  TIM_TimeBaseInit(0x40000000,&DAT_200030e4);
  DAT_200030e6 = 0;
  TIM_TimeBaseInit(&DAT_40000800,&DAT_200030e4);
  DAT_40000000 = 8;
  DAT_40000018 = 0;
  DAT_4000001c = 0x5000;
  DAT_40000020 = 0x1000;
  DAT_40000800 = 0;
  DAT_40000818 = 0x2122;
  DAT_4000081c = 0x2100;
  DAT_40000808 = 100;
  DAT_40000820 = 0;
  local_24 = 0x800;
  local_21 = 0x1c;
  local_22 = 3;
  GPIO_Init(&GPIOB_CRL,&local_24);
  local_24 = 0x40;
  local_21 = 0x14;
  local_22 = 3;
  GPIO_Init(&GPIOB_CRL,&local_24);
  local_24 = 0x8000;
  local_21 = 0x10;
  local_22 = 2;
  GPIO_Init(&GPIOC_CRL,&local_24);
  SysTick_CounterCmd(0xfffffffe);
  swim_state_fn = &DAT_08009d41;
  swim_status = 0;
  return;
}

/**
 * swim_state_read_start
 *   (uncertain) SWIM state handler: begin a target read-block operation; probe target (sub_0800938A/sub_0800AB80), arm TIM2/TIM4 + DMA input capture into 0x20001230, advance state pointer.
 *   address 0x08009D42, 210 bytes
 *   prototype: void swim_state_read_start(void)
 */
/* (uncertain) SWIM state handler: begin a target read-block operation; probe target
   (sub_0800938A/sub_0800AB80), arm TIM2/TIM4 + DMA input capture into 0x20001230, advance state
   pointer. */

void swim_state_read_start(void)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  
  led_blink_activity();
  swim_scheduler_set_mode(1,0);
  iVar3 = adc_check_voltage();
  if (iVar3 == 0) {
    swim_status = 9;
  }
  else {
    iVar3 = swim_wait_line_high();
    if (iVar3 != 0) {
      SysTick_CounterCmd(0xfffffffe);
      SysTick_SetReload(180000);
      SysTick_CounterCmd(0);
      SysTick_CounterCmd(1);
      swim_capture_done_flag = 0;
      swim_status = 1;
      DAT_40000000 = 0;
      DAT_40000028 = 0x17;
      DAT_4000001c = 0x3000;
      DAT_40000010 = 0;
      DAT_40000034 = 0x6d6a;
      DAT_40000800 = 0;
      DAT_40000820 = 0;
      DAT_40000810 = 0;
      uVar1 = DAT_4000000c;
      DAT_4000000c = uVar1 & 0xedff;
      uVar1 = DAT_4000080c;
      DAT_4000080c = uVar1 & 0xfdff;
      uVar2 = DAT_40020080;
      DAT_40020080 = uVar2 & 0xfffffffe;
      DAT_40020084 = 0x12;
      DAT_4002008c = 0x20001230;
      uVar2 = DAT_40020080;
      DAT_40020080 = uVar2 | 1;
      uVar1 = DAT_4000000c;
      DAT_4000000c = uVar1 | 0x1200;
      DAT_40000014 = 0x11;
      uVar1 = DAT_4000080c;
      DAT_4000080c = uVar1 | 0x10;
      uVar1 = DAT_40000800;
      DAT_40000800 = uVar1 | 1;
      uVar1 = DAT_40000000;
      DAT_40000000 = uVar1 | 9;
      swim_state_fn = &LAB_08009e14_1;
      return;
    }
    swim_status = 0x10;
  }
  NVIC_TriggerSWInterrupt(6);
  return;
}

/**
 * swim_state_write_start
 *   (uncertain) SWIM state handler: begin writing a data block; set up out-pattern buffer (sub_0800A800) and send first header byte (sub_0800A708), advance state pointer.
 *   address 0x08009E18, 82 bytes
 *   prototype: void swim_state_write_start(void)
 */
/* (uncertain) SWIM state handler: begin writing a data block; set up out-pattern buffer
   (sub_0800A800) and send first header byte (sub_0800A708), advance state pointer. */

void swim_state_write_start(void)

{
  led_blink_activity();
  swim_scheduler_set_mode(1,swim_byte_index);
  SysTick_CounterCmd(0xfffffffe);
  SysTick_SetReload(0x34bc);
  SysTick_CounterCmd(0);
  SysTick_CounterCmd(1);
  swim_capture_done_flag = 0;
  swim_status = 1;
  swim_captured_period = 0;
  swim_send_header_dma(&DAT_20001254);
  swim_build_byte_pattern((&DAT_200011b0)[swim_retry_level]);
  swim_state_fn = &DAT_08009e6d;
  return;
}

/**
 * swim_state_read_prep
 *   (uncertain) SWIM state handler: prepare next read byte; disable IRQ pattern, if first index arm DMA capture into DAT_200011b8 else send next byte, advance state pointer.
 *   address 0x08009EAC, 134 bytes
 *   prototype: void swim_state_read_prep(void)
 */
/* (uncertain) SWIM state handler: prepare next read byte; disable IRQ pattern, if first index arm
   DMA capture into DAT_200011b8 else send next byte, advance state pointer. */

void swim_state_read_prep(void)

{
  ushort uVar1;
  uint uVar2;
  
  SysTick_CounterCmd(0xfffffffe);
  SysTick_SetReload(0x34bc);
  SysTick_CounterCmd(0);
  SysTick_CounterCmd(1);
  swim_send_pattern_dma();
  if (swim_retry_level == 0) {
    swim_error_flag = swim_retry_level;
    uVar1 = DAT_40000800;
    DAT_40000800 = uVar1 & 0xfffe;
    uVar1 = DAT_4000080c;
    DAT_4000080c = uVar1 & 0xfdff;
    uVar2 = DAT_40020008;
    DAT_40020008 = uVar2 & 0xfffffffe;
    DAT_4002000c = 0xe;
    DAT_40020014 = 0x200011b8;
    uVar2 = DAT_40020008;
    DAT_40020008 = uVar2 | 1;
    uVar1 = DAT_4000080c;
    DAT_4000080c = uVar1 | 0x200;
    uVar1 = DAT_40000800;
    DAT_40000800 = uVar1 | 1;
  }
  else {
    swim_build_byte_pattern((&DAT_200011af)[swim_retry_level]);
  }
  swim_state_fn = &DAT_08009f35;
  return;
}

/**
 * swim_read_bits
 *   (uncertain) SWIM bit-level receive: busy-wait on per-bit capture timers (DAT_200011bc..ca), decode 8 data bits + parity into *param_1, compare against expected, drive ACK/NACK resend via TIM2/TIM4/DMA; returns SWIM status byte.
 *   address 0x08009F90, 452 bytes
 *   called from: swim_state_read_loop
 *   prototype: char swim_read_bits(undefined1 *out_byte)
 */
/* (uncertain) SWIM bit-level receive: busy-wait on per-bit capture timers (DAT_200011bc..ca),
   decode 8 data bits + parity into *param_1, compare against expected, drive ACK/NACK resend via
   TIM2/TIM4/DMA; returns SWIM status byte. */

char swim_read_bits(undefined1 *param_1)

{
  undefined2 uVar1;
  ushort uVar2;
  ushort uVar3;
  ushort uVar4;
  ushort uVar5;
  ushort uVar6;
  ushort uVar7;
  ushort uVar8;
  ushort uVar9;
  ushort uVar10;
  char cVar11;
  uint uVar12;
  char cVar13;
  bool bVar14;
  
  *param_1 = 0;
  uVar10 = DAT_200011e8;
  uVar9 = DAT_200011ca;
  uVar8 = DAT_200011c8;
  uVar7 = DAT_200011c6;
  uVar6 = DAT_200011c4;
  uVar5 = DAT_200011c2;
  uVar4 = DAT_200011c0;
  uVar3 = DAT_200011be;
  uVar2 = DAT_200011bc;
  swim_capture_buf = 0;
  uVar12 = 0;
  do {
    if (DAT_200011ba != 0) {
      if (DAT_200011e8 < DAT_200011ba) {
        cVar11 = '\a';
      }
      else {
        DAT_200011ba = 0;
        while (DAT_200011bc == 0) {
          if (swim_status != '\x01') {
            return swim_status;
          }
        }
        DAT_200011bc = 0;
        if (uVar2 < DAT_200011e8) {
          uVar12 = 0x80;
        }
        while (DAT_200011be == 0) {
          if (swim_status != '\x01') {
            return swim_status;
          }
        }
        DAT_200011be = 0;
        if (uVar3 < DAT_200011e8) {
          uVar12 = uVar12 | 0x40;
        }
        while (DAT_200011c0 == 0) {
          if (swim_status != '\x01') {
            return swim_status;
          }
        }
        DAT_200011c0 = 0;
        if (uVar4 < DAT_200011e8) {
          uVar12 = uVar12 | 0x20;
        }
        while (DAT_200011c2 == 0) {
          if (swim_status != '\x01') {
            return swim_status;
          }
        }
        DAT_200011c2 = 0;
        if (uVar5 < DAT_200011e8) {
          uVar12 = uVar12 | 0x10;
        }
        while (DAT_200011c4 == 0) {
          if (swim_status != '\x01') {
            return swim_status;
          }
        }
        DAT_200011c4 = 0;
        if (uVar6 < DAT_200011e8) {
          uVar12 = uVar12 | 8;
        }
        while (DAT_200011c6 == 0) {
          if (swim_status != '\x01') {
            return swim_status;
          }
        }
        DAT_200011c6 = 0;
        if (uVar7 < DAT_200011e8) {
          uVar12 = uVar12 | 4;
        }
        while (DAT_200011c8 == 0) {
          if (swim_status != '\x01') {
            return swim_status;
          }
        }
        DAT_200011c8 = 0;
        if (uVar8 < DAT_200011e8) {
          uVar12 = uVar12 | 2;
        }
        while (DAT_200011ca == 0) {
          if (swim_status != '\x01') {
            return swim_status;
          }
        }
        DAT_200011ca = 0;
        if (uVar9 < DAT_200011e8) {
          uVar12 = uVar12 | 1;
        }
        *param_1 = (char)uVar12;
        while (DAT_200011cc == 0) {
          if (swim_status != '\x01') {
            return swim_status;
          }
        }
        bVar14 = DAT_200011cc < uVar10;
        DAT_200011cc = 0;
        DAT_40000000 = 0;
        DAT_40000040 = 1;
        if ((bool)(&DAT_0800abec)[uVar12] == bVar14) {
          cVar13 = '\x01';
          uVar1 = DAT_200011ec;
        }
        else {
          cVar13 = '\x06';
          uVar1 = DAT_200011ea;
        }
        swim_error_flag = 0;
        uVar2 = DAT_40000800;
        DAT_40000800 = uVar2 & 0xfffe;
        uVar2 = DAT_4000080c;
        DAT_4000080c = uVar2 & 0xfdff;
        uVar12 = DAT_40020008;
        DAT_40020008 = uVar12 & 0xfffffffe;
        DAT_4002000c = 0xe;
        DAT_40020014 = 0x200011b8;
        uVar12 = DAT_40020008;
        DAT_40020008 = uVar12 | 1;
        uVar2 = DAT_4000080c;
        DAT_4000080c = uVar2 | 0x200;
        uVar2 = DAT_40000800;
        DAT_40000800 = uVar2 | 1;
        SysTick_CounterCmd(0xfffffffe);
        SysTick_SetReload(0x34bc);
        SysTick_CounterCmd(0);
        SysTick_CounterCmd(1);
        __disable_irq();
        DAT_40000014 = 1;
        uVar2 = DAT_40000000;
        DAT_40000000 = uVar2 | 9;
        DAT_40000040 = uVar1;
        __enable_irq();
        do {
        } while (swim_capture_buf == 0);
        cVar11 = swim_status;
        if (swim_status == '\x01') {
          DAT_40000000 = 0;
          cVar11 = cVar13;
        }
      }
      return cVar11;
    }
  } while (swim_status == '\x01');
  return swim_status;
}

/**
 * swim_state_read_loop
 *   (uncertain) SWIM state handler: read next byte via swim_read_bits, advance byte index DAT_200011d8, handle block/page boundaries and completion, update state pointer and status.
 *   address 0x0800A154, 224 bytes
 *   prototype: void swim_state_read_loop(void)
 */
/* (uncertain) SWIM state handler: read next byte via swim_read_bits, advance byte index
   DAT_200011d8, handle block/page boundaries and completion, update state pointer and status. */

void swim_state_read_loop(void)

{
  int iVar1;
  
  if (swim_error_flag != '\x01') {
    return;
  }
  iVar1 = swim_read_bits((uint)swim_byte_index + swim_src_ptr);
  if (iVar1 == 1) {
    swim_byte_index = swim_byte_index + 1;
    swim_scheduler_set_mode(1,swim_byte_index);
    if (swim_byte_index == swim_byte_count) {
      DAT_40000000 = 0;
      DAT_4000001c = 0x5000;
      SysTick_CounterCmd(0xfffffffe);
      swim_state_fn = &DAT_08009d41;
      swim_status = 0;
      swim_scheduler_set_mode(0,swim_byte_index);
      return;
    }
    if (swim_byte_index <= swim_byte_count) {
      if ((swim_byte_index & 0x7f) != 0) {
        swim_state_fn = &LAB_08009f84_1;
        return;
      }
      DAT_40000000 = 0;
      DAT_4000001c = 0x5000;
      SysTick_CounterCmd(0xfffffffe);
      SysTick_SetReload(0x34bc);
      SysTick_CounterCmd(0);
      SysTick_CounterCmd(1);
      DAT_200011b3 = (undefined1)(swim_byte_count - swim_byte_index);
      if (0x80 < (ushort)(swim_byte_count - swim_byte_index)) {
        DAT_200011b3 = 0x80;
      }
      DAT_20001224 = DAT_20001224 + 0x80;
      DAT_200011b0 = (char)DAT_20001224;
      DAT_200011b1 = (char)((uint)DAT_20001224 >> 8);
      DAT_200011b2 = (char)((uint)DAT_20001224 >> 0x10);
      swim_retry_level = 3;
      swim_state_fn = (undefined1 *)0x8009e19;
      return;
    }
    iVar1 = 0xd;
  }
  else {
    if (iVar1 == 6) {
      swim_state_fn = &LAB_08009f84_1;
      swim_status = 6;
      goto LAB_0800a1ca;
    }
    if (iVar1 != 7) goto LAB_0800a1ca;
  }
  swim_status = (undefined1)iVar1;
LAB_0800a1ca:
  NVIC_TriggerSWInterrupt(6);
  return;
}

/**
 * swim_state_write_block
 *   (uncertain) SWIM state handler: start writing next block; arm TIM2/TIM4+capture, set up out buffer DAT_20001268, send byte, advance state pointer.
 *   address 0x0800A240, 78 bytes
 *   prototype: void swim_state_write_block(void)
 */
/* (uncertain) SWIM state handler: start writing next block; arm TIM2/TIM4+capture, set up out
   buffer DAT_20001268, send byte, advance state pointer. */

void swim_state_write_block(void)

{
  led_blink_activity();
  swim_scheduler_set_mode(1,swim_byte_index);
  SysTick_CounterCmd(0xfffffffe);
  SysTick_SetReload(0x34bc);
  SysTick_CounterCmd(0);
  SysTick_CounterCmd(1);
  swim_capture_done_flag = 0;
  swim_status = 1;
  swim_send_header_dma(&DAT_20001268);
  swim_build_byte_pattern((&DAT_200011b0)[swim_retry_level]);
  swim_state_fn = 0x800a291;
  return;
}

/**
 * swim_state_write_wait
 *   (uncertain) SWIM state handler: wait for write ACK; on timeout set error status, else swap double buffers and send next byte, advance state pointer.
 *   address 0x0800A290, 98 bytes
 *   prototype: void swim_state_write_wait(void)
 */
/* (uncertain) SWIM state handler: wait for write ACK; on timeout set error status, else swap double
   buffers and send next byte, advance state pointer. */

void swim_state_write_wait(void)

{
  undefined4 uVar1;
  
  uVar1 = swim_bit_buf_ptr;
  if (swim_capture_done_flag == '\x01') {
    if (DAT_200011e8 < swim_captured_period) {
      swim_capture_done_flag = 0;
      swim_state_fn = 0x800a241;
      swim_status = 5;
      NVIC_TriggerSWInterrupt(6);
      return;
    }
    swim_bit_buf_ptr = swim_header_table_ptr;
    swim_header_table_ptr = uVar1;
    swim_capture_done_flag = '\0';
    SysTick_CounterCmd(0xfffffffe);
    SysTick_SetReload(0x34bc);
    SysTick_CounterCmd(0);
    SysTick_CounterCmd(1);
    swim_send_pattern_dma();
    swim_build_byte_pattern((&DAT_200011af)[swim_retry_level]);
    swim_state_fn = 0x800a33d;
  }
  return;
}

/**
 * swim_state_write_resync
 *   (uncertain) SWIM state handler: re-arm TIM2/TIM4 and send next write byte (from source buffer or pattern table), advance state pointer.
 *   address 0x0800A2F8, 68 bytes
 *   prototype: void swim_state_write_resync(void)
 */
/* (uncertain) SWIM state handler: re-arm TIM2/TIM4 and send next write byte (from source buffer or
   pattern table), advance state pointer. */

void swim_state_write_resync(void)

{
  undefined1 uVar1;
  
  SysTick_CounterCmd(0xfffffffe);
  SysTick_SetReload(0x34bc);
  SysTick_CounterCmd(0);
  SysTick_CounterCmd(1);
  swim_send_pattern_dma();
  if (swim_retry_level == 0) {
    uVar1 = *(undefined1 *)((uint)swim_byte_index + swim_src_ptr);
  }
  else {
    uVar1 = (&DAT_200011af)[swim_retry_level];
  }
  swim_build_byte_pattern(uVar1);
  swim_state_fn = 0x800a33d;
  return;
}

/**
 * swim_state_write_next
 *   (uncertain) SWIM state handler: on write ACK, swap buffers and emit next byte / decrement repeat count, advance state pointer; on timeout set error.
 *   address 0x0800A33C, 132 bytes
 *   prototype: void swim_state_write_next(void)
 */
/* (uncertain) SWIM state handler: on write ACK, swap buffers and emit next byte / decrement repeat
   count, advance state pointer; on timeout set error. */

void swim_state_write_next(void)

{
  undefined1 uVar1;
  undefined4 uVar2;
  
  uVar2 = swim_bit_buf_ptr;
  if (swim_capture_done_flag == '\x01') {
    if (DAT_200011e8 < swim_captured_period) {
      swim_state_fn = 0x800a2f9;
      swim_capture_done_flag = 0;
      swim_status = 5;
      NVIC_TriggerSWInterrupt(6);
      return;
    }
    swim_bit_buf_ptr = swim_header_table_ptr;
    swim_header_table_ptr = uVar2;
    SysTick_CounterCmd(0xfffffffe);
    SysTick_SetReload(0x34bc);
    SysTick_CounterCmd(0);
    SysTick_CounterCmd(1);
    swim_send_pattern_dma();
    if (swim_retry_level == 0) {
      swim_build_byte_pattern(*(undefined1 *)((uint)swim_byte_index + swim_src_ptr + 1));
      swim_state_fn = 0x800a405;
    }
    else {
      swim_retry_level = swim_retry_level - 1;
      swim_state_fn = 0x800a33d;
      if (swim_retry_level == 0) {
        uVar1 = *(undefined1 *)((uint)swim_byte_index + swim_src_ptr);
      }
      else {
        uVar1 = (&DAT_200011af)[swim_retry_level];
      }
      swim_build_byte_pattern(uVar1);
    }
    swim_capture_done_flag = '\0';
  }
  return;
}

/**
 * swim_state_write_second
 *   (uncertain) SWIM state handler: re-arm TIM2/TIM4 and send the second data byte of the current word, advance state pointer.
 *   address 0x0800A3C8, 58 bytes
 *   prototype: void swim_state_write_second(void)
 */
/* (uncertain) SWIM state handler: re-arm TIM2/TIM4 and send the second data byte of the current
   word, advance state pointer. */

void swim_state_write_second(void)

{
  SysTick_CounterCmd(0xfffffffe);
  SysTick_SetReload(0x34bc);
  SysTick_CounterCmd(0);
  SysTick_CounterCmd(1);
  swim_send_pattern_dma();
  swim_build_byte_pattern(*(undefined1 *)((uint)swim_byte_index + swim_src_ptr + 1));
  swim_state_fn = 0x800a405;
  return;
}

/**
 * swim_state_write_advance
 *   (uncertain) SWIM state handler: on write ACK advance byte index DAT_200011d8, handle page/block boundaries and completion (restore idle state DAT_08009d41), else set error status.
 *   address 0x0800A404, 280 bytes
 *   prototype: undefined4 swim_state_write_advance(void)
 */
/* (uncertain) SWIM state handler: on write ACK advance byte index DAT_200011d8, handle page/block
   boundaries and completion (restore idle state DAT_08009d41), else set error status. */

undefined4 swim_state_write_advance(void)

{
  undefined4 uVar1;
  undefined4 in_r3;
  
  uVar1 = swim_bit_buf_ptr;
  if (swim_capture_done_flag != '\x01') {
    return in_r3;
  }
  if (DAT_200011e8 < swim_captured_period) {
    swim_state_fn = (undefined *)0x800a3c9;
    swim_status = 5;
  }
  else {
    swim_bit_buf_ptr = swim_header_table_ptr;
    swim_header_table_ptr = uVar1;
    swim_byte_index = swim_byte_index + 1;
    swim_scheduler_set_mode(1,swim_byte_index);
    swim_capture_done_flag = 0;
    if (swim_byte_index == swim_byte_count) {
      DAT_40000000 = 0;
      DAT_4000001c = 0x5000;
      SysTick_CounterCmd(0xfffffffe);
      swim_state_fn = &DAT_08009d41;
      swim_status = 0;
      uVar1 = swim_scheduler_set_mode(0,swim_byte_index,in_r3);
      return uVar1;
    }
    if (swim_byte_index <= swim_byte_count) {
      if ((swim_byte_index & 0x7f) != 0) {
        SysTick_CounterCmd(0xfffffffe);
        SysTick_SetReload(0x34bc);
        SysTick_CounterCmd(0);
        SysTick_CounterCmd(1);
        swim_send_pattern_dma();
        swim_build_byte_pattern(*(undefined1 *)((uint)swim_byte_index + swim_src_ptr + 1));
        swim_state_fn = (undefined *)0x800a405;
        return in_r3;
      }
      DAT_40000000 = 0;
      DAT_4000001c = 0x5000;
      SysTick_CounterCmd(0xfffffffe);
      SysTick_SetReload(0x34bc);
      SysTick_CounterCmd(0);
      SysTick_CounterCmd(1);
      DAT_200011b3 = (undefined1)(swim_byte_count - swim_byte_index);
      if (0x80 < (ushort)(swim_byte_count - swim_byte_index)) {
        DAT_200011b3 = 0x80;
      }
      DAT_20001224 = DAT_20001224 + 0x80;
      DAT_200011b0 = (char)DAT_20001224;
      DAT_200011b1 = (char)((uint)DAT_20001224 >> 8);
      DAT_200011b2 = (char)((uint)DAT_20001224 >> 0x10);
      swim_retry_level = 3;
      swim_state_fn = (undefined *)0x800a241;
      return in_r3;
    }
    swim_status = 0xd;
  }
  swim_capture_done_flag = 0;
  uVar1 = NVIC_TriggerSWInterrupt(6,in_r3);
  return uVar1;
}

/**
 * _uncertain_swim_state_entry_seq
 *   SWIM state-machine handler (via DAT_2000121c fn-ptr): starts entry sequence, SysTick setup, sends on TIM4/DMA, advances state to 0800A574
 *   address 0x0800A52C, 70 bytes
 *   prototype: void swim_state_entry_seq(void)
 */
/* SWIM state-machine handler (via DAT_2000121c fn-ptr): starts entry sequence, SysTick setup, sends
   on TIM4/DMA, advances state to 0800A574 */

void _uncertain_swim_state_entry_seq(void)

{
  led_blink_activity();
  swim_scheduler_set_mode(1,0);
  SysTick_CounterCmd(0xfffffffe);
  SysTick_SetReload(0x34bc);
  SysTick_CounterCmd(0);
  SysTick_CounterCmd(1);
  swim_capture_done_flag = 0;
  swim_status = 1;
  swim_send_header_dma(&swim_pattern_table0);
  swim_state_fn = 0x800a575;
  return;
}

/**
 * _uncertain_swim_state_wait_sync
 *   SWIM state: if capture-done flag set, either re-arm (reload state 0800A52C) or stop TIM2 and go to default dispatch loop 08009D41
 *   address 0x0800A574, 50 bytes
 *   prototype: void swim_state_wait_sync(void)
 */
/* SWIM state: if capture-done flag set, either re-arm (reload state 0800A52C) or stop TIM2 and go
   to default dispatch loop 08009D41 */

void _uncertain_swim_state_wait_sync(void)

{
  if (swim_capture_done_flag != '\x01') {
    return;
  }
  if (DAT_200011e8 < swim_captured_period) {
    swim_state_fn = (undefined *)0x800a52d;
    swim_capture_done_flag = 0;
    swim_status = 5;
    NVIC_TriggerSWInterrupt(6);
    return;
  }
  DAT_40000000 = 0;
  DAT_4000001c = 0x5000;
  SysTick_CounterCmd(0xfffffffe);
  swim_state_fn = &DAT_08009d41;
  swim_status = 0;
  swim_scheduler_set_mode(0,0);
  return;
}

/**
 * _uncertain_swim_state_sync_done
 *   SWIM state: compute bit timings from measured sync period DAT_200011EE, build tables, stop TIM2, return to loop 08009D41
 *   address 0x0800A5AC, 102 bytes
 *   prototype: void swim_state_sync_done(void)
 */
/* SWIM state: compute bit timings from measured sync period DAT_200011EE, build tables, stop TIM2,
   return to loop 08009D41 */

void _uncertain_swim_state_sync_done(void)

{
  uint uVar1;
  
  uVar1 = (uint)swim_sync_period;
  DAT_200011dc = (undefined2)((int)(uVar1 * 0x16) >> 7);
  DAT_200011e0 = (undefined2)((int)(uVar1 * 0x13) >> 7);
  DAT_200011de = (undefined2)((int)(uVar1 * 7) >> 7);
  DAT_200011e2 = (undefined2)((int)(uVar1 * 3) >> 7);
  swim_timing_table = (undefined2)((int)(uVar1 * 10) >> 7);
  DAT_200011e4 = DAT_200011e2;
  swim_build_timing_tables(0);
  DAT_40000000 = 0;
  DAT_4000001c = 0x5000;
  SysTick_CounterCmd(0xfffffffe);
  swim_state_fn = &DAT_08009d41;
  swim_status = 0;
  swim_scheduler_set_mode(0,0);
  return;
}

/**
 * _uncertain_swim_state_start_frame
 *   SWIM state: full TIM2(0x40000000)+TIM4(0x40000800)+DMA7(0x40020080) setup to drive the SWIM entry pulse train, advance to LAB_0800A6DC
 *   address 0x0800A618, 194 bytes
 *   prototype: void swim_state_start_frame(void)
 */
/* SWIM state: full TIM2(0x40000000)+TIM4(0x40000800)+DMA7(0x40020080) setup to drive the SWIM entry
   pulse train, advance to LAB_0800A6DC */

undefined4 _uncertain_swim_state_start_frame(void)

{
  ushort uVar1;
  uint uVar2;
  undefined4 in_r3;
  
  led_blink_activity();
  swim_scheduler_set_mode(1,0);
  SysTick_CounterCmd(0xfffffffe);
  SysTick_SetReload(0x34bc);
  SysTick_CounterCmd(0);
  SysTick_CounterCmd(1);
  swim_capture_done_flag = 0;
  swim_error_flag = 0;
  swim_status = 1;
  DAT_2000151c = 0x1e;
  DAT_2000151e = swim_sync_period + 0x1e;
  DAT_40000000 = 0;
  DAT_40000028 = 0;
  DAT_4000001c = 0x3000;
  DAT_40000010 = 0;
  DAT_40000034 = swim_sync_period + 0x23;
  DAT_40000800 = 0;
  DAT_40000820 = 0;
  DAT_40000810 = 0;
  uVar1 = DAT_4000000c;
  DAT_4000000c = uVar1 & 0xedff;
  uVar1 = DAT_4000080c;
  DAT_4000080c = uVar1 & 0xfdff;
  uVar2 = DAT_40020080;
  DAT_40020080 = uVar2 & 0xfffffffe;
  DAT_40020084 = 2;
  DAT_4002008c = 0x2000151c;
  uVar2 = DAT_40020080;
  DAT_40020080 = uVar2 | 1;
  uVar1 = DAT_4000000c;
  DAT_4000000c = uVar1 | 0x1200;
  DAT_40000014 = 0x11;
  uVar1 = DAT_4000080c;
  DAT_4000080c = uVar1 | 0x10;
  uVar1 = DAT_40000800;
  DAT_40000800 = uVar1 | 1;
  uVar1 = DAT_40000000;
  DAT_40000000 = uVar1 | 9;
  swim_state_fn = &LAB_0800a6dc_1;
  return in_r3;
}

/**
 * swim_build_byte_pattern
 *   Build the 10-cell TIM4 compare pattern for one SWIM byte into DAT_2000122C from bit table DAT_0800ABEC[param]
 *   address 0x0800A708, 94 bytes
 *   called from: swim_state_write_advance, swim_state_write_block, swim_state_write_resync, swim_state_write_next, swim_state_write_wait, swim_state_write_start, swim_state_read_prep, swim_state_write_second
 *   prototype: void swim_build_byte_pattern(int byte)
 */
/* Build the 10-cell TIM4 compare pattern for one SWIM byte into DAT_2000122C from bit table
   DAT_0800ABEC[param] */

void swim_build_byte_pattern(int param_1)

{
  char cVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  short sVar5;
  undefined2 *puVar6;
  short sVar7;
  short *psVar8;
  short *psVar9;
  int iVar10;
  
  puVar6 = swim_bit_buf_ptr;
  sVar3 = swim_bit_len_cur;
  cVar1 = (&DAT_0800abec)[param_1];
  psVar8 = swim_bit_buf_ptr + 1;
  *swim_bit_buf_ptr = 1;
  sVar4 = DAT_200011ea;
  *psVar8 = DAT_200011ea;
  psVar8 = puVar6 + 3;
  puVar6[2] = sVar3;
  sVar5 = DAT_200011ec;
  iVar10 = 8;
  sVar7 = sVar3;
  do {
    sVar2 = sVar5;
    if (-1 < param_1 << 0x18) {
      sVar2 = sVar4;
    }
    psVar9 = psVar8 + 1;
    *psVar8 = sVar2 + sVar7;
    param_1 = param_1 << 1;
    sVar2 = sVar3 + sVar7;
    sVar7 = sVar3 + sVar7;
    iVar10 = iVar10 + -1;
    psVar8 = psVar8 + 2;
    *psVar9 = sVar2;
  } while (iVar10 != 0);
  sVar3 = DAT_200011ec;
  if (cVar1 == '\0') {
    sVar3 = sVar4;
  }
  *psVar8 = sVar3 + sVar7;
  return;
}

/**
 * swim_send_pattern_dma
 *   Arm TIM2/TIM4 + DMA7 (0x40020080) with 0x14 compare values (DAT_20001228) to shift out a SWIM frame; enables EXTI0-style sw IRQ via NVIC ISER
 *   address 0x0800A774, 128 bytes
 *   called from: swim_state_write_advance, swim_state_write_resync, swim_state_write_next, swim_state_write_wait, swim_state_read_prep, swim_state_write_second
 *   prototype: void swim_send_pattern_dma(void)
 */
/* Arm TIM2/TIM4 + DMA7 (0x40020080) with 0x14 compare values (DAT_20001228) to shift out a SWIM
   frame; enables EXTI0-style sw IRQ via NVIC ISER */

void swim_send_pattern_dma(void)

{
  ushort uVar1;
  uint uVar2;
  
  DAT_40000034 = DAT_200011ea + swim_bit_len_cur * 9 + 5;
  DAT_40000800 = 0;
  DAT_40000820 = 0;
  DAT_40000810 = 0;
  uVar1 = DAT_4000000c;
  DAT_4000000c = uVar1 & 0xedff;
  uVar1 = DAT_4000080c;
  DAT_4000080c = uVar1 & 0xfdff;
  uVar2 = DAT_40020080;
  DAT_40020080 = uVar2 & 0xfffffffe;
  DAT_40020084 = 0x14;
  DAT_4002008c = swim_header_table_ptr;
  uVar2 = DAT_40020080;
  DAT_40020080 = uVar2 | 1;
  uVar1 = DAT_4000000c;
  DAT_4000000c = uVar1 | 0x1200;
  DAT_e000e180 = 0x100000;
  DAT_40000014 = 0x11;
  uVar1 = DAT_4000080c;
  DAT_4000080c = uVar1 | 0x10;
  uVar1 = DAT_40000800;
  DAT_40000800 = uVar1 | 1;
  uVar1 = DAT_40000000;
  DAT_40000000 = uVar1 | 9;
  return;
}

/**
 * swim_send_header_dma
 *   Arm TIM2/TIM4 + DMA7 with 10 compare values (param) to send the SWIM command header; enables sw IRQ
 *   address 0x0800A800, 142 bytes
 *   called from: swim_state_write_block, swim_state_write_start, _uncertain_swim_state_entry_seq
 *   prototype: void swim_send_header_dma(void *table)
 */
/* Arm TIM2/TIM4 + DMA7 with 10 compare values (param) to send the SWIM command header; enables sw
   IRQ */

void swim_send_header_dma(undefined4 param_1)

{
  ushort uVar1;
  uint uVar2;
  
  DAT_40000000 = 0;
  DAT_40000028 = 0;
  DAT_4000001c = 0x3000;
  DAT_40000010 = 0;
  DAT_40000034 = DAT_200011ea + swim_bit_len_cur * 4 + 5;
  DAT_40000800 = 0;
  DAT_40000820 = 0;
  DAT_40000810 = 0;
  uVar1 = DAT_4000000c;
  DAT_4000000c = uVar1 & 0xedff;
  uVar1 = DAT_4000080c;
  DAT_4000080c = uVar1 & 0xfdff;
  uVar2 = DAT_40020080;
  DAT_40020080 = uVar2 & 0xfffffffe;
  DAT_40020084 = 10;
  DAT_4002008c = param_1;
  uVar2 = DAT_40020080;
  DAT_40020080 = uVar2 | 1;
  uVar1 = DAT_4000000c;
  DAT_4000000c = uVar1 | 0x1200;
  DAT_e000e180 = 0x100000;
  uVar1 = DAT_4000080c;
  DAT_4000080c = uVar1 | 0x10;
  DAT_40000014 = 0x11;
  uVar1 = DAT_40000800;
  DAT_40000800 = uVar1 | 1;
  uVar1 = DAT_40000000;
  DAT_40000000 = uVar1 | 9;
  return;
}

/**
 * swim_build_timing_tables
 *   Precompute the three SWIM sync/bit compare tables (DAT_2000127C/20001254/20001268) from timing constants; param selects fast/slow set
 *   address 0x0800A89C, 138 bytes
 *   called from: _uncertain_swim_state_sync_done, stlink_cmd_process
 *   prototype: void swim_build_timing_tables(int mode)
 */
/* Precompute the three SWIM sync/bit compare tables (DAT_2000127C/20001254/20001268) from timing
   constants; param selects fast/slow set */

void swim_build_timing_tables(int param_1)

{
  ushort uVar1;
  int iVar2;
  uint uVar3;
  short sVar4;
  uint uVar5;
  undefined2 *puVar6;
  short *psVar7;
  ushort uVar8;
  
  DAT_200011ea = DAT_200011e0;
  DAT_200011ec = DAT_200011e4;
  swim_bit_len_cur = DAT_200011dc;
  if (param_1 == 1) {
    DAT_200011ea = DAT_200011de;
    DAT_200011ec = DAT_200011e2;
    swim_bit_len_cur = swim_timing_table;
  }
  DAT_200011e8 = swim_bit_len_cur >> 1;
  iVar2 = 0;
  do {
    uVar3 = 0;
    puVar6 = &swim_pattern_table0;
    while( true ) {
      while( true ) {
        uVar5 = 1;
        psVar7 = puVar6 + 1;
        *puVar6 = 1;
        uVar1 = swim_bit_len_cur;
        uVar8 = swim_bit_len_cur;
        do {
          sVar4 = DAT_200011ec;
          if (-1 < (int)(uVar3 << 0x18)) {
            sVar4 = DAT_200011ea;
          }
          *psVar7 = sVar4 + psVar7[-1];
          if (uVar5 < 5) {
            psVar7[1] = uVar8;
          }
          uVar3 = (uVar3 & 0x7fff) << 1;
          uVar5 = uVar5 + 1;
          psVar7 = psVar7 + 2;
          uVar8 = uVar8 + uVar1;
        } while (uVar5 < 6);
        iVar2 = iVar2 + 1;
        if (2 < iVar2) {
          return;
        }
        if (iVar2 != 1) break;
        uVar3 = 0x18;
        puVar6 = &DAT_20001254;
      }
      if (iVar2 != 2) break;
      uVar3 = 0x28;
      puVar6 = &DAT_20001268;
    }
  } while( true );
}

/**
 * swim_rx_complete_dispatch
 *   End-of-transfer dispatcher keyed on result/state code DAT_200011B5 (4..0x10): handles retries (DAT_200030F4/F6), reports status via sub_0800C808, returns to loop
 *   address 0x0800A934, 164 bytes
 *   prototype: void swim_rx_complete_dispatch(void)
 */
/* End-of-transfer dispatcher keyed on result/state code DAT_200011B5 (4..0x10): handles retries
   (DAT_200030F4/F6), reports status via sub_0800C808, returns to loop */

void swim_rx_complete_dispatch(void)

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
 * swim_dma_ch7_err_isr
 *   SWIM DMA1 channel-7 transfer-error handler: checks DMA1_ISR bit27, clears via IFCR 0x8000000, flags error state 0x0B
 *   address 0x0800A9DC, 42 bytes
 *   prototype: void swim_dma_ch7_err_isr(void)
 */
/* SWIM DMA1 channel-7 transfer-error handler: checks DMA1_ISR bit27, clears via IFCR 0x8000000,
   flags error state 0x0B */

void swim_dma_ch7_err_isr(void)

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

/**
 * swim_dma_ch1_isr
 *   SWIM DMA1 channel-1 TE/HT handler: clears IFCR flags 8/4, sets error/continue flags
 *   address 0x0800AA1E, 56 bytes
 *   prototype: void swim_dma_ch1_isr(void)
 */
/* SWIM DMA1 channel-1 TE/HT handler: clears IFCR flags 8/4, sets error/continue flags */

void swim_dma_ch1_isr(void)

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
 * swim_capture_complete
 *   SWIM input-capture complete: latch TIM4_CCR (DAT_40000840) into DAT_200011D4, disable CC IRQ, set capture-done flag, pend NVIC
 *   address 0x0800AA56, 50 bytes
 *   prototype: void swim_capture_complete(void)
 */
/* SWIM input-capture complete: latch TIM4_CCR (DAT_40000840) into DAT_200011D4, disable CC IRQ, set
   capture-done flag, pend NVIC */

void swim_capture_complete(void)

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

/**
 * swim_abort_to_idle
 *   Stop SysTick and move SWIM to state 4 (abort), pend IRQ
 *   address 0x0800AA88, 32 bytes
 *   prototype: void swim_abort_to_idle(void)
 */
/* Stop SysTick and move SWIM to state 4 (abort), pend IRQ */

void swim_abort_to_idle(void)

{
  undefined4 unaff_r7;
  
  SysTick_CounterCmd(0xfffffffe);
  SysTick_CounterCmd(0);
  swim_status = 4;
  NVIC_TriggerSWInterrupt(6,unaff_r7);
  return;
}

/**
 * swim_deinit
 *   Tear down SWIM: NVIC clear, GPIOB/GPIOC pin reconfig, TIM2/TIM4 reset, DMA1 ch1/5/7 DeInit, restore loop state
 *   address 0x0800AAA8, 210 bytes
 *   called from: stlink_cmd_debug_connect, stlink_cmd_process
 *   prototype: undefined8 swim_deinit(void)
 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Tear down SWIM: NVIC clear, GPIOB/GPIOC pin reconfig, TIM2/TIM4 reset, DMA1 ch1/5/7 DeInit,
   restore loop state */

undefined8 swim_deinit(void)

{
  undefined4 in_r3;
  undefined1 local_10;
  undefined1 local_f;
  undefined2 local_e;
  undefined2 local_c;
  undefined1 uStack_a;
  undefined1 local_9;
  
  local_c = (undefined2)in_r3;
  uStack_a = (undefined1)((uint)in_r3 >> 0x10);
  local_9 = (undefined1)((uint)in_r3 >> 0x18);
  local_10 = 0xb;
  local_f = 0;
  local_e = 0;
  NVIC_Init(&local_10);
  local_10 = 0xf;
  NVIC_Init(&local_10);
  local_10 = 0xb;
  NVIC_Init(&local_10);
  local_10 = 0x1e;
  NVIC_Init(&local_10);
  local_10 = 6;
  NVIC_Init(&local_10);
  local_c = 0x40;
  local_9 = 4;
  GPIO_Init(&GPIOB_CRL,&local_c);
  local_c = 0x800;
  GPIO_Init(&GPIOB_CRL,&local_c);
  local_c = 0x8000;
  GPIO_Init(&GPIOC_CRL,&local_c);
  TIM_DeInit(0x40000000);
  TIM_DeInit(&DAT_40000800);
  jtag_spi_reset_pulse();
  DMA_DeInit(&DAT_40020008);
  DMA_DeInit(&DAT_40020058);
  DMA_DeInit(&DAT_40020080);
  RCC_APB1PeriphClockCmd(5,0);
  RCC_APB2PeriphClockCmd(1,0);
  SysTick_CounterCmd(0xfffffffe);
  SysTick_CounterCmd(0);
  swim_state_fn = &DAT_08009d41;
  _swim_capture_done_flag = 0;
  swim_error_flag = 0;
  return CONCAT17(local_9,CONCAT16(uStack_a,CONCAT24(local_c,CONCAT22(local_e,CONCAT11(local_f,
                                                  local_10)))));
}

/**
 * swim_wait_line_high
 *   Debounce loop: sample SWIM line (GPIOB pin7) up to 0x14 times, require 5 consecutive highs
 *   address 0x0800AB80, 54 bytes
 *   called from: swim_state_read_start
 *   prototype: void swim_wait_line_high(void)
 */
/* Debounce loop: sample SWIM line (GPIOB pin7) up to 0x14 times, require 5 consecutive highs */

void swim_wait_line_high(void)

{
  int iVar1;
  char cVar2;
  byte bVar3;
  bool bVar4;
  
  bVar3 = 0;
  cVar2 = '\x14';
  do {
    iVar1 = 0x30;
    do {
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
    iVar1 = GPIO_ReadInputDataBit(&GPIOB_CRL,0x80);
    if (iVar1 == 1) {
      bVar3 = bVar3 + 1;
      if (4 < bVar3) {
        return;
      }
    }
    else {
      bVar3 = 0;
    }
    bVar4 = cVar2 != '\0';
    cVar2 = cVar2 + -1;
  } while (bVar4);
  return;
}

/**
 * swim_scheduler_set_mode
 *   Set SWIM/command scheduler state (DAT_2000308C/8D), (re)configure line via sub_08009438, under IRQ lock
 *   address 0x0800C808, 46 bytes
 *   called from: swim_state_write_advance, swim_state_read_loop, _uncertain_swim_state_sync_done, swim_state_write_block, _uncertain_swim_state_start_frame, swim_state_read_start, swim_state_write_start, _uncertain_swim_state_entry_seq ...
 *   prototype: void swim_scheduler_set_mode(int mode,undefined2,undefined4,undefined4)
 */
/* Set SWIM/command scheduler state (DAT_2000308C/8D), (re)configure line via sub_08009438, under
   IRQ lock */

void swim_scheduler_set_mode(int param_1,undefined2 param_2,undefined4 param_3,undefined4 param_4)

{
  __disable_irq();
  if (param_1 != 1) {
    stlink_active_subcmd = 0;
  }
  swim_sched_mode = (undefined1)param_1;
  DAT_2000308d = param_2;
  led_set_result(-(uint)(param_1 == 0) >> 0x1f);
  __enable_irq(param_4);
  return;
}

/**
 * swim_init_counters
 *   Initialize SWIM retry/length counters DAT_20003080..83 depending on mode
 *   address 0x0800C836, 38 bytes
 *   called from: stlink_cmd_process
 *   prototype: void swim_init_counters(int mode)
 */
/* Initialize SWIM retry/length counters DAT_20003080..83 depending on mode */

void swim_init_counters(int param_1)

{
  if (param_1 == 1) {
    DAT_20003080 = 0;
  }
  else {
    DAT_20003080 = 2;
  }
  DAT_20003081 = 1;
  DAT_20003082 = 2;
  DAT_20003083 = 7;
  return;
}

/**
 * swim_command_dispatch
 *   F4 SWIM command dispatcher (switch): set next state fn-ptr DAT_2000121C (enter/sync/read/write) or drive SRST pin, re-arm scheduler
 *   address 0x0800C880, 106 bytes
 *   called from: stlink_cmd_process
 *   prototype: undefined4 swim_command_dispatch(undefined4 cmd)
 */
/* F4 SWIM command dispatcher (switch): set next state fn-ptr DAT_2000121C (enter/sync/read/write)
   or drive SRST pin, re-arm scheduler */

undefined4 swim_command_dispatch(undefined4 param_1)

{
  undefined4 uVar1;
  undefined4 unaff_r7;
  
  switch(param_1) {
  case 0:
    swim_state_fn = 0x8009d43;
    break;
  case 1:
    swim_retry_count_a = 0;
    swim_state_fn = 0x800a52d;
    break;
  case 2:
    swim_state_fn = 0x800a619;
    break;
  case 3:
    GPIO_ResetBits(&GPIOB_CRL,0x40);
    goto LAB_0800c85c;
  case 4:
    GPIO_SetBits(&GPIOB_CRL,0x40);
LAB_0800c85c:
    __disable_irq();
    stlink_active_subcmd = 0;
    swim_sched_mode = 0;
    DAT_2000308d = 0;
    led_set_result(1);
    uVar1 = __enable_irq(unaff_r7);
    return uVar1;
  default:
    return param_1;
  }
  return unaff_r7;
}

/**
 * swim_read_setup
 *   Set up a SWIM memory read: length/address params DAT_200011B0..D6, next state 0800A241
 *   address 0x0800C8DC, 64 bytes
 *   called from: stlink_cmd_process
 *   prototype: void swim_read_setup(int len,undefined4 addr,undefined4)
 */
/* Set up a SWIM memory read: length/address params DAT_200011B0..D6, next state 0800A241 */

void swim_read_setup(int param_1,undefined4 param_2,undefined4 param_3)

{
  swim_retry_count_a = 0;
  swim_retry_level = 3;
  swim_byte_index = 0;
  swim_byte_count = (short)param_1;
  DAT_200011b3 = (undefined1)param_1;
  if (0x80 < param_1) {
    DAT_200011b3 = 0x80;
  }
  DAT_200011b2 = (char)((uint)param_2 >> 0x10);
  DAT_200011b0 = (char)param_2;
  DAT_200011b1 = (char)((uint)param_2 >> 8);
  DAT_20001224 = param_2;
  swim_src_ptr = param_3;
  swim_state_fn = 0x800a241;
  return;
}

/**
 * swim_write_setup
 *   Set up a SWIM memory write: length/address params, next state 08009E19
 *   address 0x0800C920, 74 bytes
 *   called from: stlink_cmd_process
 *   prototype: void swim_write_setup(int len,undefined4 addr,undefined4)
 */
/* Set up a SWIM memory write: length/address params, next state 08009E19 */

void swim_write_setup(int param_1,undefined4 param_2,undefined4 param_3)

{
  swim_retry_count_a = 0;
  swim_retry_count_b = 0;
  swim_retry_level = 3;
  swim_byte_index = 0;
  swim_byte_count = (short)param_1;
  DAT_200011b3 = (undefined1)param_1;
  if (0x80 < param_1) {
    DAT_200011b3 = 0x80;
  }
  DAT_200011b2 = (char)((uint)param_2 >> 0x10);
  DAT_200011b0 = (char)param_2;
  DAT_200011b1 = (char)((uint)param_2 >> 8);
  DAT_20001224 = param_2;
  swim_src_ptr = param_3;
  swim_state_fn = 0x8009e19;
  return;
}

