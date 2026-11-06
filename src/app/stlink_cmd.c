/*
 * app/stlink_cmd.c - ST-Link USB command handlers
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * stlink_send_reply
 *   Sends a command reply over EP1-IN; if the target connection was lost it overwrites the reply with status 5 (dp-error/disconnect) first
 *   address 0x08005620, 70 bytes
 *   called from: stlink_cmd_rw_dap_reg, stlink_dispatch_command
 *   prototype: uint64_t stlink_send_reply(uint32_t ep, uint8_t *reply, uint32_t len, int keep)
 */
/* Sends a command reply over EP1-IN; if the target connection was lost it overwrites the reply with
   status 5 (dp-error/disconnect) first */

undefined8 stlink_send_reply(undefined4 param_1,undefined1 *param_2,undefined4 param_3,int param_4)

{
  undefined1 *puVar1;
  undefined6 uStack_16;
  
  if (param_4 == 0) {
    if (g_trace_active == '\0') {
      puVar1 = &g_target_connected;
      if (g_target_connected != '\0') {
        puVar1 = (undefined1 *)(uint)g_conn_lost;
      }
      if (g_target_connected == '\0' || puVar1 == (undefined1 *)0x1) goto LAB_0800565c;
    }
    __aeabi_memclr(param_2,param_3);
    *param_2 = 5;
    param_2[1] = 0;
  }
LAB_0800565c:
  stlink_cmd_send_reply(param_1,param_2,param_3,1);
  return CONCAT62(uStack_16,5);
}

/**
 * stlink_cmd_set_freq
 *   Handles the SWD/JTAG set-frequency command: programs the bit-bang clock delay (swd_set_clock_delay) or returns the configured divisor, then replies
 *   address 0x0800566C, 64 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint64_t stlink_cmd_set_freq(int set, int cmd_idx)
 */
/* Handles the SWD/JTAG set-frequency command: programs the bit-bang clock delay
   (swd_set_clock_delay) or returns the configured divisor, then replies */

ulonglong stlink_cmd_set_freq(int param_1,int param_2)

{
  undefined2 uVar1;
  undefined4 local_10 [2];
  
  if (param_1 == 1) {
    swd_set_clock_delay();
    uVar1 = dap_ack_to_status();
  }
  else {
    uVar1 = jtag_set_frequency(*(undefined2 *)(&g_cmd_block + param_2 * 0x10));
  }
  local_10[0] = CONCAT22(local_10[0]._2_2_,uVar1);
  stlink_cmd_send_reply(param_2,local_10,2,1);
  return CONCAT44(local_10[0],local_10[0]) & 0xffffffff0000ffff;
}

/**
 * stlink_cmd_get_conn_status
 *   (uncertain) Replies with a 4-byte status reporting the current AP index and whether a target is connected (status 5 if disconnected)
 *   address 0x080056AC, 86 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void stlink_cmd_get_conn_status(uint32_t ep)
 */
/* (uncertain) Replies with a 4-byte status reporting the current AP index and whether a target is
   connected (status 5 if disconnected) */

void stlink_cmd_get_conn_status(undefined4 param_1)

{
  char cVar1;
  undefined4 local_10;
  undefined2 local_c;
  
  local_10 = (uint)CONCAT12((char)g_current_ap,8);
  local_c = 5;
  if (g_trace_active == '\0') {
    cVar1 = '\0';
    if (g_target_connected != '\0') {
      cVar1 = g_conn_lost;
    }
    if (g_target_connected == '\0' || cVar1 == '\x01') goto LAB_080056f4;
  }
  local_10 = 5;
LAB_080056f4:
  stlink_cmd_send_reply(param_1,&local_10,4,1);
  return;
}

/**
 * stlink_cmd_rw_dap_reg
 *   APIv2 read/write-DAP-register command: performs a DP or AP register read or write (bank/AP selected from the command) and returns the 2-byte status plus 4-byte value
 *   address 0x08005714, 536 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void stlink_cmd_rw_dap_reg(int cmd_idx, int is_read)
 */
/* APIv2 read/write-DAP-register command: performs a DP or AP register read or write (bank/AP
   selected from the command) and returns the 2-byte status plus 4-byte value */

void stlink_cmd_rw_dap_reg(int param_1,int param_2)

{
  ushort uVar1;
  ushort uVar2;
  char cVar3;
  ushort uVar4;
  int iVar5;
  undefined4 uVar6;
  ushort uVar7;
  uint uVar8;
  undefined4 uVar9;
  bool bVar10;
  undefined2 local_30;
  uint local_2c [2];
  
  iVar5 = param_1 * 0x10;
  uVar1 = *(ushort *)(&g_cmd_block + iVar5);
  uVar8 = (uint)uVar1;
  uVar2 = *(ushort *)(&DAT_200030a0 + iVar5);
  uVar9 = 0;
  uVar7 = uVar2 >> 2 & 3;
  led_blink_activity();
  if (g_trace_active == '\0') {
    cVar3 = '\0';
    if (g_target_connected != '\0') {
      cVar3 = g_conn_lost;
    }
    if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_0800575c;
  }
  else {
LAB_0800575c:
    uVar4 = 0xffff;
    if (uVar8 != 0xffff) {
      uVar4 = uVar2 & 0xfff0;
    }
    if (uVar8 != 0xffff && uVar4 != 0xf0) {
      local_30 = 5;
      local_30._0_1_ = 5;
      goto LAB_080058e6;
    }
  }
  uVar9 = 1;
  if (param_2 == 0) {
    local_2c[0] = *(uint *)(&DAT_200030a2 + iVar5);
  }
  if (uVar8 == 0xffff) {
    iVar5 = dap_select_dpbank(uVar2 >> 4 & 0xf);
    local_30 = (short)iVar5;
    if (iVar5 == 0x80) {
      if (param_2 == 1) {
        uVar8 = dap_read_reg_checked(0,uVar7,local_2c);
        local_30 = (short)uVar8;
        bVar10 = uVar8 == 0x80;
        if (bVar10) {
          uVar8 = (uint)g_debug_mode_bitbang;
        }
        if (bVar10 && uVar8 == 0) {
          local_30 = dap_read_reg_checked(uVar8,3,local_2c);
        }
      }
      else {
        iVar5 = dap_write_reg_checked(0,uVar7,local_2c[0]);
        local_30 = (short)iVar5;
        if (uVar7 == 2) {
          if (iVar5 == 0x80) {
            g_dp_select_cache = local_2c[0];
            g_current_ap = local_2c[0] >> 0x18;
          }
          else {
            g_dp_select_cache = 0xfff0;
          }
        }
      }
      if ((local_30 == 0x80) && ((uVar2 & 0xf0) != 0)) {
        dap_select_dpbank(0);
      }
    }
  }
  else {
    g_requested_ap = uVar8;
    if (((uVar2 & 0xfff0) != 0xf0) &&
       ((uVar8 = (uint)(byte)uVar1, 8 < uVar8 || ((&g_ap_slot_table)[uVar8 * 0xc] == '\0')))) {
      bVar10 = uVar8 == 0;
      if (bVar10) {
        uVar8 = (uint)g_rom_detected;
      }
      if (!bVar10 || uVar8 != 1) {
        local_30 = 0x1d;
        goto LAB_080058ce;
      }
    }
    iVar5 = dap_select_ap(uVar2 & 0xf0);
    local_30 = (short)iVar5;
    if (iVar5 == 0x80) {
      if (param_2 == 1) {
        iVar5 = dap_read_reg(1,uVar7,local_2c);
        local_30 = (short)iVar5;
        if (iVar5 == 0x80) {
          iVar5 = dap_read_reg(0,3,local_2c);
          if (iVar5 != 0x80) {
            dap_read_rdbuff();
          }
          local_30 = (short)iVar5;
        }
        else {
          dap_read_rdbuff();
        }
      }
      else {
        local_30 = dap_write_reg_checked(1,uVar7,local_2c[0]);
        if ((uVar2 == 0) && ((&g_ap_csw_cache)[g_requested_ap] = 0xff, local_30 == 0x80)) {
          (&g_ap_tar_cache)[g_current_ap] = local_2c[0] & 0xffffff00;
        }
      }
    }
  }
LAB_080058ce:
LAB_080058e6:
  DAT_20001531 = local_30._1_1_;
  g_reply_buf = (undefined1)local_30;
  led_set_result(local_30 == 0x80);
  if (param_2 == 1) {
    DAT_20001532 = 0;
    DAT_20001533 = 0;
    __aeabi_memcpy(&DAT_20001534,local_2c,4);
    uVar6 = 8;
  }
  else {
    uVar6 = 2;
  }
  stlink_send_reply(param_1,&g_reply_buf,uVar6,uVar9);
  return;
}

/**
 * stlink_cmd_set_led_ack
 *   (uncertain) Sets the status LED blink state (led mode 6) and replies with status 0x80
 *   address 0x0800592C, 36 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint64_t stlink_cmd_set_led_ack(uint32_t ep)
 */
/* (uncertain) Sets the status LED blink state (led mode 6) and replies with status 0x80 */

undefined8 stlink_cmd_set_led_ack(undefined4 param_1)

{
  undefined2 local_10;
  undefined6 uStack_e;
  
  led_set_mode(6,200,0xf);
  local_10 = 0x80;
  stlink_cmd_send_reply(param_1,&local_10,2,1);
  return CONCAT62(uStack_e,local_10);
}

/**
 * stlink_cmd_get_fw_build
 *   Replies with the firmware build/version string (11 bytes copied from 0x08003C33) when the signature marker byte is present, else zero-fills
 *   address 0x08005950, 76 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint8_t stlink_cmd_get_fw_build(uint32_t ep)
 */
/* Replies with the firmware build/version string (11 bytes copied from 0x08003C33) when the
   signature marker byte is present, else zero-fills */

undefined1 stlink_cmd_get_fw_build(undefined4 param_1)

{
  undefined1 uVar1;
  
  if (data_08003C32 == (code)0x56) {
    uVar1 = 0x80;
    __aeabi_memcpy(&DAT_20001532,&DAT_08003c33,0xb);
  }
  else {
    uVar1 = 8;
    __aeabi_memset(&DAT_20001532,0xb,0x20);
  }
  DAT_2000153d = 0;
  DAT_20001531 = 0;
  g_reply_buf = uVar1;
  stlink_cmd_send_reply(param_1,&g_reply_buf,0xe,1);
  return uVar1;
}

/**
 * stlink_cmd_get_version
 *   GET_VERSION (0xF1)-family handler: assembles the 0x40-byte version reply including the embedded version/ID strings from the signature block
 *   address 0x080059A0, 128 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint32_t stlink_cmd_get_version(uint32_t ep)
 */
/* WARNING: Removing unreachable block (ram,0x080059cc) */
/* GET_VERSION (0xF1)-family handler: assembles the 0x40-byte version reply including the embedded
   version/ID strings from the signature block */

undefined4 stlink_cmd_get_version(undefined4 param_1)

{
  int iVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  
  iVar1 = stlink_cmd_is_first_block(param_1);
  if (iVar1 == 1) {
    __aeabi_memclr(&g_reply_buf,0x80);
    if (data_08003C52 == (code)0x52) {
      __aeabi_memcpy(&DAT_20001537,&DAT_08003c53,0x10);
    }
    if (data_08003C32 == (code)0x56) {
      __aeabi_memcpy(&DAT_20001548,&DAT_08003c33,0xb);
    }
    uVar3 = 0;
    g_reply_buf = 0x80;
    DAT_20001531 = 0;
    puVar2 = &g_reply_buf;
  }
  else {
    uVar3 = 1;
    puVar2 = &DAT_20001570;
  }
  stlink_cmd_send_reply(param_1,puVar2,0x40,uVar3);
  return 0x80;
}

/**
 * stlink_cmd_get_param_block
 *   (uncertain) Returns a 0x24-byte parameter/config block (defaults 2000,1,36000,0x12,0xFFFF) for the current command slot, or error 6 if unavailable
 *   address 0x08005A2C, 150 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint64_t stlink_cmd_get_param_block(int cmd_idx, uint32_t a, uint32_t b, uint32_t c)
 */
/* (uncertain) Returns a 0x24-byte parameter/config block (defaults 2000,1,36000,0x12,0xFFFF) for
   the current command slot, or error 6 if unavailable */

undefined8
stlink_cmd_get_param_block(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  undefined4 local_18;
  undefined1 local_14;
  undefined1 uStack_13;
  undefined2 uStack_12;
  
  uStack_12 = (undefined2)((uint)param_4 >> 0x10);
  local_14 = 0x80;
  uStack_13 = 0;
  cVar1 = (&g_cmd_block)[param_1 * 0x10];
  local_18 = param_3;
  __aeabi_memclr(&g_reply_buf,0x24);
  if (cVar1 == '\0') {
    local_18 = 2000;
    __aeabi_memcpy(&DAT_20001534,&local_18,4);
    local_18 = 1;
    __aeabi_memcpy(&DAT_20001538,&local_18,4);
    local_18 = 36000;
    __aeabi_memcpy(&DAT_2000153c,&local_18,4);
    local_18 = 0x12;
    __aeabi_memcpy(&DAT_20001540,&local_18,4);
    local_18 = 0xffff;
    __aeabi_memcpy(&DAT_20001544,&local_18,4);
  }
  else {
    local_14 = 6;
    uStack_13 = 0;
  }
  g_reply_buf = local_14;
  DAT_20001531 = uStack_13;
  stlink_cmd_send_reply(param_1,&g_reply_buf,0x24,1);
  return CONCAT26(uStack_12,CONCAT15(uStack_13,CONCAT14(local_14,local_18)));
}

/**
 * stlink_cmd_jtag_raw
 *   Raw JTAG IR/DR transfer command: dispatches to jtag_shift_short or jtag_shift_ir_dr from the command block and replies with the 2-byte status
 *   address 0x08005AC2, 120 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint32_t stlink_cmd_jtag_raw(int cmd_idx, uint32_t a, uint32_t b, uint32_t c)
 */
/* Raw JTAG IR/DR transfer command: dispatches to jtag_shift_short or jtag_shift_ir_dr from the
   command block and replies with the 2-byte status */

undefined4 stlink_cmd_jtag_raw(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  short sVar2;
  undefined2 uVar4;
  undefined4 uVar3;
  
  iVar1 = param_1 * 0x10;
  if ((&DAT_2000309f)[iVar1] == '\0') {
    sVar2 = jtag_shift_short();
    uVar4 = (undefined2)((uint)param_4 >> 0x10);
  }
  else {
    sVar2 = jtag_shift_ir_dr((&DAT_2000309f)[iVar1],(&g_cmd_block)[iVar1],
                             *(undefined4 *)(&DAT_200030a0 + iVar1),
                             *(undefined4 *)(&DAT_200030a4 + iVar1),param_4);
    uVar4 = (undefined2)((uint)param_4 >> 0x10);
  }
  uVar3 = CONCAT22(uVar4,sVar2);
  led_set_result(sVar2 == 0x80);
  g_reply_buf = (undefined1)uVar3;
  DAT_20001531 = (undefined1)((uint)uVar3 >> 8);
  stlink_cmd_send_reply(param_1,&g_reply_buf,2,1);
  return uVar3;
}

/**
 * stlink_cmd_get_caps
 *   (uncertain) Returns a 0x30-byte capability/status block (status 0x80, field=10, value 0x800)
 *   address 0x08005B3A, 70 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void stlink_cmd_get_caps(uint32_t ep)
 */
/* (uncertain) Returns a 0x30-byte capability/status block (status 0x80, field=10, value 0x800) */

void stlink_cmd_get_caps(undefined4 param_1)

{
  undefined4 local_18;
  undefined2 local_14;
  
  local_14 = 0x80;
  local_18 = 0x800;
  __aeabi_memclr(&g_reply_buf,0x30);
  g_reply_buf = (undefined1)local_14;
  DAT_20001531 = local_14._1_1_;
  DAT_20001532 = 10;
  __aeabi_memcpy(&DAT_20001534,&local_18,4);
  stlink_cmd_send_reply(param_1,&g_reply_buf,0x30,1);
  return;
}

/**
 * stlink_cmd_read_trace_buf
 *   Reads the SWO/trace capture buffer: on first call sets up the byte count (max 0xBFE), then streams it out in 0x40-byte chunks from DAT_20002F30
 *   address 0x08005B88, 146 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint32_t stlink_cmd_read_trace_buf(int cmd_idx, uint32_t a, uint32_t b, uint32_t ep)
 */
/* Reads the SWO/trace capture buffer: on first call sets up the byte count (max 0xBFE), then
   streams it out in 0x40-byte chunks from DAT_20002F30 */

undefined4
stlink_cmd_read_trace_buf(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  
  iVar1 = stlink_cmd_is_first_block();
  if (iVar1 == 1) {
    g_trace_count = (uint)*(ushort *)(&g_cmd_block + param_1 * 0x10);
    if (0xbfe < g_trace_count) {
      g_trace_count = 0xbfe;
    }
    g_trace_remaining = g_trace_count << 2;
    g_trace_offset = 0;
    if (g_trace_count == 0) {
      return param_4;
    }
  }
  else {
    uVar3 = g_trace_remaining;
    if (0x40 < g_trace_remaining) {
      uVar3 = 0x40;
    }
    __aeabi_memcpy(&g_reply_buf + g_trace_offset,&stlink_ep2out_buf + param_1 * 0x40,uVar3);
    g_trace_offset = uVar3 + g_trace_offset;
    g_trace_remaining = g_trace_remaining - uVar3;
    if (g_trace_remaining == 0) {
      swd_write_word_block(&g_reply_buf,g_trace_count & 0xffff);
      uVar2 = stlink_cmd_set_status_idle(param_1,param_4);
      return uVar2;
    }
  }
  uVar2 = stlink_cmd_set_status_busy(param_1,param_4);
  return uVar2;
}

/**
 * stlink_cmd_get_rw_status
 *   (uncertain) Replies with an 8-byte read/write-status block (status 0x80, field=6)
 *   address 0x08005C20, 42 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void stlink_cmd_get_rw_status(uint32_t ep, uint32_t a, uint32_t b, uint32_t c)
 */
/* (uncertain) Replies with an 8-byte read/write-status block (status 0x80, field=6) */

void stlink_cmd_get_rw_status
               (undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  __aeabi_memclr(&g_reply_buf,8,param_3,param_4,param_4);
  g_reply_buf = 0x80;
  DAT_20001532 = 6;
  stlink_cmd_send_reply(param_1,&g_reply_buf,8,1);
  return;
}

/**
 * stlink_cmd_debug_connect
 *   APIv2 enter/exit-debug command handler: selects SWD vs JTAG mode (from sub-bytes 0xA3/0xA5/0xA4), connects or disconnects the target and manages key-table slots
 *   address 0x08005ECC, 560 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint64_t stlink_cmd_debug_connect(int cmd_idx, uint32_t sub, uint32_t a, uint32_t b)
 */
/* APIv2 enter/exit-debug command handler: selects SWD vs JTAG mode (from sub-bytes 0xA3/0xA5/0xA4),
   connects or disconnects the target and manages key-table slots */

