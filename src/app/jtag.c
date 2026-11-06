/*
 * app/jtag.c - JTAG bit-bang
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * jtag_shift_short
 *   Raw JTAG transfer (SPI/DMA path only; returns 8 in bit-bang mode): loads IR=0xF then shifts a packed DR pattern built from two argument words
 *   address 0x08004282, 176 bytes
 *   called from: stlink_cmd_jtag_raw
 *   prototype: int jtag_shift_short(uint32_t w0, uint32_t w1, uint32_t a, uint32_t b)
 */
/* Raw JTAG transfer (SPI/DMA path only; returns 8 in bit-bang mode): loads IR=0xF then shifts a
   packed DR pattern built from two argument words */

int jtag_shift_short(uint param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  undefined1 local_18;
  undefined1 local_17;
  undefined1 local_16;
  undefined1 local_15;
  undefined1 local_14;
  undefined1 local_13;
  undefined4 uStack_10;
  
  if (g_debug_mode_bitbang == '\x01') {
    return 8;
  }
  uStack_10 = param_4;
  iVar1 = jtag_tap_goto_state(0xf,0x11);
  if (iVar1 != 0x80) {
    return iVar1;
  }
  local_18 = (&DAT_08008bfc)[(param_2 & 3) * 0x40];
  local_17 = (&DAT_08008bfc)[((param_2 & 0xff) >> 2) + (param_2 >> 8 & 3) * 0x40];
  local_16 = (&DAT_08008bfc)[((param_2 >> 8 & 0xff) >> 2) + ((param_2 & 0x3ffff) >> 0x10) * 0x40];
  local_15 = (&DAT_08008bfc)
             [((param_2 & 0xffffff) >> 0x12) + ((param_2 & 0x3ffffff) >> 0x18) * 0x40];
  iVar1 = 100;
  local_14 = (&DAT_08008bfc)[(param_2 >> 0x1a) + (param_1 & 3) * 0x40];
  local_13 = (&DAT_08008bfc)[(param_1 & 0xff) >> 2];
  do {
    iVar2 = jtag_spi_dma_xfer(&local_18,0);
    iVar1 = iVar1 + -1;
    if (iVar2 != 7) break;
  } while (iVar1 != 0);
  if (iVar2 == 8) {
    iVar2 = 0x80;
  }
  return iVar2;
}

/**
 * jtag_shift_ir_dr
 *   Raw JTAG IR/DR shift of a caller-selected bit length (SPI/DMA path); modes 1 and 2 shift 32/36-bit DR after IR=0xF, else returns error 6
 *   address 0x080042B0, 98 bytes
 *   called from: stlink_cmd_jtag_raw
 *   prototype: uint64_t jtag_shift_ir_dr(int mode, uint32_t ir, uint32_t dr_lo, uint32_t dr_hi)
 */
/* Raw JTAG IR/DR shift of a caller-selected bit length (SPI/DMA path); modes 1 and 2 shift
   32/36-bit DR after IR=0xF, else returns error 6 */

undefined8 jtag_shift_ir_dr(int param_1,uint param_2,uint param_3,uint param_4)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  undefined8 uVar4;
  
  if (g_debug_mode_bitbang == '\x01') {
    return CONCAT44(param_4,8);
  }
  if (param_1 == 1) {
    iVar1 = jtag_tap_goto_state(0xf,param_2 & 0x1f);
    if (iVar1 == 0x80) {
      iVar1 = jtag_write_dr(0x20,param_3,0);
    }
    if (iVar1 == 0x80) {
      uVar3 = 0;
      uVar2 = 0x20;
      param_3 = param_4;
LAB_08004306:
      uVar4 = jtag_write_dr(uVar2,param_3,uVar3,param_4);
      return uVar4;
    }
  }
  else if (param_1 == 2) {
    iVar1 = jtag_tap_goto_state(0xf,param_2 & 0x1f);
    if (iVar1 == 0x80) {
      uVar3 = param_4 & 0xf;
      uVar2 = 0x24;
      goto LAB_08004306;
    }
  }
  else {
    iVar1 = 6;
  }
  return CONCAT44(param_4,iVar1);
}

/**
 * jtag_init_gpio
 *   Configures GPIO for JTAG/alternate mode (TCK/TMS/TDI/TDO on GPIOB) and drives the nRST/enable lines via target_drive_nrst
 *   address 0x080076F2, 76 bytes
 *   called from: swd_connect
 *   prototype: uint64_t jtag_init_gpio(void)
 */
/* Configures GPIO for JTAG/alternate mode (TCK/TMS/TDI/TDO on GPIOB) and drives the nRST/enable
   lines via target_drive_nrst */

undefined8 jtag_init_gpio(void)

{
  undefined2 local_10;
  undefined1 uStack_e;
  undefined1 local_d;
  undefined4 uStack_c;
  
  local_d = 4;
  local_10 = 0x7000;
  GPIO_Init(&GPIOB_CRL,&local_10);
  target_drive_nrst(1);
  local_d = 4;
  local_10 = 0x12;
  GPIO_Init(&GPIOA_CRL,&local_10);
  local_10 = 0x420;
  GPIO_Init(&GPIOA_CRL,&local_10);
  return CONCAT44(uStack_c,CONCAT13(local_d,CONCAT12(uStack_e,local_10)));
}

