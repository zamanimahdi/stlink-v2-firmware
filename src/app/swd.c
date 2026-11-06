/*
 * app/swd.c - SWD (Serial Wire Debug) bit-bang
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * dap_read_reg
 *   Low-level DAP DP/AP register read; dispatches to bit-bang SWD path (DAT_20001022==1) or SPI/JTAG path, returns value in *out and an ack/status byte
 *   address 0x080040EC, 138 bytes
 *   called from: dap_read_rdbuff, cortexm_mem_ap_access_retry, dap_power_up_debug, mem_read_16, stlink_cmd_rw_dap_reg, mem_read_32_noinc, mem_read_32, dap_read_reg_checked
 *   prototype: int dap_read_reg(int apndp, int reg_addr, uint32_t *out, uint32_t unused)
 */
/* Low-level DAP DP/AP register read; dispatches to bit-bang SWD path (DAT_20001022==1) or SPI/JTAG
   path, returns value in *out and an ack/status byte */

int dap_read_reg(int param_1,int param_2,undefined4 *param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  undefined1 auStack_20 [16];
  undefined4 uStack_10;
  
  iVar1 = 0x80;
  uStack_10 = param_4;
  if (g_debug_mode_bitbang == '\x01') {
    uVar2 = swd_read_reg(param_1,param_2);
    iVar1 = dap_ack_to_status(uVar2,uStack_10);
    return iVar1;
  }
  if (param_1 == 0) {
    uVar2 = 10;
  }
  else {
    if (param_1 != 1) goto LAB_08004120;
    uVar2 = 0xb;
  }
  iVar1 = jtag_tap_goto_state(uVar2,0x1f);
LAB_08004120:
  if (iVar1 != 0x80) {
    return iVar1;
  }
  DAT_200030d4 = (&DAT_08008c1c)[param_2 * 0x40];
  iVar1 = 100;
  do {
    iVar3 = jtag_spi_dma_xfer(&DAT_200030d4,auStack_20);
    iVar1 = iVar1 + -1;
    if (iVar3 != 7) break;
  } while (iVar1 != 0);
  uVar2 = jtag_reverse_bits(auStack_20);
  *param_3 = uVar2;
  if (iVar3 == 7) {
    jtag_run_test_poll();
  }
  return iVar3;
}

/**
 * dap_read_reg_checked
 *   DAP register read wrapper that issues a RDBUFF/error read (dap_read_rdbuff) when the read ack is not OK
 *   address 0x08004132, 20 bytes
 *   called from: cortexm_mem_ap_access_retry, cortexm_write_reg_verify, mem_write_32_noinc, dap_power_up_debug, target_select_ap, mem_read_8, stlink_cmd_rw_dap_reg, mem_read_32_noinc ...
 *   prototype: int dap_read_reg_checked(int apndp, int reg_addr, uint32_t *out)
 */
/* DAP register read wrapper that issues a RDBUFF/error read (dap_read_rdbuff) when the read ack is
   not OK */

int dap_read_reg_checked(void)

{
  int iVar1;
  
  iVar1 = dap_read_reg();
  if (iVar1 != 0x80) {
    dap_read_rdbuff();
  }
  return iVar1;
}

/**
 * dap_write_reg
 *   Low-level DAP DP/AP register write; builds the request+32-bit data frame and drives the bit-bang SWD or SPI/JTAG backend
 *   address 0x08004146, 166 bytes
 *   called from: dap_read_rdbuff, mem_write_16, mem_write_32_noinc, mem_read_16, mem_write_32, dap_write_reg_checked, mem_read_32
 *   prototype: int dap_write_reg(int apndp, int reg_addr, uint32_t data, uint32_t unused)
 */
/* Low-level DAP DP/AP register write; builds the request+32-bit data frame and drives the bit-bang
   SWD or SPI/JTAG backend */

int dap_write_reg(int param_1,int param_2,uint param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  undefined1 local_18;
  undefined1 local_17;
  undefined1 local_16;
  undefined1 local_15;
  undefined1 local_14;
  undefined1 local_13;
  undefined4 uStack_10;
  
  iVar1 = 0x80;
  uStack_10 = param_4;
  if (g_debug_mode_bitbang == '\x01') {
    uVar2 = swd_write_reg(param_1,param_2);
    iVar1 = dap_ack_to_status(uVar2,uStack_10);
    return iVar1;
  }
  if (param_1 == 0) {
    uVar2 = 10;
  }
  else {
    if (param_1 != 1) goto LAB_0800417a;
    uVar2 = 0xb;
  }
  iVar1 = jtag_tap_goto_state(uVar2,0x1f);
LAB_0800417a:
  if (iVar1 != 0x80) {
    return iVar1;
  }
  local_18 = (&DAT_08008bfc)[param_2 * 0x40];
  iVar1 = 100;
  local_17 = (&DAT_08008bfc)[param_3 & 0xff];
  local_16 = (&DAT_08008bfc)[(param_3 & 0xffff) >> 8];
  local_15 = (&DAT_08008bfc)[(param_3 & 0xffffff) >> 0x10];
  local_14 = (&DAT_08008bfc)[param_3 >> 0x18];
  local_13 = 0;
  do {
    iVar3 = jtag_spi_dma_xfer(&local_18,0);
    iVar1 = iVar1 + -1;
    if (iVar3 != 7) {
      return iVar3;
    }
  } while (iVar1 != 0);
  jtag_run_test_poll();
  return 7;
}

/**
 * dap_write_reg_checked
 *   DAP register write wrapper that issues a RDBUFF/error read when the write ack is not OK
 *   address 0x08004192, 20 bytes
 *   called from: cortexm_mem_ap_access_retry, cortexm_write_reg_verify, mem_write_32_noinc, dap_power_up_debug, mem_ap_set_csw, mem_read_8, stlink_cmd_rw_dap_reg, mem_read_32_noinc ...
 *   prototype: int dap_write_reg_checked(int apndp, int reg_addr, uint32_t data)
 */
/* DAP register write wrapper that issues a RDBUFF/error read when the write ack is not OK */

int dap_write_reg_checked(void)

{
  int iVar1;
  
  iVar1 = dap_write_reg();
  if (iVar1 != 0x80) {
    dap_read_rdbuff();
  }
  return iVar1;
}

/**
 * dap_check_ctrl_stat
 *   Reads DP CTRL/STAT and maps sticky error/overrun bits (STICKYERR/STICKYORUN/WDATAERR) to ST-Link error status codes 0x18-0x1A
 *   address 0x080041A6, 98 bytes
 *   called from: mem_read_16, mem_read_32_noinc, dap_finish_transfer, mem_read_32
 *   prototype: uint64_t dap_check_ctrl_stat(int is_write)
 */
/* Reads DP CTRL/STAT and maps sticky error/overrun bits (STICKYERR/STICKYORUN/WDATAERR) to ST-Link
   error status codes 0x18-0x1A */

undefined8 dap_check_ctrl_stat(int param_1)

{
  int iVar1;
  uint uVar2;
  undefined8 uVar3;
  int local_10 [2];
  
  local_10[0] = 0;
  uVar3 = dap_read_reg_checked(0,1,local_10);
  uVar2 = (uint)((ulonglong)uVar3 >> 0x20);
  iVar1 = (int)uVar3;
  if (iVar1 == 0x80) {
    uVar2 = (uint)g_debug_mode_bitbang;
  }
  if (iVar1 == 0x80 && uVar2 == 0) {
    iVar1 = dap_read_reg_checked(0,3,local_10);
  }
  if (iVar1 == 0x80) {
    if ((g_debug_mode_bitbang == 1) && (local_10[0] << 0x18 < 0)) {
      iVar1 = 0x18;
    }
    if (local_10[0] << 0x1a < 0) {
      iVar1 = 0x19;
    }
    if (local_10[0] << 0x1e < 0) {
      iVar1 = 0x1a;
    }
    if ((g_debug_mode_bitbang == 1 && param_1 == 1) && (-1 < local_10[0] << 0x19)) {
      iVar1 = 0x18;
    }
  }
  return CONCAT44(local_10[0],iVar1);
}

/**
 * dap_read_rdbuff
 *   Completes a posted DAP read by reading DP RDBUFF (reg 0/3) and discarding the result; used to flush/finish transactions
 *   address 0x08004208, 86 bytes
 *   called from: cortexm_mem_ap_access_retry, mem_write_16, mem_write_32_noinc, dap_power_up_debug, mem_read_16, mem_write_32, dap_write_reg_checked, stlink_cmd_rw_dap_reg ...
 *   prototype: void dap_read_rdbuff(void)
 */
/* WARNING: Removing unreachable block (ram,0x08004246) */
/* WARNING: Removing unreachable block (ram,0x08004252) */
/* WARNING: Removing unreachable block (ram,0x0800424a) */
/* WARNING: Removing unreachable block (ram,0x08004258) */
/* Completes a posted DAP read by reading DP RDBUFF (reg 0/3) and discarding the result; used to
   flush/finish transactions */

void dap_read_rdbuff(void)

{
  uint uVar1;
  bool bVar2;
  undefined8 uVar3;
  
  uVar3 = dap_read_reg(0,1,&stack0xfffffff8);
  uVar1 = (uint)((ulonglong)uVar3 >> 0x20);
  bVar2 = (int)uVar3 == 0x80;
  if (bVar2) {
    uVar1 = (uint)g_debug_mode_bitbang;
  }
  if (bVar2 && uVar1 == 0) {
    dap_read_reg(0,3,&stack0xfffffff8);
  }
  return;
}

/**
 * dap_finish_transfer
 *   Finishes a DAP transfer: checks CTRL/STAT, reads RDBUFF, and overrides the status with the error code unless it was a wait/fault code
 *   address 0x0800425E, 36 bytes
 *   called from: mem_write_32_noinc, mem_read_8, mem_write_8
 *   prototype: uint64_t dap_finish_transfer(uint32_t v, int status, uint32_t a, uint32_t b)
 */
/* Finishes a DAP transfer: checks CTRL/STAT, reads RDBUFF, and overrides the status with the error
   code unless it was a wait/fault code */

undefined8 dap_finish_transfer(undefined4 param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  iVar1 = dap_check_ctrl_stat();
  if (((iVar1 != 0x80) && (dap_read_rdbuff(), param_2 != 0x14 && param_2 != 0x10)) && (param_2 != 7)
     ) {
    param_2 = iVar1;
  }
  return CONCAT44(param_4,param_2);
}

/**
 * dap_ack_to_status
 *   Translates a raw SWD/JTAG ack+parity result word into an ST-Link status byte (0x80 OK, 0x09/0x10-0x17 wait/fault/parity, 0x06 proto error)
 *   address 0x08004312, 120 bytes
 *   called from: stlink_cmd_set_freq, mem_write_32_noinc, target_connect, mem_write_8, stlink_dispatch_command, dap_write_reg
 *   prototype: uint32_t dap_ack_to_status(uint32_t ack_bits)
 */
/* Translates a raw SWD/JTAG ack+parity result word into an ST-Link status byte (0x80 OK,
   0x09/0x10-0x17 wait/fault/parity, 0x06 proto error) */

undefined4 dap_ack_to_status(uint param_1)

{
  undefined4 uVar1;
  uint uVar2;
  
  uVar1 = 1;
  if (param_1 == 0) {
    return 0x80;
  }
  uVar2 = param_1 & 0xfff8;
  if (uVar2 == 8) {
    uVar1 = 9;
  }
  else if (uVar2 == 0x30) {
    uVar1 = 6;
  }
  else if (uVar2 == 0x38) {
    uVar1 = 10;
  }
  else {
    param_1 = param_1 & 7;
    if (uVar2 == 0x10 || uVar2 == 0x18) {
      if (param_1 == 1) {
        uVar1 = 0x14;
      }
      else if (param_1 == 2) {
        uVar1 = 0x15;
      }
      else if (param_1 == 4) {
        uVar1 = 0x17;
      }
      else {
        uVar1 = 0x16;
      }
    }
    if (uVar2 == 0x20 || uVar2 == 0x28) {
      if (param_1 == 1) {
        uVar1 = 0x10;
      }
      else if (param_1 == 2) {
        uVar1 = 0x11;
      }
      else if (param_1 == 4) {
        uVar1 = 0x13;
      }
      else {
        uVar1 = 0x12;
      }
    }
  }
  return uVar1;
}

/**
 * dap_identify_ap
 *   Reads the access-port/debug identification and sets *is_known=1 if it matches a table of known Cortex-M part IDs (0x44A,0x44E,0x474,...,0x4B2)
 *   address 0x0800438A, 178 bytes
 *   called from: target_connect
 *   prototype: uint64_t dap_identify_ap(uint8_t *is_known)
 */
/* Reads the access-port/debug identification and sets *is_known=1 if it matches a table of known
   Cortex-M part IDs (0x44A,0x44E,0x474,...,0x4B2) */

undefined8 dap_identify_ap(undefined1 *param_1)

{
  int iVar1;
  int iVar2;
  int local_10 [2];
  
  iVar1 = 0x80;
  if (param_1 == (undefined1 *)0x0) {
    return CONCAT44(local_10[0],6);
  }
  *param_1 = 0;
  if (g_debug_mode_bitbang == '\x01') {
    iVar1 = dap_read_part_id(local_10);
    if (iVar1 == 0x80) {
      iVar2 = 0x44a;
      if (local_10[0] != 0x44a) {
        iVar2 = 1099;
      }
      if (local_10[0] != 0x44a && local_10[0] != iVar2) {
        iVar2 = 0x44e;
        if (local_10[0] != 0x44e) {
          iVar2 = 0x44f;
        }
        if ((local_10[0] != 0x44e && local_10[0] != iVar2) &&
           (local_10[0] != 0x474 && local_10[0] != 0x478)) {
          iVar2 = 0x47a;
          if (local_10[0] != 0x47a) {
            iVar2 = 0x47b;
          }
          if (local_10[0] != 0x47a && local_10[0] != iVar2) {
            iVar2 = 0x47c;
            if (local_10[0] != 0x47c) {
              iVar2 = 0x484;
            }
            if (local_10[0] != 0x47c && local_10[0] != iVar2) {
              iVar2 = 0x485;
              if (local_10[0] != 0x485) {
                iVar2 = 0x486;
              }
              if (local_10[0] != 0x485 && local_10[0] != iVar2) {
                iVar2 = 0x492;
                if (local_10[0] != 0x492) {
                  iVar2 = 0x499;
                }
                if (local_10[0] != 0x492 && local_10[0] != iVar2) {
                  if (local_10[0] != 0x4b0) {
                    iVar2 = 0x4b2;
                  }
                  if (local_10[0] != 0x4b0 && local_10[0] != iVar2) goto LAB_0800443a;
                }
              }
            }
          }
        }
      }
      *param_1 = 1;
      return CONCAT44(local_10[0],0x80);
    }
    if (iVar1 == 0x43) {
      iVar1 = 0x80;
    }
  }
LAB_0800443a:
  return CONCAT44(local_10[0],iVar1);
}

/**
 * dap_read_part_id
 *   In bit-bang mode, reads DP IDCODE (switching to AP bank via dap_select_dpbank) and extracts the part-number field; returns 0x43 if unsupported
 *   address 0x0800443C, 100 bytes
 *   called from: dap_identify_ap
 *   prototype: uint64_t dap_read_part_id(uint32_t *out_partno)
 */
/* In bit-bang mode, reads DP IDCODE (switching to AP bank via dap_select_dpbank) and extracts the
   part-number field; returns 0x43 if unsupported */

undefined8 dap_read_part_id(uint *param_1,undefined4 param_2,undefined4 param_3,uint param_4)