undefined8 stlink_cmd_debug_connect(int param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  char cVar2;
  undefined2 uVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  undefined1 local_28;
  undefined1 uStack_27;
  byte local_26;
  undefined1 uStack_25;
  undefined1 local_24;
  undefined1 uStack_23;
  undefined2 uStack_22;
  
  local_24 = (undefined1)param_4;
  uStack_23 = (undefined1)((uint)param_4 >> 8);
  uStack_22 = (undefined2)((uint)param_4 >> 0x10);
  uStack_25 = (undefined1)((uint)param_3 >> 0x18);
  local_26 = 0;
  iVar6 = 0;
  local_28 = 0x80;
  uStack_27 = 0;
  iVar7 = -1;
  iVar1 = param_1 * 0x10;
  if ((&DAT_2000309f)[iVar1] != '\0') {
    iVar6 = 0xff;
  }
  if (param_2 == 1) {
LAB_08005f00:
    cVar2 = (&g_cmd_block)[iVar1];
    if (cVar2 == -0x5d) {
      g_debug_mode_bitbang = 1;
    }
    else if (cVar2 == -0x5b) {
      g_debug_mode_bitbang = 1;
      iVar6 = 2;
    }
    else {
      g_debug_mode_bitbang = 0;
    }
    if (param_2 == 3) {
      swim_deinit();
      g_connect_phase = 1;
    }
    if (iVar6 == 2) {
      iVar7 = *(int *)(&DAT_200030a0 + iVar1);
    }
    if (CONCAT11(uStack_27,local_28) == 0x80) {
      uVar3 = target_connect(param_1,cVar2 != -0x5c,iVar6,iVar7);
      local_28 = (undefined1)uVar3;
      uStack_27 = (undefined1)((ushort)uVar3 >> 8);
    }
    ap_tables_reset();
    if (CONCAT11(uStack_27,local_28) == 0x80) {
      uVar3 = apkey_table_add(iVar1 + 0x200030a8,(&g_ap_state)[param_1],0);
      local_28 = (undefined1)uVar3;
      uStack_27 = (undefined1)((ushort)uVar3 >> 8);
    }
    if (CONCAT11(uStack_27,local_28) == 0x80) {
      g_connect_phase = 2;
      led_set_mode(2,0,0);
    }
    if ((&stlink_cmd_subcmd)[iVar1] == ' ') {
      g_connect_phase = 2;
      goto LAB_080060f2;
    }
    goto LAB_080060dc;
  }
  if (param_2 == 0) {
LAB_080060ec:
    stlink_cmd_reply_unknown(param_1);
    goto LAB_080060f2;
  }
  if (param_2 == 3) goto LAB_08005f00;
  if (2 < param_2) goto LAB_080060ec;
  iVar4 = cmd_check_allowed(iVar1 + 0x200030a8,(&g_cmd_block)[iVar1],&local_26);
  local_28 = (undefined1)iVar4;
  uStack_27 = (undefined1)((uint)iVar4 >> 8);
  if (iVar4 == 0x80) {
    if (((&g_ap_state)[param_1] == '\x03') &&
       ((iVar7 = *(int *)(&DAT_200030a0 + iVar1), g_reset_vector == -1 && iVar7 == -1 ||
        (g_reset_vector != iVar7 && iVar7 != -1)))) {
      local_28 = 0x1e;
      uStack_27 = 0;
    }
    if ((&g_cmd_block)[iVar1] == -0x5d || (&g_cmd_block)[iVar1] == -0x5b) {
      if ((&g_ap_state)[param_1] == '\x01') {
        target_disconnect(param_1);
      }
      if ((&g_cmd_block)[iVar1] != -0x5b) goto LAB_0800601a;
      iVar6 = 2;
LAB_08006020:
      uVar5 = 1;
    }
    else {
LAB_0800601a:
      if (((&g_cmd_block)[iVar1] == -0x5d) || ((&g_cmd_block)[iVar1] == -0x5b)) goto LAB_08006020;
      if ((&g_ap_state)[param_1] == '\x02') {
        target_disconnect(param_1);
      }
      g_debug_mode_bitbang = 0;
      uVar5 = 1;
      if ((&g_cmd_block)[iVar1] == -0x5c) {
        uVar5 = 0;
      }
    }
    if ((g_apkey_count == '\0') ||
       ((g_apkey_count == '\x01' && ((&DAT_20002fac)[(uint)local_26 * 5] != '\0')))) {
      uVar3 = target_connect(param_1,uVar5,iVar6,iVar7);
      local_28 = (undefined1)uVar3;
      uStack_27 = (undefined1)((ushort)uVar3 >> 8);
      ap_tables_reset();
      if (CONCAT11(uStack_27,local_28) == 0x80) goto LAB_08006082;
    }
    else {
      local_28 = 0x80;
      uStack_27 = 0;
LAB_08006082:
      uVar3 = apkey_table_add(iVar1 + 0x200030a8,(&g_ap_state)[param_1],local_26);
      local_28 = (undefined1)uVar3;
      uStack_27 = (undefined1)((ushort)uVar3 >> 8);
    }
    if (CONCAT11(uStack_27,local_28) == 0x80) {
      led_set_mode(2,0,0);
    }
  }
  if ((&stlink_cmd_subcmd)[iVar1] == ' ') {
LAB_080060f2:
    stlink_cmd_set_status_idle(param_1);
    return CONCAT26(uStack_22,
                    CONCAT15(uStack_23,
                             CONCAT14(local_24,CONCAT13(uStack_25,
                                                        CONCAT12(local_26,CONCAT11(uStack_27,
                                                                                   local_28))))));
  }
  if (iVar6 != 0xff) {
    local_24 = 5;
    uStack_23 = 0;
    if (g_trace_active == '\0') {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected == '\0' || cVar2 == '\x01') goto LAB_080060dc;
    }
    local_28 = 5;
    uStack_27 = 0;
  }
LAB_080060dc:
  stlink_cmd_send_reply(param_1,&local_28,2,1);
  return CONCAT26(uStack_22,
                  CONCAT15(uStack_23,
                           CONCAT14(local_24,CONCAT13(uStack_25,
                                                      CONCAT12(local_26,CONCAT11(uStack_27,local_28)
                                                              )))));
}

/**
 * debug_read_core_status
 *   Read Cortex-M DHCSR (0xE000EDF0) and decode S_RESET/S_HALT/S_SLEEP/S_LOCKUP bits into *param_1 (1..4); returns 0x80.
 *   address 0x08008F4C, 72 bytes
 *   called from: debug_poll_reset
 */
/* Read Cortex-M DHCSR (0xE000EDF0) and decode S_RESET/S_HALT/S_SLEEP/S_LOCKUP bits into *param_1
   (1..4); returns 0x80. */

undefined8 debug_read_core_status(undefined1 *param_1)

{
  int iVar1;
  undefined1 uVar2;
  int local_10 [2];
  
  if (param_1 == (undefined1 *)0x0) {
    return CONCAT44(local_10[0],6);
  }
  *param_1 = 0;
  iVar1 = mem_read_word(&DAT_e000edf0,local_10,0);
  if (iVar1 == 0x80) {
    if (local_10[0] << 6 < 0) {
      uVar2 = 4;
    }
    else if (local_10[0] << 0xe < 0) {
      uVar2 = 1;
    }
    else if (local_10[0] << 0xc < 0) {
      uVar2 = 2;
    }
    else {
      if (-1 < local_10[0] << 0xd) goto LAB_08008f92;
      uVar2 = 3;
    }
    *param_1 = uVar2;
  }
LAB_08008f92:
  return CONCAT44(local_10[0],iVar1);
}

/**
 * debug_poll_reset
 *   Poll debug_read_core_status twice looking for a reset-state transition; returns 1 when detected.
 *   address 0x08008F98, 52 bytes
 *   called from: target_scan_rom_table
 */
/* Poll debug_read_core_status twice looking for a reset-state transition; returns 1 when detected.
    */

longlong debug_poll_reset(void)

{
  int iVar1;
  uint unaff_r7;
  bool bVar2;
  
  iVar1 = debug_read_core_status(&stack0xfffffff8);
  bVar2 = iVar1 == 0x80;
  if (bVar2) {
    iVar1 = 0;
  }
  if (bVar2 && iVar1 == 4) {
    iVar1 = debug_read_core_status(&stack0xfffffff8);
    bVar2 = iVar1 == 0x80;
    if (bVar2) {
      iVar1 = 0;
    }
    if (bVar2 && iVar1 == 4) {
      return CONCAT44(unaff_r7 & 0xffffff00,1);
    }
  }
  return (ulonglong)(unaff_r7 & 0xffffff00) << 0x20;
}

/**
 * stlink_set_mode
 *   Map a requested mode (0/1/2/0x13) to the current-mode byte DAT_2000152B (0/5/6/7) reported by GET_CURRENT_MODE; returns 0x80, or 6 on unknown.
 *   address 0x08009360, 42 bytes
 *   called from: APP_main
 *   prototype: undefined4 stlink_set_mode(int mode)
 */
/* Map a requested mode (0/1/2/0x13) to the current-mode byte DAT_2000152B (0/5/6/7) reported by
   GET_CURRENT_MODE; returns 0x80, or 6 on unknown. */

undefined4 stlink_set_mode(int param_1)

{
  undefined4 uVar1;
  
  uVar1 = 0x80;
  stlink_current_mode = 0;
  if (param_1 != 0) {
    if (param_1 == 1) {
      stlink_current_mode = 5;
    }
    else if (param_1 == 2) {
      stlink_current_mode = 6;
    }
    else if (param_1 == 0x13) {
      stlink_current_mode = 7;
    }
    else {
      stlink_current_mode = 6;
      uVar1 = 6;
    }
  }
  return uVar1;
}

/**
 * stlink_dispatch_command
 *   Main ST-Link command dispatcher: parse 16-byte EP2-OUT command blocks for all F1/F2/F3/F4 subcommands (version, debug read/write mem/reg, run/halt/step, SWIM/JTAG/SWD ops, trace), drive the target and stage EP1-IN replies and bulk transfers.
 *   address 0x08009700, 5162 bytes
 *   prototype: uint stlink_dispatch_command(int ep)
 */
/* WARNING: Restarted to delay deadcode elimination for space: ram */
/* Main ST-Link command dispatcher: parse 16-byte EP2-OUT command blocks for all F1/F2/F3/F4
   subcommands (version, debug read/write mem/reg, run/halt/step, SWIM/JTAG/SWD ops, trace), drive
   the target and stage EP1-IN replies and bulk transfers. */

uint stlink_dispatch_command(int param_1)

{
  undefined1 *puVar1;
  char cVar2;
  byte bVar3;
  undefined1 uVar4;
  ushort uVar5;
  undefined4 uVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
  undefined4 *puVar11;
  ushort *puVar12;
  uint *puVar13;
  undefined1 *extraout_r1;
  undefined1 *puVar14;
  uint uVar15;
  uint extraout_r2;
  undefined4 uVar16;
  undefined *puVar17;
  int *piVar18;
  char *pcVar19;
  uint unaff_r8;
  code *pcVar20;
  bool bVar21;
  undefined8 uVar22;
  undefined4 local_38;
  code *local_34;
  undefined2 local_30;
  byte local_2e;
  char local_2d;
  ushort local_2c;
  undefined2 local_2a;
  uint local_28 [4];
  
  if (param_1 == 0) {
    iVar8 = GetEPRxCount(usb_ep0_idx);
    stlink_cmd_pending = (undefined2)iVar8;
    if (0x40 < iVar8) {
      stlink_cmd_pending = 0x40;
    }
    PMAToUserBufferCopy(&stlink_ep2out_buf,0xe0,stlink_cmd_pending);
  }
  if ((&stlink_cmd_reply_state)[param_1] == '\0') {
    (&DAT_200030ad)[param_1] = 1;
    return 1;
  }
  if ((&stlink_cmd_reply_state)[param_1] != '\x01') {
    stlink_cmd_reply_unknown();
    (&stlink_cmd_reply_state)[param_1] = 0;
    if (param_1 != 0) {
      return 0;
    }
    uVar9 = (uint)usb_ep0_idx;
    (&USB_EP0R)[uVar9] = (&USB_EP0R)[uVar9] & 0xbf8f ^ 0x3000 | 0x8080;
    return uVar9;
  }
  uVar9 = (uint)(byte)(&stlink_cmd_buf)[param_1 * 0x10];
  if (uVar9 != 0xf2) {
    if (uVar9 != 0xf4) {
      return uVar9;
    }
    if (g_trace_active != 0) {
      return (uint)g_trace_active;
    }
    iVar8 = param_1 * 0x10;
    bVar3 = (&stlink_cmd_subcmd)[iVar8];
    uVar9 = (uint)bVar3;
    if ((&UNK_0800cc5c)[uVar9 + (uint)stlink_active_subcmd * 0xe] == '\x01')
    goto switchD_0800c9fe_default;
    if (g_connect_phase == 1) {
      if (uVar9 == 0) {
        stlink_active_subcmd = bVar3;
        swim_init();
        led_set_mode(2,0,0);
        g_connect_phase = 3;
        goto LAB_0800cc12;
      }
joined_r0x0800c9b4:
      if (uVar9 == 1) goto LAB_0800cc12;
      goto switchD_0800c9fe_default;
    }
    if (g_connect_phase == 0) {
      return 0x1800;
    }
    if (g_connect_phase != 3) {
      if (2 < g_connect_phase) {
        return 0x1800;
      }
      if (uVar9 == 0) {
        target_disconnect(param_1);
        stlink_active_subcmd = (&stlink_cmd_subcmd)[iVar8];
        swim_init();
        led_set_mode(2,0,0);
        g_connect_phase = 3;
        goto LAB_0800cc12;
      }
      goto joined_r0x0800c9b4;
    }
    switch(uVar9) {
    case 0:
      stlink_active_subcmd = bVar3;
      led_set_mode(2,0,0);
      break;
    case 1:
      stlink_active_subcmd = bVar3;
      swim_deinit();
      led_set_mode(1,0,0);
      g_connect_phase = 1;
      break;
    case 2:
      stlink_active_subcmd = bVar3;
      swim_init_counters((&g_cmd_block)[iVar8]);
      uVar16 = 1;
      uVar6 = 8;
      puVar14 = &DAT_20003080;
      goto LAB_0800cbd6;
    case 3:
      stlink_active_subcmd = bVar3;
      swim_build_timing_tables((&g_cmd_block)[iVar8] != '\0');
      __disable_irq();
      stlink_active_subcmd = 0;
      swim_sched_mode = 0;
      DAT_2000308d = 0;
      led_set_result(1);
      __enable_irq();
      break;
    case 4:
      stlink_active_subcmd = bVar3;
      swim_command_dispatch(0);
      break;
    case 5:
      stlink_active_subcmd = bVar3;
      swim_command_dispatch(1);
      break;
    case 6:
      stlink_active_subcmd = bVar3;
      swim_command_dispatch(2);
      break;
    case 7:
      stlink_active_subcmd = bVar3;
      swim_command_dispatch(3);
      break;
    case 8:
      stlink_active_subcmd = bVar3;
      GPIO_SetBits(&GPIOB_CRL,0x40);
      __disable_irq();
      stlink_active_subcmd = 0;
      swim_sched_mode = 0;
      DAT_2000308d = 0;
      led_set_result(1);
      __enable_irq();
      break;
    case 9:
      stlink_cmd_send_reply(param_1,&swim_sched_mode,4,1);
      goto LAB_0800cc06;
    case 10:
      iVar10 = stlink_cmd_is_first_block(param_1);
      if (iVar10 != 1) {
        if (0x40 < stlink_bulk_remaining) {
          __aeabi_memcpy(&g_reply_buf + stlink_bulk_offset,&stlink_ep2out_buf + param_1 * 0x40,0x40)
          ;
          stlink_bulk_offset = stlink_bulk_offset + 0x40;
          stlink_bulk_remaining = stlink_bulk_remaining - 0x40;
LAB_0800cb68:
          stlink_cmd_set_status_busy(param_1);
          return 0x1800;
        }
        __aeabi_memcpy(&g_reply_buf + stlink_bulk_offset,&stlink_ep2out_buf + param_1 * 0x40);
        stlink_active_subcmd = (&stlink_cmd_subcmd)[iVar8];
        swim_read_setup(stlink_bulk_total,stlink_bulk_addr,&g_reply_buf);
        break;
      }
      stlink_bulk_remaining = CONCAT11((&g_cmd_block)[iVar8],(&DAT_2000309f)[iVar8]);
      stlink_bulk_addr =
           (uint)(byte)(&DAT_200030a2)[iVar8] << 8 | (uint)(byte)(&DAT_200030a1)[iVar8] << 0x10 |
           (uint)(byte)(&DAT_200030a3)[iVar8];
      DAT_20003096 = 0;
      stlink_bulk_total = stlink_bulk_remaining;
      if (stlink_bulk_remaining < 0x1801) {
        if (8 < stlink_bulk_remaining) {
          __aeabi_memcpy(&g_reply_buf,&DAT_200030a4 + iVar8,8);
          stlink_bulk_offset = 8;
          stlink_bulk_remaining = stlink_bulk_remaining - 8;
          goto LAB_0800cb68;
        }
        __aeabi_memcpy(&g_reply_buf,&DAT_200030a4 + iVar8);
        stlink_active_subcmd = (&stlink_cmd_subcmd)[iVar8];
        swim_read_setup(stlink_bulk_total,stlink_bulk_addr,&g_reply_buf);
        break;
      }
      __disable_irq();
      stlink_active_subcmd = 0;
      swim_sched_mode = 8;
      DAT_2000308d = 0;
      led_set_result();
      __enable_irq();
    default:
switchD_0800c9fe_default:
      stlink_cmd_reply_unknown(param_1);
      break;
    case 0xb:
      stlink_bulk_remaining = CONCAT11((&g_cmd_block)[iVar8],(&DAT_2000309f)[iVar8]);
      stlink_bulk_addr =
           (uint)(byte)(&DAT_200030a2)[iVar8] << 8 | (uint)(byte)(&DAT_200030a1)[iVar8] << 0x10 |
           (uint)(byte)(&DAT_200030a3)[iVar8];
      DAT_20003096 = 0;
      stlink_bulk_offset = 0;
      stlink_active_subcmd = (&stlink_cmd_subcmd)[iVar8];
      stlink_bulk_total = stlink_bulk_remaining;
      swim_write_setup(stlink_bulk_remaining,stlink_bulk_addr,&g_reply_buf);
      break;
    case 0xc:
      if (stlink_bulk_total < 0x41) {
        stlink_cmd_send_reply(param_1,&g_reply_buf + stlink_bulk_offset,stlink_bulk_total,1);
        goto LAB_0800cc06;
      }
      stlink_bulk_offset = stlink_bulk_offset + 0x40;
      stlink_bulk_total = stlink_bulk_total - 0x40;
      uVar16 = 0;
      uVar6 = 0x40;
      puVar14 = (undefined1 *)((int)&stlink_info_buf + (uint)stlink_bulk_offset);
LAB_0800cbd6:
      stlink_cmd_send_reply(param_1,puVar14,uVar6,uVar16);
      return 0x1800;
    case 0xd:
      stlink_cmd_send_reply(param_1,&stack0xffffffe8,2,1);
LAB_0800cc06:
      stlink_active_subcmd = (&stlink_cmd_subcmd)[iVar8];
      return 0x1800;
    }
LAB_0800cc12:
    stlink_cmd_set_status_idle(param_1);
    return 0x1800;
  }
  puVar11 = &local_38;
  local_28[0] = 0;
  local_2e = 0;
  iVar8 = 0;
  uVar9 = (uint)g_connect_phase;
  if (uVar9 == 1) {
    cVar2 = (&stlink_cmd_subcmd)[param_1 * 0x10];
    if (cVar2 == ' ' || cVar2 == '0') goto switchD_08006240_caseD_20;
    if (cVar2 != '<') {
      if (cVar2 != '?') {
        if (cVar2 != 'C') {
          if (cVar2 != 'D') {
            if (cVar2 != 'I') {
              if (cVar2 != 'J') {
                if (cVar2 != 'S') {
                  if (cVar2 != 'U') {
                    if (cVar2 != 'V') {
                      if (cVar2 != 'W') {
                        if (cVar2 != 'X') goto joined_r0x080061b6;
                        goto switchD_08006240_caseD_58;
                      }
                      goto switchD_08006240_caseD_57;
                    }
                    goto switchD_08006240_caseD_56;
                  }
                  goto switchD_08006240_caseD_55;
                }
                goto switchD_08006240_caseD_53;
              }
              goto switchD_08006240_caseD_4a;
            }
            goto switchD_08006240_caseD_49;
          }
          goto switchD_08006240_caseD_44;
        }
        goto LAB_0800621a;
      }
      goto switchD_08006240_caseD_3f;
    }
switchD_08006240_caseD_3c:
    local_30 = target_drive_nrst((&g_cmd_block)[param_1 * 0x10]);
    goto LAB_08006988;
  }
  if (uVar9 == 0) {
    return 0;
  }
  if (uVar9 == 3) {
    cVar2 = (&stlink_cmd_subcmd)[param_1 * 0x10];
    if (cVar2 == ' ' || cVar2 == '0') goto switchD_08006240_caseD_20;
    if (cVar2 == '?') goto switchD_08006240_caseD_3f;
    if (cVar2 == 'C') goto LAB_0800621a;
    if (cVar2 == 'D') goto switchD_08006240_caseD_44;
    if (cVar2 == 'I') goto switchD_08006240_caseD_49;
    if (cVar2 == 'J') goto switchD_08006240_caseD_4a;
    if (cVar2 == 'S') goto switchD_08006240_caseD_53;
    if (cVar2 == 'V') goto switchD_08006240_caseD_56;
    if (cVar2 == 'W') goto switchD_08006240_caseD_57;
    if (cVar2 == 'X') goto switchD_08006240_caseD_58;
joined_r0x080061b6:
    if (cVar2 == '\\') goto switchD_08006240_caseD_5c;
    goto switchD_08006240_default;
  }
  if (2 < uVar9) {
    return uVar9;
  }
  target_scan_rom_table();
  iVar10 = param_1 * 0x10;
  switch((&stlink_cmd_subcmd)[iVar10]) {
  case 1:
    if (g_trace_active != 0) goto LAB_08007596;
    cVar2 = '\0';
    if (g_target_connected != '\0') {
      cVar2 = g_conn_lost;
    }
    if (g_target_connected == '\0' || cVar2 == '\x01') {
      cortexm_check_halted(&local_2d);
      if (local_2d == '\0') {
        local_30 = 0x80;
      }
      else {
        local_30 = 0x81;
      }
      goto LAB_08007596;
    }
LAB_0800750c:
    local_38 = CONCAT22(local_38._2_2_,5);
    goto LAB_080075a2;
  case 2:
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected == '\0' || cVar2 == '\x01') {
        cortexm_halt();
      }
    }
    goto LAB_08006958;
  case 3:
  case 0x32:
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_0800750c;
      local_30 = target_connect(param_1,1,0,0xffffffff);
      if ((&stlink_cmd_subcmd)[iVar10] == '\x03') {
        cortexm_reset_and_halt(&local_2d);
        if (local_2d == '\0') {
          local_30 = 0x81;
        }
        else {
          local_30 = 0x80;
        }
      }
    }
    goto LAB_08007596;
  case 4:
  case 0x3a:
  case 0x3d:
    local_2c = 4;
    if ((&stlink_cmd_subcmd)[iVar10] == '\x04') {
      local_2c = 0;
    }
    iVar8 = stlink_cmd_is_first_block(param_1);
    if (iVar8 == 1) {
      g_requested_ap = (uint)(byte)(&g_cmd_block)[iVar10];
      if ((&stlink_cmd_subcmd)[iVar10] == '=') {
        stlink_mem_remaining = 0x88;
      }
      else if ((&stlink_cmd_subcmd)[iVar10] == ':') {
        stlink_mem_remaining = 0x58;
      }
      else {
        stlink_mem_remaining = 0x54;
      }
      if (g_trace_active == 0) {
        cVar2 = '\0';
        if (g_target_connected != '\0') {
          cVar2 = g_conn_lost;
        }
        if (g_target_connected == '\0' || cVar2 == '\x01') {
          if ((8 < g_requested_ap) || ((&g_ap_slot_table)[g_requested_ap * 0xc] == '\0')) {
            uVar9 = g_requested_ap;
            if (g_requested_ap == 0) {
              uVar9 = (uint)g_rom_detected;
            }
            if (g_requested_ap != 0 || uVar9 != 1) {
              local_30 = 0x1d;
              goto LAB_0800654a;
            }
          }
          local_30 = 0x80;
          local_30 = cortexm_read_all_regs();
        }
      }
LAB_0800654a:
      puVar13 = (uint *)&g_reply_buf;
      if ((&stlink_cmd_subcmd)[iVar10] != '\x04') {
        led_set_result(local_30 == 0x80);
        g_reply_buf = (undefined1)local_30;
        DAT_20001531 = local_30._1_1_;
      }
      __aeabi_memcpy(&g_reply_buf + local_2c,&DAT_200010a0,0x40 - (uint)local_2c);
      if (stlink_mem_remaining < 0x41) goto LAB_08006e74;
      stlink_mem_remaining = stlink_mem_remaining - 0x40;
      uVar16 = 0;
      uVar6 = 0x40;
      stlink_mem_offset = 0x40;
    }
    else {
      if (stlink_mem_remaining < 0x41) {
        iVar8 = 0;
        puVar17 = &DAT_200010a0 + -(uint)local_2c + (uint)stlink_mem_offset;
        uVar5 = stlink_mem_remaining;
        goto LAB_080065c8;
      }
      stlink_mem_offset = stlink_mem_offset + 0x40;
      stlink_mem_remaining = stlink_mem_remaining - 0x40;
      uVar16 = 0;
      uVar6 = 0x40;
      puVar13 = (uint *)(-(uint)local_2c + (uint)stlink_mem_offset + 0x20001060);
    }
    goto LAB_08006314;
  case 5:
  case 0x33:
    g_requested_ap = (uint)(byte)(&DAT_2000309f)[iVar10];
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_08006666;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_08006650:
        local_30 = 0x80;
        local_30 = cortexm_mem_ap_access_retry((&g_cmd_block)[iVar10],&g_last_rw_value);
        goto LAB_08006666;
      }
      uVar9 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar9 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar9 == 1) goto LAB_08006650;
      local_30 = 0x1d;