/**
 * jtag_set_frequency
 *   SWD/JTAG SET_FREQ: map a frequency code (2..0x100) to an SPI baud-prescaler index into DAT_200012a0 and re-init the SPI/DMA transport; returns 0x80.
 *   address 0x0800828C, 84 bytes
 *   called from: stlink_cmd_set_freq
 *   prototype: undefined4 jtag_set_frequency(int freq_code)
 */
/* SWD/JTAG SET_FREQ: map a frequency code (2..0x100) to an SPI baud-prescaler index into
   DAT_200012a0 and re-init the SPI/DMA transport; returns 0x80. */

undefined4 jtag_set_frequency(int param_1)

{
  if (param_1 == 2) {
    param_1 = 0;
  }
  else if (param_1 == 4) {
    param_1 = 8;
  }
  else if (param_1 == 8) {
    param_1 = 0x10;
  }
  else if (param_1 == 0x10) {
    param_1 = 0x18;
  }
  else if (param_1 != 0x20) {
    if (param_1 == 0x40) {
      param_1 = 0x28;
    }
    else if (param_1 == 0x80) {
      param_1 = 0x30;
    }
    else {
      if (param_1 != 0x100) {
        return 8;
      }
      param_1 = 0x38;
    }
  }
  target_clk_prescaler = (undefined2)param_1;
  jtag_spi_dma_init();
  return 0x80;
}

/**
 * jtag_spi_dma_init
 *   Initialise JTAG transport: configure SPI1 (0x40013000) and SPI2 (0x40003800) and the three DMA1 channels (0x4002001C/30/58) with the DAT_200012a0 prescaler.
 *   address 0x080082E0, 344 bytes
 *   called from: jtag_connect, jtag_set_frequency
 */
/* Initialise JTAG transport: configure SPI1 (0x40013000) and SPI2 (0x40003800) and the three DMA1
   channels (0x4002001C/30/58) with the DAT_200012a0 prescaler. */

void jtag_spi_dma_init(void)

{
  undefined2 local_30;
  undefined2 local_2e;
  undefined4 local_2c;
  undefined2 local_28;
  undefined2 local_26;
  undefined2 local_24;
  undefined2 local_22;
  undefined2 local_20;
  
  local_30 = 0;
  local_2e = 0x104;
  local_2c = 0;
  local_28 = 0;
  local_26 = 0x200;
  local_24 = target_clk_prescaler;
  local_22 = 0;
  local_20 = 7;
  _uncertain_gpio_cfg_or_merge(&DAT_40013000,&local_30);
  local_30 = 0xc000;
  local_2e = 0;
  _uncertain_gpio_cfg_or_merge(&DAT_40003800,&local_30);
  spi_ssoe_config(&DAT_40013000,1);
  DMA_DeInit(&DAT_4002001c);
  DAT_200012d8 = &DAT_2000131c;
  DAT_200012dc = 0;
  DAT_200012e0 = DAT_20001300;
  DAT_200012d4 = &DAT_4001300c;
  DAT_200012e4 = 0;
  DAT_200012e8 = 0x80;
  DAT_200012ec = 0;
  DAT_200012f0 = 0;
  DAT_200012f4 = 0;
  DAT_200012f8 = 0x2000;
  DAT_200012fc = 0;
  DMA_Init(&DAT_4002001c,&DAT_200012d4);
  DMA_DeInit(&DAT_40020030);
  DAT_200012ac = &DAT_20001310;
  DAT_200012a8 = &DAT_4001300c;
  DAT_200012b0 = 0x10;
  DAT_200012b4 = DAT_20001300;
  DAT_200012b8 = 0;
  DAT_200012bc = 0x80;
  DAT_200012c0 = 0;
  DAT_200012c4 = 0;
  DAT_200012c8 = 0;
  DAT_200012cc = 0;
  DAT_200012d0 = 0;
  DMA_Init(&DAT_40020030,&DAT_200012a8);
  DMA_DeInit(&DAT_40020058);
  DAT_20002fdc = &DAT_4000380c;
  DAT_20002fe0 = &DAT_20001328;
  DAT_20002fe4 = 0x10;
  DAT_20002fe8 = DAT_20001300;
  DAT_20002fec = 0;
  DAT_20002ff0 = 0x80;
  DAT_20002ff4 = 0;
  DAT_20002ff8 = 0;
  DAT_20002ffc = 0;
  DAT_20003000 = 0x1000;
  DAT_20003004 = 0;
  DMA_Init(&DAT_40020058);
  spi_cr2_bits_config(&DAT_40013000,2,1);
  spi_cr2_bits_config(&DAT_40003800,2,1);
  spi_cr2_bits_config(&DAT_40013000,1,1);
  SPI_Cmd(&DAT_40013000,1);
  SPI_Cmd(&DAT_40003800,1);
  return;
}

/**
 * jtag_reverse_bits
 *   Byte/bit-reverse a 32-bit word using the 256-entry reverse table at DAT_08008BFC, reordering bytes [0,2,3,1] for SPI MSB-first shifting.
 *   address 0x08008438, 36 bytes
 *   called from: dap_get_last_rw_status, dap_read_reg
 *   prototype: uint jtag_reverse_bits(byte *p)
 */
/* Byte/bit-reverse a 32-bit word using the 256-entry reverse table at DAT_08008BFC, reordering
   bytes [0,2,3,1] for SPI MSB-first shifting. */

uint jtag_reverse_bits(byte *param_1)