{
  int iVar1;
  uint local_10;
  
  if (param_1 == (uint *)0x0) {
    return CONCAT44(param_4,6);
  }
  *param_1 = 0;
  local_10 = param_4;
  if (g_debug_mode_bitbang == '\x01') {
    iVar1 = dap_read_reg_checked(0,0,&local_10);
    if (iVar1 != 0x80) goto LAB_0800449c;
    if (1 < (local_10 & 0xffff) >> 0xc) {
      iVar1 = dap_select_dpbank(2);
      if (iVar1 == 0x80) {
        iVar1 = dap_read_reg_checked(0,1,&local_10);
      }
      if (iVar1 == 0x80) {
        *param_1 = (local_10 & 0xfffffff) >> 0x10;
      }
      dap_select_dpbank(0);
      goto LAB_0800449c;
    }
  }
  iVar1 = 0x43;
LAB_0800449c:
  return CONCAT44(local_10,iVar1);
}

/**
 * target_connect
 *   Connects/initialises the debug target: resets per-AP CSW/TAR/IDCODE caches, powers up the DAP, reads IDCODE, detects the ROM table and sets AP state
 *   address 0x080044A0, 314 bytes
 *   called from: stlink_cmd_debug_connect, stlink_dispatch_command
 *   prototype: uint64_t target_connect(int ap_index, uint32_t mode_arg, uint32_t conn_type, uint32_t reset_vector)
 */
/* Connects/initialises the debug target: resets per-AP CSW/TAR/IDCODE caches, powers up the DAP,
   reads IDCODE, detects the ROM table and sets AP state */

undefined8 target_connect(int param_1,undefined4 param_2,uint param_3,uint param_4)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined1 uVar4;
  bool bVar5;
  uint local_20;
  
  local_20 = param_4 & 0xffffff00;
  g_target_connected = 1;
  g_conn_lost = 0;
  g_detected_partid = 0;
  iVar1 = anticlone_is_active();
  if (iVar1 == 1) {
    anticlone_init();
  }
  puVar2 = &g_ap_rombase_cache;
  iVar1 = 3;
  do {
    *puVar2 = 0xffffffff;
    puVar3 = puVar2 + 2;
    puVar2[1] = 0xffffffff;
    iVar1 = iVar1 + -1;
    puVar2 = puVar2 + 3;
    *puVar3 = 0xffffffff;
  } while (iVar1 != 0);
  bVar5 = param_3 == 2;
  g_dp_select_cache = 0xfff0;
  puVar2 = &g_ap_csw_cache;
  iVar1 = 3;
  do {
    *puVar2 = 0xff;
    puVar3 = puVar2 + 2;
    puVar2[1] = 0xff;
    iVar1 = iVar1 + -1;
    puVar2 = puVar2 + 3;
    *puVar3 = 0xff;
  } while (iVar1 != 0);
  if (g_current_ap < 9) {
    g_dp_select_cache = g_current_ap << 0x18 | 0xfff0;
  }
  if (g_debug_mode_bitbang == '\x01') {
    if (bVar5) {
      swd_enter_special_sequence();
      iVar1 = dap_ack_to_status();
    }
    else {
      swd_read_idcode(0);
      iVar1 = dap_ack_to_status();
    }
  }
  else {
    iVar1 = jtag_connect(param_2);
  }
  if (iVar1 == 0x80) {
    if (!bVar5) {
      iVar1 = dap_power_up_debug();
    }
  }
  else if (g_debug_mode_bitbang == '\x01') {
    swd_connect(bVar5);
  }
  else {
    jtag_disconnect();
  }
  if (iVar1 == 0x80 && param_3 == 1) {
    iVar1 = dap_identify_ap(&local_20);
  }
  if ((((iVar1 == 0x80) && (param_3 < 2)) &&
      (iVar1 = target_select_ap(local_20 & 0xff), iVar1 == 0x80)) &&
     (target_scan_rom_table(), param_3 == 0)) {
    g_rom_detected = 1;
  }
  if (iVar1 == 0x80) {
    if (g_debug_mode_bitbang == '\x01') {
      if (param_3 == 2) {
        (&g_ap_state)[param_1] = 3;
        if ((g_reset_vector == 0xffffffff) && (param_4 != 0xffffffff)) {
          swd_write_word_txn(param_4);
          iVar1 = dap_ack_to_status();
          if (iVar1 == 0x80) {
            g_reset_vector = param_4;
          }
        }
        goto LAB_080045d4;
      }
      uVar4 = 2;
    }
    else {
      uVar4 = 1;
    }
    (&g_ap_state)[param_1] = uVar4;
  }
LAB_080045d4:
  return CONCAT44(local_20,iVar1);
}

/**
 * dap_power_up_debug
 *   Powers up the debug domain by writing DP CTRL/STAT CDBGPWRUPREQ/CSYSPWRUPREQ (0x50000000) and polling for the ack bits
 *   address 0x080045DA, 118 bytes
 *   called from: target_connect, target_select_ap
 *   prototype: uint64_t dap_power_up_debug(void)
 */
/* Powers up the debug domain by writing DP CTRL/STAT CDBGPWRUPREQ/CSYSPWRUPREQ (0x50000000) and
   polling for the ack bits */

undefined8 dap_power_up_debug(void)

{
  int iVar1;
  int iVar2;
  uint local_10;
  
  local_10 = 0;
  iVar1 = dap_read_reg_checked(0,1,&local_10);
  if (iVar1 == 0x80) {
    if ((local_10 & 0xf0000000) == 0xf0000000) {
      return CONCAT44(local_10,0x80);
    }
    iVar1 = dap_write_reg_checked(0,1,0x50000000);
    if (iVar1 == 0x80) {
      iVar2 = 0;
      do {
        iVar1 = dap_read_reg(0,1,&local_10);
        if (iVar1 != 0x80) {
          dap_read_rdbuff();
          goto LAB_0800464c;
        }
        if (((local_10 & 0xf0000000) != 0xf0000000) && (1 < iVar2)) {
          iVar1 = 0xb;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < 3);
    }
    if (iVar1 == 0x80) {
      iVar1 = dap_read_rdbuff();
    }
  }
LAB_0800464c:
  return CONCAT44(local_10,iVar1);
}

/**
 * rom_read_component_id
 *   Reads a ROM-table/component PID from registers at base+0xFE0/0xFE4/0xFE8 and assembles the component/part number and class
 *   address 0x08004650, 144 bytes
 *   called from: target_scan_rom_table
 *   prototype: int rom_read_component_id(int base, uint32_t *out_partno, uint8_t *is_cm, int *out_class)
 */
/* Reads a ROM-table/component PID from registers at base+0xFE0/0xFE4/0xFE8 and assembles the
   component/part number and class */

int rom_read_component_id(int param_1,uint *param_2,undefined1 *param_3,int *param_4)

{
  int iVar1;
  uint uVar2;
  bool bVar3;
  uint local_24 [2];
  
  iVar1 = 0x80;
  bVar3 = false;
  local_24[0] = 0;
  uVar2 = 0;
  *param_4 = 0;
  if (param_1 != -1) {
    iVar1 = mem_read_32(param_1 + 0xfe4,local_24,1,0,0);
    if (iVar1 == 0x80) {
      uVar2 = (local_24[0] & 0xff) >> 4;
      *param_4 = (local_24[0] & 0xf) << 8;
      iVar1 = mem_read_32(param_1 + 0xfe8,local_24,1,0,0);
    }
    if (iVar1 == 0x80) {
      uVar2 = uVar2 | (local_24[0] & 0xf) << 4;
      *param_2 = uVar2;
      bVar3 = uVar2 == 0xa0;
      iVar1 = mem_read_32(param_1 + 0xfe0,local_24,1,0,0);
      if (iVar1 == 0x80) {
        *param_4 = (local_24[0] & 0xff) + *param_4;
      }
    }
  }
  *param_3 = bVar3;
  return iVar1;
}

/**
 * target_scan_rom_table
 *   Scans the ROM table / debug AP of the current and other access ports to detect the core, handling connection-loss and auto re-detection
 *   address 0x080046E0, 344 bytes
 *   called from: target_connect, stlink_dispatch_command
 *   prototype: uint32_t target_scan_rom_table(void)
 */
/* Scans the ROM table / debug AP of the current and other access ports to detect the core, handling
   connection-loss and auto re-detection */

uint target_scan_rom_table(void)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  undefined4 *unaff_r9;
  int *piVar6;
  bool bVar7;
  uint local_38;
  char local_34 [4];
  uint local_30;
  uint local_2c;
  undefined4 local_28;
  undefined4 local_24;
  
  uVar2 = g_current_ap;
  local_34[0] = '\0';
  local_2c = 0xffffffff;
  local_28 = 0;
  iVar3 = 0;
  if (g_target_connected != '\0') {
    unaff_r9 = &g_ap_rombase_cache;
    iVar3 = (&g_ap_rombase_cache)[g_current_ap];
  }
  if ((g_target_connected == '\0' || iVar3 == -1) ||
     ((g_conn_lost == '\x01' && (iVar3 = debug_poll_reset(), iVar3 == 1)))) {
    return 0x80;
  }
  uVar4 = rom_read_component_id(unaff_r9[uVar2],&local_2c,local_34,&local_28);
  if (uVar4 == 0x80) {
    if (local_34[0] == '\x01') {
      g_detected_partid = local_28;
      goto LAB_08004742;
    }
    if (local_2c == 0) {
      local_30 = local_2c;
      local_38 = local_2c;
      uVar4 = mem_read_32(&DAT_40004000,&local_30,1,0);
      if ((uVar4 == 0x80) &&
         ((local_30 = (local_30 & 0xffffff) >> 0xc, local_30 == 0x9a8 || (local_30 == 0x9b0))))
      goto LAB_08004742;
    }
    else if (local_2c == 0xbb) {
      local_30 = 0;
      local_38 = 0;
      uVar4 = mem_read_32(0x4090001c,&local_30,1,0);
      uVar5 = uVar4;
      if (uVar4 == 0x80) {
        uVar5 = local_30 & 0xfffffff0;
      }
      if (uVar4 == 0x80 && uVar5 == 0x110) goto LAB_08004742;
    }
  }
  local_38 = local_38 & 0xffffff00;
  bVar1 = false;
  local_24 = 0;
  local_30 = 0;
  uVar5 = 0;
  piVar6 = &g_ap_tar_cache;
  do {
    if (bVar1 || (char)local_38 != '\0') break;
    if (uVar5 != uVar2) {
      iVar3 = target_select_ap(uVar5 & 0xff);
      bVar7 = iVar3 == 0x80;
      if (bVar7) {
        iVar3 = *piVar6;
      }
      if (bVar7 && iVar3 == -0x80000000) {
        rom_read_component_id(*unaff_r9,&local_24,&local_38,&local_30);
        bVar1 = true;
      }
    }
    uVar5 = uVar5 + 1;
    unaff_r9 = unaff_r9 + 1;
    piVar6 = piVar6 + 1;
  } while ((int)uVar5 < 5);
  g_requested_ap = uVar2;
  dap_select_ap(0xf0);
  if ((local_38 & 0xff) == 0) {
    iVar3 = debug_poll_reset();
    if ((iVar3 != 0) && (local_2c == 0)) {
      g_conn_lost = 1;
      return uVar4;
    }
    anticlone_reflash_signature();
    anticlone_start();
    g_target_connected = 0;
    g_conn_lost = 0;
    return uVar4;
  }
LAB_08004742:
  g_conn_lost = 0;
  g_target_connected = 0;
  anticlone_init();
  return uVar4;
}

/**
 * target_part_is_special
 *   (uncertain) Returns 1 if the detected ROM/part id (DAT_20001078) is one of a special set (0x1E,0x27,0x28,0x32,0x49A) used to vary the SWD line-reset sequence
 *   address 0x08004838, 46 bytes
 *   called from: swd_connect
 *   prototype: uint32_t target_part_is_special(void)
 */
/* (uncertain) Returns 1 if the detected ROM/part id (DAT_20001078) is one of a special set
   (0x1E,0x27,0x28,0x32,0x49A) used to vary the SWD line-reset sequence */

undefined4 target_part_is_special(void)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  
  if (g_detected_partid != (undefined1 *)0x1e && g_detected_partid != (undefined1 *)0x27) {
    puVar2 = g_detected_partid;
    puVar1 = &g_target_connected;
    if (g_detected_partid != (undefined1 *)0x28 && g_detected_partid != (undefined1 *)0x32) {
      puVar2 = (undefined1 *)0x49a;
      puVar1 = g_detected_partid;
    }
    if ((g_detected_partid != (undefined1 *)0x28 && g_detected_partid != (undefined1 *)0x32) &&
        puVar1 != puVar2) {
      return 0;
    }
  }
  return 1;
}

/**
 * target_slot_check
 *   Validates an AP/slot index against the active-slot table (DAT_20001358); returns 0x80 if valid, 0x1D (bad index) otherwise
 *   address 0x08004866, 40 bytes
 *   called from: stlink_debug_ap_access_batch
 *   prototype: uint32_t target_slot_check(uint32_t ap_index)
 */
/* Validates an AP/slot index against the active-slot table (DAT_20001358); returns 0x80 if valid,
   0x1D (bad index) otherwise */

undefined4 target_slot_check(uint param_1)

{
  bool bVar1;
  
  if ((8 < (int)param_1) || ((&g_ap_slot_table)[param_1 * 0xc] == '\0')) {
    bVar1 = param_1 != 0;
    if (!bVar1) {
      param_1 = (uint)g_rom_detected;
    }
    if (bVar1 || param_1 != 1) {
      return 0x1d;
    }
  }
  return 0x80;
}

/**
 * target_select_ap
 *   Selects/configures an access port: reads its IDR/CFG/BASE registers, caches the ROM base in the per-AP table, and sets the AP CSW defaults
 *   address 0x0800488E, 270 bytes
 *   called from: target_connect, target_scan_rom_table, stlink_dispatch_command
 *   prototype: int target_select_ap(uint32_t ap_index)
 */
/* WARNING: Type propagation algorithm not settling */
/* Selects/configures an access port: reads its IDR/CFG/BASE registers, caches the ROM base in the
   per-AP table, and sets the AP CSW defaults */

int target_select_ap(uint param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  uint uVar5;
  bool bVar6;
  uint local_20;
  uint local_1c;
  int local_18 [3];
  
  local_18[1] = 0;
  local_1c = 0;
  local_20 = 0;
  local_18[0] = 0;
  if (8 < (int)param_1) {
    return 0x1d;
  }
  iVar3 = dap_power_up_debug();
  (&g_ap_csw_cache)[param_1] = 0xff;
  g_requested_ap = param_1;
  if (iVar3 == 0x80) {
    iVar3 = dap_select_ap();
  }
  if (iVar3 == 0x80) {
    iVar3 = dap_read_reg_checked(1,3,local_18 + 1);
  }
  if (iVar3 == 0x80) {
    iVar3 = dap_read_reg_checked(1,2,&local_1c);
  }
  if (iVar3 == 0x80) {
    iVar3 = dap_read_reg_checked(0,3,&local_20);
  }
  if (iVar3 == 0x80) {
    iVar3 = dap_select_ap();
  }
  if (iVar3 == 0x80) {
    iVar3 = dap_read_reg_checked(1,0,local_18 + 1);
  }
  if (iVar3 == 0x80) {
    iVar3 = dap_read_reg_checked(0,3,local_18);
  }
  iVar2 = g_current_ap;
  puVar4 = (undefined4 *)(&g_target_connected + g_current_ap * 4);
  (&g_ap_tar_cache)[g_current_ap] = 0x23000000;
  uVar5 = local_1c & 0xf;
  if (uVar5 == 1) {
    bVar6 = (local_1c & 0xf0) == 0;
    param_1 = local_1c;
LAB_08004944:
    if (!bVar6) goto LAB_0800495a;
    if (local_18[0] << 8 < 0) {
      param_1 = 0xb000000;
      (&g_ap_tar_cache)[iVar2] = 0xb000000;
      goto LAB_0800495a;
    }
    param_1 = 0x4b000000;
  }
  else if (uVar5 == 4) {
    param_1 = 0x30006000;
  }
  else {
    if (uVar5 != 2 && uVar5 != 6) {
      bVar6 = uVar5 == 5;
      goto LAB_08004944;
    }
    param_1 = 0x80000000;
  }
  (&g_ap_tar_cache)[iVar2] = param_1;
LAB_0800495a:
  if (iVar3 == 0x80) {
    puVar4 = &g_ap_rombase_cache;
    param_1 = (&g_ap_rombase_cache)[iVar2];
  }
  if (iVar3 == 0x80 && param_1 == 0xffffffff) {
    uVar1 = uVar5;
    if (uVar5 != 2) {
      param_1 = 0x20170111;
      uVar1 = local_1c;
    }
    if ((uVar5 == 2 || uVar1 == param_1) ||
       (((int)(local_20 << 0x1f) < 0 && (local_20 != 0xffffffff)))) {
      puVar4[iVar2] = local_20 & 0xfffffffc;
      return iVar3;
    }
  }
  return iVar3;
}