LAB_08006610:
      uVar6 = 0;
    }
    else {
LAB_08006666:
      if (local_30 != 0x80) goto LAB_08006610;
      uVar6 = 1;
    }
    led_set_result(uVar6);
    if ((&stlink_cmd_subcmd)[iVar10] == '\x05') {
      local_38 = CONCAT22(local_38._2_2_,5);
      if (g_trace_active == 0) {
        cVar2 = '\0';
        if (g_target_connected != '\0') {
          cVar2 = g_conn_lost;
        }
        if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_08006632;
      }
      else {
LAB_08006632:
        g_last_rw_value = 5;
      }
      uVar16 = 1;
      uVar6 = 4;
      puVar13 = &g_last_rw_value;
    }
    else {
      g_reply_buf = (undefined1)local_30;
      DAT_20001531 = local_30._1_1_;
      __aeabi_memcpy(&DAT_20001534,&g_last_rw_value,4);
      local_38 = CONCAT22(local_38._2_2_,5);
      if (g_trace_active == 0) {
        cVar2 = '\0';
        if (g_target_connected != '\0') {
          cVar2 = g_conn_lost;
        }
        if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_080066a2;
      }
      else {
LAB_080066a2:
        __aeabi_memclr(&g_reply_buf,8);
        g_reply_buf = (undefined1)local_38;
        DAT_20001531 = local_38._1_1_;
      }
      uVar16 = 1;
      uVar6 = 8;
      puVar13 = (uint *)&g_reply_buf;
    }
    goto LAB_08006314;
  case 6:
  case 0x34:
    g_requested_ap = (uint)(byte)(&DAT_200030a3)[iVar10];
    g_last_rw_value = *(uint *)(&DAT_2000309f + iVar10);
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_0800674a;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_08006736:
        local_30 = 0x80;
        local_30 = cortexm_write_reg_verify(g_last_rw_value,(&g_cmd_block)[iVar10]);
        goto LAB_0800674a;
      }
      uVar9 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar9 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar9 == 1) goto LAB_08006736;
      local_30 = 0x1d;
LAB_08006716:
      uVar6 = 0;
    }
    else {
LAB_0800674a:
      if (local_30 != 0x80) goto LAB_08006716;
      uVar6 = 1;
    }
    led_set_result(uVar6);
    if ((local_30 != 0x80) && ((&stlink_cmd_subcmd)[iVar10] == '\x06')) {
      local_30 = 0x81;
    }
    goto LAB_08007596;
  case 7:
  case 0x47:
  case 0x54:
    g_requested_ap = (uint)(byte)(&DAT_200030a4)[iVar10];
    uVar9 = 4;
    local_34 = mem_read_32;
    if ((&stlink_cmd_subcmd)[iVar10] == 'G') {
      local_34 = (code *)0x80052a1;
      uVar9 = 2;
    }
    if ((&stlink_cmd_subcmd)[iVar10] == 'T') {
      local_34 = (code *)0x80051df;
    }
    iVar8 = stlink_cmd_is_first_block(param_1);
    if (iVar8 == 1) {
      stlink_tgt_addr = *(uint *)(&g_cmd_block + iVar10);
      stlink_mem_remaining = *(ushort *)(&DAT_200030a2 + iVar10);
      if (0x1800 < stlink_mem_remaining) {
        stlink_mem_remaining = 0x1800;
      }
      stlink_mem_offset = 0;
      stlink_ap_access =
           (uint)(byte)(&DAT_200030a6)[iVar10] << 0x10 | (uint)(byte)(&DAT_200030a5)[iVar10] << 8 |
           (uint)(byte)(&DAT_200030a7)[iVar10] << 0x18;
      local_2c = stlink_mem_remaining;
      iVar8 = apkey_table_lookup(iVar10 + 0x200030a8,&local_38);
      local_30 = (ushort)iVar8;
      if (iVar8 == 0x80) {
        uVar15 = local_38 & 0xff;
        piVar18 = &stlink_ap_status + uVar15 * 3;
        uVar7 = g_requested_ap & 0xff;
        if ((8 < uVar7) || ((&g_ap_slot_table)[uVar7 * 0xc] == '\0')) {
          bVar21 = uVar7 == 0;
          if (bVar21) {
            uVar7 = (uint)g_rom_detected;
          }
          if (!bVar21 || uVar7 != 1) {
            local_30 = 0x1d;
            *piVar18 = 0x1d;
            iVar8 = 0x1d;
            uVar9 = stlink_tgt_addr;
            goto LAB_08006d8e;
          }
        }
        local_30 = 0x80;
        *piVar18 = 0x80;
        iVar8 = 0x80;
        local_2a = (undefined2)(0x800 / uVar9);
        while (0x800 < local_2c) {
          if (g_trace_active == 0) {
            cVar2 = '\0';
            if (g_target_connected != '\0') {
              cVar2 = g_conn_lost;
            }
            if (g_target_connected == '\0' || cVar2 == '\x01') {
              local_38 = stlink_ap_access;
              iVar8 = (*local_34)(stlink_tgt_addr,&g_reply_buf + stlink_mem_offset,local_2a,local_28
                                 );
            }
          }
          led_set_result(iVar8 == 0x80);
          stlink_mem_offset = stlink_mem_offset + 0x800;
          local_2c = local_2c - 0x800;
          if (local_34 != (code *)0x80051df) {
            stlink_tgt_addr = stlink_tgt_addr + 0x800;
          }
          if (*piVar18 == 0x80) {
            *piVar18 = iVar8;
            if (iVar8 != 0x80) {
              (&DAT_2000113c)[uVar15 * 3] = local_28[0];
            }
          }
        }
        if (g_trace_active == 0) {
          cVar2 = '\0';
          if (g_target_connected != '\0') {
            cVar2 = g_conn_lost;
          }
          if (g_target_connected == '\0' || cVar2 == '\x01') {
            local_38 = stlink_ap_access;
            iVar8 = (*local_34)(stlink_tgt_addr,&g_reply_buf + stlink_mem_offset,local_2c / uVar9,
                                local_28);
          }
        }
        if ((*piVar18 == 0x80) && (*piVar18 = iVar8, uVar9 = local_28[0], iVar8 != 0x80))
        goto LAB_08006d8e;
      }
      else {
        piVar18 = &stlink_last_rw_status;
        stlink_last_rw_status = 0x30;
        iVar8 = 0x30;
        uVar9 = stlink_tgt_addr;
LAB_08006d8e:
        piVar18[1] = uVar9;
      }
      led_set_result(iVar8 == 0x80);
      if (stlink_mem_remaining < 0x41) goto LAB_08006e74;
      stlink_mem_offset = 0x40;
      stlink_mem_remaining = stlink_mem_remaining - 0x40;
      uVar16 = 0;
      uVar6 = 0x40;
      puVar13 = (uint *)&g_reply_buf;
    }
    else {
      if (stlink_mem_remaining < 0x41) {
        iVar8 = 0;
        puVar17 = &g_reply_buf + stlink_mem_offset;
        uVar5 = stlink_mem_remaining;
        goto LAB_080065c8;
      }
      stlink_mem_offset = stlink_mem_offset + 0x40;
      stlink_mem_remaining = stlink_mem_remaining - 0x40;
      uVar16 = 0;
      uVar6 = 0x40;
      puVar13 = (uint *)((int)&stlink_info_buf + (uint)stlink_mem_offset);
    }
    goto LAB_08006314;
  case 8:
  case 0x48:
  case 0x50:
    pcVar20 = mem_write_32;
    uVar9 = 4;
    if ((&stlink_cmd_subcmd)[iVar10] == 'H') {
      pcVar20 = (code *)0x8005505;
      uVar9 = 2;
    }
    if ((&stlink_cmd_subcmd)[iVar10] == 'P') {
      pcVar20 = (code *)0x8005475;
    }
    iVar8 = apkey_table_lookup(iVar10 + 0x200030a8,&local_38);
    if (iVar8 == 0x80) {
      puVar13 = &stlink_ap_status + (local_38 & 0xff) * 3;
    }
    else {
      puVar13 = &stlink_last_rw_status;
    }
    local_30 = (ushort)iVar8;
    iVar8 = stlink_cmd_is_first_block(param_1);
    if (iVar8 == 1) {
LAB_080070a4:
      g_requested_ap = (uint)(byte)(&DAT_200030a4)[iVar10];
      stlink_tgt_addr = *(uint *)(&g_cmd_block + iVar10);
      stlink_mem_remaining = *(ushort *)(&DAT_200030a2 + iVar10);
      stlink_ap_access =
           (uint)(byte)(&DAT_200030a6)[iVar10] << 0x10 | (uint)(byte)(&DAT_200030a5)[iVar10] << 8 |
           (uint)(byte)(&DAT_200030a7)[iVar10] << 0x18;
      if (local_30 == 0x80) {
        if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_08007126:
          local_30 = 0x80;
        }
        else {
          uVar9 = g_requested_ap;
          if (g_requested_ap == 0) {
            uVar9 = (uint)g_rom_detected;
          }
          if (g_requested_ap == 0 && uVar9 == 1) goto LAB_08007126;
          local_30 = 0x1d;
          puVar13[1] = stlink_tgt_addr;
        }
        *puVar13 = (uint)local_30;
      }
      else {
        *puVar13 = 0x30;
        puVar13[1] = stlink_tgt_addr;
      }
    }
    else {
      uVar7 = *puVar13;
      if (stlink_mem_remaining < 0x41) {
        if (uVar7 == 0x30 || uVar7 == 0x1d) {
          unaff_r8 = uVar7 & 0xffff;
        }
        else if (g_trace_active == 0) {
          cVar2 = '\0';
          if (g_target_connected != '\0') {
            cVar2 = g_conn_lost;
          }
          if (g_target_connected == '\0' || cVar2 == '\x01') {
            unaff_r8 = (*pcVar20)(stlink_tgt_addr,&stlink_ep2out_buf + param_1 * 0x40,
                                  stlink_mem_remaining / uVar9,stlink_ap_access);
          }
        }
        led_set_result(unaff_r8 == 0x80);
        if ((*puVar13 == 0x80) && (*puVar13 = unaff_r8, unaff_r8 != 0x80)) {
          puVar13[1] = stlink_tgt_addr;
        }
        goto LAB_080061c0;
      }
      if (uVar7 == 0x30 || uVar7 == 0x1d) {
        unaff_r8 = uVar7 & 0xffff;
      }
      else if (g_trace_active == 0) {
        cVar2 = '\0';
        if (g_target_connected != '\0') {
          cVar2 = g_conn_lost;
        }
        if (g_target_connected == '\0' || cVar2 == '\x01') {
          unaff_r8 = (*pcVar20)(stlink_tgt_addr,&stlink_ep2out_buf + param_1 * 0x40,0x40 / uVar9,
                                stlink_ap_access);
        }
      }
      led_set_result(unaff_r8 == 0x80);
      if (*puVar13 == 0x80) {
        *puVar13 = unaff_r8;
        if (unaff_r8 != 0x80) {
          puVar13[1] = stlink_tgt_addr;
        }
      }
      if (pcVar20 != (code *)0x8005475) {
        stlink_tgt_addr = stlink_tgt_addr + 0x40;
      }
      stlink_mem_remaining = stlink_mem_remaining - 0x40;
    }
    uVar9 = stlink_cmd_set_status_busy(param_1);
    break;
  case 9:
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected == '\0' || cVar2 == '\x01') {
        cortexm_write_dhcsr_word();
      }
    }
LAB_08006958:
    local_30 = 0x80;
    local_38 = CONCAT22(local_38._2_2_,5);
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_08006978;
    }
    else {
LAB_08006978:
      local_30 = 5;
    }
    goto LAB_08006988;
  case 10:
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_0800750c;
      local_38 = 0;
      mem_read_32(&DAT_e000edf0,&g_last_rw_value,1,0);
      g_last_rw_value = g_last_rw_value | 0xa05f0008;
      local_38 = g_last_rw_value;
      mem_write_32(&DAT_e000edf0,&local_38,1,0);
      cortexm_step(&local_2d);
      local_38 = 0;
      if (local_2d == '\0') {
        local_30 = 0x81;
      }
      else {
        local_30 = 0x80;
      }
      mem_read_32(&DAT_e000edf0,&g_last_rw_value,1,0);
      g_last_rw_value = g_last_rw_value & 0xfffffff3 | 0xa05f0000;
      local_38 = g_last_rw_value;
      mem_write_32(&DAT_e000edf0,&local_38,1,0);
    }
    goto LAB_08007596;
  case 0xb:
  case 0x38:
    g_requested_ap = (uint)(byte)(&DAT_200030a4)[iVar10];
    g_last_rw_value = *(uint *)(&DAT_2000309f + iVar10);
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_08006b26;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_08006b0e:
        local_30 = 0x80;
        local_30 = fpb_set_breakpoint(g_last_rw_value,(&g_cmd_block)[iVar10],(&DAT_200030a3)[iVar10]
                                     );
        goto LAB_08006b26;
      }
      uVar9 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar9 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar9 == 1) goto LAB_08006b0e;
      local_30 = 0x1d;
LAB_08006af8:
      uVar6 = 0;
    }
    else {
LAB_08006b26:
      if (local_30 != 0x80) goto LAB_08006af8;
      uVar6 = 1;
    }
    led_set_result(uVar6);
    if ((&stlink_cmd_subcmd)[iVar10] == '\v') {
      local_30 = 0x80;
    }
    goto LAB_08007596;
  case 0xc:
    g_requested_ap = (uint)(byte)(&DAT_200030a4)[iVar10];
    stlink_tgt_addr = *(uint *)(&g_cmd_block + iVar10);
    stlink_mem_remaining = *(ushort *)(&DAT_200030a2 + iVar10);
    if (0x40 < stlink_mem_remaining) {
      stlink_mem_remaining = 0x40;
    }
    stlink_ap_access =
         (uint)(byte)(&DAT_200030a6)[iVar10] << 0x10 | (uint)(byte)(&DAT_200030a5)[iVar10] << 8 |
         (uint)(byte)(&DAT_200030a7)[iVar10] << 0x18;
    iVar8 = apkey_table_lookup(iVar10 + 0x200030a8,&local_38);
    local_30 = (ushort)iVar8;
    if (iVar8 == 0x80) {
      uVar7 = local_38 & 0xff;
      puVar13 = &stlink_ap_status + uVar7 * 3;
      uVar9 = g_requested_ap & 0xff;
      if ((8 < uVar9) || ((&g_ap_slot_table)[uVar9 * 0xc] == '\0')) {
        bVar21 = uVar9 == 0;
        if (bVar21) {
          uVar9 = (uint)g_rom_detected;
        }
        if (!bVar21 || uVar9 != 1) {
          uVar9 = 0x1d;
          local_30 = 0x1d;
          goto LAB_08006e5e;
        }
      }
      local_30 = 0x80;
      *puVar13 = 0x80;
      for (uVar9 = 0; (uVar9 & 0xff) != (uint)stlink_mem_remaining; uVar9 = uVar9 + 1) {
        if (g_trace_active == 0) {
          cVar2 = '\0';
          if (g_target_connected != '\0') {
            cVar2 = g_conn_lost;
          }
          if (g_target_connected == '\0' || cVar2 == '\x01') {
            uVar9 = uVar9 & 0xff;
            unaff_r8 = mem_read_8(stlink_tgt_addr,&g_reply_buf + uVar9,stlink_ap_access);
          }
        }
        led_set_result(unaff_r8 == 0x80);
        if (*puVar13 == 0x80) {
          *puVar13 = unaff_r8;
          if (unaff_r8 != 0x80) {
            (&DAT_2000113c)[uVar7 * 3] = stlink_tgt_addr;
          }
        }
        stlink_tgt_addr = stlink_tgt_addr + 1;
      }
    }
    else {
      puVar13 = &stlink_last_rw_status;
      uVar9 = 0x30;
LAB_08006e5e:
      *puVar13 = uVar9;
      led_set_result(0);
      puVar13[1] = stlink_tgt_addr;
    }
    if (stlink_mem_remaining == 1) {
      stlink_mem_remaining = 2;
    }
LAB_08006e74:
    puVar17 = &g_reply_buf;
    iVar8 = 0;
    uVar5 = stlink_mem_remaining;
    goto LAB_080065c8;
  case 0xd:
    iVar8 = apkey_table_lookup(iVar10 + 0x200030a8,&local_38);
    if (iVar8 == 0x80) {
      puVar13 = &stlink_ap_status + (local_38 & 0xff) * 3;
    }
    else {
      puVar13 = &stlink_last_rw_status;
    }
    local_30 = (ushort)iVar8;
    iVar8 = stlink_cmd_is_first_block(param_1);
    if (iVar8 == 1) goto LAB_080070a4;
    if (*puVar13 == 0x30 || *puVar13 == 0x1d) {
      led_set_result(0);
    }
    else {
      for (uVar9 = 0; (uVar9 & 0xff) < (uint)stlink_mem_remaining; uVar9 = uVar9 + 1) {
        if (g_trace_active == 0) {
          cVar2 = '\0';
          if (g_target_connected != '\0') {
            cVar2 = g_conn_lost;
          }
          if (g_target_connected == '\0' || cVar2 == '\x01') {
            uVar9 = uVar9 & 0xff;
            unaff_r8 = mem_write_8(stlink_tgt_addr,(&stlink_ep2out_buf)[uVar9 + param_1 * 0x40],
                                   stlink_ap_access);
          }
        }
        led_set_result(unaff_r8 == 0x80);
        if (*puVar13 == 0x80) {
          *puVar13 = unaff_r8;
          if (unaff_r8 != 0x80) {
            puVar13[1] = stlink_tgt_addr;
          }
        }
        stlink_tgt_addr = stlink_tgt_addr + 1;
      }
    }
    goto LAB_080061c0;
  case 0xe:
  case 0x39:
    g_requested_ap = (uint)(byte)(&DAT_2000309f)[iVar10];
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_08006ba6;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_08006b84:
        local_30 = 0x80;
        local_38 = 0;
        local_30 = mem_write_32(*(undefined4 *)
                                 (&DAT_08007610 + (uint)(byte)(&g_cmd_block)[iVar10] * 4),&local_38,
                                1,0);
        goto LAB_08006ba6;
      }
      uVar9 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar9 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar9 == 1) goto LAB_08006b84;
      local_30 = 0x1d;
LAB_08006b6e:
      uVar6 = 0;
    }
    else {
LAB_08006ba6:
      if (local_30 != 0x80) goto LAB_08006b6e;
      uVar6 = 1;
    }
    led_set_result(uVar6);
    if ((&stlink_cmd_subcmd)[iVar10] == '\x0e') {
      local_30 = 0x80;
    }
    goto LAB_08007596;
  case 0xf:
  case 0x35:
    g_requested_ap = (uint)(byte)(&DAT_200030a6)[iVar10];
    g_last_rw_value = *(uint *)(&g_cmd_block + iVar10);
    stlink_reg_tmp2 = *(uint *)(&DAT_200030a2 + iVar10);
    if (((&DAT_200030a7)[iVar10] == 0) || (8 < g_requested_ap)) {
      stlink_ap_access = 0;
    }
    else {
      stlink_ap_access =
           (&g_ap_tar_cache)[g_requested_ap] & 0xffff00 |
           (uint)(byte)(&DAT_200030a7)[iVar10] << 0x18;
    }
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_08006812;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_0800683c:
        local_30 = 0x80;
        local_38 = stlink_reg_tmp2;
        local_30 = mem_write_32(g_last_rw_value,&local_38,1,stlink_ap_access);
        goto LAB_08006830;
      }
      uVar9 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar9 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar9 == 1) goto LAB_0800683c;
      local_30 = 0x1d;