{
  uint uVar1;
  
  uVar1 = 0;
  if (param_1 != (byte *)0x0) {
    uVar1 = (uint)(byte)(&DAT_08008bfc)[*param_1] |
            (uint)(byte)(&DAT_08008bfc)[param_1[2]] << 0x10 |
            (uint)(byte)(&DAT_08008bfc)[param_1[3]] << 0x18 |
            (uint)(byte)(&DAT_08008bfc)[param_1[1]] << 8;
  }
  return uVar1;
}

/**
 * jtag_spi_dma_transfer
 *   Set up and trigger a JTAG shift over SPI1/SPI2 using DMA (TMS/TDI/TDO buffers), then wait for completion; returns 0x80 or 3 on timeout.
 *   address 0x0800845C, 256 bytes
 *   called from: jtag_write_dr, jtag_shift_idcode
 */
/* Set up and trigger a JTAG shift over SPI1/SPI2 using DMA (TMS/TDI/TDO buffers), then wait for
   completion; returns 0x80 or 3 on timeout. */

undefined8
jtag_spi_dma_transfer(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  ushort uVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  uint uVar5;
  
  uVar1 = DAT_40013004;
  DAT_40013004 = uVar1 & 0xfffd;
  uVar4 = 0x80;
  uVar1 = DAT_40003804;
  uVar5 = 0;
  DAT_40003804 = uVar1 & 0xfffd;
  uVar1 = DAT_40013004;
  DAT_40013004 = uVar1 & 0xfffe;
  uVar3 = DAT_40020030;
  DAT_40020030 = uVar3 & 0xfffffffe;
  uVar3 = DAT_40020058;
  DAT_40020058 = uVar3 & 0xfffffffe;
  uVar3 = DAT_4002001c;
  DAT_4002001c = uVar3 & 0xfffffffe;
  DAT_40020004 = 0x20220;
  DAT_20001300 = 6;
  DAT_200012b4 = 6;
  DAT_200012ac = param_1;
  DMA_Init(&DAT_40020030,&DAT_200012a8);
  DAT_20002fe8 = 6;
  DAT_20002fe0 = param_2;
  DMA_Init(&DAT_40020058);
  DAT_200012e0 = 6;
  DAT_200012d8 = param_3;
  DMA_Init(&DAT_4002001c,&DAT_200012d4);
  DMA_Cmd(&DAT_40020058,1);
  DMA_Cmd(&DAT_4002001c,1);
  DMA_Cmd(&DAT_40020030,1);
  spi_cr2_bits_config(&DAT_40003800,2,1);
  spi_cr2_bits_config(&DAT_40013000,1,1);
  spi_cr2_bits_config(&DAT_40013000,2,1);
  for (; (iVar2 = DAT_40020000, -1 < iVar2 << 0x1a && (uVar5 < 0xffff)); uVar5 = uVar5 + 1) {
  }
  if (0xfffe < uVar5) {
    uVar4 = 3;
  }
  return CONCAT44(param_4,uVar4);
}

/**
 * jtag_shift_idcode
 *   Move the TAP to a state then shift DR/IR via DMA to read an IDCODE-like value; selects buffer by param_2 (1=0x1F IR).
 *   address 0x0800855C, 80 bytes
 *   called from: dap_get_last_rw_status
 */
/* Move the TAP to a state then shift DR/IR via DMA to read an IDCODE-like value; selects buffer by
   param_2 (1=0x1F IR). */

int jtag_shift_idcode(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 uVar3;
  undefined *puVar4;
  
  uVar2 = 0xf;
  if (param_2 == 1) {
    puVar4 = (undefined *)0x20001342;
    uVar2 = 0xe;
    uVar3 = 0x1f;
  }
  else {
    puVar4 = &DAT_2000133c;
    uVar3 = DAT_20001294;
  }
  led_blink_activity();
  iVar1 = jtag_tap_goto_state(uVar2,uVar3);
  if (iVar1 == 0x80) {
    iVar1 = jtag_spi_dma_transfer(&DAT_20001334,puVar4,param_1);
  }
  led_set_result(iVar1 == 0x80);
  return iVar1;
}

/**
 * jtag_write_dr
 *   (uncertain) Build a bit-reversed DR payload from param_2 and shift it via jtag_spi_dma_transfer; param_1 selects 0x20/0x24 register.
 *   address 0x080085AC, 100 bytes
 *   called from: jtag_shift_ir_dr
 */
/* (uncertain) Build a bit-reversed DR payload from param_2 and shift it via jtag_spi_dma_transfer;
   param_1 selects 0x20/0x24 register. */

undefined4 jtag_write_dr(int param_1,uint param_2,uint param_3)

{
  undefined *puVar1;
  undefined4 uVar2;
  undefined1 local_18;
  undefined1 local_17;
  undefined1 local_16;
  undefined1 local_15;
  undefined1 local_14;
  undefined1 local_13;
  undefined1 auStack_10 [8];
  
  if (param_1 == 0x20) {
    puVar1 = &DAT_2000133c;
    param_3 = 0;
  }
  else {
    if (param_1 != 0x24) {
      return 6;
    }
    puVar1 = (undefined *)0x20001348;
    param_3 = param_3 & 0xf;
  }
  local_18 = 0;
  local_17 = (&DAT_08008bfc)[param_2 & 0xff];
  local_16 = (&DAT_08008bfc)[(param_2 & 0xffff) >> 8];
  local_15 = (&DAT_08008bfc)[(param_2 & 0xffffff) >> 0x10];
  local_14 = (&DAT_08008bfc)[param_2 >> 0x18];
  local_13 = (&DAT_08008bfc)[param_3];
  uVar2 = jtag_spi_dma_transfer(&local_18,puVar1,auStack_10);
  return uVar2;
}