/**
 * target_disconnect
 *   Disconnects the target: tears down the bit-bang/SPI backend and invalidates the per-AP CSW caches
 *   address 0x0800499C, 72 bytes
 *   called from: stlink_cmd_debug_connect, stlink_dispatch_command, stlink_cmd_process
 *   prototype: void target_disconnect(int ap_index)
 */
/* Disconnects the target: tears down the bit-bang/SPI backend and invalidates the per-AP CSW caches
    */

void target_disconnect(int param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  
  if (g_debug_mode_bitbang == '\x01') {
    swd_connect((&g_ap_state)[param_1] == '\x03');
  }
  else {
    jtag_disconnect();
  }
  g_dp_select_cache = 0xfff0;
  iVar3 = 3;
  puVar1 = &g_ap_csw_cache;
  do {
    *puVar1 = 0xff;
    puVar2 = puVar1 + 2;
    puVar1[1] = 0xff;
    iVar3 = iVar3 + -1;
    puVar1 = puVar1 + 3;
    *puVar2 = 0xff;
  } while (iVar3 != 0);
  g_rom_detected = 0;
  return;
}

/**
 * dap_select_ap
 *   Writes DP SELECT to select the requested access port and AP register bank, caching the current value in DAT_2000102c
 *   address 0x080049E4, 88 bytes
 *   called from: cortexm_mem_ap_access_retry, cortexm_write_reg_verify, mem_write_16, mem_write_32_noinc, mem_read_16, mem_ap_set_csw, mem_write_32, mem_read_8 ...
 *   prototype: void dap_select_ap(uint32_t ap_bank)
 */
/* Writes DP SELECT to select the requested access port and AP register bank, caching the current
   value in DAT_2000102c */

void dap_select_ap(uint param_1)

{
  int iVar1;
  bool bVar2;
  uint uVar3;
  
  iVar1 = 0x80;
  bVar2 = false;
  if (g_current_ap != g_requested_ap) {
    if (g_requested_ap < 9) {
      g_dp_select_cache = g_dp_select_cache & 0xffffff | g_requested_ap << 0x18;
    }
    else {
      iVar1 = 0x1d;
    }
    bVar2 = true;
  }
  uVar3 = g_dp_select_cache & 0xff000000 | param_1 & 0xfff0;
  if ((iVar1 == 0x80) && (g_dp_select_cache != uVar3 || bVar2)) {
    iVar1 = dap_write_reg_checked(0,2,uVar3);
    if (iVar1 != 0x80) {
      g_dp_select_cache = 0xfff0;
      return;
    }
    g_current_ap = g_requested_ap;
    g_dp_select_cache = uVar3;
  }
  return;
}

/**
 * dap_select_dpbank
 *   Writes DP SELECT to change only the DP bank-select nibble (e.g. to reach TARGETID/DLCR), preserving the current AP selection
 *   address 0x08004A3C, 74 bytes
 *   called from: stlink_cmd_rw_dap_reg, dap_read_part_id
 *   prototype: int dap_select_dpbank(uint32_t dpbanksel)
 */
/* Writes DP SELECT to change only the DP bank-select nibble (e.g. to reach TARGETID/DLCR),
   preserving the current AP selection */

int dap_select_dpbank(uint param_1)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  uVar2 = g_dp_select_cache;
  if ((g_dp_select_cache & 0xfff0) == 0xfff0) {
    uVar2 = g_current_ap << 0x18;
  }
  uVar3 = param_1 & 0xf | uVar2 & 0xff00fff0;
  if (g_dp_select_cache != uVar3) {
    iVar1 = dap_write_reg_checked(0,2,uVar3);
    if (iVar1 != 0x80) {
      g_dp_select_cache = 0xfff0;
      return iVar1;
    }
    g_current_ap = uVar2 >> 0x18;
    g_dp_select_cache = uVar3;
  }
  return 0x80;
}

/**
 * mem_ap_set_csw
 *   Configures the MEM-AP CSW for a given access size/mode (0=8b,1=16b,2=32b,3=no-increment) when it differs from the cached value
 *   address 0x08004A86, 128 bytes
 *   called from: cortexm_mem_ap_access_retry, fpb_set_breakpoint, cortexm_halt, cortexm_write_dhcsr_word, cortexm_write_reg_verify, cortexm_reset_and_halt, mem_write_16, mem_write_32_noinc ...
 *   prototype: uint64_t mem_ap_set_csw(uint32_t size_mode, uint32_t ap_sel, uint32_t a, uint32_t b)
 */
/* Configures the MEM-AP CSW for a given access size/mode (0=8b,1=16b,2=32b,3=no-increment) when it
   differs from the cached value */

undefined8 mem_ap_set_csw(uint param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  
  iVar1 = 0x80;
  uVar4 = g_requested_ap;
  if (8 < g_requested_ap) {
    uVar4 = 0;
  }
  uVar2 = (&g_ap_tar_cache)[uVar4];
  uVar3 = uVar2;
  uVar5 = uVar4;
  if (param_2 != 0) {
    uVar5 = param_2 & 0xffffff00;
    uVar3 = uVar2 >> 8;
  }
  if ((param_2 != 0 && uVar5 != uVar3 * 0x100) ||
     (param_2 = uVar2, (&g_ap_csw_cache)[uVar4] != param_1)) {
    param_2 = param_2 & 0xffffff00;
    uVar5 = param_2;
    if (param_1 != 0) {
      if (param_1 == 2) {
        uVar5 = param_2 | 0x12;
      }
      else if (param_1 < 2) {
        uVar5 = param_2 | 1;
      }
      else {
        if (param_1 != 3) {
          return CONCAT44(param_4,6);
        }
        uVar5 = param_2 | 2;
      }
    }
    iVar1 = dap_select_ap(0);
    if (iVar1 == 0x80) {
      iVar1 = dap_write_reg_checked(1,0,uVar5);
      if (iVar1 != 0x80) {
        (&g_ap_csw_cache)[uVar4] = 0xff;
        return CONCAT44(param_4,iVar1);
      }
      (&g_ap_csw_cache)[uVar4] = param_1;
      (&g_ap_tar_cache)[uVar4] = param_2;
      iVar1 = 0x80;
    }
  }
  return CONCAT44(param_4,iVar1);
}

/**
 * target_drive_nrst
 *   Drives the target nRST line via GPIOB (through gpio helper): assert low, pulse with delay, or release high depending on the mode
 *   address 0x08004B06, 54 bytes
 *   called from: jtag_init_gpio, jtag_init_pins, _uncertain_stlink_mode_enter, jtag_deinit_pins, _uncertain_stlink_mode_leave, stlink_dispatch_command, _uncertain_stlink_target_mode_apply
 *   prototype: uint32_t target_drive_nrst(int mode)
 */
/* Drives the target nRST line via GPIOB (through gpio helper): assert low, pulse with delay, or
   release high depending on the mode */

undefined4 target_drive_nrst(int param_1)

{
  if (param_1 == 0 || param_1 == 2) {
    GPIO_WriteBit(&GPIOB_CRL,1,0);
  }
  if (param_1 == 2) {
    timer_delay_ticks(10);
  }
  else if (param_1 != 1) {
    return 0x80;
  }
  GPIO_WriteBit(&GPIOB_CRL,1,1);
  return 0x80;
}

/**
 * mem_read_word
 *   Convenience wrapper reading a single 32-bit word from target memory via mem_read_32
 *   address 0x08004B3C, 14 bytes
 *   called from: debug_read_core_status
 *   prototype: void mem_read_word(uint32_t addr, uint32_t *out, uint32_t ap)
 */
/* Convenience wrapper reading a single 32-bit word from target memory via mem_read_32 */

void mem_read_word(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  mem_read_32(param_1,param_2,1,0,param_3);
  return;
}

/**
 * cortexm_write_reg_verify
 *   (uncertain) Sets MEM-AP TAR to DHCSR region, writes a Cortex-M debug register and reads it back (DAT_2000107c) to verify the written value
 *   address 0x08004B50, 196 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint64_t cortexm_write_reg_verify(uint32_t value, uint32_t reg, uint32_t a, int b)
 */
/* (uncertain) Sets MEM-AP TAR to DHCSR region, writes a Cortex-M debug register and reads it back
   (DAT_2000107c) to verify the written value */

undefined8 cortexm_write_reg_verify(uint param_1,uint param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  bool bVar2;
  int local_10;
  
  local_10 = param_4;
  led_blink_activity();
  iVar1 = mem_ap_set_csw(2,0);
  if (iVar1 == 0x80) {
    iVar1 = dap_select_ap();
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_write_reg_checked(1,1,&DAT_e000edf0);
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_select_ap();
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_read_reg_checked(1,0,&local_10);
    if (iVar1 == 0x80) {
      iVar1 = dap_read_reg_checked(0,3,&local_10);
    }
    if ((iVar1 == 0x80) && (-1 < local_10 << 0xe)) {
      return CONCAT44(local_10,8);
    }
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_write_reg_checked(1,2,param_1);
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_write_reg_checked(1,1,param_2 | 0x10000);
  }
  if (iVar1 == 0x80) {
    iVar1 = cortexm_mem_ap_access_retry(param_2,&g_last_rw_value);
  }
  if (iVar1 == 0x80) {
    if (param_2 == 0xf) {
      bVar2 = (g_last_rw_value & 0xfffffffe) == (param_1 & 0xfffffffe);
    }
    else if (param_2 == 0x10) {
      bVar2 = (g_last_rw_value & 0xfffffe00) == (param_1 & 0xfffffe00);
    }
    else {
      bVar2 = g_last_rw_value == param_1;
    }
    if (bVar2) {
      return CONCAT44(local_10,0x80);
    }
    iVar1 = 0xd;
  }
  return CONCAT44(local_10,iVar1);
}

/**
 * cortexm_mem_ap_access_retry
 *   (uncertain) Sets up the MEM-AP at the DHCSR address and performs a debug-register access, retrying up to 10 times while polling a ready bit
 *   address 0x08004C14, 256 bytes
 *   called from: cortexm_write_reg_verify, stlink_dispatch_command, cortexm_read_all_regs
 *   prototype: uint64_t cortexm_mem_ap_access_retry(uint32_t a, uint32_t b)
 */
/* (uncertain) Sets up the MEM-AP at the DHCSR address and performs a debug-register access,
   retrying up to 10 times while polling a ready bit */

undefined8 cortexm_mem_ap_access_retry(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int local_18 [2];
  
  led_blink_activity();
  iVar1 = mem_ap_set_csw(2,0);
  if (iVar1 == 0x80) {
    iVar1 = dap_select_ap(0);
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_write_reg_checked(1,1,&DAT_e000edf0);
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_select_ap(0x10);
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_read_reg_checked(1,0,local_18);
    if (iVar1 == 0x80) {
      iVar1 = dap_read_reg_checked(0,3,local_18);
    }
    if ((iVar1 != 0x80) || (local_18[0] << 0xe < 0)) goto LAB_08004c7e;
  }
  else {
LAB_08004c7e:
    if (iVar1 == 0x80) {
      iVar1 = dap_write_reg_checked(1,1,param_1);
    }
    if (iVar1 != 0x80) {
LAB_08004ce4:
      if (iVar1 == 0x80) {
        iVar1 = dap_read_reg_checked(1,2,param_2);
      }
      if (iVar1 == 0x80) {
        iVar1 = dap_read_reg_checked(0,3,param_2);
      }
      if (iVar1 == 0x80) {
        iVar1 = dap_select_ap(0);
      }
      goto LAB_08004d10;
    }
    iVar3 = 0;
    do {
      do {
        if ((iVar1 != 0x80) || (9 < iVar3)) goto LAB_08004ce4;
        iVar3 = iVar3 + 1;
        iVar1 = dap_read_reg(1,0,local_18);
        if (iVar1 == 0x80) {
          if (iVar3 == 10) {
            uVar2 = 3;
          }
          else {
            uVar2 = 0;
          }
          iVar1 = dap_read_reg(iVar3 != 10,uVar2,local_18);
          if (iVar1 != 0x80) {
            dap_read_rdbuff();
          }
        }
        else {
          dap_read_rdbuff();
        }
      } while (iVar1 != 0x80);
      if (local_18[0] << 0xf < 0) goto LAB_08004ce4;
    } while (iVar3 != 10);
  }
  iVar1 = 8;
LAB_08004d10:
  return CONCAT44(local_18[0],iVar1);
}

/**
 * dap_get_last_rw_status
 *   Reads the last read/write transfer status/value (GETLASTRWSTATUS): returns cached DAT_2000107c in bit-bang mode or re-reads it via the SPI path
 *   address 0x08004D14, 92 bytes
 *   called from: stlink_dispatch_command
 *   prototype: int dap_get_last_rw_status(uint32_t *out_status, uint32_t *out_value)
 */
/* Reads the last read/write transfer status/value (GETLASTRWSTATUS): returns cached DAT_2000107c in
   bit-bang mode or re-reads it via the SPI path */

int dap_get_last_rw_status(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = 0x80;
  if (g_debug_mode_bitbang == '\x01') {
    *param_1 = g_swd_reconnect_val;
    uVar1 = 0;
  }
  else {
    iVar2 = jtag_shift_idcode(&g_reply_buf,1);
    g_last_rw_value = jtag_reverse_bits(&DAT_20001531);
    *param_1 = g_last_rw_value;
    if (iVar2 != 0x80) {
      return iVar2;
    }
    iVar2 = jtag_shift_idcode(&g_reply_buf,0);
    uVar1 = jtag_reverse_bits(&DAT_20001531);
    g_last_rw_value = uVar1;
  }
  *param_2 = uVar1;
  return iVar2;
}

/**
 * cortexm_read_all_regs
 *   Reads all Cortex-M core registers (0x15 or 0x21 regs depending on mode) via DCRSR/DCRDR into the DAT_200010A0 register array
 *   address 0x08004D78, 68 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void cortexm_read_all_regs(int mode)
 */
/* Reads all Cortex-M core registers (0x15 or 0x21 regs depending on mode) via DCRSR/DCRDR into the
   DAT_200010A0 register array */

void cortexm_read_all_regs(int param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  char cVar4;
  int iVar5;
  undefined *puVar6;
  
  led_blink_activity();
  iVar2 = mem_ap_set_csw(2,0);
  if (param_1 == 1) {
    iVar3 = 0x21;
    cVar4 = '@';
  }
  else {
    iVar3 = 0x15;
    cVar4 = '\0';
  }
  iVar5 = 0;
  puVar6 = &DAT_200010a0;
  do {
    if (iVar2 == 0x80) {
      cVar1 = cVar4;
      if (iVar5 == 0x20) {
        cVar1 = '!';
      }
      iVar2 = cortexm_mem_ap_access_retry(cVar1,puVar6);
    }
    iVar5 = iVar5 + 1;
    puVar6 = puVar6 + 4;
    cVar4 = cVar4 + '\x01';
  } while (iVar5 < iVar3);
  return;
}

/**
 * cortexm_check_halted
 *   Reads DHCSR and sets *halted=1 if the core is halted (S_HALT bit set)
 *   address 0x08004DBC, 64 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void cortexm_check_halted(uint8_t *halted)
 */
/* Reads DHCSR and sets *halted=1 if the core is halted (S_HALT bit set) */

void cortexm_check_halted(undefined1 *param_1)