LAB_080067ee:
      uVar6 = 0;
    }
    else {
LAB_08006812:
      if (g_last_rw_value == 0xe0044100) {
        local_38 = stlink_reg_tmp2;
        local_30 = mem_write_32(0xe0044100,&local_38,1,stlink_ap_access);
        iVar8 = 1;
      }
LAB_08006830:
      if (local_30 != 0x80) goto LAB_080067ee;
      uVar6 = 1;
    }
    led_set_result(uVar6);
    if ((&stlink_cmd_subcmd)[iVar10] == '\x06') {
      local_30 = 0x80;
    }
    puVar11 = (undefined4 *)&local_30;
    goto LAB_08006808;
  case 0x10:
  case 0x37:
    g_last_rw_value = *(uint *)(&DAT_2000309f + iVar10);
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected == '\0' || cVar2 == '\x01') {
        local_34 = (code *)(uint)(byte)(&DAT_200030a7)[iVar10];
        local_38 = (uint)(byte)(&DAT_200030a4)[iVar10];
        local_30 = dwt_set_watchpoint((&DAT_200030a3)[iVar10],g_last_rw_value,
                                      (&DAT_200030a5)[iVar10],(&DAT_200030a6)[iVar10]);
      }
    }
    led_set_result(local_30 == 0x80);
    if ((&stlink_cmd_subcmd)[iVar10] == '\x10') {
      local_30 = 0x80;
    }
    goto LAB_08007596;
  case 0x11:
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
  case 0x23:
  case 0x24:
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2c:
  case 0x2d:
  case 0x2e:
  case 0x2f:
  case 0x4d:
  case 0x4e:
  case 0x4f:
  case 0x59:
  default:
switchD_08006240_default:
    stlink_cmd_reply_unknown(param_1);
LAB_080061c0:
    uVar9 = stlink_cmd_set_status_idle(param_1);
    break;
  case 0x20:
  case 0x30:
switchD_08006240_caseD_20:
    uVar9 = stlink_cmd_debug_connect(param_1,g_connect_phase);
    return uVar9;
  case 0x21:
    iVar10 = apkey_table_lookup(iVar10 + 0x200030a8,&local_2e);
    local_30 = (ushort)iVar10;
    if (iVar10 == 0x80) {
      if (g_apkey_count < 2) {
        local_30 = apkey_table_remove(local_2e);
        pcVar19 = &g_ap_slot_table;
        do {
          if (*pcVar19 != '\0') {
            local_30 = cortexm_run_core(iVar8);
            *pcVar19 = '\0';
            pcVar19[1] = '\0';
            __aeabi_memset(pcVar19 + 2,10,0xff);
          }
          iVar8 = iVar8 + 1;
          pcVar19 = pcVar19 + 0xc;
        } while (iVar8 < 9);
        uVar5 = local_30;
        if (local_30 == 0x80) {
          uVar5 = (ushort)(byte)(&g_ap_state)[param_1];
        }
        if (local_30 == 0x80 && uVar5 == 3) {
          swd_write_idle_word();
          iVar8 = dap_ack_to_status();
          local_30 = (ushort)iVar8;
          if (iVar8 == 0x80) {
            g_reset_vector = 0xffffffff;
          }
        }
        GPIO_WriteBit(&GPIOB_CRL,1,1);
        target_disconnect(param_1);
        (&g_ap_state)[param_1] = 0;
        g_debug_mode_bitbang = 0;
        led_set_mode(1,0,0);
        g_connect_phase = 1;
      }
      else {
        local_30 = apkey_table_remove(local_2e);
      }
    }
    goto LAB_080061c0;
  case 0x22:
    puVar13 = (uint *)&stlink_coreid;
    dap_get_last_rw_status(&stlink_coreid,&stlink_dpid);
    uVar16 = 1;
    uVar6 = 4;
    goto LAB_08006314;
  case 0x31:
    iVar8 = dap_get_last_rw_status(&stlink_coreid,&stlink_dpid);
    local_30 = (ushort)iVar8;
    led_set_result(iVar8 == 0x80);
    g_reply_buf = (undefined1)local_30;
    DAT_20001531 = local_30._1_1_;
    __aeabi_memcpy(&DAT_20001534,&stlink_coreid,4);
    __aeabi_memcpy(&DAT_20001538,&stlink_dpid,4);
    uVar16 = 1;
    uVar6 = 0xc;
    puVar13 = (uint *)&g_reply_buf;
    goto LAB_08006314;
  case 0x36:
    g_requested_ap = (uint)(byte)(&DAT_200030a2)[iVar10];
    g_last_rw_value = *(uint *)(&g_cmd_block + iVar10);
    stlink_ap_access =
         (uint)(byte)(&DAT_200030a4)[iVar10] << 0x10 | (uint)(byte)(&DAT_200030a3)[iVar10] << 8 |
         (uint)(byte)(&DAT_200030a5)[iVar10] << 0x18;
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_080068f0;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_0800691c:
        local_30 = 0x80;
        local_38 = stlink_ap_access;
        local_30 = mem_read_32(g_last_rw_value,&stlink_reg_tmp2,1,0);
        goto LAB_08006910;
      }
      uVar9 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar9 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar9 == 1) goto LAB_0800691c;
      local_30 = 0x1d;
LAB_080068c8:
      uVar6 = 0;
    }
    else {
LAB_080068f0:
      if (g_last_rw_value == 0xe0044104) {
        local_38 = stlink_ap_access;
        local_30 = mem_read_32(0xe0044104,&stlink_reg_tmp2,1,0);
        iVar8 = 1;
      }
LAB_08006910:
      if (local_30 != 0x80) goto LAB_080068c8;
      uVar6 = 1;
    }
    led_set_result(uVar6);
    g_reply_buf = (undefined1)local_30;
    DAT_20001531 = local_30._1_1_;
    __aeabi_memcpy(&DAT_20001534,&stlink_reg_tmp2,4);
    uVar5 = 8;
    puVar17 = &g_reply_buf;
    goto LAB_080065c8;
  case 0x3b:
    iVar8 = apkey_table_lookup(iVar10 + 0x200030a8,&local_38);
    local_30 = (ushort)iVar8;
    if (iVar8 == 0x80) {
      puVar13 = &stlink_ap_status + (local_38 & 0xff) * 3;
      local_30 = 0x80;
    }
    else {
      puVar13 = &stlink_last_rw_status;
      stlink_last_rw_status = 0x30;
    }
    local_38 = CONCAT22(local_38._2_2_,5);
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_080071f8;
    }
    else {
LAB_080071f8:
      *(undefined1 *)puVar13 = 0;
      *(undefined1 *)((int)puVar13 + 1) = 0;
      *(undefined1 *)puVar13 = 5;
      *(undefined1 *)((int)puVar13 + 1) = 0;
    }
    uVar16 = 1;
    uVar6 = 2;
    goto LAB_08006314;
  case 0x3c:
    goto switchD_08006240_caseD_3c;
  case 0x3e:
    iVar8 = apkey_table_lookup(iVar10 + 0x200030a8,&local_38);
    local_30 = (ushort)iVar8;
    if (iVar8 == 0x80) {
      puVar13 = &stlink_ap_status + (local_38 & 0xff) * 3;
      local_30 = 0x80;
    }
    else {
      puVar13 = &stlink_last_rw_status;
      stlink_last_rw_status = 0x30;
    }
    local_38 = CONCAT22(local_38._2_2_,5);
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_0800725a;
    }
    else {
LAB_0800725a:
      __aeabi_memclr(puVar13,0xc);
      *(undefined1 *)puVar13 = (undefined1)local_38;
      *(undefined1 *)((int)puVar13 + 1) = local_38._1_1_;
    }
    uVar16 = 1;
    uVar6 = 0xc;
LAB_08006314:
    uVar9 = stlink_cmd_send_reply(param_1,puVar13,uVar6,uVar16);
    break;
  case 0x3f:
switchD_08006240_caseD_3f:
    uVar9 = stlink_cmd_get_conn_status(param_1);
    break;
  case 0x40:
    if (g_debug_mode_bitbang != 1) goto LAB_08007386;
    local_2c = *(ushort *)(&g_cmd_block + iVar10);
    g_last_rw_value = *(uint *)(&DAT_200030a0 + iVar10);
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_0800750c;
      usart_bridge_set_baud(g_last_rw_value,local_2c);
      local_30 = dap_ack_to_status();
    }
    goto LAB_08007596;
  case 0x41:
    bVar3 = g_debug_mode_bitbang;
    if (g_debug_mode_bitbang == 1) {
      bVar3 = g_trace_active;
    }
    if (g_debug_mode_bitbang == 1 && bVar3 == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected == '\0' || cVar2 == '\x01') {
        usart_bridge_close();
        local_30 = dap_ack_to_status();
        goto LAB_08007596;
      }
    }
LAB_08007386:
    local_30 = 0x20;
    goto LAB_08007596;
  case 0x42:
    bVar3 = g_debug_mode_bitbang;
    if (g_debug_mode_bitbang == 1) {
      bVar3 = g_trace_active;
    }
    if (g_debug_mode_bitbang != 1 || bVar3 != 0) {
LAB_080073ae:
      local_2c = 0;
    }
    else {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_080073ae;
      local_2c = usart_bridge_poll_rx();
    }
    local_38 = CONCAT22(local_38._2_2_,5);
    if (g_trace_active == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_080073ca;
    }
    else {
LAB_080073ca:
      local_2c = 5;
    }
    puVar12 = &local_2c;
    goto LAB_0800698e;
  case 0x43:
LAB_0800621a:
    uVar6 = 1;
    goto LAB_0800621e;
  case 0x44:
switchD_08006240_caseD_44:
    uVar6 = 0;
LAB_0800621e:
    uVar9 = stlink_cmd_set_freq(uVar6,param_1);
    break;
  case 0x45:
    uVar6 = 1;
    goto LAB_080073f6;
  case 0x46:
    uVar6 = 0;
LAB_080073f6:
    uVar9 = stlink_cmd_rw_dap_reg(param_1,uVar6);
    break;
  case 0x49:
switchD_08006240_caseD_49:
    uVar9 = stlink_cmd_set_led_ack(param_1);
    break;
  case 0x4a:
switchD_08006240_caseD_4a:
    uVar9 = stlink_cmd_get_fw_build(param_1);
    break;
  case 0x4b:
    iVar8 = apkey_table_lookup(iVar10 + 0x200030a8,&local_2e);
    local_30 = (ushort)iVar8;
    if (iVar8 == 0x80) {
      if (g_trace_active == 0) {
        cVar2 = '\0';
        if (g_target_connected != '\0') {
          cVar2 = g_conn_lost;
        }
        if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_08007456;
      }
      else {
LAB_08007456:
        if (g_target_connected != '\x01') goto LAB_08007596;
      }
      uVar9 = (uint)(byte)(&g_cmd_block)[iVar10];
      uVar4 = (&DAT_2000309f)[iVar10];
      if ((8 < uVar9) || (puVar14 = &g_ap_slot_table, (&g_ap_slot_table)[uVar9 * 0xc] == '\0')) {
        uVar22 = target_select_ap(uVar9);
        puVar14 = (undefined1 *)((ulonglong)uVar22 >> 0x20);
        local_30 = (ushort)uVar22;
        if ((int)uVar22 == 0x80) {
          target_scan_rom_table();
          puVar14 = extraout_r1;
        }
      }
      uVar5 = local_30;
      if (local_30 == 0x80) {
        uVar5 = (ushort)g_trace_active;
      }
      if (local_30 == 0x80 && uVar5 == 0) {
        cVar2 = '\0';
        if (g_target_connected != '\0') {
          cVar2 = g_conn_lost;
        }
        if (g_target_connected == '\0' || cVar2 == '\x01') {
          local_30 = 0x1d;
          bVar21 = SBORROW4(uVar9,9);
          puVar1 = (undefined1 *)(uVar9 - 9);
          if (uVar9 < 9) {
            puVar14 = (undefined1 *)(uint)local_2e;
            bVar21 = SBORROW4((int)puVar14,10);
            puVar1 = puVar14 + -10;
          }
          if ((int)puVar1 < 0 != bVar21) {
            iVar8 = uVar9 * 0xc;
            if ((&g_ap_slot_table)[iVar8] == '\0') {
              (&DAT_20001359)[iVar8] = uVar4;
            }
            if ((&g_ap_slot_table)[(int)(puVar14 + iVar8 + 2)] == -1) {
              (&g_ap_slot_table)[iVar8] = (&g_ap_slot_table)[iVar8] + '\x01';
              (&g_ap_slot_table)[(int)(puVar14 + iVar8 + 2)] = local_2e;
            }
            local_30 = 0x80;
          }
        }
        goto LAB_0800750c;
      }
    }
    goto LAB_08007596;
  case 0x4c:
    uVar9 = apkey_table_lookup(iVar10 + 0x200030a8,&local_2e);
    local_30 = (ushort)uVar9;
    bVar21 = uVar9 == 0x80;
    if (bVar21) {
      uVar9 = (uint)g_trace_active;
    }
    if (bVar21 && uVar9 == 0) {
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_0800750c;
      uVar7 = (uint)(byte)(&g_cmd_block)[iVar10];
      uVar9 = (uint)local_2e;
      bVar21 = SBORROW4(uVar7,9);
      iVar8 = uVar7 - 9;
      if (uVar7 < 9) {
        bVar21 = SBORROW4(uVar9,10);
        iVar8 = uVar9 - 10;
      }
      if (iVar8 < 0 != bVar21) {
        uVar15 = uVar7 * 0xc;
        uVar9 = (uint)(byte)(&DAT_2000135a)[uVar9 + uVar15];
        if (uVar9 != 0xff) {
          uVar15 = (uint)(byte)(&g_ap_slot_table)[uVar15];
        }
        if (((uVar9 != 0xff && uVar15 != 0) && (uVar7 < 9)) &&
           (uVar9 = uVar7 * 3, (&g_ap_slot_table)[uVar7 * 0xc] == '\x01')) {
          local_30 = cortexm_run_core(uVar7);
          uVar9 = extraout_r2;
        }
      }
      if (local_30 == 0x80) {
        local_30 = 0x1d;
        bVar21 = SBORROW4(uVar7,9);
        iVar8 = uVar7 - 9;
        if (uVar7 < 9) {
          uVar9 = (uint)local_2e;
          bVar21 = SBORROW4(uVar9,10);
          iVar8 = uVar9 - 10;
        }
        if (iVar8 < 0 != bVar21) {
          iVar8 = uVar7 * 0xc;
          if ((&g_ap_slot_table)[uVar9 + iVar8 + 2] == -1) {
LAB_0800758a:
            if ((&g_ap_slot_table)[iVar8] == '\0') goto LAB_0800758e;
          }
          else {
            if ((&g_ap_slot_table)[iVar8] != '\0') {
              (&g_ap_slot_table)[iVar8] = (&g_ap_slot_table)[iVar8] + -1;
              (&g_ap_slot_table)[uVar9 + iVar8 + 2] = 0xff;
              goto LAB_0800758a;
            }
LAB_0800758e:
            (&DAT_20001359)[iVar8] = 0;
          }
          local_30 = 0x80;
        }
      }
    }
LAB_08007596:
    local_38 = CONCAT22(local_38._2_2_,5);
    if (g_trace_active == 0) {
LAB_080075a2:
      cVar2 = '\0';
      if (g_target_connected != '\0') {
        cVar2 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar2 != '\x01') goto LAB_080075b0;
    }
    else {
LAB_080075b0:
      local_30 = (ushort)local_38;
    }
LAB_08006988:
    puVar12 = &local_30;
LAB_0800698e:
    uVar9 = stlink_cmd_send_reply(param_1,puVar12,2,1);
    break;
  case 0x51:
    uVar9 = usb_bulk_debug_access(param_1);
    break;
  case 0x52:
    iVar10 = stlink_cmd_is_first_block(param_1);
    if (iVar10 == 1) {
      stlink_debug_get_result_ptr(&stlink_mem_remaining,&DAT_2000109c);
      stlink_mem_offset = 0;
    }
    puVar17 = DAT_2000109c + stlink_mem_offset;
    uVar5 = stlink_mem_remaining;
    if (0x40 < stlink_mem_remaining) {
      stlink_cmd_send_reply(param_1,puVar17,0x40,0);
      stlink_mem_offset = stlink_mem_offset + 0x40;
      uVar9 = (uint)stlink_mem_remaining;
      stlink_mem_remaining = (ushort)(uVar9 - 0x40);
      return uVar9 - 0x40;
    }
    goto LAB_080065c8;
  case 0x53:
switchD_08006240_caseD_53:
    local_38 = CONCAT22(local_38._2_2_,0x40);
    iVar8 = 1;
LAB_08006808:
    uVar9 = stlink_send_reply(param_1,puVar11,2,iVar8);
    break;
  case 0x55:
switchD_08006240_caseD_55:
    uVar9 = stlink_cmd_jtag_raw(param_1);
    break;
  case 0x56:
switchD_08006240_caseD_56:
    uVar9 = stlink_cmd_get_version(param_1);
    break;
  case 0x57:
switchD_08006240_caseD_57:
    uVar9 = stlink_cmd_get_param_block(param_1);
    break;
  case 0x58:
switchD_08006240_caseD_58:
    uVar9 = stlink_cmd_get_caps(param_1);
    break;
  case 0x5a:
    uVar9 = stlink_cmd_read_trace_buf(param_1);
    break;
  case 0x5b:
    puVar17 = &g_reply_buf;
    DAT_2000109c = &g_reply_buf;
    local_2c = (short)g_trace_count * 2 + 4;
    iVar8 = 1;
    uVar5 = local_2c;
LAB_080065c8:
    uVar9 = stlink_send_reply(param_1,puVar17,uVar5,iVar8);
    break;
  case 0x5c:
switchD_08006240_caseD_5c:
    uVar9 = stlink_cmd_get_rw_status(param_1);
  }
  return uVar9;
}

/**
 * stlink_cmd_process
 *   Main ST-Link command dispatcher: handles top-level F1-F8/FA/FD commands and, for F2(DEBUG)/F4(SWIM), switches on the subcommand byte to run RW_MEM/RW_REG/RW_DEBUGREG/READALLREGS/enter-mode/freq/trace handlers; reads 16-byte command from per-channel buffer, drives reply. param_1 = command slot (0 = USB EP2 bulk path).
 *   address 0x08009772, 1440 bytes
 *   called from: APP_main
 *   prototype: uint stlink_cmd_process(int chan, undefined4 p2, undefined4 p3, undefined4 p4)
 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Restarted to delay deadcode elimination for space: ram */
/* Main ST-Link command dispatcher: handles top-level F1-F8/FA/FD commands and, for
   F2(DEBUG)/F4(SWIM), switches on the subcommand byte to run
   RW_MEM/RW_REG/RW_DEBUGREG/READALLREGS/enter-mode/freq/trace handlers; reads 16-byte command from
   per-channel buffer, drives reply. param_1 = command slot (0 = USB EP2 bulk path). */