/**
 * jtag_tap_goto_state
 *   Drive the JTAG TAP to a target state by shifting a TMS pattern (built from tables DAT_08008BEC) over SPI+DMA; caches last state in DAT_200012A2.
 *   address 0x08008692, 196 bytes
 *   called from: jtag_shift_ir_dr, jtag_shift_short, jtag_run_test_poll, dap_read_reg, dap_write_reg, jtag_shift_idcode
 *   prototype: int jtag_tap_goto_state(int tms_a, uint tms_b)
 */
/* Drive the JTAG TAP to a target state by shifting a TMS pattern (built from tables DAT_08008BEC)
   over SPI+DMA; caches last state in DAT_200012A2. */

int jtag_tap_goto_state(int param_1,uint param_2)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  
  DAT_200012a4 = (ushort)(byte)(&DAT_08008bec)[param_1] +
                 (ushort)(byte)(&DAT_08008bec)[param_2 & 0xf] * 0x1000 +
                 (short)((param_2 & 0x10) << 7);
  if (DAT_200012a4 == jtag_last_tap_state) {
    return 0x80;
  }
  uVar1 = DAT_40013004;
  DAT_40013004 = uVar1 & 0xfffd;
  uVar1 = DAT_40003804;
  DAT_40003804 = uVar1 & 0xfffd;
  uVar1 = DAT_40013004;
  DAT_40013004 = uVar1 & 0xfffe;
  uVar2 = DAT_40020030;
  DAT_40020030 = uVar2 & 0xfffffffe;
  uVar2 = DAT_40020058;
  DAT_40020058 = uVar2 & 0xfffffffe;
  uVar2 = DAT_4002001c;
  DAT_4002001c = uVar2 & 0xfffffffe;
  DAT_40020004 = 0x20220;
  DAT_20001300 = 2;
  DAT_40020034 = 2;
  DAT_4002003c = 0x200012a4;
  DAT_4002005c = 2;
  DAT_40020064 = DAT_20001308;
  uVar2 = DAT_40020058;
  DAT_40020058 = uVar2 | 1;
  uVar2 = DAT_40020030;
  DAT_40020030 = uVar2 | 1;
  uVar1 = DAT_40003804;
  DAT_40003804 = uVar1 | 2;
  uVar1 = DAT_40013004;
  DAT_40013004 = uVar1 | 1;
  uVar1 = DAT_40013004;
  DAT_40013004 = uVar1 | 2;
  iVar3 = jtag_spi_wait_ready();
  jtag_last_tap_state = DAT_200012a4;
  if (iVar3 != 0x80) {
    jtag_last_tap_state = -1;
  }
  return iVar3;
}

/**
 * jtag_run_test_poll
 *   (uncertain) Move to a state and poll via sub_0800B4D0 up to 100 times; returns packed status bytes. Likely a DP read / run-test-idle poll.
 *   address 0x08008756, 58 bytes
 *   called from: dap_read_reg, dap_write_reg
 */
/* (uncertain) Move to a state and poll via sub_0800B4D0 up to 100 times; returns packed status
   bytes. Likely a DP read / run-test-idle poll. */

undefined8
jtag_run_test_poll(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  undefined1 local_10;
  undefined1 local_f;
  undefined2 local_e;
  undefined2 local_c;
  undefined2 uStack_a;
  
  local_c = (undefined2)param_4;
  uStack_a = (undefined2)((uint)param_4 >> 0x10);
  local_10 = (undefined1)param_3;
  local_f = (undefined1)((uint)param_3 >> 8);
  local_e = (undefined2)((uint)param_3 >> 0x10);
  iVar1 = jtag_tap_goto_state(8,0x1f);
  if (iVar1 == 0x80) {
    local_10 = 0;
    iVar1 = 100;
    local_f = 0x80;
    local_e = 0;
    local_c = 0;
    do {
      iVar2 = jtag_spi_dma_xfer(&local_10,0);
      iVar1 = iVar1 + -1;
      if (iVar2 != 7) break;
    } while (iVar1 != 0);
  }
  return CONCAT26(uStack_a,CONCAT24(local_c,CONCAT22(local_e,CONCAT11(local_f,local_10))));
}

/**
 * jtag_spi_xfer_byte
 *   Full-duplex SPI byte exchange (SPI2 DR 0x4000380C out, SPI1 DR 0x4001300C) gated on the SPI status register *DAT_20001304; returns received byte and 0x80/2.
 *   address 0x0800882E, 88 bytes
 *   called from: jtag_connect, jtag_detect_tap
 */
/* Full-duplex SPI byte exchange (SPI2 DR 0x4000380C out, SPI1 DR 0x4001300C) gated on the SPI
   status register *DAT_20001304; returns received byte and 0x80/2. */

undefined8
jtag_spi_xfer_byte(undefined2 param_1,undefined2 param_2,undefined1 *param_3,undefined4 param_4)

{
  undefined2 uVar1;
  uint uVar2;
  undefined4 local_c;
  
  if ((int)((uint)*spi_status_reg_ptr << 0x1e) < 0) {
    DAT_4000380c = param_1;
    DAT_4001300c = param_2;
    for (uVar2 = 0; (-1 < (int)((uint)*spi_status_reg_ptr << 0x1e) && (uVar2 < 0xffff));
        uVar2 = uVar2 + 1) {
    }
    if (uVar2 < 0xffff) {
      for (uVar2 = 0; (-1 < (int)((uint)*spi_status_reg_ptr << 0x1f) && (uVar2 < 0xffff));
          uVar2 = uVar2 + 1) {
      }
      if (uVar2 < 0xffff) {
        uVar1 = DAT_4001300c;
        if (param_3 == (undefined1 *)0x0) {
          local_c = CONCAT31((int3)((uint)param_4 >> 8),(char)uVar1);
        }
        else {
          *param_3 = (char)uVar1;
          local_c = param_4;
        }
        return CONCAT44(local_c,0x80);
      }
    }
  }
  return CONCAT44(param_4,2);
}