{
  int iVar1;
  
  *param_1 = 0;
  led_blink_activity();
  iVar1 = mem_ap_set_csw(2,0);
  if (((iVar1 == 0x80) && (iVar1 = mem_read_32(&DAT_e000edf0,&g_last_rw_value,1,0,0), iVar1 == 0x80)
      ) && (g_last_rw_value << 0xe < 0)) {
    *param_1 = 1;
  }
  return;
}

/**
 * dwt_set_watchpoint
 *   Programs a DWT comparator/mask/function (data watchpoint) from indexed register-address tables for the given comparator index
 *   address 0x08004DFC, 110 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void dwt_set_watchpoint(int comp_idx, uint32_t addr, int func, int mask_idx, uint32_t data, int enable)
 */
/* Programs a DWT comparator/mask/function (data watchpoint) from indexed register-address tables
   for the given comparator index */

void dwt_set_watchpoint(int param_1,uint param_2,int param_3,int param_4,uint param_5,int param_6)

{
  int iVar1;
  uint local_18 [2];
  
  local_18[0] = param_2;
  iVar1 = mem_write_32(*(undefined4 *)(&DAT_08004e6c + param_1 * 4),local_18,1,0);
  if (iVar1 == 0x80) {
    local_18[0] = param_5;
    iVar1 = mem_write_32(*(undefined4 *)(&DAT_08004e7c + param_1 * 4),local_18,1,0);
  }
  if (iVar1 == 0x80) {
    local_18[0] = *(uint *)(&DAT_08004e8c + param_4 * 4);
    if (param_6 == 0x80) {
      local_18[0] = *(uint *)(&DAT_08004eb0 + param_3 * 4) | local_18[0];
    }
    mem_write_32(*(undefined4 *)(&DAT_08004ea0 + param_1 * 4),local_18,1,0);
  }
  return;
}

/**
 * fpb_set_breakpoint
 *   Programs an FPB flash-patch comparator (FP_COMPx) for a hardware breakpoint and enables FP_CTRL (0xE0002000)
 *   address 0x08004EC0, 96 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void fpb_set_breakpoint(uint32_t addr, int comp_idx, int half, uint32_t a)
 */
/* Programs an FPB flash-patch comparator (FP_COMPx) for a hardware breakpoint and enables FP_CTRL
   (0xE0002000) */

void fpb_set_breakpoint(uint param_1,int param_2,int param_3,uint param_4)

{
  int iVar1;
  uint local_10;
  
  param_1 = param_1 & 0x1ffffffc;
  if (param_3 == 0) {
    param_1 = param_1 | 0x40000000;
  }
  else if (param_3 == 1) {
    param_1 = param_1 | 0x80000000;
  }
  else {
    param_1 = param_1 | 0xc0000000;
  }
  local_10 = param_4;
  led_blink_activity();
  iVar1 = mem_ap_set_csw(2,0);
  if (iVar1 == 0x80) {
    local_10 = param_1 | 1;
    iVar1 = mem_write_32(*(undefined4 *)(&DAT_08007610 + param_2 * 4),&local_10,1,0);
  }
  if (iVar1 == 0x80) {
    local_10 = 3;
    mem_write_32(0xe0002000,&local_10,1,0);
  }
  return;
}

/**
 * cortexm_reset_and_halt
 *   Resets and halts the core: sets DEMCR VC_CORERESET, issues AIRCR SYSRESETREQ (0x05FA0001), polls for reset/halt and leaves the core halted
 *   address 0x08004F24, 218 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void cortexm_reset_and_halt(uint8_t *ok, uint32_t a, uint32_t b, uint32_t c)
 */
/* Resets and halts the core: sets DEMCR VC_CORERESET, issues AIRCR SYSRESETREQ (0x05FA0001), polls
   for reset/halt and leaves the core halted */

void cortexm_reset_and_halt
               (undefined1 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined1 uVar2;
  uint uVar3;
  int iVar4;
  undefined4 local_18;
  
  iVar4 = 0;
  *param_1 = 0;
  local_18 = param_4;
  led_blink_activity();
  iVar1 = mem_ap_set_csw(2,0);
  if (iVar1 == 0x80) {
    iVar1 = cortexm_halt();
  }
  if (iVar1 == 0x80) {
    local_18 = 1;
    iVar1 = mem_write_32(&DAT_e000edfc,&local_18,1,0);
  }
  if (iVar1 == 0x80) {
    local_18 = 0x5fa0001;
    iVar1 = mem_write_32(&SCB_AIRCR,&local_18,1,0);
  }
  do {
    if (iVar1 == 0x80) {
      local_18 = 0;
      iVar1 = mem_read_32(&DAT_e000edf0,&g_last_rw_value,1,0);
    }
    iVar4 = iVar4 + 1;
    uVar3 = g_last_rw_value >> 0x19 & 1;
  } while ((iVar1 == 0x80 && uVar3 == 0) && (iVar4 < 1000));
  if (iVar1 != 0x80 || uVar3 != 0) {
    iVar1 = 0;
    do {
      local_18 = 0;
      iVar4 = mem_read_32(&DAT_e000edf0,&g_last_rw_value,1,0);
      iVar1 = iVar1 + 1;
      if ((-1 < (int)(g_last_rw_value << 6)) || (iVar4 != 0x80)) break;
    } while (iVar1 < 1000);
    if ((int)(g_last_rw_value << 0xe) < 0) {
      if (iVar4 == 0x80) {
        local_18 = 0xa05f0003;
        iVar4 = mem_write_32(&DAT_e000edf0,&local_18,1,0);
      }
      if (iVar4 != 0x80) {
        return;
      }
      uVar2 = 1;
    }
    else {
      uVar2 = 0;
    }
    *param_1 = uVar2;
  }
  return;
}

/**
 * cortexm_step
 *   Single-steps the core by writing DHCSR 0xA05F0005 and polling DHCSR S_REGRDY/S_HALT
 *   address 0x08004FFE, 122 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void cortexm_step(uint8_t *ok)
 */
/* Single-steps the core by writing DHCSR 0xA05F0005 and polling DHCSR S_REGRDY/S_HALT */

void cortexm_step(undefined1 *param_1)

{
  int iVar1;
  undefined4 local_18 [2];
  
  *param_1 = 0;
  led_blink_activity();
  iVar1 = mem_ap_set_csw(2,0);
  if (iVar1 == 0x80) {
    local_18[0] = 0xa05f0005;
    iVar1 = mem_write_32(&DAT_e000edf0,local_18,1,0);
  }
  if (iVar1 == 0x80) {
    local_18[0] = 0;
    iVar1 = mem_read_32(&DAT_e000edf0,&g_last_rw_value,1,0);
    if ((g_last_rw_value & 0x1030001) == 0) {
      return;
    }
    while ((iVar1 == 0x80 && ((int)(g_last_rw_value << 7) < 0))) {
      local_18[0] = 0;
      iVar1 = mem_read_32(&DAT_e000edf0,&g_last_rw_value,1,0);
    }
    *param_1 = 1;
  }
  return;
}

/**
 * cortexm_halt
 *   Halts the core by writing DHCSR 0xA05F0003 and polling until S_HALT is set (with timeout)
 *   address 0x08005078, 118 bytes
 *   called from: cortexm_reset_and_halt, stlink_dispatch_command
 *   prototype: void cortexm_halt(void)
 */
/* Halts the core by writing DHCSR 0xA05F0003 and polling until S_HALT is set (with timeout) */

void cortexm_halt(void)

{
  int iVar1;
  int iVar2;
  undefined4 local_18 [2];
  
  iVar2 = 0;
  led_blink_activity();
  iVar1 = mem_ap_set_csw(2,0);
  while (iVar1 == 0x80) {
    local_18[0] = 0xa05f0003;
    iVar1 = mem_write_32(&DAT_e000edf0,local_18,1,0);
    if (iVar1 == 0x80) {
      local_18[0] = 0;
      iVar1 = mem_read_32(&DAT_e000edf0,&g_last_rw_value,1,0);
    }
    iVar2 = iVar2 + 1;
    if (((g_last_rw_value << 0xe < 0) && (-1 < g_last_rw_value << 0xc)) || (199 < iVar2)) break;
  }
  if (iVar1 == 0x80) {
    local_18[0] = 0;
    mem_read_32(&DAT_e000edf0,&g_last_rw_value,1,0);
  }
  return;
}

/**
 * mem_read_32
 *   Reads a block of 32-bit words from target memory via the MEM-AP with auto-incrementing TAR
 *   address 0x080050FC, 226 bytes
 *   called from: cortexm_halt, cortexm_reset_and_halt, stlink_debug_ap_access_batch, mem_read_word, cortexm_check_halted, target_scan_rom_table, cortexm_step, stlink_dispatch_command ...
 *   prototype: void mem_read_32(uint32_t addr, uint32_t *buf, uint32_t count, uint32_t *out_fail_addr, uint32_t ap)
 */
/* Reads a block of 32-bit words from target memory via the MEM-AP with auto-incrementing TAR */

void mem_read_32(uint param_1,int param_2,uint param_3,uint *param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint *puStack_28;
  
  uVar3 = 0;
  if (param_4 != (uint *)0x0) {
    *param_4 = 0;
  }
  puStack_28 = param_4;
  led_blink_activity();
  iVar1 = mem_ap_set_csw(2,param_5);
  while ((uVar3 < param_3 && (iVar1 == 0x80))) {
    if ((uVar3 == 0) || ((param_1 & 0x3ff) == 0)) {
      iVar1 = dap_select_ap(0);
      if ((iVar1 == 0x80) && (iVar1 = dap_write_reg(1,1,param_1), iVar1 != 0x80)) {
        dap_read_rdbuff();
      }
      if ((iVar1 == 0x80) && (iVar1 = dap_read_reg(1,3,&puStack_28), iVar1 != 0x80)) {
        dap_read_rdbuff();
      }
    }
    if (uVar3 < param_3 - 1) {
      if ((iVar1 == 0x80) && (iVar1 = dap_read_reg(1,3,param_2), iVar1 != 0x80)) {
        dap_read_rdbuff();
      }
    }
    else {
      if ((iVar1 == 0x80) && (iVar1 = dap_read_reg(0,3,param_2), iVar1 != 0x80)) {
        dap_read_rdbuff();
      }
      iVar2 = dap_check_ctrl_stat(1);
      if (((iVar2 != 0x80) && (dap_read_rdbuff(), iVar1 != 0x14 && iVar1 != 0x10)) && (iVar1 != 7))
      {
        iVar1 = iVar2;
      }
    }
    if ((param_4 != (uint *)0x0 && iVar1 != 0x80) && (*param_4 == 0)) {
      *param_4 = param_1;
    }
    uVar3 = uVar3 + 1;
    param_2 = param_2 + 4;
    param_1 = param_1 + 4;
  }
  return;
}

/**
 * mem_read_32_noinc
 *   Reads count 32-bit words from a single (non-incrementing) target address via MEM-AP mode 3 (e.g. draining a FIFO)
 *   address 0x080051DE, 194 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint64_t mem_read_32_noinc(int addr, int buf, uint32_t count, int *out_fail_addr, uint32_t ap)
 */
/* Reads count 32-bit words from a single (non-incrementing) target address via MEM-AP mode 3 (e.g.
   draining a FIFO) */

undefined8 mem_read_32_noinc(int param_1,int param_2,uint param_3,int *param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined4 auStack_28 [2];
  
  uVar3 = 0;
  if (param_4 != (int *)0x0) {
    *param_4 = 0;
  }
  led_blink_activity();
  iVar1 = mem_ap_set_csw(3,param_5);
  if (iVar1 == 0x80) {
    iVar1 = dap_select_ap(0);
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_write_reg_checked(1,1,param_1);
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_read_reg_checked(1,3,auStack_28);
  }
  while ((uVar3 < param_3 && (iVar1 == 0x80))) {
    iVar1 = param_2 + uVar3 * 4;
    if (uVar3 < param_3 - 1) {
      iVar1 = dap_read_reg(1,3,iVar1);
      if (iVar1 != 0x80) {
        dap_read_rdbuff();
      }
    }
    else {
      iVar1 = dap_read_reg(0,3,iVar1);
      if (iVar1 != 0x80) {
        dap_read_rdbuff();
      }
      iVar2 = dap_check_ctrl_stat(1);
      if (((iVar2 != 0x80) && (dap_read_rdbuff(), iVar1 != 0x14 && iVar1 != 0x10)) && (iVar1 != 7))
      {
        iVar1 = iVar2;
      }
    }
    if ((param_4 != (int *)0x0 && iVar1 != 0x80) && (*param_4 == 0)) {
      *param_4 = param_1 + uVar3 * 4;
    }
    uVar3 = uVar3 + 1;
  }
  return CONCAT44(auStack_28[0],iVar1);
}

/**
 * mem_read_16
 *   Reads a block of 16-bit half-words from target memory via the MEM-AP (half-word CSW mode)
 *   address 0x080052A0, 250 bytes
 *   prototype: void mem_read_16(uint32_t addr, uint16_t *buf, uint32_t count, uint32_t *out_fail_addr, uint32_t ap)
 */
/* Reads a block of 16-bit half-words from target memory via the MEM-AP (half-word CSW mode) */

void mem_read_16(uint param_1,undefined2 *param_2,uint param_3,uint *param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  uint *puVar3;
  uint uVar4;
  uint *local_28;
  
  uVar4 = 0;
  param_1 = param_1 & 0xfffffffe;
  if (param_4 != (uint *)0x0) {
    *param_4 = 0;
  }
  local_28 = param_4;
  led_blink_activity();
  iVar1 = mem_ap_set_csw(1,param_5);
  while ((uVar4 < param_3 && (iVar1 == 0x80))) {
    iVar1 = dap_select_ap(0);
    if ((iVar1 == 0x80) && (iVar1 = dap_write_reg(1,1,param_1), iVar1 != 0x80)) {
      dap_read_rdbuff();
    }
    if ((iVar1 == 0x80) && (iVar1 = dap_read_reg(1,3,&local_28), iVar1 != 0x80)) {
      dap_read_rdbuff();
    }
    if (uVar4 < param_3 - 1) {
      if (iVar1 == 0x80) {
        iVar1 = dap_read_reg(1,3,&local_28);
        if (iVar1 != 0x80) {
          dap_read_rdbuff();
        }
        puVar3 = local_28;
        if ((int)(param_1 << 0x1e) < 0) {
          puVar3 = (uint *)((uint)local_28 >> 0x10);
        }
        *param_2 = (short)puVar3;
      }
    }
    else {
      if (iVar1 == 0x80) {
        iVar1 = dap_read_reg(0,3,&local_28);
        if (iVar1 != 0x80) {
          dap_read_rdbuff();
        }
        puVar3 = local_28;
        if ((int)(param_1 << 0x1e) < 0) {
          puVar3 = (uint *)((uint)local_28 >> 0x10);
        }
        *param_2 = (short)puVar3;
      }
      iVar2 = dap_check_ctrl_stat(1);
      if (((iVar2 != 0x80) && (dap_read_rdbuff(), iVar1 != 0x14 && iVar1 != 0x10)) && (iVar1 != 7))
      {
        iVar1 = iVar2;
      }
    }
    if ((param_4 != (uint *)0x0 && iVar1 != 0x80) && (*param_4 == 0)) {
      *param_4 = param_1;
    }
    uVar4 = uVar4 + 1;
    param_2 = param_2 + 1;
    param_1 = param_1 + 2;
  }
  return;
}

/**
 * mem_read_8
 *   Reads a single byte from target memory via the MEM-AP (byte CSW mode)
 *   address 0x0800539A, 104 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void mem_read_8(uint32_t addr, uint8_t *out, uint32_t ap)
 */
/* Reads a single byte from target memory via the MEM-AP (byte CSW mode) */

void mem_read_8(uint param_1,undefined1 *param_2,undefined4 param_3)