uint stlink_cmd_process(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined1 *puVar1;
  byte bVar2;
  char cVar3;
  undefined1 uVar4;
  ushort uVar5;
  uint uVar6;
  code *UNRECOVERED_JUMPTABLE;
  undefined4 uVar7;
  uint uVar8;
  int iVar9;
  undefined4 *puVar10;
  ushort *puVar11;
  uint *puVar12;
  undefined1 *extraout_r1;
  undefined1 *puVar13;
  uint uVar14;
  uint extraout_r2;
  undefined2 uVar16;
  undefined2 extraout_var;
  undefined4 uVar15;
  undefined *puVar17;
  int *piVar18;
  int iVar19;
  char *pcVar20;
  uint unaff_r8;
  bool bVar21;
  undefined8 uVar22;
  undefined4 uStack_38;
  code *pcStack_34;
  undefined2 uStack_30;
  byte bStack_2e;
  char cStack_2d;
  ushort uStack_2c;
  undefined2 uStack_2a;
  uint auStack_28 [2];
  undefined4 local_18;
  
  uVar16 = (undefined2)((uint)param_4 >> 0x10);
  iVar19 = param_1 * 0x10;
  if ((&stlink_cmd_pending)[param_1] != 0) {
    __aeabi_memcpy(&stlink_cmd_buf + iVar19,&stlink_ep2out_buf + param_1 * 0x40);
    uVar16 = extraout_var;
  }
  cVar3 = (&stlink_cmd_buf)[iVar19];
  if (cVar3 == -0xf) {
    if (param_1 == 0) {
      UserToPMABufferCopy(&stack0xfffffff0,0xa0,6);
      stlink_cmd_reply_state = 3;
      SetEPTxCount(1,6);
      SetEPTxStatus(1,0x30);
      return 0xc72b;
    }
    (&stlink_cmd_reply_state)[param_1] = 3;
    return 0xc72b;
  }
  if (cVar3 != -0xe) {
    if (cVar3 == -0xd) {
      if ((&stlink_cmd_subcmd)[iVar19] != '\a') {
        stlink_cmd_reply_unknown(param_1);
      }
LAB_08009838:
      (&stlink_cmd_reply_state)[param_1] = 0;
      if (param_1 != 0) {
        return 0;
      }
      uVar8 = (uint)usb_ep0_idx;
      (&USB_EP0R)[uVar8] = (&USB_EP0R)[uVar8] & 0xbf8f ^ 0x3000 | 0x8080;
      return uVar8;
    }
    if (cVar3 != -0xc) {
      if (cVar3 == -0xb) {
        uVar8 = (uint)g_connect_phase;
        stlink_cmd_send_reply(param_1,&stack0xfffffff8,2,1);
        return uVar8;
      }
      if (cVar3 == -9) {
        local_18 = adc_voltage_sample;
        ADC_SoftwareStartConvCmd(&DAT_40012400,1);
        do {
          iVar19 = ADC_GetFlagStatus(&DAT_40012400,2);
        } while (iVar19 == 0);
        ADC_GetConversionValue(&DAT_40012400);
        uVar8 = stlink_cmd_send_reply(param_1,&local_18,8,1);
        return uVar8;
      }
      if (cVar3 == -8) {
        iVar19 = RCC_APB2ENR;
        if (-1 < iVar19 << 0x1b) {
          uVar8 = RCC_APB2ENR;
          RCC_APB2ENR = uVar8 | 0x10;
        }
        local_18 = 0x48036000;
        GPIO_Init(&GPIOC_CRL,&local_18);
        uVar7 = GPIOC_IDR;
        uVar8 = RCC_APB2ENR;
        RCC_APB2ENR = uVar8 & 0xffffffef;
        stlink_cmd_send_reply(param_1,&stack0xffffffec,4,1);
        return local_18;
      }
      if (cVar3 == -7) {
        uVar8 = CONCAT22(uVar16,0x80);
        if ((&stlink_cmd_subcmd)[iVar19] == '\0') {
          if (_DAT_08000100 == 0x47a53c15) {
            vt_region_base_ptr = (undefined4 *)0x8000000;
            vt_app_base_ptr = (undefined4 *)&DAT_08000100;
          }
          else {
            vt_region_base_ptr = &DAT_0801b000;
            vt_app_base_ptr = &DAT_0801b100;
            uVar8 = CONCAT22(uVar16,0x21);
          }
          stlink_cmd_send_reply(param_1,&stack0xfffffff0,2,1);
          return uVar8;
        }
        if (_DAT_08000100 == 0x47a53c15) {
          vt_region_base_ptr = (undefined4 *)0x8000000;
          vt_app_base_ptr = (undefined4 *)&DAT_08000100;
          USB_CNTR = 1;
          USB_ISTR = 0;
          iVar19 = _uncertain_board_detect_variant();
          if (iVar19 == 0) {
            swd_jtag_pins_init();
            _uncertain_stlink_mode_leave(1);
          }
          led_set_mode(0,0,0);
          set_msp_if_privileged(*vt_region_base_ptr);
          UNRECOVERED_JUMPTABLE = (code *)nop_function(vt_app_base_ptr[2]);
                    /* WARNING: Could not recover jumptable at 0x0800931a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          uVar8 = (*UNRECOVERED_JUMPTABLE)();
          return uVar8;
        }
        vt_region_base_ptr = &DAT_0801b000;
        vt_app_base_ptr = &DAT_0801b100;
        return uVar8;
      }
      if (cVar3 == -6) {
        bVar2 = (&stlink_cmd_subcmd)[iVar19];
        uVar8 = 0x80;
        if (bVar2 == 0) {
          uVar7 = 0;
        }
        else if (bVar2 == 2) {
          uVar7 = 6;
        }
        else if (bVar2 < 2) {
          uVar7 = 5;
        }
        else {
          if (bVar2 != 3) {
            uVar8 = 6;
            goto LAB_08009352;
          }
          uVar7 = 7;
        }
        RCC_MCOConfig(uVar7);
LAB_08009352:
        stlink_cmd_send_reply(param_1,&stack0xfffffff0,2,1);
        return uVar8;
      }
      if (cVar3 != -3) {
        stlink_cmd_reply_unknown(param_1);
        goto LAB_08009838;
      }
      uVar8 = 0x80;
      if (g_connect_phase == 1) {
        if ((&stlink_cmd_subcmd)[iVar19] == '\0') {
          _uncertain_stlink_mode_leave(0);
          goto LAB_080096c2;
        }
        if ((&stlink_cmd_subcmd)[iVar19] == '\x01') {
          _uncertain_stlink_mode_enter(0);
          goto LAB_080096c2;
        }
      }
      uVar8 = 6;
LAB_080096c2:
      stlink_cmd_send_reply(param_1,&stack0xfffffff0,2,1);
      return uVar8;
    }
    if (g_trace_active != 0) {
      return (uint)g_trace_active;
    }
    local_18 = CONCAT22(uVar16,0x1800);
    iVar19 = param_1 * 0x10;
    bVar2 = (&stlink_cmd_subcmd)[iVar19];
    uVar8 = (uint)bVar2;
    if ((&UNK_0800cc5c)[uVar8 + (uint)stlink_active_subcmd * 0xe] == '\x01')
    goto switchD_0800c9fe_default;
    if (g_connect_phase == 1) {
      if (uVar8 == 0) {
        stlink_active_subcmd = bVar2;
        swim_init();
        led_set_mode(2,0,0);
        g_connect_phase = 3;
        goto LAB_0800cc12;
      }
joined_r0x0800c9b4:
      if (uVar8 == 1) goto LAB_0800cc12;
      goto switchD_0800c9fe_default;
    }
    if (g_connect_phase == 0) {
      return local_18;
    }
    if (g_connect_phase != 3) {
      if (2 < g_connect_phase) {
        return local_18;
      }
      if (uVar8 == 0) {
        target_disconnect(param_1);
        stlink_active_subcmd = (&stlink_cmd_subcmd)[iVar19];
        swim_init();
        led_set_mode(2,0,0);
        g_connect_phase = 3;
        goto LAB_0800cc12;
      }
      goto joined_r0x0800c9b4;
    }
    switch(uVar8) {
    case 0:
      stlink_active_subcmd = bVar2;
      led_set_mode(2,0,0);
      break;
    case 1:
      stlink_active_subcmd = bVar2;
      swim_deinit();
      led_set_mode(1,0,0);
      g_connect_phase = 1;
      break;
    case 2:
      stlink_active_subcmd = bVar2;
      swim_init_counters((&g_cmd_block)[iVar19]);
      uVar15 = 1;
      uVar7 = 8;
      puVar13 = &DAT_20003080;
      goto LAB_0800cbd6;
    case 3:
      stlink_active_subcmd = bVar2;
      swim_build_timing_tables((&g_cmd_block)[iVar19] != '\0');
      __disable_irq();
      stlink_active_subcmd = 0;
      swim_sched_mode = 0;
      DAT_2000308d = 0;
      led_set_result(1);
      __enable_irq();
      break;
    case 4:
      stlink_active_subcmd = bVar2;
      swim_command_dispatch(0);
      break;
    case 5:
      stlink_active_subcmd = bVar2;
      swim_command_dispatch(1);
      break;
    case 6:
      stlink_active_subcmd = bVar2;
      swim_command_dispatch(2);
      break;
    case 7:
      stlink_active_subcmd = bVar2;
      swim_command_dispatch(3);
      break;
    case 8:
      stlink_active_subcmd = bVar2;
      GPIO_SetBits(&GPIOB_CRL,0x40);
      __disable_irq();
      stlink_active_subcmd = 0;
      swim_sched_mode = 0;
      DAT_2000308d = 0;
      led_set_result(1);
      __enable_irq();
      break;
    case 9:
      stlink_cmd_send_reply(param_1,&swim_sched_mode,4,1);
      goto LAB_0800cc06;
    case 10:
      iVar9 = stlink_cmd_is_first_block(param_1);
      if (iVar9 != 1) {
        if (0x40 < stlink_bulk_remaining) {
          __aeabi_memcpy(&g_reply_buf + stlink_bulk_offset,&stlink_ep2out_buf + param_1 * 0x40,0x40)
          ;
          stlink_bulk_offset = stlink_bulk_offset + 0x40;
          stlink_bulk_remaining = stlink_bulk_remaining - 0x40;
LAB_0800cb68:
          stlink_cmd_set_status_busy(param_1);
          return local_18;
        }
        __aeabi_memcpy(&g_reply_buf + stlink_bulk_offset,&stlink_ep2out_buf + param_1 * 0x40);
        stlink_active_subcmd = (&stlink_cmd_subcmd)[iVar19];
        swim_read_setup(stlink_bulk_total,stlink_bulk_addr,&g_reply_buf);
        break;
      }
      stlink_bulk_remaining = CONCAT11((&g_cmd_block)[iVar19],(&DAT_2000309f)[iVar19]);
      stlink_bulk_addr =
           (uint)(byte)(&DAT_200030a2)[iVar19] << 8 | (uint)(byte)(&DAT_200030a1)[iVar19] << 0x10 |
           (uint)(byte)(&DAT_200030a3)[iVar19];
      DAT_20003096 = 0;
      stlink_bulk_total = stlink_bulk_remaining;
      if (stlink_bulk_remaining < 0x1801) {
        if (8 < stlink_bulk_remaining) {
          __aeabi_memcpy(&g_reply_buf,&DAT_200030a4 + iVar19,8);
          stlink_bulk_offset = 8;
          stlink_bulk_remaining = stlink_bulk_remaining - 8;
          goto LAB_0800cb68;
        }
        __aeabi_memcpy(&g_reply_buf,&DAT_200030a4 + iVar19);
        stlink_active_subcmd = (&stlink_cmd_subcmd)[iVar19];
        swim_read_setup(stlink_bulk_total,stlink_bulk_addr,&g_reply_buf);
        break;
      }
      __disable_irq();
      stlink_active_subcmd = 0;
      swim_sched_mode = 8;
      DAT_2000308d = 0;
      led_set_result();
      __enable_irq();
    default:
switchD_0800c9fe_default:
      stlink_cmd_reply_unknown(param_1);
      break;
    case 0xb:
      stlink_bulk_remaining = CONCAT11((&g_cmd_block)[iVar19],(&DAT_2000309f)[iVar19]);
      stlink_bulk_addr =
           (uint)(byte)(&DAT_200030a2)[iVar19] << 8 | (uint)(byte)(&DAT_200030a1)[iVar19] << 0x10 |
           (uint)(byte)(&DAT_200030a3)[iVar19];
      DAT_20003096 = 0;
      stlink_bulk_offset = 0;
      stlink_active_subcmd = (&stlink_cmd_subcmd)[iVar19];
      stlink_bulk_total = stlink_bulk_remaining;
      swim_write_setup(stlink_bulk_remaining,stlink_bulk_addr,&g_reply_buf);
      break;
    case 0xc:
      if (stlink_bulk_total < 0x41) {
        stlink_cmd_send_reply(param_1,&g_reply_buf + stlink_bulk_offset,stlink_bulk_total,1);
        goto LAB_0800cc06;
      }
      stlink_bulk_offset = stlink_bulk_offset + 0x40;
      stlink_bulk_total = stlink_bulk_total - 0x40;
      uVar15 = 0;
      uVar7 = 0x40;
      puVar13 = (undefined1 *)((int)&stlink_info_buf + (uint)stlink_bulk_offset);
LAB_0800cbd6:
      stlink_cmd_send_reply(param_1,puVar13,uVar7,uVar15);
      return local_18;
    case 0xd:
      stlink_cmd_send_reply(param_1,&local_18,2,1);
LAB_0800cc06:
      stlink_active_subcmd = (&stlink_cmd_subcmd)[iVar19];
      return local_18;
    }
LAB_0800cc12:
    stlink_cmd_set_status_idle(param_1);
    return local_18;
  }
  puVar10 = &uStack_38;
  auStack_28[0] = 0;
  bStack_2e = 0;
  iVar19 = 0;
  uVar8 = (uint)g_connect_phase;
  if (uVar8 == 1) {
    cVar3 = (&stlink_cmd_subcmd)[param_1 * 0x10];
    if (cVar3 == ' ' || cVar3 == '0') goto switchD_08006240_caseD_20;
    if (cVar3 != '<') {
      if (cVar3 != '?') {
        if (cVar3 != 'C') {
          if (cVar3 != 'D') {
            if (cVar3 != 'I') {
              if (cVar3 != 'J') {
                if (cVar3 != 'S') {
                  if (cVar3 != 'U') {
                    if (cVar3 != 'V') {
                      if (cVar3 != 'W') {
                        if (cVar3 != 'X') goto joined_r0x080061b6;
                        goto switchD_08006240_caseD_58;
                      }
                      goto switchD_08006240_caseD_57;
                    }
                    goto switchD_08006240_caseD_56;
                  }
                  goto switchD_08006240_caseD_55;
                }
                goto switchD_08006240_caseD_53;
              }
              goto switchD_08006240_caseD_4a;
            }
            goto switchD_08006240_caseD_49;
          }
          goto switchD_08006240_caseD_44;
        }
        goto LAB_0800621a;
      }
      goto switchD_08006240_caseD_3f;
    }
switchD_08006240_caseD_3c:
    uStack_30 = target_drive_nrst((&g_cmd_block)[param_1 * 0x10]);
    goto LAB_08006988;
  }
  if (uVar8 == 0) {
    return 0;
  }
  if (uVar8 == 3) {
    cVar3 = (&stlink_cmd_subcmd)[param_1 * 0x10];
    if (cVar3 == ' ' || cVar3 == '0') goto switchD_08006240_caseD_20;
    if (cVar3 == '?') goto switchD_08006240_caseD_3f;
    if (cVar3 == 'C') goto LAB_0800621a;
    if (cVar3 == 'D') goto switchD_08006240_caseD_44;
    if (cVar3 == 'I') goto switchD_08006240_caseD_49;
    if (cVar3 == 'J') goto switchD_08006240_caseD_4a;
    if (cVar3 == 'S') goto switchD_08006240_caseD_53;
    if (cVar3 == 'V') goto switchD_08006240_caseD_56;
    if (cVar3 == 'W') goto switchD_08006240_caseD_57;
    if (cVar3 == 'X') goto switchD_08006240_caseD_58;
joined_r0x080061b6:
    if (cVar3 == '\\') goto switchD_08006240_caseD_5c;
    goto switchD_08006240_default;
  }
  if (2 < uVar8) {
    return uVar8;
  }
  target_scan_rom_table();
  iVar9 = param_1 * 0x10;
  switch((&stlink_cmd_subcmd)[iVar9]) {
  case 1:
    if (g_trace_active != 0) goto LAB_08007596;
    cVar3 = '\0';
    if (g_target_connected != '\0') {
      cVar3 = g_conn_lost;
    }
    if (g_target_connected == '\0' || cVar3 == '\x01') {
      cortexm_check_halted(&cStack_2d);
      if (cStack_2d == '\0') {
        uStack_30 = 0x80;
      }
      else {
        uStack_30 = 0x81;
      }
      goto LAB_08007596;
    }
LAB_0800750c:
    uStack_38 = CONCAT22(uStack_38._2_2_,5);
    goto LAB_080075a2;
  case 2:
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected == '\0' || cVar3 == '\x01') {
        cortexm_halt();
      }
    }
    goto LAB_08006958;
  case 3:
  case 0x32:
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_0800750c;
      uStack_30 = target_connect(param_1,1,0,0xffffffff);
      if ((&stlink_cmd_subcmd)[iVar9] == '\x03') {
        cortexm_reset_and_halt(&cStack_2d);
        if (cStack_2d == '\0') {
          uStack_30 = 0x81;
        }
        else {
          uStack_30 = 0x80;
        }
      }
    }
    goto LAB_08007596;
  case 4:
  case 0x3a:
  case 0x3d:
    uStack_2c = 4;
    if ((&stlink_cmd_subcmd)[iVar9] == '\x04') {
      uStack_2c = 0;
    }
    iVar19 = stlink_cmd_is_first_block(param_1);
    if (iVar19 == 1) {
      g_requested_ap = (uint)(byte)(&g_cmd_block)[iVar9];
      if ((&stlink_cmd_subcmd)[iVar9] == '=') {
        stlink_mem_remaining = 0x88;
      }
      else if ((&stlink_cmd_subcmd)[iVar9] == ':') {
        stlink_mem_remaining = 0x58;
      }
      else {
        stlink_mem_remaining = 0x54;
      }
      if (g_trace_active == 0) {
        cVar3 = '\0';
        if (g_target_connected != '\0') {
          cVar3 = g_conn_lost;
        }
        if (g_target_connected == '\0' || cVar3 == '\x01') {
          if ((8 < g_requested_ap) || ((&g_ap_slot_table)[g_requested_ap * 0xc] == '\0')) {
            uVar8 = g_requested_ap;
            if (g_requested_ap == 0) {
              uVar8 = (uint)g_rom_detected;
            }
            if (g_requested_ap != 0 || uVar8 != 1) {
              uStack_30 = 0x1d;
              goto LAB_0800654a;
            }
          }
          uStack_30 = 0x80;
          uStack_30 = cortexm_read_all_regs();
        }
      }
LAB_0800654a:
      puVar12 = (uint *)&g_reply_buf;
      if ((&stlink_cmd_subcmd)[iVar9] != '\x04') {
        led_set_result(uStack_30 == 0x80);
        g_reply_buf = (undefined1)uStack_30;
        DAT_20001531 = uStack_30._1_1_;
      }
      __aeabi_memcpy(&g_reply_buf + uStack_2c,&DAT_200010a0,0x40 - (uint)uStack_2c);
      if (stlink_mem_remaining < 0x41) goto LAB_08006e74;
      stlink_mem_remaining = stlink_mem_remaining - 0x40;
      uVar15 = 0;
      uVar7 = 0x40;
      stlink_mem_offset = 0x40;
    }
    else {
      if (stlink_mem_remaining < 0x41) {
        iVar19 = 0;
        puVar17 = &DAT_200010a0 + -(uint)uStack_2c + (uint)stlink_mem_offset;
        uVar5 = stlink_mem_remaining;
        goto LAB_080065c8;
      }
      stlink_mem_offset = stlink_mem_offset + 0x40;
      stlink_mem_remaining = stlink_mem_remaining - 0x40;
      uVar15 = 0;
      uVar7 = 0x40;
      puVar12 = (uint *)(-(uint)uStack_2c + (uint)stlink_mem_offset + 0x20001060);
    }
    goto LAB_08006314;
  case 5:
  case 0x33:
    g_requested_ap = (uint)(byte)(&DAT_2000309f)[iVar9];
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_08006666;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_08006650:
        uStack_30 = 0x80;
        uStack_30 = cortexm_mem_ap_access_retry((&g_cmd_block)[iVar9],&g_last_rw_value);
        goto LAB_08006666;
      }
      uVar8 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar8 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar8 == 1) goto LAB_08006650;
      uStack_30 = 0x1d;
LAB_08006610:
      uVar7 = 0;
    }
    else {
LAB_08006666:
      if (uStack_30 != 0x80) goto LAB_08006610;
      uVar7 = 1;
    }
    led_set_result(uVar7);
    if ((&stlink_cmd_subcmd)[iVar9] == '\x05') {
      uStack_38 = CONCAT22(uStack_38._2_2_,5);
      if (g_trace_active == 0) {
        cVar3 = '\0';
        if (g_target_connected != '\0') {
          cVar3 = g_conn_lost;
        }
        if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_08006632;
      }
      else {
LAB_08006632:
        g_last_rw_value = 5;
      }
      uVar15 = 1;
      uVar7 = 4;
      puVar12 = &g_last_rw_value;
    }
    else {
      g_reply_buf = (undefined1)uStack_30;
      DAT_20001531 = uStack_30._1_1_;
      __aeabi_memcpy(&DAT_20001534,&g_last_rw_value,4);
      uStack_38 = CONCAT22(uStack_38._2_2_,5);
      if (g_trace_active == 0) {
        cVar3 = '\0';
        if (g_target_connected != '\0') {
          cVar3 = g_conn_lost;
        }
        if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_080066a2;
      }
      else {
LAB_080066a2:
        __aeabi_memclr(&g_reply_buf,8);
        g_reply_buf = (undefined1)uStack_38;
        DAT_20001531 = uStack_38._1_1_;
      }
      uVar15 = 1;
      uVar7 = 8;
      puVar12 = (uint *)&g_reply_buf;
    }
    goto LAB_08006314;
  case 6:
  case 0x34:
    g_requested_ap = (uint)(byte)(&DAT_200030a3)[iVar9];
    g_last_rw_value = *(uint *)(&DAT_2000309f + iVar9);
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_0800674a;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_08006736:
        uStack_30 = 0x80;
        uStack_30 = cortexm_write_reg_verify(g_last_rw_value,(&g_cmd_block)[iVar9]);
        goto LAB_0800674a;
      }
      uVar8 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar8 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar8 == 1) goto LAB_08006736;
      uStack_30 = 0x1d;
LAB_08006716:
      uVar7 = 0;
    }
    else {
LAB_0800674a:
      if (uStack_30 != 0x80) goto LAB_08006716;
      uVar7 = 1;
    }
    led_set_result(uVar7);
    if ((uStack_30 != 0x80) && ((&stlink_cmd_subcmd)[iVar9] == '\x06')) {
      uStack_30 = 0x81;
    }
    goto LAB_08007596;
  case 7:
  case 0x47:
  case 0x54:
    g_requested_ap = (uint)(byte)(&DAT_200030a4)[iVar9];
    uVar8 = 4;
    pcStack_34 = mem_read_32;
    if ((&stlink_cmd_subcmd)[iVar9] == 'G') {
      pcStack_34 = (code *)0x80052a1;
      uVar8 = 2;
    }
    if ((&stlink_cmd_subcmd)[iVar9] == 'T') {
      pcStack_34 = (code *)0x80051df;
    }
    iVar19 = stlink_cmd_is_first_block(param_1);
    if (iVar19 == 1) {
      stlink_tgt_addr = *(uint *)(&g_cmd_block + iVar9);
      stlink_mem_remaining = *(ushort *)(&DAT_200030a2 + iVar9);
      if (0x1800 < stlink_mem_remaining) {
        stlink_mem_remaining = 0x1800;
      }
      stlink_mem_offset = 0;
      stlink_ap_access =
           (uint)(byte)(&DAT_200030a6)[iVar9] << 0x10 | (uint)(byte)(&DAT_200030a5)[iVar9] << 8 |
           (uint)(byte)(&DAT_200030a7)[iVar9] << 0x18;
      uStack_2c = stlink_mem_remaining;
      iVar19 = apkey_table_lookup(iVar9 + 0x200030a8,&uStack_38);
      uStack_30 = (ushort)iVar19;
      if (iVar19 == 0x80) {
        uVar14 = uStack_38 & 0xff;
        piVar18 = &stlink_ap_status + uVar14 * 3;
        uVar6 = g_requested_ap & 0xff;
        if ((8 < uVar6) || ((&g_ap_slot_table)[uVar6 * 0xc] == '\0')) {
          bVar21 = uVar6 == 0;
          if (bVar21) {
            uVar6 = (uint)g_rom_detected;
          }
          if (!bVar21 || uVar6 != 1) {
            uStack_30 = 0x1d;
            *piVar18 = 0x1d;
            iVar19 = 0x1d;
            uVar8 = stlink_tgt_addr;
            goto LAB_08006d8e;
          }
        }
        uStack_30 = 0x80;
        *piVar18 = 0x80;
        iVar19 = 0x80;
        uStack_2a = (undefined2)(0x800 / uVar8);
        while (0x800 < uStack_2c) {
          if (g_trace_active == 0) {
            cVar3 = '\0';
            if (g_target_connected != '\0') {
              cVar3 = g_conn_lost;
            }
            if (g_target_connected == '\0' || cVar3 == '\x01') {
              uStack_38 = stlink_ap_access;
              iVar19 = (*pcStack_34)(stlink_tgt_addr,&g_reply_buf + stlink_mem_offset,uStack_2a,
                                     auStack_28);
            }
          }
          led_set_result(iVar19 == 0x80);
          stlink_mem_offset = stlink_mem_offset + 0x800;
          uStack_2c = uStack_2c - 0x800;
          if (pcStack_34 != (code *)0x80051df) {
            stlink_tgt_addr = stlink_tgt_addr + 0x800;
          }
          if (*piVar18 == 0x80) {
            *piVar18 = iVar19;
            if (iVar19 != 0x80) {
              (&DAT_2000113c)[uVar14 * 3] = auStack_28[0];
            }
          }
        }
        if (g_trace_active == 0) {
          cVar3 = '\0';
          if (g_target_connected != '\0') {
            cVar3 = g_conn_lost;
          }
          if (g_target_connected == '\0' || cVar3 == '\x01') {
            uStack_38 = stlink_ap_access;
            iVar19 = (*pcStack_34)(stlink_tgt_addr,&g_reply_buf + stlink_mem_offset,
                                   uStack_2c / uVar8,auStack_28);
          }
        }
        if ((*piVar18 == 0x80) && (*piVar18 = iVar19, uVar8 = auStack_28[0], iVar19 != 0x80))
        goto LAB_08006d8e;
      }
      else {
        piVar18 = &stlink_last_rw_status;
        stlink_last_rw_status = 0x30;
        iVar19 = 0x30;
        uVar8 = stlink_tgt_addr;
LAB_08006d8e:
        piVar18[1] = uVar8;
      }
      led_set_result(iVar19 == 0x80);
      if (stlink_mem_remaining < 0x41) goto LAB_08006e74;
      stlink_mem_offset = 0x40;
      stlink_mem_remaining = stlink_mem_remaining - 0x40;
      uVar15 = 0;
      uVar7 = 0x40;
      puVar12 = (uint *)&g_reply_buf;
    }
    else {
      if (stlink_mem_remaining < 0x41) {
        iVar19 = 0;
        puVar17 = &g_reply_buf + stlink_mem_offset;
        uVar5 = stlink_mem_remaining;
        goto LAB_080065c8;
      }
      stlink_mem_offset = stlink_mem_offset + 0x40;
      stlink_mem_remaining = stlink_mem_remaining - 0x40;
      uVar15 = 0;
      uVar7 = 0x40;
      puVar12 = (uint *)((int)&stlink_info_buf + (uint)stlink_mem_offset);
    }
    goto LAB_08006314;
  case 8:
  case 0x48:
  case 0x50:
    UNRECOVERED_JUMPTABLE = mem_write_32;
    uVar8 = 4;
    if ((&stlink_cmd_subcmd)[iVar9] == 'H') {
      UNRECOVERED_JUMPTABLE = (code *)0x8005505;
      uVar8 = 2;
    }
    if ((&stlink_cmd_subcmd)[iVar9] == 'P') {
      UNRECOVERED_JUMPTABLE = (code *)0x8005475;
    }
    iVar19 = apkey_table_lookup(iVar9 + 0x200030a8,&uStack_38);
    if (iVar19 == 0x80) {
      puVar12 = &stlink_ap_status + (uStack_38 & 0xff) * 3;
    }
    else {
      puVar12 = &stlink_last_rw_status;
    }
    uStack_30 = (ushort)iVar19;
    iVar19 = stlink_cmd_is_first_block(param_1);
    if (iVar19 == 1) {
LAB_080070a4:
      g_requested_ap = (uint)(byte)(&DAT_200030a4)[iVar9];
      stlink_tgt_addr = *(uint *)(&g_cmd_block + iVar9);
      stlink_mem_remaining = *(ushort *)(&DAT_200030a2 + iVar9);
      stlink_ap_access =
           (uint)(byte)(&DAT_200030a6)[iVar9] << 0x10 | (uint)(byte)(&DAT_200030a5)[iVar9] << 8 |
           (uint)(byte)(&DAT_200030a7)[iVar9] << 0x18;
      if (uStack_30 == 0x80) {
        if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_08007126:
          uStack_30 = 0x80;
        }
        else {
          uVar8 = g_requested_ap;
          if (g_requested_ap == 0) {
            uVar8 = (uint)g_rom_detected;
          }
          if (g_requested_ap == 0 && uVar8 == 1) goto LAB_08007126;
          uStack_30 = 0x1d;
          puVar12[1] = stlink_tgt_addr;
        }
        *puVar12 = (uint)uStack_30;
      }
      else {
        *puVar12 = 0x30;
        puVar12[1] = stlink_tgt_addr;
      }
    }
    else {
      uVar6 = *puVar12;
      if (stlink_mem_remaining < 0x41) {
        if (uVar6 == 0x30 || uVar6 == 0x1d) {
          unaff_r8 = uVar6 & 0xffff;
        }
        else if (g_trace_active == 0) {
          cVar3 = '\0';
          if (g_target_connected != '\0') {
            cVar3 = g_conn_lost;
          }
          if (g_target_connected == '\0' || cVar3 == '\x01') {
            unaff_r8 = (*UNRECOVERED_JUMPTABLE)
                                 (stlink_tgt_addr,&stlink_ep2out_buf + param_1 * 0x40,
                                  stlink_mem_remaining / uVar8,stlink_ap_access);
          }
        }
        led_set_result(unaff_r8 == 0x80);
        if ((*puVar12 == 0x80) && (*puVar12 = unaff_r8, unaff_r8 != 0x80)) {
          puVar12[1] = stlink_tgt_addr;
        }
        goto LAB_080061c0;
      }
      if (uVar6 == 0x30 || uVar6 == 0x1d) {
        unaff_r8 = uVar6 & 0xffff;
      }
      else if (g_trace_active == 0) {
        cVar3 = '\0';
        if (g_target_connected != '\0') {
          cVar3 = g_conn_lost;
        }
        if (g_target_connected == '\0' || cVar3 == '\x01') {
          unaff_r8 = (*UNRECOVERED_JUMPTABLE)
                               (stlink_tgt_addr,&stlink_ep2out_buf + param_1 * 0x40,0x40 / uVar8,
                                stlink_ap_access);
        }
      }
      led_set_result(unaff_r8 == 0x80);
      if (*puVar12 == 0x80) {
        *puVar12 = unaff_r8;
        if (unaff_r8 != 0x80) {
          puVar12[1] = stlink_tgt_addr;
        }
      }
      if (UNRECOVERED_JUMPTABLE != (code *)0x8005475) {
        stlink_tgt_addr = stlink_tgt_addr + 0x40;
      }
      stlink_mem_remaining = stlink_mem_remaining - 0x40;
    }
    uVar8 = stlink_cmd_set_status_busy(param_1);
    break;
  case 9:
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected == '\0' || cVar3 == '\x01') {
        cortexm_write_dhcsr_word();
      }
    }
LAB_08006958:
    uStack_30 = 0x80;
    uStack_38 = CONCAT22(uStack_38._2_2_,5);
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_08006978;
    }
    else {
LAB_08006978:
      uStack_30 = 5;
    }
    goto LAB_08006988;
  case 10:
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_0800750c;
      uStack_38 = 0;
      mem_read_32(&DAT_e000edf0,&g_last_rw_value,1,0);
      g_last_rw_value = g_last_rw_value | 0xa05f0008;
      uStack_38 = g_last_rw_value;
      mem_write_32(&DAT_e000edf0,&uStack_38,1,0);
      cortexm_step(&cStack_2d);
      uStack_38 = 0;
      if (cStack_2d == '\0') {
        uStack_30 = 0x81;
      }
      else {
        uStack_30 = 0x80;
      }
      mem_read_32(&DAT_e000edf0,&g_last_rw_value,1,0);
      g_last_rw_value = g_last_rw_value & 0xfffffff3 | 0xa05f0000;
      uStack_38 = g_last_rw_value;
      mem_write_32(&DAT_e000edf0,&uStack_38,1,0);
    }
    goto LAB_08007596;
  case 0xb:
  case 0x38:
    g_requested_ap = (uint)(byte)(&DAT_200030a4)[iVar9];
    g_last_rw_value = *(uint *)(&DAT_2000309f + iVar9);
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_08006b26;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_08006b0e:
        uStack_30 = 0x80;
        uStack_30 = fpb_set_breakpoint(g_last_rw_value,(&g_cmd_block)[iVar9],(&DAT_200030a3)[iVar9])
        ;
        goto LAB_08006b26;
      }
      uVar8 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar8 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar8 == 1) goto LAB_08006b0e;
      uStack_30 = 0x1d;
LAB_08006af8:
      uVar7 = 0;
    }
    else {
LAB_08006b26:
      if (uStack_30 != 0x80) goto LAB_08006af8;
      uVar7 = 1;
    }
    led_set_result(uVar7);
    if ((&stlink_cmd_subcmd)[iVar9] == '\v') {
      uStack_30 = 0x80;
    }
    goto LAB_08007596;
  case 0xc:
    g_requested_ap = (uint)(byte)(&DAT_200030a4)[iVar9];
    stlink_tgt_addr = *(uint *)(&g_cmd_block + iVar9);
    stlink_mem_remaining = *(ushort *)(&DAT_200030a2 + iVar9);
    if (0x40 < stlink_mem_remaining) {
      stlink_mem_remaining = 0x40;
    }
    stlink_ap_access =
         (uint)(byte)(&DAT_200030a6)[iVar9] << 0x10 | (uint)(byte)(&DAT_200030a5)[iVar9] << 8 |
         (uint)(byte)(&DAT_200030a7)[iVar9] << 0x18;
    iVar19 = apkey_table_lookup(iVar9 + 0x200030a8,&uStack_38);
    uStack_30 = (ushort)iVar19;
    if (iVar19 == 0x80) {
      uVar6 = uStack_38 & 0xff;
      puVar12 = &stlink_ap_status + uVar6 * 3;
      uVar8 = g_requested_ap & 0xff;
      if ((8 < uVar8) || ((&g_ap_slot_table)[uVar8 * 0xc] == '\0')) {
        bVar21 = uVar8 == 0;
        if (bVar21) {
          uVar8 = (uint)g_rom_detected;
        }
        if (!bVar21 || uVar8 != 1) {
          uVar8 = 0x1d;
          uStack_30 = 0x1d;
          goto LAB_08006e5e;
        }
      }
      uStack_30 = 0x80;
      *puVar12 = 0x80;
      for (uVar8 = 0; (uVar8 & 0xff) != (uint)stlink_mem_remaining; uVar8 = uVar8 + 1) {
        if (g_trace_active == 0) {
          cVar3 = '\0';
          if (g_target_connected != '\0') {
            cVar3 = g_conn_lost;
          }
          if (g_target_connected == '\0' || cVar3 == '\x01') {
            uVar8 = uVar8 & 0xff;
            unaff_r8 = mem_read_8(stlink_tgt_addr,&g_reply_buf + uVar8,stlink_ap_access);
          }
        }
        led_set_result(unaff_r8 == 0x80);
        if (*puVar12 == 0x80) {
          *puVar12 = unaff_r8;
          if (unaff_r8 != 0x80) {
            (&DAT_2000113c)[uVar6 * 3] = stlink_tgt_addr;
          }
        }
        stlink_tgt_addr = stlink_tgt_addr + 1;
      }
    }
    else {
      puVar12 = &stlink_last_rw_status;
      uVar8 = 0x30;
LAB_08006e5e:
      *puVar12 = uVar8;
      led_set_result(0);
      puVar12[1] = stlink_tgt_addr;
    }
    if (stlink_mem_remaining == 1) {
      stlink_mem_remaining = 2;
    }
LAB_08006e74:
    puVar17 = &g_reply_buf;
    iVar19 = 0;
    uVar5 = stlink_mem_remaining;
    goto LAB_080065c8;
  case 0xd:
    iVar19 = apkey_table_lookup(iVar9 + 0x200030a8,&uStack_38);
    if (iVar19 == 0x80) {
      puVar12 = &stlink_ap_status + (uStack_38 & 0xff) * 3;
    }
    else {
      puVar12 = &stlink_last_rw_status;
    }
    uStack_30 = (ushort)iVar19;
    iVar19 = stlink_cmd_is_first_block(param_1);
    if (iVar19 == 1) goto LAB_080070a4;
    if (*puVar12 == 0x30 || *puVar12 == 0x1d) {
      led_set_result(0);
    }
    else {
      for (uVar8 = 0; (uVar8 & 0xff) < (uint)stlink_mem_remaining; uVar8 = uVar8 + 1) {
        if (g_trace_active == 0) {
          cVar3 = '\0';
          if (g_target_connected != '\0') {
            cVar3 = g_conn_lost;
          }
          if (g_target_connected == '\0' || cVar3 == '\x01') {
            uVar8 = uVar8 & 0xff;
            unaff_r8 = mem_write_8(stlink_tgt_addr,(&stlink_ep2out_buf)[uVar8 + param_1 * 0x40],
                                   stlink_ap_access);
          }
        }
        led_set_result(unaff_r8 == 0x80);
        if (*puVar12 == 0x80) {
          *puVar12 = unaff_r8;
          if (unaff_r8 != 0x80) {
            puVar12[1] = stlink_tgt_addr;
          }
        }
        stlink_tgt_addr = stlink_tgt_addr + 1;
      }
    }
    goto LAB_080061c0;
  case 0xe:
  case 0x39:
    g_requested_ap = (uint)(byte)(&DAT_2000309f)[iVar9];
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_08006ba6;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_08006b84:
        uStack_30 = 0x80;
        uStack_38 = 0;
        uStack_30 = mem_write_32(*(undefined4 *)
                                  (&DAT_08007610 + (uint)(byte)(&g_cmd_block)[iVar9] * 4),&uStack_38
                                 ,1,0);
        goto LAB_08006ba6;
      }
      uVar8 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar8 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar8 == 1) goto LAB_08006b84;
      uStack_30 = 0x1d;
LAB_08006b6e:
      uVar7 = 0;
    }
    else {
LAB_08006ba6:
      if (uStack_30 != 0x80) goto LAB_08006b6e;
      uVar7 = 1;
    }
    led_set_result(uVar7);
    if ((&stlink_cmd_subcmd)[iVar9] == '\x0e') {
      uStack_30 = 0x80;
    }
    goto LAB_08007596;
  case 0xf:
  case 0x35:
    g_requested_ap = (uint)(byte)(&DAT_200030a6)[iVar9];
    g_last_rw_value = *(uint *)(&g_cmd_block + iVar9);
    stlink_reg_tmp2 = *(uint *)(&DAT_200030a2 + iVar9);
    if (((&DAT_200030a7)[iVar9] == 0) || (8 < g_requested_ap)) {
      stlink_ap_access = 0;
    }
    else {
      stlink_ap_access =
           (&g_ap_tar_cache)[g_requested_ap] & 0xffff00 | (uint)(byte)(&DAT_200030a7)[iVar9] << 0x18
      ;
    }
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_08006812;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_0800683c:
        uStack_30 = 0x80;
        uStack_38 = stlink_reg_tmp2;
        uStack_30 = mem_write_32(g_last_rw_value,&uStack_38,1,stlink_ap_access);
        goto LAB_08006830;
      }
      uVar8 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar8 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar8 == 1) goto LAB_0800683c;
      uStack_30 = 0x1d;
LAB_080067ee:
      uVar7 = 0;
    }
    else {
LAB_08006812:
      if (g_last_rw_value == 0xe0044100) {
        uStack_38 = stlink_reg_tmp2;
        uStack_30 = mem_write_32(0xe0044100,&uStack_38,1,stlink_ap_access);
        iVar19 = 1;
      }
LAB_08006830:
      if (uStack_30 != 0x80) goto LAB_080067ee;
      uVar7 = 1;
    }
    led_set_result(uVar7);
    if ((&stlink_cmd_subcmd)[iVar9] == '\x06') {
      uStack_30 = 0x80;
    }
    puVar10 = (undefined4 *)&uStack_30;
    goto LAB_08006808;
  case 0x10:
  case 0x37:
    g_last_rw_value = *(uint *)(&DAT_2000309f + iVar9);
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected == '\0' || cVar3 == '\x01') {
        pcStack_34 = (code *)(uint)(byte)(&DAT_200030a7)[iVar9];
        uStack_38 = (uint)(byte)(&DAT_200030a4)[iVar9];
        uStack_30 = dwt_set_watchpoint((&DAT_200030a3)[iVar9],g_last_rw_value,(&DAT_200030a5)[iVar9]
                                       ,(&DAT_200030a6)[iVar9]);
      }
    }
    led_set_result(uStack_30 == 0x80);
    if ((&stlink_cmd_subcmd)[iVar9] == '\x10') {
      uStack_30 = 0x80;
    }
    goto LAB_08007596;
  case 0x11:
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
  case 0x23:
  case 0x24:
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2c:
  case 0x2d:
  case 0x2e:
  case 0x2f:
  case 0x4d:
  case 0x4e:
  case 0x4f:
  case 0x59:
  default:
switchD_08006240_default:
    stlink_cmd_reply_unknown(param_1);
LAB_080061c0:
    uVar8 = stlink_cmd_set_status_idle(param_1);
    break;
  case 0x20:
  case 0x30:
switchD_08006240_caseD_20:
    uVar8 = stlink_cmd_debug_connect(param_1,g_connect_phase);
    return uVar8;
  case 0x21:
    iVar9 = apkey_table_lookup(iVar9 + 0x200030a8,&bStack_2e);
    uStack_30 = (ushort)iVar9;
    if (iVar9 == 0x80) {
      if (g_apkey_count < 2) {
        uStack_30 = apkey_table_remove(bStack_2e);
        pcVar20 = &g_ap_slot_table;
        do {
          if (*pcVar20 != '\0') {
            uStack_30 = cortexm_run_core(iVar19);
            *pcVar20 = '\0';
            pcVar20[1] = '\0';
            __aeabi_memset(pcVar20 + 2,10,0xff);
          }
          iVar19 = iVar19 + 1;
          pcVar20 = pcVar20 + 0xc;
        } while (iVar19 < 9);
        uVar5 = uStack_30;
        if (uStack_30 == 0x80) {
          uVar5 = (ushort)(byte)(&g_ap_state)[param_1];
        }
        if (uStack_30 == 0x80 && uVar5 == 3) {
          swd_write_idle_word();
          iVar19 = dap_ack_to_status();
          uStack_30 = (ushort)iVar19;
          if (iVar19 == 0x80) {
            g_reset_vector = 0xffffffff;
          }
        }
        GPIO_WriteBit(&GPIOB_CRL,1,1);
        target_disconnect(param_1);
        (&g_ap_state)[param_1] = 0;
        g_debug_mode_bitbang = 0;
        led_set_mode(1,0,0);
        g_connect_phase = 1;
      }
      else {
        uStack_30 = apkey_table_remove(bStack_2e);
      }
    }
    goto LAB_080061c0;
  case 0x22:
    puVar12 = (uint *)&stlink_coreid;
    dap_get_last_rw_status(&stlink_coreid,&stlink_dpid);
    uVar15 = 1;
    uVar7 = 4;
    goto LAB_08006314;
  case 0x31:
    iVar19 = dap_get_last_rw_status(&stlink_coreid,&stlink_dpid);
    uStack_30 = (ushort)iVar19;
    led_set_result(iVar19 == 0x80);
    g_reply_buf = (undefined1)uStack_30;
    DAT_20001531 = uStack_30._1_1_;
    __aeabi_memcpy(&DAT_20001534,&stlink_coreid,4);
    __aeabi_memcpy(&DAT_20001538,&stlink_dpid,4);
    uVar15 = 1;
    uVar7 = 0xc;
    puVar12 = (uint *)&g_reply_buf;
    goto LAB_08006314;
  case 0x36:
    g_requested_ap = (uint)(byte)(&DAT_200030a2)[iVar9];
    g_last_rw_value = *(uint *)(&g_cmd_block + iVar9);
    stlink_ap_access =
         (uint)(byte)(&DAT_200030a4)[iVar9] << 0x10 | (uint)(byte)(&DAT_200030a3)[iVar9] << 8 |
         (uint)(byte)(&DAT_200030a5)[iVar9] << 0x18;
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_080068f0;
      if ((g_requested_ap < 9) && ((&g_ap_slot_table)[g_requested_ap * 0xc] != '\0')) {
LAB_0800691c:
        uStack_30 = 0x80;
        uStack_38 = stlink_ap_access;
        uStack_30 = mem_read_32(g_last_rw_value,&stlink_reg_tmp2,1,0);
        goto LAB_08006910;
      }
      uVar8 = g_requested_ap;
      if (g_requested_ap == 0) {
        uVar8 = (uint)g_rom_detected;
      }
      if (g_requested_ap == 0 && uVar8 == 1) goto LAB_0800691c;
      uStack_30 = 0x1d;
LAB_080068c8:
      uVar7 = 0;
    }
    else {
LAB_080068f0:
      if (g_last_rw_value == 0xe0044104) {
        uStack_38 = stlink_ap_access;
        uStack_30 = mem_read_32(0xe0044104,&stlink_reg_tmp2,1,0);
        iVar19 = 1;
      }
LAB_08006910:
      if (uStack_30 != 0x80) goto LAB_080068c8;
      uVar7 = 1;
    }
    led_set_result(uVar7);
    g_reply_buf = (undefined1)uStack_30;
    DAT_20001531 = uStack_30._1_1_;
    __aeabi_memcpy(&DAT_20001534,&stlink_reg_tmp2,4);
    uVar5 = 8;
    puVar17 = &g_reply_buf;
    goto LAB_080065c8;
  case 0x3b:
    iVar19 = apkey_table_lookup(iVar9 + 0x200030a8,&uStack_38);
    uStack_30 = (ushort)iVar19;
    if (iVar19 == 0x80) {
      puVar12 = &stlink_ap_status + (uStack_38 & 0xff) * 3;
      uStack_30 = 0x80;
    }
    else {
      puVar12 = &stlink_last_rw_status;
      stlink_last_rw_status = 0x30;
    }
    uStack_38 = CONCAT22(uStack_38._2_2_,5);
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_080071f8;
    }
    else {
LAB_080071f8:
      *(undefined1 *)puVar12 = 0;
      *(undefined1 *)((int)puVar12 + 1) = 0;
      *(undefined1 *)puVar12 = 5;
      *(undefined1 *)((int)puVar12 + 1) = 0;
    }
    uVar15 = 1;
    uVar7 = 2;
    goto LAB_08006314;
  case 0x3c:
    goto switchD_08006240_caseD_3c;
  case 0x3e:
    iVar19 = apkey_table_lookup(iVar9 + 0x200030a8,&uStack_38);
    uStack_30 = (ushort)iVar19;
    if (iVar19 == 0x80) {
      puVar12 = &stlink_ap_status + (uStack_38 & 0xff) * 3;
      uStack_30 = 0x80;
    }
    else {
      puVar12 = &stlink_last_rw_status;
      stlink_last_rw_status = 0x30;
    }
    uStack_38 = CONCAT22(uStack_38._2_2_,5);
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_0800725a;
    }
    else {
LAB_0800725a:
      __aeabi_memclr(puVar12,0xc);
      *(undefined1 *)puVar12 = (undefined1)uStack_38;
      *(undefined1 *)((int)puVar12 + 1) = uStack_38._1_1_;
    }
    uVar15 = 1;
    uVar7 = 0xc;
LAB_08006314:
    uVar8 = stlink_cmd_send_reply(param_1,puVar12,uVar7,uVar15);
    break;
  case 0x3f:
switchD_08006240_caseD_3f:
    uVar8 = stlink_cmd_get_conn_status(param_1);
    break;
  case 0x40:
    if (g_debug_mode_bitbang != 1) goto LAB_08007386;
    uStack_2c = *(ushort *)(&g_cmd_block + iVar9);
    g_last_rw_value = *(uint *)(&DAT_200030a0 + iVar9);
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_0800750c;
      usart_bridge_set_baud(g_last_rw_value,uStack_2c);
      uStack_30 = dap_ack_to_status();
    }
    goto LAB_08007596;
  case 0x41:
    bVar2 = g_debug_mode_bitbang;
    if (g_debug_mode_bitbang == 1) {
      bVar2 = g_trace_active;
    }
    if (g_debug_mode_bitbang == 1 && bVar2 == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected == '\0' || cVar3 == '\x01') {
        usart_bridge_close();
        uStack_30 = dap_ack_to_status();
        goto LAB_08007596;
      }
    }
LAB_08007386:
    uStack_30 = 0x20;
    goto LAB_08007596;
  case 0x42:
    bVar2 = g_debug_mode_bitbang;
    if (g_debug_mode_bitbang == 1) {
      bVar2 = g_trace_active;
    }
    if (g_debug_mode_bitbang != 1 || bVar2 != 0) {
LAB_080073ae:
      uStack_2c = 0;
    }
    else {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_080073ae;
      uStack_2c = usart_bridge_poll_rx();
    }
    uStack_38 = CONCAT22(uStack_38._2_2_,5);
    if (g_trace_active == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_080073ca;
    }
    else {
LAB_080073ca:
      uStack_2c = 5;
    }
    puVar11 = &uStack_2c;
    goto LAB_0800698e;
  case 0x43:
LAB_0800621a:
    uVar7 = 1;
    goto LAB_0800621e;
  case 0x44:
switchD_08006240_caseD_44:
    uVar7 = 0;
LAB_0800621e:
    uVar8 = stlink_cmd_set_freq(uVar7,param_1);
    break;
  case 0x45:
    uVar7 = 1;
    goto LAB_080073f6;
  case 0x46:
    uVar7 = 0;
LAB_080073f6:
    uVar8 = stlink_cmd_rw_dap_reg(param_1,uVar7);
    break;
  case 0x49:
switchD_08006240_caseD_49:
    uVar8 = stlink_cmd_set_led_ack(param_1);
    break;
  case 0x4a:
switchD_08006240_caseD_4a:
    uVar8 = stlink_cmd_get_fw_build(param_1);
    break;
  case 0x4b:
    iVar19 = apkey_table_lookup(iVar9 + 0x200030a8,&bStack_2e);
    uStack_30 = (ushort)iVar19;
    if (iVar19 == 0x80) {
      if (g_trace_active == 0) {
        cVar3 = '\0';
        if (g_target_connected != '\0') {
          cVar3 = g_conn_lost;
        }
        if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_08007456;
      }
      else {
LAB_08007456:
        if (g_target_connected != '\x01') goto LAB_08007596;
      }
      uVar8 = (uint)(byte)(&g_cmd_block)[iVar9];
      uVar4 = (&DAT_2000309f)[iVar9];
      if ((8 < uVar8) || (puVar13 = &g_ap_slot_table, (&g_ap_slot_table)[uVar8 * 0xc] == '\0')) {
        uVar22 = target_select_ap(uVar8);
        puVar13 = (undefined1 *)((ulonglong)uVar22 >> 0x20);
        uStack_30 = (ushort)uVar22;
        if ((int)uVar22 == 0x80) {
          target_scan_rom_table();
          puVar13 = extraout_r1;
        }
      }
      uVar5 = uStack_30;
      if (uStack_30 == 0x80) {
        uVar5 = (ushort)g_trace_active;
      }
      if (uStack_30 == 0x80 && uVar5 == 0) {
        cVar3 = '\0';
        if (g_target_connected != '\0') {
          cVar3 = g_conn_lost;
        }
        if (g_target_connected == '\0' || cVar3 == '\x01') {
          uStack_30 = 0x1d;
          bVar21 = SBORROW4(uVar8,9);
          puVar1 = (undefined1 *)(uVar8 - 9);
          if (uVar8 < 9) {
            puVar13 = (undefined1 *)(uint)bStack_2e;
            bVar21 = SBORROW4((int)puVar13,10);
            puVar1 = puVar13 + -10;
          }
          if ((int)puVar1 < 0 != bVar21) {
            iVar19 = uVar8 * 0xc;
            if ((&g_ap_slot_table)[iVar19] == '\0') {
              (&DAT_20001359)[iVar19] = uVar4;
            }
            if ((&g_ap_slot_table)[(int)(puVar13 + iVar19 + 2)] == -1) {
              (&g_ap_slot_table)[iVar19] = (&g_ap_slot_table)[iVar19] + '\x01';
              (&g_ap_slot_table)[(int)(puVar13 + iVar19 + 2)] = bStack_2e;
            }
            uStack_30 = 0x80;
          }
        }
        goto LAB_0800750c;
      }
    }
    goto LAB_08007596;
  case 0x4c:
    uVar8 = apkey_table_lookup(iVar9 + 0x200030a8,&bStack_2e);
    uStack_30 = (ushort)uVar8;
    bVar21 = uVar8 == 0x80;
    if (bVar21) {
      uVar8 = (uint)g_trace_active;
    }
    if (bVar21 && uVar8 == 0) {
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_0800750c;
      uVar6 = (uint)(byte)(&g_cmd_block)[iVar9];
      uVar8 = (uint)bStack_2e;
      bVar21 = SBORROW4(uVar6,9);
      iVar19 = uVar6 - 9;
      if (uVar6 < 9) {
        bVar21 = SBORROW4(uVar8,10);
        iVar19 = uVar8 - 10;
      }
      if (iVar19 < 0 != bVar21) {
        uVar14 = uVar6 * 0xc;
        uVar8 = (uint)(byte)(&DAT_2000135a)[uVar8 + uVar14];
        if (uVar8 != 0xff) {
          uVar14 = (uint)(byte)(&g_ap_slot_table)[uVar14];
        }
        if (((uVar8 != 0xff && uVar14 != 0) && (uVar6 < 9)) &&
           (uVar8 = uVar6 * 3, (&g_ap_slot_table)[uVar6 * 0xc] == '\x01')) {
          uStack_30 = cortexm_run_core(uVar6);
          uVar8 = extraout_r2;
        }
      }
      if (uStack_30 == 0x80) {
        uStack_30 = 0x1d;
        bVar21 = SBORROW4(uVar6,9);
        iVar19 = uVar6 - 9;
        if (uVar6 < 9) {
          uVar8 = (uint)bStack_2e;
          bVar21 = SBORROW4(uVar8,10);
          iVar19 = uVar8 - 10;
        }
        if (iVar19 < 0 != bVar21) {
          iVar19 = uVar6 * 0xc;
          if ((&g_ap_slot_table)[uVar8 + iVar19 + 2] == -1) {
LAB_0800758a:
            if ((&g_ap_slot_table)[iVar19] == '\0') goto LAB_0800758e;
          }
          else {
            if ((&g_ap_slot_table)[iVar19] != '\0') {
              (&g_ap_slot_table)[iVar19] = (&g_ap_slot_table)[iVar19] + -1;
              (&g_ap_slot_table)[uVar8 + iVar19 + 2] = 0xff;
              goto LAB_0800758a;
            }
LAB_0800758e:
            (&DAT_20001359)[iVar19] = 0;
          }
          uStack_30 = 0x80;
        }
      }
    }
LAB_08007596:
    uStack_38 = CONCAT22(uStack_38._2_2_,5);
    if (g_trace_active == 0) {
LAB_080075a2:
      cVar3 = '\0';
      if (g_target_connected != '\0') {
        cVar3 = g_conn_lost;
      }
      if (g_target_connected != '\0' && cVar3 != '\x01') goto LAB_080075b0;
    }
    else {
LAB_080075b0:
      uStack_30 = (ushort)uStack_38;
    }
LAB_08006988:
    puVar11 = &uStack_30;
LAB_0800698e:
    uVar8 = stlink_cmd_send_reply(param_1,puVar11,2,1);
    break;
  case 0x51:
    uVar8 = usb_bulk_debug_access(param_1);
    break;
  case 0x52:
    iVar9 = stlink_cmd_is_first_block(param_1);
    if (iVar9 == 1) {
      stlink_debug_get_result_ptr(&stlink_mem_remaining,&DAT_2000109c);
      stlink_mem_offset = 0;
    }
    puVar17 = DAT_2000109c + stlink_mem_offset;
    uVar5 = stlink_mem_remaining;
    if (0x40 < stlink_mem_remaining) {
      stlink_cmd_send_reply(param_1,puVar17,0x40,0);
      stlink_mem_offset = stlink_mem_offset + 0x40;
      uVar8 = (uint)stlink_mem_remaining;
      stlink_mem_remaining = (ushort)(uVar8 - 0x40);
      return uVar8 - 0x40;
    }
    goto LAB_080065c8;
  case 0x53:
switchD_08006240_caseD_53:
    uStack_38 = CONCAT22(uStack_38._2_2_,0x40);
    iVar19 = 1;
LAB_08006808:
    uVar8 = stlink_send_reply(param_1,puVar10,2,iVar19);
    break;
  case 0x55:
switchD_08006240_caseD_55:
    uVar8 = stlink_cmd_jtag_raw(param_1);
    break;
  case 0x56:
switchD_08006240_caseD_56:
    uVar8 = stlink_cmd_get_version(param_1);
    break;
  case 0x57:
switchD_08006240_caseD_57:
    uVar8 = stlink_cmd_get_param_block(param_1);
    break;
  case 0x58:
switchD_08006240_caseD_58:
    uVar8 = stlink_cmd_get_caps(param_1);
    break;
  case 0x5a:
    uVar8 = stlink_cmd_read_trace_buf(param_1);
    break;
  case 0x5b:
    puVar17 = &g_reply_buf;
    DAT_2000109c = &g_reply_buf;
    uStack_2c = (short)g_trace_count * 2 + 4;
    iVar19 = 1;
    uVar5 = uStack_2c;
LAB_080065c8:
    uVar8 = stlink_send_reply(param_1,puVar17,uVar5,iVar19);
    break;
  case 0x5c:
switchD_08006240_caseD_5c:
    uVar8 = stlink_cmd_get_rw_status(param_1);
  }
  return uVar8;
}

/**
 * stlink_cmd_send_reply
 *   Queue reply payload for a command slot: for slot 0 copies to EP1 IN PMA (sub_0800C600) and arms EP1 TX, sets per-slot reply state (3 if last fragment else 2).
 *   address 0x08009854, 50 bytes
 *   called from: stlink_cmd_set_freq, stlink_cmd_set_led_ack, stlink_cmd_get_caps, stlink_dispatch_command, stlink_cmd_get_version, stlink_cmd_get_rw_status, stlink_cmd_debug_connect, stlink_cmd_get_param_block ...
 *   prototype: void stlink_cmd_send_reply(int chan, void *data, uint len, int last)
 */
/* Queue reply payload for a command slot: for slot 0 copies to EP1 IN PMA (sub_0800C600) and arms
   EP1 TX, sets per-slot reply state (3 if last fragment else 2). */

void stlink_cmd_send_reply(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  undefined1 uVar1;
  
  if (param_1 == 0) {
    UserToPMABufferCopy(param_2);
  }
  if (param_4 == 1) {
    uVar1 = 3;
  }
  else {
    uVar1 = 2;
  }
  (&stlink_cmd_reply_state)[param_1] = uVar1;
  if (param_1 != 0) {
    return;
  }
  SetEPTxCount(1,param_3);
  SetEPTxStatus(1,0x30);
  return;
}

/**
 * stlink_cmd_set_status_busy
 *   Set per-slot reply state = 1 (more data expected) and, for slot 0, re-arm EP0R control endpoint.
 *   address 0x08009886, 14 bytes
 *   called from: stlink_dispatch_command, stlink_cmd_read_trace_buf, usb_bulk_debug_access, stlink_cmd_process
 *   prototype: void stlink_cmd_set_status_busy(int chan)
 */
/* Set per-slot reply state = 1 (more data expected) and, for slot 0, re-arm EP0R control endpoint.
    */

void stlink_cmd_set_status_busy(int param_1)

{
  (&stlink_cmd_reply_state)[param_1] = 1;
  if (param_1 != 0) {
    return;
  }
  (&USB_EP0R)[usb_ep0_idx] = (&USB_EP0R)[usb_ep0_idx] & 0xbf8f ^ 0x3000 | 0x8080;
  return;
}

/**
 * stlink_cmd_set_status_idle
 *   Set per-slot reply state = 0 (idle/done) and, for slot 0, re-arm EP0R control endpoint.
 *   address 0x08009894, 14 bytes
 *   called from: stlink_dispatch_command, usb_class_reset, stlink_cmd_read_trace_buf, stlink_cmd_debug_connect, usb_bulk_debug_access, stlink_cmd_process
 *   prototype: void stlink_cmd_set_status_idle(int chan)
 */
/* Set per-slot reply state = 0 (idle/done) and, for slot 0, re-arm EP0R control endpoint. */

void stlink_cmd_set_status_idle(int param_1)

{
  (&stlink_cmd_reply_state)[param_1] = 0;
  if (param_1 != 0) {
    return;
  }
  (&USB_EP0R)[usb_ep0_idx] = (&USB_EP0R)[usb_ep0_idx] & 0xbf8f ^ 0x3000 | 0x8080;
  return;
}

/**
 * stlink_cmd_is_first_block
 *   Return 1 when this is the first block of a multi-block transfer (slot 0 and reply state == 0), else 0.
 *   address 0x080098A2, 20 bytes
 *   called from: stlink_dispatch_command, stlink_cmd_read_trace_buf, stlink_cmd_get_version, usb_bulk_debug_access, stlink_cmd_process
 *   prototype: int stlink_cmd_is_first_block(uint chan)
 */
/* Return 1 when this is the first block of a multi-block transfer (slot 0 and reply state == 0),
   else 0. */

undefined4 stlink_cmd_is_first_block(uint param_1)

{
  bool bVar1;
  
  bVar1 = param_1 == 0;
  if (bVar1) {
    param_1 = (uint)stlink_cmd_reply_state;
  }
  if (!bVar1 || param_1 != 0) {
    return 0;
  }
  return 1;
}

/**
 * stlink_cmd_reset_status
 *   Reset slot-0 reply state to 0.
 *   address 0x080098B6, 8 bytes
 *   called from: usb_class_set_configured
 *   prototype: void stlink_cmd_reset_status(void)
 */
/* Reset slot-0 reply state to 0. */

void stlink_cmd_reset_status(void)

{
  stlink_cmd_reply_state = 0;
  return;
}

/**
 * stlink_cmd_reply_unknown
 *   Build and send the unknown/invalid-command status reply (status byte 0x42) for the given slot.
 *   address 0x080098BE, 60 bytes
 *   called from: stlink_dispatch_command, stlink_cmd_debug_connect, stlink_cmd_process
 *   prototype: void stlink_cmd_reply_unknown(int chan)
 */
/* Build and send the unknown/invalid-command status reply (status byte 0x42) for the given slot. */

void stlink_cmd_reply_unknown(int param_1)

{
  g_reply_buf = 0x42;
  DAT_20001531 = 0;
  if (param_1 == 0) {
    UserToPMABufferCopy(&g_reply_buf,0xa0,2);
    stlink_cmd_reply_state = 3;
    SetEPTxCount(1,2);
    SetEPTxStatus(1,0x30);
    return;
  }
  (&stlink_cmd_reply_state)[param_1] = 3;
  return;
}

/**
 * stlink_debug_ap_access_batch
 *   Process a batch (<=0x40) of debug access-port ops (switch cases 0..5): select AP (sub_08004866), mem read/write via SWD core (sub_08005402/sub_080050FC), store results at DAT_20002D30/+param
 *   address 0x0800ACEC, 250 bytes
 *   called from: usb_bulk_debug_access
 *   prototype: undefined8 stlink_debug_ap_access_batch(undefined1 *ops, uint count)
 */
/* Process a batch (<=0x40) of debug access-port ops (switch cases 0..5): select AP (sub_08004866),
   mem read/write via SWD core (sub_08005402/sub_080050FC), store results at DAT_20002D30/+param */

undefined8 stlink_debug_ap_access_batch(undefined1 *param_1,uint param_2)

{
  uint *puVar1;
  int iVar2;
  int *piVar3;
  undefined4 *puVar4;
  undefined4 local_28;
  undefined4 uStack_24;
  
  puVar1 = (uint *)(param_1 + (param_2 - 1 & 0xfffffffc));
  if (param_2 < 0x41) {
    if (param_2 == 0) goto LAB_0800ade8;
  }
  else {
    param_2 = 0x40;
  }
  puVar4 = &debug_ap_result_buf;
  piVar3 = &debug_ap_result_buf + param_2;
  do {
    puVar1 = puVar1 + 1;
    switch(*param_1) {
    case 0:
      *puVar4 = 0;
      if (*puVar1 == 4) {
        iVar2 = 0x80;
        swd_ap_state = iVar2;
      }
      else {
        iVar2 = 8;
        swd_ap_state = iVar2;
      }
      break;
    case 1:
      swd_ap_addr = *puVar1;
    case 4:
      *puVar4 = 0;
      iVar2 = 0x80;
      break;
    case 2:
      if (swd_ap_state == 0x80) {
        swd_ap_state = target_slot_check(swd_ap_select & 0xff);
      }
      iVar2 = swd_ap_state;
      if (swd_ap_state == 0x80) {
        g_requested_ap = swd_ap_select;
        iVar2 = mem_write_32(swd_ap_addr,puVar1,1,0);
      }
      *piVar3 = iVar2;
      goto LAB_0800add4;
    case 3:
      if (swd_ap_state == 0x80) {
        swd_ap_state = target_slot_check(swd_ap_select & 0xff);
      }
      iVar2 = swd_ap_state;
      if (swd_ap_state == 0x80) {
        g_requested_ap = swd_ap_select;
        local_28 = 0;
        iVar2 = mem_read_32(*puVar1,puVar4,1,0);
      }
      break;
    case 5:
      if (swd_ap_state == 0x80) {
        iVar2 = target_slot_check(*puVar1 & 0xff);
        if (iVar2 == 0x80) {
          swd_ap_select = *puVar1;
        }
        else {
          *piVar3 = swd_ap_state;
          swd_ap_state = 0x1d;
        }
      }
      *piVar3 = swd_ap_state;
LAB_0800add4:
      *puVar4 = 0;
    default:
      goto switchD_0800ad28_default;
    }
    *piVar3 = iVar2;
switchD_0800ad28_default:
    piVar3 = piVar3 + 1;
    puVar4 = puVar4 + 1;
    param_2 = param_2 - 1;
    param_1 = param_1 + 1;
  } while (param_2 != 0);
LAB_0800ade8:
  return CONCAT44(uStack_24,local_28);
}

/**
 * usb_bulk_debug_access
 *   USB bulk handler (EP2 OUT) that accumulates the AP-access command block then runs stlink_debug_ap_access_batch; manages DAT_200030C0/C4/C8 counters
 *   address 0x0800ADEC, 146 bytes
 *   called from: stlink_dispatch_command
 *   prototype: undefined4 usb_bulk_debug_access(int ep,undefined4,undefined4,undefined4)
 */
/* USB bulk handler (EP2 OUT) that accumulates the AP-access command block then runs
   stlink_debug_ap_access_batch; manages DAT_200030C0/C4/C8 counters */

undefined4
usb_bulk_debug_access(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  
  iVar1 = stlink_cmd_is_first_block();
  if (iVar1 == 1) {
    usb_bulk_count = *(uint *)(&g_cmd_block + param_1 * 0x10);
    if (usb_bulk_count == 0) {
      usb_bulk_count = 1;
    }
    else if (0x40 < usb_bulk_count) {
      usb_bulk_count = 0x40;
    }
    usb_bulk_remaining = (usb_bulk_count + (usb_bulk_count - 1 >> 2) + 1) * 4;
    usb_bulk_offset = 0;
    if (usb_bulk_remaining == 0) {
      return param_4;
    }
  }
  else {
    uVar3 = usb_bulk_remaining;
    if (0x40 < usb_bulk_remaining) {
      uVar3 = 0x40;
    }
    __aeabi_memcpy(&g_reply_buf + usb_bulk_offset,&stlink_ep2out_buf + param_1 * 0x40,uVar3);
    usb_bulk_offset = uVar3 + usb_bulk_offset;
    usb_bulk_remaining = usb_bulk_remaining - uVar3;
    if (usb_bulk_remaining == 0) {
      stlink_debug_ap_access_batch(&g_reply_buf,usb_bulk_count);
      uVar2 = stlink_cmd_set_status_idle(param_1,param_4);
      return uVar2;
    }
  }
  uVar2 = stlink_cmd_set_status_busy(param_1,param_4);
  return uVar2;
}

/**
 * stlink_debug_get_result_ptr
 *   Return pointer to the AP-access result buffer (DAT_20002D30) and its byte length (DAT_200030C0<<3)
 *   address 0x0800AE7E, 14 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void stlink_debug_get_result_ptr(undefined2 *len,undefined4 **buf)
 */
/* Return pointer to the AP-access result buffer (DAT_20002D30) and its byte length
   (DAT_200030C0<<3) */

void stlink_debug_get_result_ptr(undefined2 *param_1,undefined4 *param_2)

{
  *param_2 = &debug_ap_result_buf;
  *param_1 = (short)(usb_bulk_count << 3);
  return;
}

/**
 * usart_bridge_dma_init
 *   Init USART1 (0x40013800) for the F6 bootloader bridge: USART_DeInit, config 9600? via sub_0800CFC8/CF38, enable
 *   address 0x0800AEA8, 82 bytes
 *   called from: usart_bridge_init, usart_bridge_set_baud
 *   prototype: void usart_bridge_dma_init(undefined4 baud)
 */
/* Init USART1 (0x40013800) for the F6 bootloader bridge: USART_DeInit, config 9600? via
   sub_0800CFC8/CF38, enable */

void usart_bridge_dma_init(undefined4 param_1)

{
  undefined4 local_20;
  undefined2 local_1c;
  undefined2 local_1a;
  undefined4 local_18;
  undefined4 local_14;
  undefined2 local_10;
  undefined2 local_e;
  undefined2 local_c;
  
  local_14 = 0;
  local_10 = 0;
  local_c = 0;
  local_e = 4;
  local_20 = 0;
  local_1c = 0x200;
  local_1a = 0;
  local_18 = param_1;
  USART_DeInit(&DAT_40013800);
  usart_clock_config(&DAT_40013800,&local_20);
  usart_init(&DAT_40013800,&local_18);
  usart_cr3_bit_config(&DAT_40013800,0x40,1);
  return;
}

/**
 * usart_bridge_init
 *   Set up GPIOA USART pins, enable USART1 + DMA1 ch5 (0x40020058) RX into 0x20000020 ring, NVIC; F6 UART bootloader bridge init
 *   address 0x0800AEFA, 228 bytes
 *   called from: swd_enter_special_sequence, swd_read_idcode
 *   prototype: undefined4 usart_bridge_init(void)
 */
/* Set up GPIOA USART pins, enable USART1 + DMA1 ch5 (0x40020058) RX into 0x20000020 ring, NVIC; F6
   UART bootloader bridge init */

undefined4 usart_bridge_init(void)

{
  undefined1 local_40;
  undefined1 local_3f;
  undefined1 local_3e;
  undefined1 local_3d;
  undefined2 local_3c;
  undefined1 local_3a;
  undefined1 local_39;
  undefined2 *local_38;
  undefined *local_34;
  undefined4 local_30;
  uint local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  
  local_3c = 0x400;
  local_39 = 0x48;
  local_3a = 3;
  GPIO_Init(&GPIOA_CRL,&local_3c);
  RCC_APB2PeriphClockCmd(0x4000,1);
  RCC_AHBPeriphClockCmd(1,1);
  usart_bridge_dma_init(usart_bridge_cur_target);
  RCC_APB2PeriphClockCmd(0x4000,0);
  DMA_DeInit(&DAT_40020058);
  DAT_40020004 = 0x10000;
  __aeabi_memclr(&DAT_20000020,0x1000);
  local_38 = &DAT_40013804;
  local_34 = &DAT_20000020 + DAT_20000018 * 0x800;
  local_30 = 0;
  local_2c = (uint)(ushort)(&usart_bridge_rx_len)[DAT_20000018];
  local_28 = 0;
  local_24 = 0x80;
  local_20 = 0;
  local_1c = 0;
  local_18 = 0;
  local_14 = 0x2000;
  local_10 = 0;
  DMA_Init(&DAT_40020058,&local_38);
  DMA_ITConfig(&DAT_40020058,2,1);
  DMA_ITConfig(&DAT_40020058,8,0);
  DMA_ITConfig(&DAT_40020058,4,0);
  local_40 = 0xf;
  local_3f = 0;
  local_3e = 1;
  local_3d = 1;
  NVIC_Init(&local_40);
  usb_trace_ep_maxpkt = 0x40;
  return 0;
}

/**
 * usart_bridge_set_baud
 *   (Re)configure USART1 baud/DMA for bridge target param_1, restart DMA1 ch5 RX; returns status
 *   address 0x0800B062, 192 bytes
 *   called from: stlink_dispatch_command
 *   prototype: longlong usart_bridge_set_baud(int baud,uint len)
 */
/* (Re)configure USART1 baud/DMA for bridge target param_1, restart DMA1 ch5 RX; returns status */

longlong usart_bridge_set_baud(int param_1,uint param_2)

{
  int iVar1;
  ushort uVar2;
  undefined2 uVar3;
  uint uVar4;
  bool bVar5;
  uint local_18;
  
  if (usart_bridge_active == '\x01') {
    bVar5 = (uint)usart_bridge_rx_len * 2 - param_2 == 0;
    iVar1 = (uint)usart_bridge_rx_len * 2;
    if (bVar5) {
      iVar1 = usart_bridge_cur_target;
    }
    if (!bVar5 || iVar1 != param_1) {
      uVar2 = DAT_4001380c;
      DAT_4001380c = uVar2 & 0xdfff;
      uVar2 = DAT_40013800;
      if ((int)((uint)uVar2 << 0x1c) < 0) {
        uVar3 = DAT_40013804;
        local_18 = CONCAT22(local_18._2_2_,uVar3);
      }
      RCC_APB2PeriphClockCmd(0x4000,0);
      DMA_Cmd(&DAT_40020058,0);
      usart_bridge_active = '\0';
    }
  }
  if (0x1000 < (int)param_2) {
LAB_0800b0c2:
    return CONCAT44(local_18,0x30);
  }
  usart_bridge_rx_len = (ushort)(param_2 >> 1);
  DAT_20000006 = usart_bridge_rx_len;
  if (usart_bridge_cur_target == param_1) {
    RCC_APB2PeriphClockCmd(0x4000,1);
  }
  else {
    if (param_1 == 0) goto LAB_0800b0c2;
    RCC_APB2PeriphClockCmd(0x4000,1);
    usart_bridge_dma_init(param_1);
    usart_bridge_cur_target = param_1;
  }
  DAT_20000014 = 0;
  DAT_20000018 = 0;
  DAT_2000000e = 0;
  DAT_20000010 = 0;
  DAT_20000008 = 0;
  DAT_2000000a = DAT_20000006;
  uVar4 = DAT_40020058;
  DAT_40020058 = uVar4 & 0xfffffffe;
  DAT_4002005c = (uint)usart_bridge_rx_len;
  DAT_40020064 = 0x20000020;
  uVar4 = DAT_40020058;
  DAT_40020058 = uVar4 | 1;
  uVar2 = DAT_4001380c;
  DAT_4001380c = uVar2 | 0x2000;
  usart_bridge_active = 1;
  return (ulonglong)local_18 << 0x20;
}

/**
 * usart_bridge_close
 *   Disable USART1 DMA request and DMA1 ch5, mark bridge inactive
 *   address 0x0800B122, 52 bytes
 *   called from: stlink_dispatch_command
 *   prototype: void usart_bridge_close(void)
 */
/* Disable USART1 DMA request and DMA1 ch5, mark bridge inactive */

void usart_bridge_close(void)

{
  ushort uVar1;
  undefined2 uVar2;
  
  uVar1 = DAT_4001380c;
  DAT_4001380c = uVar1 & 0xdfff;
  uVar1 = DAT_40013800;
  if ((int)((uint)uVar1 << 0x1c) < 0) {
    uVar2 = DAT_40013804;
  }
  RCC_APB2PeriphClockCmd(0x4000,0);
  DMA_Cmd(&DAT_40020058,0);
  usart_bridge_active = 0;
  return;
}

/**
 * usart_bridge_poll_rx
 *   Compute bytes received in the bridge RX ring from DMA1 ch5 counter and forward them to USB trace EP
 *   address 0x0800B1D2, 92 bytes
 *   called from: stlink_dispatch_command
 *   prototype: ushort usart_bridge_poll_rx(void)
 */
/* Compute bytes received in the bridge RX ring from DMA1 ch5 counter and forward them to USB trace
   EP */

ushort usart_bridge_poll_rx(void)

{
  short sVar1;
  
  if ((DAT_20000010 == 0) && (usart_bridge_active != '\0')) {
    if ((DAT_20000014 == DAT_20000018) &&
       (sVar1 = DMA_GetCurrDataCounter(&DAT_40020058), DAT_20000014 == DAT_20000018)) {
      DAT_20000010 = ((&usart_bridge_rx_len)[DAT_20000014] - sVar1) - (&DAT_20000008)[DAT_20000014];
    }
    else {
      DAT_20000010 = (&usart_bridge_rx_len)[DAT_20000014] - (&DAT_20000008)[DAT_20000014];
    }
    if ((ushort)(&usart_bridge_rx_len)[DAT_20000014] < DAT_20000010) {
      DAT_20000010 = 0;
    }
    usart_bridge_send_trace();
    return DAT_20000010;
  }
  return 0;
}

/**
 * usart_bridge_send_trace
 *   Copy up to DAT_2000000C bytes of bridge RX data to USB EP3 PMA, set tx count, mark EP3 TX valid
 *   address 0x0800B22E, 66 bytes
 *   called from: usart_bridge_poll_rx
 *   prototype: undefined4 usart_bridge_send_trace(void)
 */
/* Copy up to DAT_2000000C bytes of bridge RX data to USB EP3 PMA, set tx count, mark EP3 TX valid
    */

undefined4 usart_bridge_send_trace(void)

{
  undefined4 uVar1;
  undefined4 in_r3;
  ushort uVar2;
  
  if (DAT_20000010 != 0) {
    uVar2 = DAT_20000010;
    if (usb_trace_ep_maxpkt < DAT_20000010) {
      uVar2 = usb_trace_ep_maxpkt;
    }
    UserToPMABufferCopy(&DAT_20000020 +
                        (uint)(ushort)(&DAT_20000008)[DAT_20000014] + DAT_20000014 * 0x800,0x120,
                        uVar2);
    DAT_2000000e = uVar2;
    SetEPTxCount(3,uVar2);
    uVar1 = SetEPTxStatus(3,0x30,in_r3);
    return uVar1;
  }
  return in_r3;
}

/**
 * usart_bridge_dma_tc_handler
 *   USART1 RX DMA transfer-complete handler: advance ring indices, send more via EP3, swap double buffers
 *   address 0x0800B270, 168 bytes
 *   prototype: void usart_bridge_dma_tc_handler(void)
 */
/* USART1 RX DMA transfer-complete handler: advance ring indices, send more via EP3, swap double
   buffers */

void usart_bridge_dma_tc_handler(void)

{
  uint uVar1;
  int iVar2;
  ushort uVar3;
  ushort uVar4;
  
  iVar2 = DAT_20000014;
  uVar3 = DAT_2000000e + (&DAT_20000008)[DAT_20000014];
  (&DAT_20000008)[DAT_20000014] = uVar3;
  DAT_20000010 = DAT_20000010 - DAT_2000000e;
  DAT_2000000e = 0;
  if (DAT_20000010 != 0) {
    uVar4 = DAT_20000010;
    if (usb_trace_ep_maxpkt < DAT_20000010) {
      uVar4 = usb_trace_ep_maxpkt;
    }
    UserToPMABufferCopy(&DAT_20000020 + (uint)uVar3 + iVar2 * 0x800,0x120,uVar4);
    DAT_2000000e = uVar4;
    SetEPTxCount(3,uVar4);
    SetEPTxStatus(3,0x30);
  }
  if ((ushort)(&usart_bridge_rx_len)[DAT_20000014] <= uVar3) {
    if (DAT_20000001 == '\x01') {
      DAT_20000001 = '\0';
      iVar2 = 1 - DAT_20000018;
      DAT_20000018 = iVar2;
      (&DAT_20000008)[iVar2] = 0;
      uVar1 = DAT_40020058;
      DAT_40020058 = uVar1 & 0xfffffffe;
      DAT_4002005c = (uint)(ushort)(&usart_bridge_rx_len)[iVar2];
      DAT_40020064 = &DAT_20000020 + iVar2 * 0x800;
      uVar1 = DAT_40020058;
      DAT_40020058 = uVar1 | 1;
      uVar3 = DAT_4001380c;
      DAT_4001380c = uVar3 | 0x2000;
    }
    DAT_20000014 = 1 - DAT_20000014;
  }
  return;
}

/**
 * _uncertain_target_mco_io_config
 *   Set MCO + reconfigure PA debug IO, returns mode-descriptor 0x18020100 (part of mode/current-mode logic)
 *   address 0x0800C0A4, 46 bytes
 *   called from: usb_class_reset
 *   prototype: undefined4 target_mco_io_config(void)
 */
/* Set MCO + reconfigure PA debug IO, returns mode-descriptor 0x18020100 (part of mode/current-mode
   logic) */

undefined4 _uncertain_target_mco_io_config(void)

{
  RCC_MCOConfig(stlink_current_mode);
  GPIO_Init(&GPIOA_CRL,&stack0xfffffff8);
  return 0x18020100;
}

/**
 * _uncertain_target_io_reinit
 *   Reconfigure PA15 debug IO then re-apply target mode (sub_0800C35C/08009444)
 *   address 0x0800C18E, 56 bytes
 *   called from: usb_class_reset
 *   prototype: undefined8 target_io_reinit(void)
 */
/* Reconfigure PA15 debug IO then re-apply target mode (sub_0800C35C/08009444) */

undefined8 _uncertain_target_io_reinit(void)

{
  undefined2 local_10;
  undefined1 local_e;
  undefined1 local_d;
  undefined4 uStack_c;
  
  GPIO_SetBits(&GPIOA_CRL,0x8000);
  local_10 = 0x8000;
  local_d = 0x14;
  local_e = 1;
  GPIO_Init(&GPIOA_CRL,&local_10);
  _uncertain_stlink_target_mode_apply();
  led_tim1_init();
  return CONCAT44(uStack_c,CONCAT13(local_d,CONCAT12(local_e,local_10)));
}

/**
 * _uncertain_stlink_mode_leave
 *   Leave current debug mode: SWD off (sub_08004B06(0)), restore IO, clear active flag, return mode 0x04020100
 *   address 0x0800C1C6, 60 bytes
 *   called from: stlink_cmd_process
 *   prototype: undefined4 stlink_mode_leave(int mode)
 */
/* Leave current debug mode: SWD off (sub_08004B06(0)), restore IO, clear active flag, return mode
   0x04020100 */

undefined4 _uncertain_stlink_mode_leave(int param_1)

{
  if (param_1 == 1) {
    target_drive_nrst(0);
  }
  GPIO_Init(&GPIOA_CRL,&stack0xfffffff8);
  GPIO_WriteBit(&GPIOB_CRL,0x8000,1);
  target_mode_active = 0;
  return 0x4020100;
}

/**
 * _uncertain_stlink_mode_enter
 *   Enter debug/bootloader mode: drive NRST, MCO, enable SWD (sub_08004B06(1)), set active flag, return 0x18020100
 *   address 0x0800C202, 80 bytes
 *   called from: stlink_cmd_process, APP_main
 *   prototype: undefined4 stlink_mode_enter(int mode,undefined4,undefined4,undefined4)
 */
/* Enter debug/bootloader mode: drive NRST, MCO, enable SWD (sub_08004B06(1)), set active flag,
   return 0x18020100 */

undefined4
_uncertain_stlink_mode_enter(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 local_10;
  
  local_10 = param_4;
  if (target_mode_active == '\0') {
    GPIO_WriteBit(&GPIOB_CRL,0x8000,0);
    RCC_MCOConfig(stlink_current_mode);
    local_10 = 0x18020100;
    GPIO_Init(&GPIOA_CRL,&local_10);
    timer_delay_ticks(0x32);
    if (param_1 == 1) {
      target_drive_nrst(1);
    }
    target_mode_active = '\x01';
  }
  return local_10;
}

/**
 * _uncertain_board_detect_variant
 *   One-shot board detection reading GPIOB IDR bit3 into DAT_20003100 (board-ID/variant), returns the bit
 *   address 0x0800C252, 66 bytes
 *   called from: stlink_cmd_process, _uncertain_stlink_target_mode_apply, APP_main
 *   prototype: undefined8 board_detect_variant(void)
 */
/* One-shot board detection reading GPIOB IDR bit3 into DAT_20003100 (board-ID/variant), returns the
   bit */

undefined8 _uncertain_board_detect_variant(void)

{
  uint uVar1;
  int iVar2;
  undefined4 in_r3;
  undefined4 local_10;
  
  local_10 = in_r3;
  if (board_detect_pending == '\x01') {
    board_detect_pending = '\0';
    local_10 = 0x48010008;
    GPIO_Init(&GPIOB_CRL,&local_10);
    iVar2 = check_app_valid();
    if (iVar2 == 1) {
      board_id_bit = 0;
    }
    else {
      uVar1 = GPIOB_IDR;
      board_id_bit = (byte)(uVar1 >> 3) & 1;
    }
  }
  return CONCAT44(local_10,(uint)board_id_bit);
}

/**
 * _uncertain_stlink_target_mode_apply
 *   Apply target connect/disconnect mode: enable AFIO, (dis)connect NRST/SWD IO on GPIOA/B, drive LED, manage active flag DAT_20003101
 *   address 0x0800C35C, 232 bytes
 *   called from: app_hw_init, _uncertain_target_io_reinit
 *   prototype: undefined8 stlink_target_mode_apply(undefined4,undefined4,undefined4,undefined4)
 */
/* Apply target connect/disconnect mode: enable AFIO, (dis)connect NRST/SWD IO on GPIOA/B, drive
   LED, manage active flag DAT_20003101 */

undefined8
_uncertain_stlink_target_mode_apply
          (undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined2 local_18;
  undefined1 local_16;
  undefined1 local_15;
  undefined2 local_14;
  undefined1 local_12;
  undefined1 local_11;
  
  local_14 = (undefined2)param_4;
  local_12 = (undefined1)((uint)param_4 >> 0x10);
  local_11 = (undefined1)((uint)param_4 >> 0x18);
  local_18 = (undefined2)param_3;
  local_16 = (undefined1)((uint)param_3 >> 0x10);
  local_15 = (undefined1)((uint)param_3 >> 0x18);
  RCC_APB2PeriphClockCmd(1,1);
  GPIO_PinRemapConfig(0x300200,1);
  gpio_pb0_init();
  _uncertain_board_detect_variant();
  iVar1 = select_vector_table();
  if (iVar1 != 1) {
    iVar1 = _uncertain_board_detect_variant();
    if (iVar1 == 0) {
      GPIO_WriteBit(&GPIOB_CRL,0x8000,1);
      local_11 = 0x14;
      local_14 = 0x8000;
      local_12 = 1;
      GPIO_Init(&GPIOB_CRL,&local_14);
      target_drive_nrst(0);
      local_18 = 0x100;
      local_15 = 4;
      local_16 = 2;
      GPIO_Init(&GPIOA_CRL,&local_18);
      GPIO_WriteBit(&GPIOB_CRL,0x8000,1);
      target_mode_active = '\0';
    }
    else {
      GPIO_WriteBit(&GPIOB_CRL,0x8000,0);
      local_11 = 0x14;
      local_14 = 0x8000;
      local_12 = 1;
      GPIO_Init(&GPIOB_CRL,&local_14);
      if (target_mode_active == '\0') {
        GPIO_WriteBit(&GPIOB_CRL,0x8000,0);
        RCC_MCOConfig(stlink_current_mode);
        local_18 = 0x100;
        local_15 = 0x18;
        local_16 = 2;
        GPIO_Init(&GPIOA_CRL,&local_18);
        timer_delay_ticks(0x32);
        target_drive_nrst(1);
        target_mode_active = '\x01';
      }
    }
  }
  return CONCAT17(local_11,CONCAT16(local_12,CONCAT24(local_14,CONCAT13(local_15,CONCAT12(local_16,
                                                  local_18)))));
}