/**
 * jtag_detect_tap
 *   (uncertain) TAP detection/sync: exchange 0xC0/0x00 bytes, decode the returned pattern to select IR config (DAT_20001308/DAT_20001294); returns status.
 *   address 0x08008886, 148 bytes
 *   called from: jtag_connect
 */
/* (uncertain) TAP detection/sync: exchange 0xC0/0x00 bytes, decode the returned pattern to select
   IR config (DAT_20001308/DAT_20001294); returns status. */

undefined8 jtag_detect_tap(void)

{
  int iVar1;
  uint uVar2;
  char cVar3;
  uint local_10 [2];
  
  cVar3 = '\0';
  iVar1 = jtag_spi_xfer_byte(0xc0,0xff,0);
  if (((iVar1 == 0x80) && (iVar1 = jtag_spi_xfer_byte(0,0xff), iVar1 == 0x80)) &&
     (iVar1 = jtag_spi_xfer_byte(0,0x7f,local_10), iVar1 == 0x80)) {
    if ((local_10[0] & 0xff) == 0) {
      return CONCAT44(local_10[0],5);
    }
    if ((local_10[0] & 0xff) == 0xff) {
      cVar3 = '\b';
      iVar1 = jtag_spi_xfer_byte(0,0xff,local_10);
      if (iVar1 != 0x80) goto LAB_08008918;
    }
    if ((local_10[0] & 0xff) != 0xff) {
      for (uVar2 = 0x80; (uVar2 ^ local_10[0] & 0xff) != 0xff; uVar2 = (int)uVar2 >> 1) {
        if (uVar2 == 0) goto LAB_08008916;
        cVar3 = cVar3 + '\x01';
      }
      if (uVar2 != 0) {
        if (cVar3 == '\b') {
          DAT_20001308 = &DAT_2000130e;
          DAT_20001294 = 0xe;
LAB_08008910:
          return CONCAT44(local_10[0],0x80);
        }
        if (cVar3 == '\t') {
          DAT_20001308 = &DAT_2000130c;
          DAT_20001294 = 1;
          goto LAB_08008910;
        }
      }
    }
LAB_08008916:
    iVar1 = 4;
  }
LAB_08008918:
  return CONCAT44(local_10[0],iVar1);
}

/**
 * jtag_config_pin_speed
 *   Configure GPIOA/GPIOB output speed for the JTAG pins based on the frequency index DAT_200012A0; returns packed GPIO config words.
 *   address 0x080089A6, 58 bytes
 *   called from: jtag_init_pins
 */
/* Configure GPIOA/GPIOB output speed for the JTAG pins based on the frequency index DAT_200012A0;
   returns packed GPIO config words. */

undefined4 jtag_config_pin_speed(void)

{
  byte bVar1;
  
  if (target_clk_prescaler == 0 || target_clk_prescaler == 8) {
    bVar1 = 3;
  }
  else {
    bVar1 = 1;
  }
  GPIO_Init(&GPIOA_CRL,&stack0xfffffff8);
  GPIO_Init(&GPIOB_CRL,&stack0xfffffff8);
  return CONCAT22((short)(CONCAT13(0x18,(int3)CONCAT22((ushort)bVar1,0xe0)) >> 0x10),0x6000);
}

/**
 * jtag_init_pins
 *   Configure GPIOB JTAG pins and issue initial TCK pulses (sub_08004B06) to reset the TAP; param_1==1 adds extra clocks.
 *   address 0x080089E0, 158 bytes
 *   called from: jtag_connect
 */
/* Configure GPIOB JTAG pins and issue initial TCK pulses (sub_08004B06) to reset the TAP;
   param_1==1 adds extra clocks. */

undefined8 jtag_init_pins(int param_1)

{
  undefined1 uVar1;
  undefined2 local_18;
  undefined1 local_16;
  undefined1 local_15;
  undefined4 uStack_14;
  
  if (target_clk_prescaler == 0 || target_clk_prescaler == 8) {
    uVar1 = 3;
  }
  else {
    uVar1 = 1;
  }
  local_18 = 0x10;
  local_15 = 0x10;
  local_16 = uVar1;
  GPIO_Init(&GPIOB_CRL,&local_18);
  GPIO_WriteBit(&GPIOB_CRL,0x10,0);
  local_18 = 2;
  GPIO_Init(&GPIOB_CRL,&local_18);
  GPIO_WriteBit(&GPIOB_CRL,2,1);
  if (param_1 == 1) {
    target_drive_nrst(1);
    target_drive_nrst(0);
  }
  GPIO_WriteBit(&GPIOB_CRL,2,0);
  if (param_1 == 1) {
    target_drive_nrst(1);
  }
  GPIO_WriteBit(&GPIOB_CRL,2,1);
  local_18 = 2;
  local_15 = 0x48;
  local_16 = uVar1;
  GPIO_Init(&GPIOB_CRL,&local_18);
  jtag_config_pin_speed();
  return CONCAT44(uStack_14,CONCAT13(local_15,CONCAT12(local_16,local_18)));
}