{
  int iVar1;
  uint local_18 [2];
  
  led_blink_activity();
  iVar1 = mem_ap_set_csw(0,param_3);
  if (iVar1 == 0x80) {
    iVar1 = dap_select_ap(0);
    if (iVar1 == 0x80) {
      iVar1 = dap_write_reg_checked(1,1,param_1);
    }
    if (iVar1 == 0x80) {
      iVar1 = dap_read_reg_checked(1,3,local_18);
    }
    if (iVar1 == 0x80) {
      iVar1 = dap_read_reg_checked(0,3,local_18);
    }
    *param_2 = (char)(local_18[0] >> ((param_1 & 3) << 3));
    dap_finish_transfer(1,iVar1);
  }
  return;
}

/**
 * mem_write_32
 *   Writes a block of 32-bit words to target memory via the MEM-AP with auto-incrementing TAR
 *   address 0x08005402, 114 bytes
 *   called from: fpb_set_breakpoint, cortexm_halt, cortexm_write_dhcsr_word, cortexm_reset_and_halt, stlink_debug_ap_access_batch, cortexm_step, cortexm_run_core, stlink_dispatch_command ...
 *   prototype: void mem_write_32(uint32_t addr, uint32_t *buf, uint32_t count, uint32_t ap)
 */
/* Writes a block of 32-bit words to target memory via the MEM-AP with auto-incrementing TAR */

void mem_write_32(int param_1,int param_2,uint param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  undefined1 auStack_20 [8];
  
  led_blink_activity();
  iVar1 = mem_ap_set_csw(2,param_4);
  for (uVar2 = 0; uVar2 < param_3; uVar2 = uVar2 + 1) {
    do {
      if (iVar1 != 0x80) goto LAB_08005472;
      if ((((uVar2 == 0) || ((param_1 + uVar2 * 4 & 0x3ff) == 0)) &&
          (iVar1 = dap_select_ap(0), iVar1 == 0x80)) &&
         (iVar1 = dap_write_reg(1,1,param_1 + uVar2 * 4), iVar1 != 0x80)) {
        dap_read_rdbuff();
      }
    } while (iVar1 != 0x80);
    iVar1 = dap_write_reg(1,3,*(undefined4 *)(param_2 + uVar2 * 4));
    if (iVar1 != 0x80) {
      dap_read_rdbuff();
    }
  }
LAB_08005472:
  if (iVar1 == 0x80) {
    if (g_debug_mode_bitbang == '\x01') {
      swd_read_reg_retry();
      iVar1 = dap_ack_to_status();
    }
    else {
      iVar1 = dap_read_reg_checked(1,1,auStack_20);
    }
  }
  dap_finish_transfer(0,iVar1);
  return;
}

/**
 * mem_write_32_noinc
 *   Writes count 32-bit words to a single (non-incrementing) target address via MEM-AP mode 3
 *   address 0x08005474, 138 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void mem_write_32_noinc(uint32_t addr, uint32_t *buf, uint32_t count, uint32_t ap)
 */
/* Writes count 32-bit words to a single (non-incrementing) target address via MEM-AP mode 3 */

void mem_write_32_noinc(undefined4 param_1,int param_2,uint param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  undefined1 auStack_20 [8];
  
  uVar2 = 0;
  led_blink_activity();
  iVar1 = mem_ap_set_csw(3,param_4);
  if (iVar1 == 0x80) {
    iVar1 = dap_select_ap(0);
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_write_reg_checked(1,1,param_1);
  }
  while ((uVar2 < param_3 && (iVar1 == 0x80))) {
    iVar1 = dap_write_reg(1,3,*(undefined4 *)(param_2 + uVar2 * 4));
    if (iVar1 != 0x80) {
      dap_read_rdbuff();
    }
    uVar2 = uVar2 + 1;
  }
  if (iVar1 == 0x80) {
    if (g_debug_mode_bitbang == '\x01') {
      swd_read_reg_retry();
      iVar1 = dap_ack_to_status();
    }
    else {
      iVar1 = dap_read_reg_checked(1,1,auStack_20);
    }
  }
  dap_finish_transfer(0,iVar1);
  return;
}

/**
 * mem_write_16
 *   Writes a block of 16-bit half-words to target memory via the MEM-AP
 *   address 0x08005504, 122 bytes
 *   prototype: void mem_write_16(uint32_t addr, uint16_t *buf, uint32_t count, uint32_t ap)
 */
/* Writes a block of 16-bit half-words to target memory via the MEM-AP */

void mem_write_16(uint param_1,int param_2,uint param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  int local_20 [2];
  
  led_blink_activity();
  iVar1 = mem_ap_set_csw(1,param_4);
  for (uVar2 = 0; uVar2 < param_3; uVar2 = uVar2 + 1) {
    do {
      if (iVar1 != 0x80) goto LAB_080054d0;
      iVar1 = dap_select_ap(0);
      if ((iVar1 == 0x80) &&
         (iVar1 = dap_write_reg(1,1,(param_1 & 0xfffffffe) + uVar2 * 2), iVar1 != 0x80)) {
        dap_read_rdbuff();
      }
    } while (iVar1 != 0x80);
    local_20[0] = (uint)*(ushort *)(param_2 + uVar2 * 2) <<
                  (((param_1 & 0xfffffffe) + uVar2 * 2 & 2) << 3);
    iVar1 = dap_write_reg(1,3,local_20[0]);
    if (iVar1 != 0x80) {
      dap_read_rdbuff();
    }
  }
LAB_080054d0:
  if (iVar1 == 0x80) {
    if (g_debug_mode_bitbang == '\x01') {
      swd_read_reg_retry();
      iVar1 = dap_ack_to_status();
    }
    else {
      iVar1 = dap_read_reg_checked(1,1,local_20);
    }
  }
  dap_finish_transfer(0,iVar1);
  return;
}

/**
 * mem_write_8
 *   Writes a single byte to target memory via the MEM-AP (byte CSW mode)
 *   address 0x0800557E, 116 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void mem_write_8(uint32_t addr, int value, uint32_t ap)
 */
/* Writes a single byte to target memory via the MEM-AP (byte CSW mode) */

void mem_write_8(uint param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int local_18 [2];
  
  led_blink_activity();
  iVar1 = mem_ap_set_csw(0,param_3);
  local_18[0] = param_2 << ((param_1 & 3) << 3);
  if (iVar1 == 0x80) {
    iVar1 = dap_select_ap();
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_write_reg_checked(1,1,param_1);
  }
  if (iVar1 == 0x80) {
    iVar1 = dap_write_reg_checked(1,3,local_18[0]);
  }
  if (iVar1 == 0x80) {
    if (g_debug_mode_bitbang == '\x01') {
      swd_read_reg_retry();
      iVar1 = dap_ack_to_status();
    }
    else {
      iVar1 = dap_read_reg_checked(1,1,local_18);
    }
  }
  dap_finish_transfer(0,iVar1);
  return;
}

/**
 * cortexm_write_dhcsr_word
 *   (uncertain) Sets up the MEM-AP and writes one word to DHCSR (0xE000EDF0), e.g. to resume/run the core
 *   address 0x080055F2, 40 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void cortexm_write_dhcsr_word(void)
 */
/* (uncertain) Sets up the MEM-AP and writes one word to DHCSR (0xE000EDF0), e.g. to resume/run the
   core */

void cortexm_write_dhcsr_word(void)

{
  int iVar1;
  
  led_blink_activity();
  iVar1 = mem_ap_set_csw(2,0);
  if (iVar1 == 0x80) {
    mem_write_32(&DAT_e000edf0,&stack0xfffffff8,1,0);
  }
  return;
}

/**
 * cortexm_run_core
 *   Writes DHCSR 0xA05F0000 (debug-enable, run) for the given valid AP slot to resume the core
 *   address 0x08005C50, 64 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint64_t cortexm_run_core(int ap_index)
 */
/* Writes DHCSR 0xA05F0000 (debug-enable, run) for the given valid AP slot to resume the core */

undefined8 cortexm_run_core(int param_1)

{
  undefined4 uVar1;
  undefined4 unaff_r7;
  
  uVar1 = 0x80;
  if (param_1 < 9) {
    if ((&DAT_20001359)[param_1 * 0xc] == '\x01') {
      g_last_rw_value = 0xa05f0000;
      g_requested_ap = param_1;
      uVar1 = mem_write_32(&DAT_e000edf0,&stack0xfffffff8,1,0);
      return CONCAT44(0xa05f0000,uVar1);
    }
  }
  else {
    uVar1 = 0x1d;
  }
  return CONCAT44(unaff_r7,uVar1);
}

/**
 * apkey_table_lookup
 *   (uncertain) Matches a 4-byte key/id against the registered access-key table (DAT_20002FA8, up to 10 entries) and returns the matching slot in *slot
 *   address 0x08005C98, 180 bytes
 *   called from: stlink_dispatch_command, cmd_check_allowed
 *   prototype: uint32_t apkey_table_lookup(char *key, uint8_t *slot)
 */
/* (uncertain) Matches a 4-byte key/id against the registered access-key table (DAT_20002FA8, up to
   10 entries) and returns the matching slot in *slot */

undefined4 apkey_table_lookup(char *param_1,byte *param_2)

{
  char cVar1;
  char cVar2;
  undefined4 uVar3;
  byte bVar4;
  char *pcVar5;
  byte bVar6;
  char cVar7;
  bool bVar8;
  
  uVar3 = 0x31;
  bVar4 = 10;
  if (g_apkey_count == '\0') {
    bVar4 = 0;
LAB_08005cf0:
    uVar3 = 0x30;
  }
  else {
    if (DAT_20002fac == '\0') {
      cVar7 = '\0';
      bVar4 = 0;
    }
    else {
      cVar7 = *param_1;
      bVar8 = cVar7 == g_apkey_table;
      cVar1 = g_apkey_table;
      if (bVar8) {
        cVar7 = param_1[1];
        cVar1 = DAT_20002fa9;
      }
      if (bVar8 && cVar7 == cVar1) {
        cVar7 = param_1[2];
        bVar8 = cVar7 == DAT_20002faa;
        cVar1 = DAT_20002faa;
        if (bVar8) {
          cVar7 = param_1[3];
          cVar1 = DAT_20002fab;
        }
        if (bVar8 && cVar7 == cVar1) {
          uVar3 = 0x80;
          bVar4 = 0;
          goto LAB_08005d46;
        }
      }
      cVar7 = '\x01';
    }
    bVar6 = 1;
    pcVar5 = &DAT_20002fad;
    do {
      if (cVar7 == g_apkey_count) {
        if (bVar4 == 10) {
          bVar4 = bVar6;
        }
        goto LAB_08005cf0;
      }
      if (pcVar5[4] == '\0') {
        if (bVar4 == 10) {
          bVar4 = bVar6;
        }
      }
      else {
        cVar1 = *param_1;
        cVar2 = *pcVar5;
        cVar7 = cVar7 + '\x01';
        bVar8 = cVar1 == cVar2;
        if (bVar8) {
          cVar1 = param_1[1];
          cVar2 = pcVar5[1];
        }
        if (bVar8 && cVar1 == cVar2) {
          cVar1 = param_1[2];
          cVar2 = pcVar5[2];
          bVar8 = cVar1 == cVar2;
          if (bVar8) {
            cVar1 = param_1[3];
            cVar2 = pcVar5[3];
          }
          if (bVar8 && cVar1 == cVar2) {
            uVar3 = 0x80;
            bVar4 = bVar6;
            break;
          }
        }
      }
      bVar6 = bVar6 + 1;
      pcVar5 = pcVar5 + 5;
    } while (bVar6 < 10);
  }
LAB_08005d46:
  *param_2 = bVar4;
  return uVar3;
}

/**
 * ap_tables_reset
 *   Clears the per-slot key table (DAT_20002FA8), counters (DAT_20001138) and the per-AP slot table (DAT_20001358) to their defaults
 *   address 0x08005D50, 84 bytes
 *   called from: stlink_cmd_debug_connect
 *   prototype: void ap_tables_reset(void)
 */
/* Clears the per-slot key table (DAT_20002FA8), counters (DAT_20001138) and the per-AP slot table
   (DAT_20001358) to their defaults */

undefined4 ap_tables_reset(void)

{
  int iVar1;
  undefined1 *puVar2;
  undefined4 in_r3;
  undefined4 *puVar3;
  undefined4 *puVar4;
  
  puVar2 = &g_apkey_table;
  iVar1 = 0;
  puVar3 = &stlink_ap_status;
  do {
    *puVar3 = 0;
    puVar4 = puVar3 + 2;
    puVar3[1] = 0;
    puVar3 = puVar3 + 3;
    *puVar4 = 0;
    puVar2[4] = 0;
    *puVar2 = 0xff;
    puVar2[1] = 0xff;
    puVar2[2] = 0xff;
    puVar2[3] = 0xff;
    iVar1 = iVar1 + 1;
    puVar2 = puVar2 + 5;
  } while (iVar1 < 10);
  g_apkey_count = 0;
  iVar1 = 0;
  puVar2 = &g_ap_slot_table;
  do {
    *puVar2 = 0;
    puVar2[1] = 0;
    __aeabi_memset(puVar2 + 2,10,0xff);
    iVar1 = iVar1 + 1;
    puVar2 = puVar2 + 0xc;
  } while (iVar1 < 9);
  return in_r3;
}

/**
 * cmd_check_allowed
 *   (uncertain) Validates a command against the current AP/connection state and key table; returns 0x80 if allowed, error (0x0E/0x30) otherwise
 *   address 0x08005DA4, 124 bytes
 *   called from: stlink_cmd_debug_connect
 *   prototype: uint64_t cmd_check_allowed(char *key, int cmd, uint8_t *out_slot)
 */
/* (uncertain) Validates a command against the current AP/connection state and key table; returns
   0x80 if allowed, error (0x0E/0x30) otherwise */

undefined8 cmd_check_allowed(undefined4 param_1,int param_2,undefined1 *param_3)

{
  int iVar1;
  bool bVar2;
  undefined4 local_18 [2];
  
  local_18[0] = CONCAT31(local_18[0]._1_3_,10);
  if ((g_ap_state != '\0') && (1 < g_apkey_count)) {
    if (g_ap_state == '\x02') {
      bVar2 = param_2 == 0xa3;
LAB_08005dce:
      if (bVar2) goto LAB_08005dde;
    }
    else {
      if (g_ap_state == '\x03') {
        bVar2 = param_2 == 0xa5;
        goto LAB_08005dce;
      }
      if ((g_ap_state != '\x01') || (param_2 != 0xa3 && param_2 != 0xa5)) goto LAB_08005dde;
    }
    goto LAB_08005e1c;
  }
LAB_08005dde:
  iVar1 = apkey_table_lookup(param_1,local_18);
  if ((iVar1 != 0x30) || (9 < (byte)local_18[0])) goto LAB_08005e02;
  if (g_apkey_count == 1) {
    if (g_ap_state != '\x02') {
      if (g_ap_state == '\x03') {
        if (param_2 == 3) goto LAB_08005e00;
      }
      else if (g_ap_state != '\x01' || param_2 != 0xa3) goto LAB_08005e00;
LAB_08005e1c:
      iVar1 = 0xe;
      goto LAB_08005e02;
    }
    if (param_2 != 0xa3) goto LAB_08005e1c;
  }
LAB_08005e00:
  iVar1 = 0x80;
LAB_08005e02:
  *param_3 = (byte)local_18[0];
  return CONCAT44(local_18[0],iVar1);
}

/**
 * apkey_table_add
 *   Registers a key-table entry: stores the 4-byte key and flag at the given slot and clears its counters, bumping the active-entry count
 *   address 0x08005E20, 100 bytes
 *   called from: stlink_cmd_debug_connect
 *   prototype: uint32_t apkey_table_add(uint8_t *key, uint8_t flag, int slot)
 */
/* Registers a key-table entry: stores the 4-byte key and flag at the given slot and clears its
   counters, bumping the active-entry count */