/**
 * jtag_deinit_pins
 *   Release JTAG GPIO pins back to inputs/analog and issue a final TCK pulse (used on disconnect).
 *   address 0x08008A7E, 90 bytes
 *   called from: jtag_disconnect
 */
/* Release JTAG GPIO pins back to inputs/analog and issue a final TCK pulse (used on disconnect). */

undefined8 jtag_deinit_pins(void)

{
  undefined2 local_10;
  undefined1 local_e;
  undefined1 local_d;
  undefined4 uStack_c;
  
  local_10 = 0x10;
  local_d = 4;
  local_e = 1;
  GPIO_Init(&GPIOB_CRL,&local_10);
  target_drive_nrst(1);
  local_10 = 2;
  GPIO_Init(&GPIOB_CRL,&local_10);
  local_10 = 0xe0;
  local_d = 4;
  GPIO_Init(&GPIOA_CRL,&local_10);
  local_10 = 0x6000;
  GPIO_Init(&GPIOB_CRL,&local_10);
  return CONCAT44(uStack_c,CONCAT13(local_d,CONCAT12(local_e,local_10)));
}

/**
 * jtag_connect
 *   JTAG enter: enable clocks/GPIO, init pins, SPI+DMA init, reset TAP and read IR/IDCODE; returns status byte.
 *   address 0x08008AD8, 196 bytes
 *   called from: target_connect
 */
/* JTAG enter: enable clocks/GPIO, init pins, SPI+DMA init, reset TAP and read IR/IDCODE; returns
   status byte. */

undefined8 jtag_connect(undefined4 param_1)

{
  undefined2 uVar1;
  byte bVar2;
  uint uVar3;
  undefined4 local_18;
  
  RCC_AHBPeriphClockCmd(1,1);
  RCC_APB2PeriphClockCmd(0x1000,1);
  RCC_APB1PeriphClockCmd(0x4000,1);
  RCC_APB2PeriphClockCmd(0x18,1);
  jtag_init_pins(param_1);
  jtag_spi_dma_init();
  spi_cr2_bits_config(&DAT_40013000,2,0);
  spi_cr2_bits_config(&DAT_40003800,2,0);
  spi_cr2_bits_config(&DAT_40013000,1,0);
  jtag_last_tap_state = 0xffff;
  uVar1 = DAT_4001300c;
  local_18 = CONCAT31(local_18._1_3_,(char)uVar1);
  if ((int)((uint)*spi_status_reg_ptr << 0x1e) < 0) {
    uVar3 = jtag_spi_xfer_byte(0xfe,0,0);
    if (uVar3 == 0x80) {
      uVar3 = jtag_detect_tap();
    }
    if ((uVar3 != 2) && (bVar2 = jtag_spi_xfer_byte(0xfe,0,0), uVar3 == 0x80)) {
      uVar3 = (uint)bVar2;
    }
  }
  else {
    uVar3 = 2;
  }
  spi_cr2_bits_config(&DAT_40003800,2,1);
  spi_cr2_bits_config(&DAT_40013000,1,1);
  spi_cr2_bits_config(&DAT_40013000,2,1);
  return CONCAT44(local_18,uVar3);
}

/**
 * jtag_disconnect
 *   JTAG exit: deinit pins, disable SPI1/SPI2 and the DMA channels, and turn off the related peripheral clocks.
 *   address 0x08008B10, 142 bytes
 *   called from: target_disconnect, target_connect
 */
/* JTAG exit: deinit pins, disable SPI1/SPI2 and the DMA channels, and turn off the related
   peripheral clocks. */

void jtag_disconnect(void)

{
  undefined4 in_r3;
  
  jtag_deinit_pins();
  spi_ssoe_config(&DAT_40013000,0);
  spi_ssoe_config(&DAT_40003800,0);
  SPI_Cmd(&DAT_40013000,0);
  SPI_Cmd(&DAT_40003800,0);
  spi_reset_pulse(&DAT_40013000);
  spi_reset_pulse(&DAT_40003800);
  spi_cr2_bits_config(&DAT_40013000,2,0);
  spi_cr2_bits_config(&DAT_40003800,2,0);
  spi_cr2_bits_config(&DAT_40013000,1,0);
  DMA_DeInit(&DAT_4002001c);
  DMA_DeInit(&DAT_40020030);
  DMA_DeInit(&DAT_40020058);
  RCC_AHBPeriphClockCmd(1,0);
  RCC_APB2PeriphClockCmd(0x1000,0);
  RCC_APB1PeriphClockCmd(0x4000,0);
  RCC_APB2PeriphClockCmd(0x10,0,in_r3);
  return;
}

/**
 * jtag_spi_reset_pulse
 *   (uncertain) Pulse peripheral-clock mask 1 (AFIO) on then off via sub_0800B85C; a reset/toggle helper.
 *   address 0x08008FCC, 22 bytes
 *   called from: swim_deinit
 */
/* (uncertain) Pulse peripheral-clock mask 1 (AFIO) on then off via sub_0800B85C; a reset/toggle
   helper. */

void jtag_spi_reset_pulse(void)

{
  undefined4 unaff_r7;
  
  RCC_APB2PeriphResetCmd(1,1);
  RCC_APB2PeriphResetCmd(1,0,unaff_r7);
  return;
}