undefined4 apkey_table_add(undefined1 *param_1,undefined1 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0x80;
  if (param_3 < 10) {
    iVar1 = param_3 * 5;
    if (((&DAT_20002fac)[iVar1] == '\0') && (g_apkey_count < 10)) {
      g_apkey_count = g_apkey_count + 1;
    }
    (&g_apkey_table)[iVar1] = *param_1;
    (&DAT_20002fa9)[iVar1] = param_1[1];
    (&DAT_20002faa)[iVar1] = param_1[2];
    (&DAT_20002fab)[iVar1] = param_1[3];
    (&stlink_ap_status)[param_3 * 3] = 0;
    (&DAT_2000113c)[param_3 * 3] = 0;
    (&DAT_20001140)[param_3 * 3] = 0;
    (&DAT_20002fac)[iVar1] = param_2;
  }
  else {
    uVar2 = 0x31;
  }
  return uVar2;
}

/**
 * apkey_table_remove
 *   Removes a key-table entry: clears the slot's key/flag/counters and decrements the active-entry count
 *   address 0x08005E84, 72 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint32_t apkey_table_remove(int slot)
 */
/* Removes a key-table entry: clears the slot's key/flag/counters and decrements the active-entry
   count */

undefined4 apkey_table_remove(int param_1)

{
  if ((param_1 < 10) && (g_apkey_count != '\0')) {
    (&stlink_ap_status)[param_1 * 3] = 0;
    (&DAT_2000113c)[param_1 * 3] = 0;
    (&DAT_20001140)[param_1 * 3] = 0;
    param_1 = param_1 * 5;
    (&DAT_20002fac)[param_1] = 0;
    (&g_apkey_table)[param_1] = 0xff;
    (&DAT_20002fa9)[param_1] = 0xff;
    (&DAT_20002faa)[param_1] = 0xff;
    (&DAT_20002fab)[param_1] = 0xff;
    g_apkey_count = g_apkey_count + -1;
    return 0x80;
  }
  return 0x31;
}

/**
 * swd_set_clock_delay
 *   Stores the bit-bang SWD/JTAG clock half-period busy-loop count (DAT_200014D0) used to set the interface frequency
 *   address 0x08007630, 10 bytes
 *   called from: stlink_cmd_set_freq
 *   prototype: uint32_t swd_set_clock_delay(uint32_t delay_count)
 */
/* Stores the bit-bang SWD/JTAG clock half-period busy-loop count (DAT_200014D0) used to set the
   interface frequency */

undefined4 swd_set_clock_delay(undefined4 param_1)

{
  g_swd_clock_delay = param_1;
  return 0;
}

/**
 * swd_init_gpio
 *   Configures GPIO for bit-bang SWD: SWCLK (PB13), SWDIO (PB14) and the direction/enable pins on GPIOA/GPIOB
 *   address 0x0800763A, 184 bytes
 *   called from: swd_enter_special_sequence, swd_read_idcode
 *   prototype: uint32_t swd_init_gpio(void)
 */
/* Configures GPIO for bit-bang SWD: SWCLK (PB13), SWDIO (PB14) and the direction/enable pins on
   GPIOA/GPIOB */

undefined4 swd_init_gpio(void)

{
  undefined4 local_10;
  
  local_10 = 0x10012000;
  GPIO_Init(&GPIOB_CRL,&local_10);
  local_10 = CONCAT22(local_10._2_2_,0x5000);
  local_10 = CONCAT13(4,(undefined3)local_10);
  GPIO_Init(&GPIOB_CRL,&local_10);
  local_10 = 0x4010020;
  GPIO_Init(&GPIOA_CRL,&local_10);
  local_10 = 0x4010010;
  GPIO_Init(&GPIOB_CRL,&local_10);
  local_10 = 0x10010010;
  GPIO_Init(&GPIOA_CRL,&local_10);
  GPIO_WriteBit(&GPIOA_CRL,0x10,0);
  local_10 = 0x10010002;
  GPIO_Init(&GPIOA_CRL,&local_10);
  GPIO_WriteBit(&GPIOA_CRL,2,0);
  return local_10;
}

/**
 * swd_bitbang_request_fast
 *   Bit-bangs an 8-bit SWD request on SWCLK/SWDIO (no inter-bit delay) then reads the 3-bit turnaround/ack from SWDIO (PB12)
 *   address 0x0800773E, 102 bytes
 *   called from: swd_bb_write_word
 *   prototype: uint32_t swd_bitbang_request_fast(uint32_t request, uint8_t *ack)
 */
/* Bit-bangs an 8-bit SWD request on SWCLK/SWDIO (no inter-bit delay) then reads the 3-bit
   turnaround/ack from SWDIO (PB12) */

undefined4 swd_bitbang_request_fast(uint param_1,byte *param_2)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  
  uVar3 = 0;
  do {
    GPIOB_BRR = 0x2000;
    if ((int)((param_1 >> (uVar3 & 0xff)) << 0x1f) < 0) {
      GPIOB_BSRR = 0x4000;
    }
    else {
      GPIOB_BRR = 0x4000;
    }
    uVar3 = uVar3 + 1;
    GPIOB_BSRR = 0x2000;
  } while ((int)uVar3 < 8);
  GPIOA_BSRR = 2;
  uVar3 = GPIOB_CRH;
  GPIOB_CRH = uVar3 & 0xf0ffffff | 0x4000000;
  GPIOB_BRR = 0x2000;
  uVar2 = GPIOB_BSRR;
  GPIOB_BSRR = 0x2000;
  uVar3 = 0;
  *param_2 = 0;
  do {
    GPIOB_BRR = 0x2000;
    iVar1 = GPIOB_IDR;
    GPIOB_BSRR = 0x2000;
    if (iVar1 << 0x13 < 0) {
      *param_2 = *param_2 | (byte)(1 << (uVar3 & 0xff));
    }
    uVar3 = uVar3 + 1;
  } while ((int)uVar3 < 3);
  return uVar2;
}

/**
 * swd_bitbang_send_request
 *   Bit-bangs an 8-bit SWD request on SWCLK/SWDIO with the configured clock delay and reads the 3-bit ack
 *   address 0x080077A4, 180 bytes
 *   called from: swd_bitbang_read_word, swd_bb_write_word, swd_bitbang_write_word
 *   prototype: uint32_t swd_bitbang_send_request(uint32_t request, uint8_t *ack)
 */
/* Bit-bangs an 8-bit SWD request on SWCLK/SWDIO with the configured clock delay and reads the 3-bit
   ack */

undefined4 swd_bitbang_send_request(uint param_1,byte *param_2)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  undefined4 local_18;
  
  iVar3 = g_swd_clock_delay;
  uVar4 = 0;
  do {
    GPIOB_BRR = 0x2000;
    if ((int)((param_1 >> (uVar4 & 0xff)) << 0x1f) < 0) {
      GPIOB_BSRR = 0x4000;
    }
    else {
      GPIOB_BRR = 0x4000;
    }
    local_18 = g_swd_clock_delay;
    do {
      local_18 = local_18 + -1;
    } while (local_18 != 0);
    GPIOB_BSRR = 0x2000;
    local_18 = g_swd_clock_delay;
    do {
      local_18 = local_18 + -1;
    } while (local_18 != 0);
    uVar4 = uVar4 + 1;
  } while ((int)uVar4 < 8);
  GPIOA_BSRR = 2;
  uVar4 = GPIOB_CRH;
  GPIOB_CRH = uVar4 & 0xf0ffffff | 0x4000000;
  GPIOB_BRR = 0x2000;
  local_18 = g_swd_clock_delay;
  do {
    local_18 = local_18 + -1;
  } while (local_18 != 0);
  uVar2 = GPIOB_BSRR;
  GPIOB_BSRR = 0x2000;
  local_18 = g_swd_clock_delay;
  do {
    local_18 = local_18 + -1;
  } while (local_18 != 0);
  *param_2 = 0;
  uVar4 = 0;
  do {
    GPIOB_BRR = 0x2000;
    local_18 = iVar3;
    do {
      local_18 = local_18 + -1;
    } while (local_18 != 0);
    uVar1 = GPIOB_IDR;
    GPIOB_BSRR = 0x2000;
    local_18 = iVar3;
    do {
      local_18 = local_18 + -1;
    } while (local_18 != 0);
    if ((uVar1 >> 0xc & 1) != 0) {
      *param_2 = *param_2 | (byte)(1 << (uVar4 & 0xff));
    }
    uVar4 = uVar4 + 1;
  } while ((int)uVar4 < 3);
  return 0;
}

/**
 * swd_bitbang_read_word
 *   Bit-bangs an SWD data-phase read of a 32-bit word plus parity, handling WAIT retries, ack decode, parity check and auto-reconnect
 *   address 0x080079B2, 384 bytes
 *   called from: swd_read_access
 *   prototype: int swd_bitbang_read_word(uint32_t request, uint32_t *out, uint32_t no_retry)
 */
/* Bit-bangs an SWD data-phase read of a 32-bit word plus parity, handling WAIT retries, ack decode,
   parity check and auto-reconnect */

int swd_bitbang_read_word(uint param_1,uint *param_2,undefined4 param_3)

{
  uint uVar1;
  undefined4 uVar2;
  bool bVar3;
  int iVar4;
  byte bVar5;
  uint uVar6;
  int iVar7;
  char cVar8;
  int local_38;
  char local_34 [8];
  uint local_2c;
  undefined4 local_28;
  
  cVar8 = '\0';
  local_2c = param_1;
  local_28 = param_3;
  do {
    bVar5 = 0;
    iVar7 = 0;
    while( true ) {
      swd_bitbang_send_request(local_2c & 0xff,local_34);
      bVar5 = bVar5 + 1;
      if (local_34[0] != '\x02') break;
      if (99 < bVar5) {
        iVar7 = 1;
        goto LAB_08007abc;
      }
      if (bVar5 != 0) {
        GPIOA_BRR = 2;
        uVar6 = GPIOB_CRH;
        GPIOB_CRH = uVar6 & 0xf0ffffff | 0x3000000;
        GPIOB_BRR = 0x2000;
        local_38 = g_swd_clock_delay;
        do {
          local_38 = local_38 + -1;
        } while (local_38 != 0);
        uVar2 = GPIOB_BSRR;
        GPIOB_BSRR = 0x2000;
        local_38 = g_swd_clock_delay;
        do {
          local_38 = local_38 + -1;
        } while (local_38 != 0);
      }
    }
    if (local_34[0] == '\x01') {
      *param_2 = 0;
      bVar3 = false;
      uVar6 = 0;
      do {
        iVar4 = g_swd_clock_delay;
        GPIOB_BRR = 0x2000;
        local_38 = g_swd_clock_delay;
        do {
          local_38 = local_38 + -1;
        } while (local_38 != 0);
        uVar1 = GPIOB_IDR;
        GPIOB_BSRR = 0x2000;
        local_38 = g_swd_clock_delay;
        do {
          local_38 = local_38 + -1;
        } while (local_38 != 0);
        if ((uVar1 >> 0xc & 1) != 0) {
          *param_2 = 1 << (uVar6 & 0xff) | *param_2;
          bVar3 = (bool)(bVar3 ^ 1);
        }
        uVar6 = uVar6 + 1;
      } while ((int)uVar6 < 0x20);
      GPIOB_BRR = 0x2000;
      local_38 = iVar4;
      do {
        local_38 = local_38 + -1;
      } while (local_38 != 0);
      uVar6 = GPIOB_IDR;
      GPIOB_BSRR = 0x2000;
      uVar6 = uVar6 >> 0xc & 1;
      local_38 = iVar4;
      do {
        local_38 = local_38 + -1;
      } while (local_38 != 0);
      if (((!bVar3) && (uVar6 != 0)) || ((bVar3 && (uVar6 == 0)))) {
        iVar7 = 4;
      }
    }
    else if (local_34[0] == '\x04') {
      iVar7 = 2;
    }
    else {
      iVar7 = 3;
    }
LAB_08007abc:
    GPIOA_BRR = 2;
    uVar6 = GPIOB_CRH;
    GPIOB_CRH = uVar6 & 0xf0ffffff | 0x3000000;
    GPIOB_BRR = 0x2000;
    local_38 = g_swd_clock_delay;
    do {
      local_38 = local_38 + -1;
    } while (local_38 != 0);
    uVar2 = GPIOB_BSRR;
    GPIOB_BSRR = 0x2000;
    local_38 = g_swd_clock_delay;
    do {
      local_38 = local_38 + -1;
    } while (local_38 != 0);
    if (iVar7 == 0) {
      return 0;
    }
    if (iVar7 == 1) {
      return 1;
    }
    if ((char)local_28 != '\0') {
      return iVar7;
    }
    if (cVar8 == '\0') {
      cVar8 = '\x03';
    }
    else {
      cVar8 = cVar8 + -1;
    }
    if (g_swd_reconnect_pending == 0) {
      g_swd_reconnect_val = (uint)g_swd_reconnect_pending;
      swd_send_switch_sequence(0xe79e);
      swd_read_access(swd_dp_read_req_table,&g_swd_reconnect_val,1);
    }
    if (cVar8 == '\0') {
      return iVar7;
    }
  } while( true );
}

/**
 * swd_bitbang_write_word
 *   Bit-bangs an SWD data-phase write of a 32-bit word plus parity, handling WAIT retries, ack decode and auto-reconnect
 *   address 0x08007C52, 280 bytes
 *   called from: swd_write_access
 *   prototype: uint32_t swd_bitbang_write_word(uint32_t request, uint32_t value)
 */
/* Bit-bangs an SWD data-phase write of a 32-bit word plus parity, handling WAIT retries, ack decode
   and auto-reconnect */

undefined4 swd_bitbang_write_word(undefined4 param_1,uint param_2)

{
  undefined4 uVar1;
  uint uVar2;
  byte bVar3;
  uint uVar4;
  char cVar5;
  uint uVar6;
  int local_30;
  char local_2c [4];
  uint local_28;
  
  cVar5 = '\0';
  local_28 = param_2;
  do {
    bVar3 = 0;
    do {
      swd_bitbang_send_request(param_1,local_2c);
      GPIOA_BRR = 2;
      uVar2 = GPIOB_CRH;
      GPIOB_CRH = uVar2 & 0xf0ffffff | 0x3000000;
      GPIOB_BRR = 0x2000;
      local_30 = g_swd_clock_delay;
      do {
        local_30 = local_30 + -1;
      } while (local_30 != 0);
      uVar1 = GPIOB_BSRR;
      GPIOB_BSRR = 0x2000;
      local_30 = g_swd_clock_delay;
      do {
        local_30 = local_30 + -1;
      } while (local_30 != 0);
      bVar3 = bVar3 + 1;
      if (local_2c[0] != '\x02') {
        if (local_2c[0] == '\x01') {
          uVar2 = 0;
          uVar4 = 0;
          do {
            GPIOB_BRR = 0x2000;
            uVar6 = local_28 >> (uVar4 & 0xff) & 1;
            if (uVar6 == 0) {
              GPIOB_BRR = 0x4000;
            }
            else {
              GPIOB_BSRR = 0x4000;
            }
            local_30 = g_swd_clock_delay;
            do {
              local_30 = local_30 + -1;
            } while (local_30 != 0);
            GPIOB_BSRR = 0x2000;
            local_30 = g_swd_clock_delay;
            do {
              local_30 = local_30 + -1;
            } while (local_30 != 0);
            uVar2 = uVar2 ^ uVar6;
            uVar4 = uVar4 + 1;
          } while ((int)uVar4 < 0x20);
          GPIOB_BRR = 0x2000;
          if (uVar2 == 0) {
            GPIOB_BRR = 0x4000;
          }
          else {
            GPIOB_BSRR = 0x4000;
          }
          local_30 = g_swd_clock_delay;
          do {
            local_30 = local_30 + -1;
          } while (local_30 != 0);
          GPIOB_BSRR = 0x2000;
          local_30 = g_swd_clock_delay;
          do {
            local_30 = local_30 + -1;
          } while (local_30 != 0);
          return 0;
        }
        break;
      }
    } while (bVar3 < 100);
    if (cVar5 == '\0') {
      cVar5 = '\x03';
    }
    else {
      cVar5 = cVar5 + -1;
    }
    if (local_2c[0] == '\x02') goto LAB_08007c4a;
    if (g_swd_reconnect_pending == 0) {
      g_swd_reconnect_val = (uint)g_swd_reconnect_pending;
      swd_send_switch_sequence(0xe79e);
      swd_read_access(swd_dp_read_req_table,&g_swd_reconnect_val,1);
    }
  } while (cVar5 != '\0');
  if (local_2c[0] == '\x02') {
LAB_08007c4a:
    uVar1 = 1;
  }
  else if (local_2c[0] == '\x04') {
    uVar1 = 2;
  }
  else {
    uVar1 = 3;
  }
  return uVar1;
}