/**
 * spi_reset_pulse
 *   Reset pulse an SPI block: SPI2 (0x40003800) via APB1RSTR or SPI1 (0x40013000) via APB2RSTR
 *   address 0x0800B338, 64 bytes
 *   called from: jtag_disconnect
 *   prototype: undefined4 spi_reset_pulse(undefined *spi_base)
 */
/* Reset pulse an SPI block: SPI2 (0x40003800) via APB1RSTR or SPI1 (0x40013000) via APB2RSTR */

undefined4 spi_reset_pulse(undefined *param_1)

{
  undefined4 uVar1;
  undefined4 unaff_r7;
  
  if (param_1 == &DAT_40003800) {
    RCC_APB1PeriphResetCmd(0x4000,1);
    uVar1 = RCC_APB1PeriphResetCmd(0x4000,0,unaff_r7);
    return uVar1;
  }
  if (param_1 == &DAT_40013000) {
    RCC_APB2PeriphResetCmd(0x1000,1);
    uVar1 = RCC_APB2PeriphResetCmd(0x1000,0,unaff_r7);
    return uVar1;
  }
  return unaff_r7;
}

/**
 * SPI_Cmd
 *   ST SPI_Cmd: set/clear SPE (CR1 bit6 / 0x40)
 *   address 0x0800B3AE, 20 bytes
 *   called from: jtag_disconnect, jtag_spi_dma_init
 *   prototype: void SPI_Cmd(ushort *spi,int enable)
 */
/* ST SPI_Cmd: set/clear SPE (CR1 bit6 / 0x40) */

void SPI_Cmd(ushort *param_1,int param_2)

{
  ushort uVar1;
  
  if (param_2 == 0) {
    uVar1 = *param_1 & 0xffbf;
  }
  else {
    uVar1 = *param_1 | 0x40;
  }
  *param_1 = uVar1;
  return;
}

/**
 * spi_cr2_bits_config
 *   Set/clear mask bits in SPI_CR2 (reg+4) - DMA/IT enable helper
 *   address 0x0800B3C2, 16 bytes
 *   called from: jtag_connect, jtag_disconnect, jtag_spi_dma_init, jtag_spi_dma_transfer
 *   prototype: void spi_cr2_bits_config(int spi,ushort mask,int enable)
 */
/* Set/clear mask bits in SPI_CR2 (reg+4) - DMA/IT enable helper */

void spi_cr2_bits_config(int param_1,ushort param_2,int param_3)

{
  if (param_3 == 0) {
    param_2 = *(ushort *)(param_1 + 4) & ~param_2;
  }
  else {
    param_2 = param_2 | *(ushort *)(param_1 + 4);
  }
  *(ushort *)(param_1 + 4) = param_2;
  return;
}

/**
 * spi_ssoe_config
 *   Set/clear SPI_CR2 SSOE bit (0x04)
 *   address 0x0800B3D2, 20 bytes
 *   called from: jtag_disconnect, jtag_spi_dma_init
 *   prototype: void spi_ssoe_config(int spi,int enable)
 */
/* Set/clear SPI_CR2 SSOE bit (0x04) */

void spi_ssoe_config(int param_1,int param_2)

{
  ushort uVar1;
  
  if (param_2 == 0) {
    uVar1 = *(ushort *)(param_1 + 4) & 0xfffb;
  }
  else {
    uVar1 = *(ushort *)(param_1 + 4) | 4;
  }
  *(ushort *)(param_1 + 4) = uVar1;
  return;
}

/**
 * jtag_spi_dma_xfer
 *   JTAG-over-SPI transfer: configure SPI1(0x40013004)/SPI2(0x40003804) + DMA1 ch2/3/5, shift sequence, read reply into param_2 buffer
 *   address 0x0800B4D0, 288 bytes
 *   called from: jtag_run_test_poll, jtag_shift_short, dap_read_reg, dap_write_reg
 *   prototype: undefined8 jtag_spi_dma_xfer(undefined4,undefined1 *out,undefined4,undefined4)
 */
/* JTAG-over-SPI transfer: configure SPI1(0x40013004)/SPI2(0x40003804) + DMA1 ch2/3/5, shift
   sequence, read reply into param_2 buffer */

undefined8
jtag_spi_dma_xfer(undefined4 param_1,undefined1 *param_2,undefined4 param_3,undefined4 param_4)