/**
 * swd_bb_write_word
 *   Bit-bang a 32-bit value plus even parity on SWDIO (PB14) with SWCLK (PB13); fast path when DAT_200014d0==0, delayed path otherwise. Used as the data phase of raw SWD write sequences.
 *   address 0x08007D62, 248 bytes
 *   called from: swd_write_idle_word, swd_write_word_txn
 *   prototype: undefined4 swd_bb_write_word(int delay_sel, uint data)
 */
/* Bit-bang a 32-bit value plus even parity on SWDIO (PB14) with SWCLK (PB13); fast path when
   DAT_200014d0==0, delayed path otherwise. Used as the data phase of raw SWD write sequences. */

undefined4 swd_bb_write_word(undefined4 param_1,uint param_2)

{
  undefined4 uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int local_20;
  undefined1 auStack_1c [8];
  
  if (g_swd_clock_delay == 0) {
    swd_bitbang_request_fast();
  }
  else {
    swd_bitbang_send_request(param_1,auStack_1c);
  }
  GPIOA_BRR = 2;
  uVar2 = 0;
  uVar3 = GPIOB_CRH;
  GPIOB_CRH = uVar3 & 0xf0ffffff | 0x3000000;
  GPIOB_BRR = 0x2000;
  if (g_swd_clock_delay == 0) {
    uVar1 = GPIOB_BSRR;
    GPIOB_BSRR = 0x2000;
    uVar3 = 0;
    do {
      uVar4 = param_2 >> (uVar3 & 0xff) & 1;
      GPIOB_BRR = 0x2000;
      if (uVar4 == 0) {
        GPIOB_BRR = 0x4000;
      }
      else {
        GPIOB_BSRR = 0x4000;
      }
      uVar2 = uVar2 ^ uVar4;
      uVar3 = uVar3 + 1;
      GPIOB_BSRR = 0x2000;
    } while ((int)uVar3 < 0x20);
    GPIOB_BRR = 0x2000;
    if (uVar2 == 0) {
      GPIOB_BRR = 0x4000;
    }
    else {
      GPIOB_BSRR = 0x4000;
    }
    GPIOB_BSRR = 0x2000;
  }
  else {
    local_20 = g_swd_clock_delay;
    do {
      local_20 = local_20 + -1;
    } while (local_20 != 0);
    uVar1 = GPIOB_BSRR;
    GPIOB_BSRR = 0x2000;
    local_20 = g_swd_clock_delay;
    do {
      local_20 = local_20 + -1;
    } while (local_20 != 0);
    uVar3 = 0;
    do {
      uVar4 = param_2 >> (uVar3 & 0xff) & 1;
      GPIOB_BRR = 0x2000;
      if (uVar4 == 0) {
        GPIOB_BRR = 0x4000;
      }
      else {
        GPIOB_BSRR = 0x4000;
      }
      local_20 = g_swd_clock_delay;
      do {
        local_20 = local_20 + -1;
      } while (local_20 != 0);
      GPIOB_BSRR = 0x2000;
      local_20 = g_swd_clock_delay;
      do {
        local_20 = local_20 + -1;
      } while (local_20 != 0);
      uVar2 = uVar2 ^ uVar4;
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < 0x20);
    GPIOB_BRR = 0x2000;
    if (uVar2 == 0) {
      GPIOB_BRR = 0x4000;
    }
    else {
      GPIOB_BSRR = 0x4000;
    }
    local_20 = g_swd_clock_delay;
    do {
      local_20 = local_20 + -1;
    } while (local_20 != 0);
    GPIOB_BSRR = 0x2000;
    local_20 = g_swd_clock_delay;
    do {
      local_20 = local_20 + -1;
    } while (local_20 != 0);
  }
  return 0;
}

/**
 * swd_read_access
 *   Core SWD read: shift 8-bit request out, read 3-bit ACK, on OK read 32 data bits+parity into *param_2 and verify parity; retries on WAIT(2) up to 100. Returns 0=OK,1=WAIT,2=FAULT,3=noack,4=parity.
 *   address 0x08007E5A, 362 bytes
 *   called from: swd_read_reg_retry, swd_read_reg, swd_bitbang_read_word, swd_read_idcode, swd_read_access, swd_write_access, swd_bitbang_write_word, swd_write_word_txn
 *   prototype: uint swd_read_access(byte request, uint *data_out, char no_retry)
 */
/* Core SWD read: shift 8-bit request out, read 3-bit ACK, on OK read 32 data bits+parity into
   *param_2 and verify parity; retries on WAIT(2) up to 100. Returns
   0=OK,1=WAIT,2=FAULT,3=noack,4=parity. */

uint swd_read_access(byte param_1,uint *param_2,char param_3)

{
  int iVar1;
  undefined4 uVar2;
  byte bVar3;
  bool bVar4;
  uint uVar5;
  uint uVar6;
  char cVar7;
  uint uVar8;
  
  if (g_swd_clock_delay != 0) {
    uVar8 = swd_bitbang_read_word();
    return uVar8;
  }
  cVar7 = '\0';
  while( true ) {
    bVar3 = 0;
    uVar8 = 0;
    while( true ) {
      uVar5 = 0;
      do {
        GPIOB_BRR = 0x2000;
        if ((int)((uint)(param_1 >> (uVar5 & 0xff)) << 0x1f) < 0) {
          GPIOB_BSRR = 0x4000;
        }
        else {
          GPIOB_BRR = 0x4000;
        }
        uVar5 = uVar5 + 1;
        GPIOB_BSRR = 0x2000;
      } while ((int)uVar5 < 8);
      GPIOA_BSRR = 2;
      uVar5 = GPIOB_CRH;
      GPIOB_CRH = uVar5 & 0xf0ffffff | 0x4000000;
      GPIOB_BRR = 0x2000;
      uVar2 = GPIOB_BSRR;
      GPIOB_BSRR = 0x2000;
      uVar5 = 0;
      uVar6 = 0;
      do {
        GPIOB_BRR = 0x2000;
        iVar1 = GPIOB_IDR;
        GPIOB_BSRR = 0x2000;
        if (iVar1 << 0x13 < 0) {
          uVar5 = uVar5 | 1 << (uVar6 & 0xff);
        }
        uVar6 = uVar6 + 1;
      } while ((int)uVar6 < 3);
      bVar3 = bVar3 + 1;
      uVar5 = uVar5 & 0xff;
      if ((uVar5 != 2) || (99 < bVar3)) break;
      if (bVar3 != 0) {
        GPIOA_BRR = 2;
        uVar5 = GPIOB_CRH;
        GPIOB_CRH = uVar5 & 0xf0ffffff | 0x3000000;
        GPIOB_BRR = 0x2000;
        uVar2 = GPIOB_BSRR;
        GPIOB_BSRR = 0x2000;
      }
    }
    if (uVar5 == 1) {
      bVar4 = false;
      *param_2 = 0;
      uVar5 = 0;
      do {
        GPIOB_BRR = 0x2000;
        iVar1 = GPIOB_IDR;
        GPIOB_BSRR = 0x2000;
        if (iVar1 << 0x13 < 0) {
          *param_2 = *param_2 | 1 << (uVar5 & 0xff);
          bVar4 = (bool)(bVar4 ^ 1);
        }
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < 0x20);
      GPIOB_BRR = 0x2000;
      uVar5 = GPIOB_IDR;
      GPIOB_BSRR = 0x2000;
      uVar5 = uVar5 >> 0xc & 1;
      if (((!bVar4) && (uVar5 != 0)) || ((bVar4 && (uVar5 == 0)))) {
        uVar8 = 4;
      }
    }
    else {
      uVar8 = (uint)(uVar5 == 2);
      if (uVar5 != 2) {
        if (uVar5 == 4) {
          uVar8 = 2;
        }
        else {
          uVar8 = 3;
        }
      }
    }
    GPIOA_BRR = 2;
    uVar5 = GPIOB_CRH;
    GPIOB_CRH = uVar5 & 0xf0ffffff | 0x3000000;
    GPIOB_BRR = 0x2000;
    uVar2 = GPIOB_BSRR;
    GPIOB_BSRR = 0x2000;
    if (uVar8 == 0) {
      return 0;
    }
    if (uVar8 == 1) {
      return 1;
    }
    if (param_3 != '\0') break;
    if (cVar7 == '\0') {
      cVar7 = '\x03';
    }
    else {
      cVar7 = cVar7 + -1;
    }
    if (g_swd_reconnect_pending == 0) {
      g_swd_reconnect_val = (uint)g_swd_reconnect_pending;
      swd_send_switch_sequence(0xe79e);
      swd_read_access(swd_dp_read_req_table,&g_swd_reconnect_val,1);
    }
    if (cVar7 == '\0') {
      return uVar8;
    }
  }
  return uVar8;
}

/**
 * swd_read_reg
 *   Select DP(0)/AP(1) read-request byte from table (DAT_200014c0 / DAT_200014d4), perform swd_read_access, map status into 0x10/0x20 range; issues abort on WAIT.
 *   address 0x08007E6A, 72 bytes
 *   called from: dap_read_reg
 */
/* Select DP(0)/AP(1) read-request byte from table (DAT_200014c0 / DAT_200014d4), perform
   swd_read_access, map status into 0x10/0x20 range; issues abort on WAIT. */

ulonglong swd_read_reg(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 *puVar3;
  int iVar4;
  
  puVar3 = (undefined1 *)0x0;
  iVar4 = 0;
  if (param_1 == 0) {
    puVar3 = &swd_dp_read_req_table;
    iVar4 = 0x10;
  }
  else if (param_1 == 1) {
    puVar3 = &DAT_200014d4;
    iVar4 = 0x20;
  }
  if (puVar3 != (undefined1 *)0x0) {
    iVar1 = swd_read_access(puVar3[param_2],param_3,0);
    if (iVar1 == 1) {
      swd_write_access(swd_dp_write_req_table,0x1f);
    }
    else {
      uVar2 = 0;
      if (iVar1 == 0) goto LAB_08007eb0;
    }
    return CONCAT44(param_4,iVar1 + iVar4) & 0xffffffff0000ffff;
  }
  uVar2 = 0x30;
LAB_08007eb0:
  return CONCAT44(param_4,uVar2);
}

/**
 * swd_write_access
 *   Core SWD write: shift 8-bit request, read 3-bit ACK, on OK shift 32 data bits+parity (param_2). Returns 0=OK,1=WAIT,2=FAULT,3=noack.
 *   address 0x08007EB2, 296 bytes
 *   called from: swd_read_reg, swd_write_reg, swd_connect
 *   prototype: undefined4 swd_write_access(byte request, uint data)
 */
/* Core SWD write: shift 8-bit request, read 3-bit ACK, on OK shift 32 data bits+parity (param_2).
   Returns 0=OK,1=WAIT,2=FAULT,3=noack. */

undefined4 swd_write_access(byte param_1,uint param_2)

{
  int iVar1;
  byte bVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  char cVar6;
  uint uVar7;
  
  if (g_swd_clock_delay != 0) {
    uVar3 = swd_bitbang_write_word();
    return uVar3;
  }
  cVar6 = '\0';
  while( true ) {
    bVar2 = 0;
    do {
      uVar4 = 0;
      do {
        GPIOB_BRR = 0x2000;
        if ((int)((uint)(param_1 >> (uVar4 & 0xff)) << 0x1f) < 0) {
          GPIOB_BSRR = 0x4000;
        }
        else {
          GPIOB_BRR = 0x4000;
        }
        uVar4 = uVar4 + 1;
        GPIOB_BSRR = 0x2000;
      } while ((int)uVar4 < 8);
      GPIOA_BSRR = 2;
      uVar7 = 0;
      uVar4 = GPIOB_CRH;
      GPIOB_CRH = uVar4 & 0xf0ffffff | 0x4000000;
      GPIOB_BRR = 0x2000;
      uVar3 = GPIOB_BSRR;
      GPIOB_BSRR = 0x2000;
      uVar4 = 0;
      do {
        GPIOB_BRR = 0x2000;
        iVar1 = GPIOB_IDR;
        GPIOB_BSRR = 0x2000;
        if (iVar1 << 0x13 < 0) {
          uVar7 = uVar7 | 1 << (uVar4 & 0xff);
        }
        uVar4 = uVar4 + 1;
      } while ((int)uVar4 < 3);
      GPIOA_BRR = 2;
      bVar2 = bVar2 + 1;
      uVar4 = GPIOB_CRH;
      GPIOB_CRH = uVar4 & 0xf0ffffff | 0x3000000;
      GPIOB_BRR = 0x2000;
      uVar7 = uVar7 & 0xff;
      uVar3 = GPIOB_BSRR;
      GPIOB_BSRR = 0x2000;
    } while ((uVar7 == 2) && (bVar2 < 100));
    if (uVar7 == 1) break;
    if (cVar6 == '\0') {
      cVar6 = '\x03';
    }
    else {
      cVar6 = cVar6 + -1;
    }
    if (uVar7 == 2) {
      return 1;
    }
    if (g_swd_reconnect_pending == 0) {
      g_swd_reconnect_val = (uint)g_swd_reconnect_pending;
      swd_send_switch_sequence(0xe79e);
      swd_read_access(swd_dp_read_req_table,&g_swd_reconnect_val,1);
    }
    if (cVar6 == '\0') {
      if (uVar7 == 4) {
        uVar3 = 2;
      }
      else {
        uVar3 = 3;
      }
      return uVar3;
    }
  }
  uVar4 = 0;
  uVar7 = 0;
  do {
    GPIOB_BRR = 0x2000;
    uVar5 = param_2 >> (uVar7 & 0xff) & 1;
    if (uVar5 == 0) {
      GPIOB_BRR = 0x4000;
    }
    else {
      GPIOB_BSRR = 0x4000;
    }
    uVar4 = uVar4 ^ uVar5;
    uVar7 = uVar7 + 1;
    GPIOB_BSRR = 0x2000;
  } while ((int)uVar7 < 0x20);
  GPIOB_BRR = 0x2000;
  if (uVar4 == 0) {
    GPIOB_BRR = 0x4000;
  }
  else {
    GPIOB_BSRR = 0x4000;
  }
  GPIOB_BSRR = 0x2000;
  return 0;
}

/**
 * swd_write_reg
 *   Select DP(0)/AP(1) write-request byte from table (DAT_200014c4 / DAT_200014d8), perform swd_write_access, map status into 0x18/0x28 range.
 *   address 0x08007EC2, 70 bytes
 *   called from: dap_write_reg
 */
/* Select DP(0)/AP(1) write-request byte from table (DAT_200014c4 / DAT_200014d8), perform
   swd_write_access, map status into 0x18/0x28 range. */

ulonglong swd_write_reg(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 *puVar3;
  int iVar4;
  
  puVar3 = (undefined1 *)0x0;
  iVar4 = 0;
  if (param_1 == 0) {
    puVar3 = &swd_dp_write_req_table;
    iVar4 = 0x18;
  }
  else if (param_1 == 1) {
    puVar3 = &DAT_200014d8;
    iVar4 = 0x28;
  }
  if (puVar3 != (undefined1 *)0x0) {
    iVar1 = swd_write_access(puVar3[param_2],param_3);
    if (iVar1 == 1) {
      swd_write_access(swd_dp_write_req_table,0x1f);
    }
    else {
      uVar2 = 0;
      if (iVar1 == 0) goto LAB_08007f06;
    }
    return CONCAT44(param_4,iVar1 + iVar4) & 0xffffffff0000ffff;
  }
  uVar2 = 0x30;
LAB_08007f06:
  return CONCAT44(param_4,uVar2);
}