{
  ushort uVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined1 *puVar5;
  
  uVar1 = DAT_40013004;
  DAT_40013004 = uVar1 & 0xfffd;
  uVar1 = DAT_40003804;
  DAT_40003804 = uVar1 & 0xfffd;
  uVar1 = DAT_40013004;
  DAT_40013004 = uVar1 & 0xfffe;
  uVar3 = DAT_40020030;
  DAT_40020030 = uVar3 & 0xfffffffe;
  uVar3 = DAT_40020058;
  DAT_40020058 = uVar3 & 0xfffffffe;
  uVar3 = DAT_4002001c;
  DAT_4002001c = uVar3 & 0xfffffffe;
  DAT_40020004 = 0x20220;
  DAT_20001300 = 6;
  DAT_40020034 = 6;
  DAT_4002003c = param_1;
  DAT_4002005c = 6;
  DAT_40020064 = 0x20001350;
  DAT_40020020 = 6;
  DAT_40020028 = 0x20001298;
  uVar3 = DAT_40020058;
  DAT_40020058 = uVar3 | 1;
  uVar3 = DAT_4002001c;
  DAT_4002001c = uVar3 | 1;
  uVar3 = DAT_40020030;
  DAT_40020030 = uVar3 | 1;
  uVar1 = DAT_40003804;
  DAT_40003804 = uVar1 | 2;
  uVar1 = DAT_40013004;
  DAT_40013004 = uVar1 | 1;
  uVar1 = DAT_40013004;
  DAT_40013004 = uVar1 | 2;
  iVar2 = jtag_spi_wait_ready();
  if (iVar2 == 0x80) {
    for (uVar3 = 0; (iVar2 = DAT_40020000, -1 < iVar2 << 0x1a && (uVar3 < 0xffff));
        uVar3 = uVar3 + 1) {
    }
    if (uVar3 < 0xffff) {
      if ((DAT_20001298 & 7) != 2) {
        if ((DAT_20001298 & 7) == 4) {
          uVar4 = 7;
        }
        else {
          uVar4 = 8;
        }
        return CONCAT44(param_4,uVar4);
      }
      uVar1 = DAT_40013004;
      DAT_40013004 = uVar1 & 0xfffd;
      iVar2 = 0x80;
      uVar1 = DAT_40003804;
      DAT_40003804 = uVar1 & 0xfffd;
      uVar1 = DAT_40013004;
      DAT_40013004 = uVar1 & 0xfffe;
      uVar3 = DAT_40020030;
      DAT_40020030 = uVar3 & 0xfffffffe;
      uVar3 = DAT_40020058;
      DAT_40020058 = uVar3 & 0xfffffffe;
      uVar3 = DAT_4002001c;
      DAT_4002001c = uVar3 & 0xfffffffe;
      DAT_40020004 = 0x20220;
      if (param_2 != (undefined1 *)0x0) {
        puVar5 = &DAT_20001299;
        for (uVar3 = 1; uVar3 < DAT_20001300; uVar3 = uVar3 + 1) {
          *param_2 = *puVar5;
          puVar5 = puVar5 + 1;
          param_2 = param_2 + 1;
        }
      }
    }
    else {
      iVar2 = 3;
    }
  }
  return CONCAT44(param_4,iVar2);
}

/**
 * jtag_spi_wait_ready
 *   Poll DMA TC and SPI status (DAT_20001304) with 0xFFFF timeouts; returns 0x80 ok / 2 / 3 timeout
 *   address 0x0800B604, 142 bytes
 *   called from: jtag_spi_dma_xfer, jtag_tap_goto_state
 *   prototype: undefined4 jtag_spi_wait_ready(void)
 */
/* Poll DMA TC and SPI status (DAT_20001304) with 0xFFFF timeouts; returns 0x80 ok / 2 / 3 timeout
    */

undefined4 jtag_spi_wait_ready(void)

{
  undefined2 uVar1;
  int iVar2;
  uint uVar3;
  
  for (uVar3 = 0; (iVar2 = DAT_40020000, -1 < iVar2 << 0x16 && (uVar3 < 0xffff)); uVar3 = uVar3 + 1)
  {
  }
  if (uVar3 < 0xffff) {
    for (uVar3 = 0; (iVar2 = DAT_40020000, -1 < iVar2 << 0xe && (uVar3 < 0xffff)); uVar3 = uVar3 + 1
        ) {
    }
    if (uVar3 < 0xffff) {
      for (uVar3 = 0; ((int)((uint)*spi_status_reg_ptr << 0x18) < 0 && (uVar3 < 0xffff));
          uVar3 = uVar3 + 1) {
      }
      if (uVar3 < 0xffff) {
        for (uVar3 = 0; (*spi_status_reg_ptr == 0 && (uVar3 < 0xffff)); uVar3 = uVar3 + 1) {
        }
        if (uVar3 < 0xffff) {
          for (uVar3 = 0; ((int)((uint)*spi_status_reg_ptr << 0x1f) < 0 && (uVar3 < 0xffff));
              uVar3 = uVar3 + 1) {
            uVar1 = DAT_4001300c;
          }
          if (uVar3 < 0xffff) {
            return 0x80;
          }
        }
      }
      return 2;
    }
  }
  return 3;
}

/**
 * swd_jtag_pins_init
 *   Configure all SWD/JTAG signal pins on GPIOA/GPIOB (SWCLK/SWDIO/TCK/TMS/TDI/TDO/etc) to default states
 *   address 0x0800C478, 138 bytes
 *   called from: stlink_cmd_process
 *   prototype: undefined4 swd_jtag_pins_init(void)
 */
/* Configure all SWD/JTAG signal pins on GPIOA/GPIOB (SWCLK/SWDIO/TCK/TMS/TDI/TDO/etc) to default
   states */

undefined4 swd_jtag_pins_init(void)

{
  undefined4 local_10;
  
  local_10._2_2_ = 0x401;
  local_10._0_2_ = 0x6000;
  GPIO_Init(&GPIOB_CRL,&local_10);
  local_10._0_2_ = 0x12;
  GPIO_Init(&GPIOA_CRL,&local_10);
  local_10._0_2_ = 0x10;
  GPIO_Init(&GPIOB_CRL,&local_10);
  local_10._0_2_ = 2;
  GPIO_Init(&GPIOB_CRL,&local_10);
  local_10._0_2_ = 0xa0;
  GPIO_Init(&GPIOA_CRL,&local_10);
  local_10._0_2_ = 0x4000;
  GPIO_Init(&GPIOB_CRL,&local_10);
  local_10._0_2_ = 0x40;
  GPIO_Init(&GPIOB_CRL,&local_10);
  local_10 = CONCAT22(local_10._2_2_,0x800);
  GPIO_Init(&GPIOB_CRL,&local_10);
  return local_10;
}