/**
 * swd_send_line_reset_clocks
 *   Drive SWDIO high and issue 53 SWCLK cycles (>50 = SWD line reset).
 *   address 0x08007F08, 62 bytes
 *   called from: swd_line_reset_idle, swd_send_switch_sequence, swd_connect
 */
/* Drive SWDIO high and issue 53 SWCLK cycles (>50 = SWD line reset). */

undefined4 swd_send_line_reset_clocks(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 local_8;
  
  GPIOB_BSRR = 0x4000;
  iVar2 = 0x35;
  do {
    GPIOB_BRR = 0x2000;
    local_8 = g_swd_clock_delay + 1;
    do {
      local_8 = local_8 + -1;
    } while (local_8 != 0);
    uVar1 = GPIOB_BSRR;
    GPIOB_BSRR = 0x2000;
    local_8 = g_swd_clock_delay + 1;
    do {
      local_8 = local_8 + -1;
    } while (local_8 != 0);
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return 0;
}

/**
 * swd_line_reset_idle
 *   Line-reset clocks (swd_send_line_reset_clocks) followed by 2 idle clocks with SWDIO low.
 *   address 0x08007F46, 66 bytes
 *   called from: swd_enter_special_sequence, swd_write_idle_word, swd_write_word_txn
 */
/* Line-reset clocks (swd_send_line_reset_clocks) followed by 2 idle clocks with SWDIO low. */

longlong swd_line_reset_idle(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 local_10;
  undefined4 uStack_c;
  
  swd_send_line_reset_clocks();
  GPIOB_BRR = 0x4000;
  iVar2 = 2;
  do {
    GPIOB_BRR = 0x2000;
    local_10 = g_swd_clock_delay + 1;
    do {
      local_10 = local_10 + -1;
    } while (local_10 != 0);
    uVar1 = GPIOB_BSRR;
    GPIOB_BSRR = 0x2000;
    local_10 = g_swd_clock_delay + 1;
    do {
      local_10 = local_10 + -1;
    } while (local_10 != 0);
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return (ulonglong)uStack_c << 0x20;
}

/**
 * swd_read_reg_retry
 *   (uncertain) Read a DP register (DAT_200014c3) via swd_read_access, retry up to 3 times, return status+0x10.
 *   address 0x08007F88, 38 bytes
 *   called from: mem_write_32_noinc, mem_write_8
 */
/* (uncertain) Read a DP register (DAT_200014c3) via swd_read_access, retry up to 3 times, return
   status+0x10. */

undefined8 swd_read_reg_retry(void)

{
  int iVar1;
  uint uVar2;
  undefined4 in_r3;
  int iVar3;
  undefined4 uStack_10;
  
  iVar1 = 1;
  iVar3 = 3;
  uStack_10 = in_r3;
  do {
    if (iVar3 < 1) goto LAB_08007fa8;
    iVar1 = swd_read_access(DAT_200014c3,&uStack_10,0);
    iVar3 = iVar3 + -1;
  } while (iVar1 == 1);
  uVar2 = 0;
  if (iVar1 != 0) {
LAB_08007fa8:
    uVar2 = iVar1 + 0x10U & 0xffff;
  }
  return CONCAT44(uStack_10,uVar2);
}

/**
 * swd_send_switch_sequence
 *   Line reset, then shift a 16-bit magic (param_1, e.g. 0xE79E JTAG-to-SWD) LSB-first, then idle clocks.
 *   address 0x08007FAE, 142 bytes
 *   called from: swd_bitbang_read_word, swd_read_idcode, swd_read_access, swd_write_access, swd_bitbang_write_word, swd_connect
 */
/* Line reset, then shift a 16-bit magic (param_1, e.g. 0xE79E JTAG-to-SWD) LSB-first, then idle
   clocks. */

longlong swd_send_switch_sequence(int param_1)

{
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  undefined4 local_20;
  undefined4 uStack_1c;
  
  GPIOB_BSRR = 0x4000;
  GPIOA_BRR = 2;
  uVar2 = GPIOB_CRH;
  GPIOB_CRH = uVar2 & 0xf0ffffff | 0x3000000;
  GPIOB_BSRR = 0x2000;
  swd_send_line_reset_clocks();
  uVar2 = 0;
  iVar3 = g_swd_clock_delay + 1;
  do {
    GPIOB_BRR = 0x2000;
    local_20 = iVar3;
    if ((param_1 >> (uVar2 & 0xff)) << 0x1f < 0) {
      GPIOB_BSRR = 0x4000;
    }
    else {
      GPIOB_BRR = 0x4000;
    }
    do {
      local_20 = local_20 + -1;
    } while (local_20 != 0);
    GPIOB_BSRR = 0x2000;
    local_20 = iVar3;
    do {
      local_20 = local_20 + -1;
    } while (local_20 != 0);
    uVar2 = uVar2 + 1;
  } while ((int)uVar2 < 0x10);
  swd_send_line_reset_clocks();
  GPIOB_BRR = 0x4000;
  iVar3 = 2;
  do {
    GPIOB_BRR = 0x2000;
    local_20 = g_swd_clock_delay + 1;
    do {
      local_20 = local_20 + -1;
    } while (local_20 != 0);
    uVar1 = GPIOB_BSRR;
    GPIOB_BSRR = 0x2000;
    local_20 = g_swd_clock_delay + 1;
    do {
      local_20 = local_20 + -1;
    } while (local_20 != 0);
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return (ulonglong)uStack_1c << 0x20;
}

/**
 * swd_read_idcode
 *   SWD attach: optional GPIO/clock re-init, send switch sequence 0xE79E, read DPIDR via swd_read_access, return status+8.
 *   address 0x0800803C, 44 bytes
 *   called from: target_connect
 */
/* SWD attach: optional GPIO/clock re-init, send switch sequence 0xE79E, read DPIDR via
   swd_read_access, return status+8. */

undefined8 swd_read_idcode(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  
  g_swd_reconnect_pending = 0;
  g_swd_reconnect_val = 0;
  if (param_1 == 0) {
    swd_init_gpio();
    usart_bridge_init();
  }
  swd_send_switch_sequence(0xe79e);
  iVar1 = swd_read_access(swd_dp_read_req_table,&g_swd_reconnect_val,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = iVar1 + 8U & 0xffff;
  }
  return CONCAT44(param_4,uVar2);
}

/**
 * swd_bb_write_byte
 *   Bit-bang 8 bits of param_1 LSB-first on SWDIO with SWCLK and inter-bit delay (DAT_200014d0).
 *   address 0x08008068, 66 bytes
 *   called from: swd_enter_special_sequence, swd_send_auth_key, swd_connect
 */
/* Bit-bang 8 bits of param_1 LSB-first on SWDIO with SWCLK and inter-bit delay (DAT_200014d0). */

undefined4 swd_bb_write_byte(uint param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 local_10;
  
  uVar1 = 0;
  iVar2 = g_swd_clock_delay + 1;
  do {
    GPIOB_BRR = 0x2000;
    local_10 = iVar2;
    if ((int)((param_1 >> (uVar1 & 0xff)) << 0x1f) < 0) {
      GPIOB_BSRR = 0x4000;
    }
    else {
      GPIOB_BRR = 0x4000;
    }
    do {
      local_10 = local_10 + -1;
    } while (local_10 != 0);
    GPIOB_BSRR = 0x2000;
    local_10 = iVar2;
    do {
      local_10 = local_10 + -1;
    } while (local_10 != 0);
    uVar1 = uVar1 + 1;
  } while ((int)uVar1 < 8);
  return 0;
}

/**
 * swd_write_word_txn
 *   (uncertain) Line-reset idle, write one 32-bit word (swd_bb_write_word) then read back/ack (swd_read_access); one word of a write transaction.
 *   address 0x080080AA, 40 bytes
 *   called from: target_connect, swd_write_word_block
 */
/* (uncertain) Line-reset idle, write one 32-bit word (swd_bb_write_word) then read back/ack
   (swd_read_access); one word of a write transaction. */

undefined8
swd_write_word_txn(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  
  swd_line_reset_idle();
  iVar1 = swd_bb_write_word(DAT_200014c7,param_1);
  if (iVar1 == 0) {
    iVar1 = swd_read_access(swd_dp_read_req_table,&g_swd_reconnect_val,1);
  }
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = iVar1 + 8U & 0xffff;
  }
  return CONCAT44(param_4,uVar2);
}

/**
 * swd_write_idle_word
 *   Line-reset idle then write an all-ones (0xFFFFFFFF) word via swd_bb_write_word.
 *   address 0x080080D2, 26 bytes
 *   called from: swd_write_word_block, stlink_dispatch_command
 */
/* Line-reset idle then write an all-ones (0xFFFFFFFF) word via swd_bb_write_word. */

undefined8 swd_write_idle_word(void)

{
  int iVar1;
  uint uVar2;
  undefined4 unaff_r7;
  
  swd_line_reset_idle();
  iVar1 = swd_bb_write_word(DAT_200014c7,0xffffffff);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = iVar1 + 8U & 0xffff;
  }
  return CONCAT44(unaff_r7,uVar2);
}

/**
 * swd_write_word_block
 *   (uncertain) Loop writing an array of 32-bit words via swd_write_word_txn, recording success count and terminator offsets; builds a 0x80 status header.
 *   address 0x080080EC, 98 bytes
 *   called from: stlink_cmd_read_trace_buf
 */
/* (uncertain) Loop writing an array of 32-bit words via swd_write_word_txn, recording success count
   and terminator offsets; builds a 0x80 status header. */

undefined4
swd_write_word_block(undefined4 *param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  short sVar6;
  bool bVar7;
  
  uVar4 = 0;
  uVar3 = 4;
  sVar6 = 0;
  iVar1 = swd_write_word_txn(*param_1);
  if (iVar1 == 0) {
    bVar7 = true;
    sVar6 = 1;
    swd_write_idle_word();
  }
  else {
    bVar7 = false;
  }
  while( true ) {
    uVar5 = uVar4 & 0xffff;
    if (param_2 <= uVar5) break;
    uVar2 = param_1[uVar5 + 1];
    if (bVar7) {
      *(short *)(uVar3 + (int)param_1) = (short)uVar4;
      uVar3 = uVar3 + 2 & 0xffff;
    }
    iVar1 = swd_write_word_txn(uVar2);
    bVar7 = iVar1 == 0;
    if (bVar7) {
      sVar6 = sVar6 + 1;
      swd_write_idle_word();
    }
    uVar4 = uVar5 + 1;
  }
  if (bVar7) {
    *(short *)(uVar3 + (int)param_1) = (short)uVar4 + -1;
  }
  *(undefined2 *)param_1 = 0x80;
  *(short *)((int)param_1 + 2) = sVar6;
  return param_4;
}

/**
 * swd_send_auth_key
 *   (uncertain) Send an 18-byte constant key (0xF392FFFF 0x2D956209 ...) byte-by-byte via swd_bb_write_byte, then idle clocks; part of a target unlock/auth handshake.
 *   address 0x0800814E, 82 bytes
 *   called from: swd_enter_special_sequence, swd_connect
 */
/* (uncertain) Send an 18-byte constant key (0xF392FFFF 0x2D956209 ...) byte-by-byte via
   swd_bb_write_byte, then idle clocks; part of a target unlock/auth handshake. */

void swd_send_auth_key(void)

{
  code *extraout_r1;
  undefined4 *puVar1;
  int iVar2;
  code *pcVar3;
  int local_28;
  undefined4 local_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  
  pcVar3 = jtag_set_frequency;
  local_24 = 0xf392ffff;
  uStack_20 = 0x2d956209;
  uStack_1c = 0xafe98685;
  uStack_18 = 0xea2e3dd;
  uStack_14 = 0x19bc;
  iVar2 = 0x12;
  puVar1 = &local_24;
  do {
    swd_bb_write_byte(*(undefined1 *)puVar1,pcVar3);
    iVar2 = iVar2 + -1;
    pcVar3 = extraout_r1;
    puVar1 = (undefined4 *)((int)puVar1 + 1);
  } while (iVar2 != 0);
  iVar2 = 4;
  do {
    GPIOB_BRR = 0x2000;
    GPIOB_BRR = 0x4000;
    local_28 = g_swd_clock_delay + 1;
    do {
      local_28 = local_28 + -1;
    } while (local_28 != 0);
    GPIOB_BSRR = 0x2000;
    local_28 = g_swd_clock_delay + 1;
    do {
      local_28 = local_28 + -1;
    } while (local_28 != 0);
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}

/**
 * swd_enter_special_sequence
 *   (uncertain) Mode-1 special enter: GPIO re-init then send 0xFF 0xBA 0xBB 0xBB 0x33, the auth key, and 0x1A; sets DAT_200014c8=1.
 *   address 0x080081A0, 94 bytes
 *   called from: target_connect
 */
/* (uncertain) Mode-1 special enter: GPIO re-init then send 0xFF 0xBA 0xBB 0xBB 0x33, the auth key,
   and 0x1A; sets DAT_200014c8=1. */

longlong swd_enter_special_sequence(void)

{
  uint uVar1;
  uint unaff_r7;
  
  g_swd_reconnect_pending = 1;
  g_swd_reconnect_val = 0;
  swd_init_gpio();
  usart_bridge_init();
  GPIOB_BSRR = 0x4000;
  GPIOA_BRR = 2;
  uVar1 = GPIOB_CRH;
  GPIOB_CRH = uVar1 & 0xf0ffffff | 0x3000000;
  swd_bb_write_byte(0xff);
  swd_bb_write_byte(0xba);
  swd_bb_write_byte(0xbb);
  swd_bb_write_byte(0xbb);
  swd_bb_write_byte(0x33);
  swd_send_auth_key();
  swd_bb_write_byte(0x1a);
  swd_line_reset_idle();
  return (ulonglong)unaff_r7 << 0x20;
}

/**
 * swd_connect
 *   Full SWD connect/HW config: SWD write, optional dormant/switch bytes, EXTI/GPIOA config, DMA (DAT_40020058) and timer setup for the bit-bang transport.
 *   address 0x080081FE, 210 bytes
 *   called from: target_disconnect, target_connect
 */
/* Full SWD connect/HW config: SWD write, optional dormant/switch bytes, EXTI/GPIOA config, DMA
   (DAT_40020058) and timer setup for the bit-bang transport. */

undefined4 swd_connect(int param_1)

{
  ushort uVar1;
  int iVar2;
  undefined1 local_18;
  undefined1 local_17;
  undefined1 local_16;
  undefined1 local_15;
  undefined2 local_14;
  undefined1 local_12;
  undefined1 local_11;
  undefined2 local_10;
  
  swd_write_access(DAT_200014c5,0);
  if (param_1 == 1) {
    swd_send_line_reset_clocks();
    swd_bb_write_byte(0xbc);
    swd_bb_write_byte(0xe3);
    swd_send_auth_key();
    swd_bb_write_byte(0);
    swd_bb_write_byte(0);
  }
  else {
    iVar2 = target_part_is_special();
    if (iVar2 == 0) {
      swd_send_switch_sequence(0xe73c);
    }
  }
  jtag_init_gpio();
  uVar1 = DAT_4001380c;
  DAT_4001380c = uVar1 & 0xdfff;
  uVar1 = DAT_40013800;
  if ((int)((uint)uVar1 << 0x1c) < 0) {
    local_10 = DAT_40013804;
  }
  USART_DeInit();
  local_14 = 0x400;
  local_11 = 4;
  local_12 = 3;
  GPIO_Init(&GPIOA_CRL,&local_14);
  DMA_DeInit(&DAT_40020058);
  DMA_ITConfig(&DAT_40020058,2,0);
  local_18 = 0xf;
  local_17 = 0;
  local_16 = 1;
  local_15 = 0;
  NVIC_Init(&local_18);
  RCC_APB2PeriphClockCmd(0x4000,0);
  RCC_AHBPeriphClockCmd(1,0);
  return 0;
}

