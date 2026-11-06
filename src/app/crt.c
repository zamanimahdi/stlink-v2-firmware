/*
 * app/crt.c - Keil ARMCC C library helpers
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * set_msp_if_privileged
 *   If the core is in a privileged mode, set the Main Stack Pointer to param_1.
 *   address 0x0800922C, 6 bytes
 *   called from: stlink_cmd_process
 *   prototype: void set_msp_if_privileged(undefined4 sp)
 */
/* If the core is in a privileged mode, set the Main Stack Pointer to param_1. */

void set_msp_if_privileged(undefined4 param_1)

{
  bool bVar1;
  
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    setMainStackPointer(param_1);
  }
  return;
}

/**
 * __scatterload_zeroinit
 *   ARM C runtime scatter-load ZI handler: walk {size,addr} table entries and zero-fill each region.
 *   address 0x08009416, 34 bytes
 *   called from: __rt_lib_init
 *   prototype: void __scatterload_zeroinit(int *table)
 */
/* ARM C runtime scatter-load ZI handler: walk {size,addr} table entries and zero-fill each region.
    */

void __scatterload_zeroinit(int *param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  int unaff_r9;
  
  while( true ) {
    piVar1 = param_1 + 1;
    iVar2 = *param_1;
    if (iVar2 == 0) break;
    param_1 = param_1 + 2;
    puVar3 = (undefined4 *)*piVar1;
    if ((int)puVar3 << 0x1f < 0) {
      puVar3 = (undefined4 *)(unaff_r9 + -1 + (int)puVar3);
    }
    do {
      iVar2 = iVar2 + -4;
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    } while (iVar2 != 0);
  }
  return;
}

/**
 * __aeabi_memclr
 *   Zero-fill param_2 bytes ending at param_1+param_2 with word/halfword/byte alignment handling (runtime memclr).
 *   address 0x0800962C, 6 bytes
 *   called from: stlink_cmd_get_caps, usart_bridge_init, stlink_cmd_get_version, stlink_cmd_get_rw_status, stlink_dispatch_command, stlink_send_reply, stlink_cmd_get_param_block
 *   prototype: undefined4 *__aeabi_memclr(int dst, uint n)
 */
/* Zero-fill param_2 bytes ending at param_1+param_2 with word/halfword/byte alignment handling
   (runtime memclr). */

undefined4 * __aeabi_memclr(int param_1,uint param_2)

{
  undefined4 *puVar1;
  uint uVar2;
  uint uVar3;
  
  puVar1 = (undefined4 *)(param_1 + param_2);
  uVar3 = (uint)puVar1 & 3;
  if (uVar3 != 0) {
    uVar2 = param_2 - uVar3;
    if (param_2 < uVar3) {
      if (uVar2 + uVar3 != 0) {
        puVar1 = (undefined4 *)((int)puVar1 + -1);
        *(undefined1 *)puVar1 = 0;
      }
      if ((uVar2 + uVar3 & 2) != 0) {
        puVar1 = (undefined4 *)((int)puVar1 + -1);
        *(undefined1 *)puVar1 = 0;
      }
      return puVar1;
    }
    uVar3 = (uint)puVar1 & 2;
    if ((int)puVar1 * -0x80000000 < 0) {
      puVar1 = (undefined4 *)((int)puVar1 + -1);
      *(undefined1 *)puVar1 = 0;
    }
    param_2 = uVar2;
    if (uVar3 != 0) {
      puVar1 = (undefined4 *)((int)puVar1 + -2);
      *(undefined2 *)puVar1 = 0;
    }
  }
  do {
    uVar3 = param_2;
    param_2 = uVar3 - 0x10;
    if (0xf < uVar3) {
      puVar1[-1] = 0;
      puVar1[-2] = 0;
      puVar1[-3] = 0;
      puVar1 = puVar1 + -4;
      *puVar1 = 0;
    }
  } while (0xf < uVar3 && param_2 != 0);
  if ((param_2 & 8) != 0) {
    puVar1[-1] = 0;
    puVar1 = puVar1 + -2;
    *puVar1 = 0;
  }
  if ((int)(uVar3 * 0x20000000) < 0) {
    puVar1 = puVar1 + -1;
    *puVar1 = 0;
  }
  if ((uVar3 * 0x20000000 & 0x40000000) != 0) {
    puVar1 = (undefined4 *)((int)puVar1 + -2);
    *(undefined2 *)puVar1 = 0;
  }
  if ((int)(uVar3 * -0x80000000) < 0) {
    puVar1 = (undefined4 *)((int)puVar1 + -1);
    *(undefined1 *)puVar1 = 0;
  }
  return puVar1;
}

/**
 * __aeabi_memcpy
 *   Keil ARMCC optimized word-aligned memcpy runtime helper (handles misaligned head/tail, 16/8-byte unrolled body).
 *   address 0x0800991C, 118 bytes
 *   called from: stlink_cmd_get_caps, stlink_dispatch_command, stlink_cmd_rw_dap_reg, anticlone_verify_signature, stlink_cmd_read_trace_buf, stlink_cmd_get_version, stlink_cmd_get_fw_build, stlink_cmd_get_param_block ...
 *   prototype: void *__aeabi_memcpy(void *dst, const void *src, uint n)
 */
/* Keil ARMCC optimized word-aligned memcpy runtime helper (handles misaligned head/tail, 16/8-byte
   unrolled body). */

void __aeabi_memcpy(undefined4 *param_1,undefined4 *param_2,uint param_3)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  uint uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  bool bVar10;
  
  while( true ) {
    if (param_3 == 0) {
      return;
    }
    if (((uint)param_2 & 3) == 0) break;
    param_3 = param_3 - 1;
    *(undefined1 *)param_1 = *(undefined1 *)param_2;
    param_2 = (undefined4 *)((int)param_2 + 1);
    param_1 = (undefined4 *)((int)param_1 + 1);
  }
  if (((uint)param_1 & 3) == 0) {
    while (uVar5 = param_3 - 0x10, 0xf < param_3) {
      uVar6 = *param_2;
      uVar7 = param_2[1];
      uVar8 = param_2[2];
      uVar9 = param_2[3];
      param_2 = param_2 + 4;
      *param_1 = uVar6;
      param_1[1] = uVar7;
      param_1[2] = uVar8;
      param_1[3] = uVar9;
      param_1 = param_1 + 4;
      param_3 = uVar5;
    }
    bVar10 = (int)(param_3 << 0x1d) < 0;
    if ((uVar5 & 8) != 0) {
      uVar6 = *param_2;
      uVar7 = param_2[1];
      param_2 = param_2 + 2;
      *param_1 = uVar6;
      param_1[1] = uVar7;
      param_1 = param_1 + 2;
    }
  }
  else {
    while (uVar5 = param_3 - 8, 7 < param_3) {
      uVar6 = *param_2;
      uVar7 = param_2[1];
      param_2 = param_2 + 2;
      puVar3 = param_1 + 1;
      *param_1 = uVar6;
      param_1 = param_1 + 2;
      *puVar3 = uVar7;
      param_3 = uVar5;
    }
    bVar10 = (int)(param_3 << 0x1d) < 0;
  }
  puVar1 = param_1;
  puVar3 = param_2;
  if (bVar10) {
    puVar3 = param_2 + 1;
    puVar1 = param_1 + 1;
    *param_1 = *param_2;
  }
  puVar2 = puVar1;
  puVar4 = puVar3;
  if ((uVar5 & 2) != 0) {
    puVar4 = (undefined4 *)((int)puVar3 + 2);
    puVar2 = (undefined4 *)((int)puVar1 + 2);
    *(undefined2 *)puVar1 = *(undefined2 *)puVar3;
  }
  if ((int)(uVar5 << 0x1f) < 0) {
    *(undefined1 *)puVar2 = *(undefined1 *)puVar4;
  }
  return;
}

/**
 * __aeabi_memset
 *   Keil ARMCC memset/fill runtime helper: fills n bytes ending at dst+len with byte value, writing backwards with word/halfword unrolling.
 *   address 0x08009994, 102 bytes
 *   called from: stlink_dispatch_command, stlink_cmd_get_fw_build, ap_tables_reset
 *   prototype: void *__aeabi_memset(int dst, uint len, uint c)
 */
/* Keil ARMCC memset/fill runtime helper: fills n bytes ending at dst+len with byte value, writing
   backwards with word/halfword unrolling. */

uint * __aeabi_memset(int param_1,uint param_2,uint param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  uVar5 = (param_3 & 0xff) << 8;
  uVar4 = param_3 & 0xff | uVar5;
  uVar5 = uVar4 | (param_3 & 0xffff00ff | uVar5) << 0x10;
  puVar3 = (uint *)(param_1 + param_2);
  uVar6 = (uint)puVar3 & 3;
  uVar1 = (undefined1)param_3;
  uVar2 = (undefined2)uVar4;
  if (uVar6 != 0) {
    uVar4 = param_2 - uVar6;
    if (param_2 < uVar6) {
      if (uVar4 + uVar6 != 0) {
        puVar3 = (uint *)((int)puVar3 + -1);
        *(undefined1 *)puVar3 = uVar1;
      }
      if ((uVar4 + uVar6 & 2) != 0) {
        puVar3 = (uint *)((int)puVar3 + -1);
        *(undefined1 *)puVar3 = uVar1;
      }
      return puVar3;
    }
    uVar6 = (uint)puVar3 & 2;
    if ((int)puVar3 * -0x80000000 < 0) {
      puVar3 = (uint *)((int)puVar3 + -1);
      *(undefined1 *)puVar3 = uVar1;
    }
    param_2 = uVar4;
    if (uVar6 != 0) {
      puVar3 = (uint *)((int)puVar3 + -2);
      *(undefined2 *)puVar3 = uVar2;
    }
  }
  do {
    uVar4 = param_2;
    param_2 = uVar4 - 0x10;
    if (0xf < uVar4) {
      puVar3[-1] = uVar5;
      puVar3[-2] = uVar5;
      puVar3[-3] = uVar5;
      puVar3 = puVar3 + -4;
      *puVar3 = uVar5;
    }
  } while (0xf < uVar4 && param_2 != 0);
  if ((param_2 & 8) != 0) {
    puVar3[-1] = uVar5;
    puVar3 = puVar3 + -2;
    *puVar3 = uVar5;
  }
  if ((int)(uVar4 * 0x20000000) < 0) {
    puVar3 = puVar3 + -1;
    *puVar3 = uVar5;
  }
  if ((uVar4 * 0x20000000 & 0x40000000) != 0) {
    puVar3 = (uint *)((int)puVar3 + -2);
    *(undefined2 *)puVar3 = uVar2;
  }
  if ((int)(uVar4 * -0x80000000) < 0) {
    puVar3 = (uint *)((int)puVar3 + -1);
    *(undefined1 *)puVar3 = uVar1;
  }
  return puVar3;
}

/**
 * crt_decompress_read_byte
 *   ARMCC __decompress helper: returns next byte of the compressed input stream, advancing the run/literal cursor
 *   address 0x0800E268, 34 bytes
 *   called from: __decompress
 *   prototype: uint8_t crt_decompress_read_byte(uint32_t *state)
 */
/* ARMCC __decompress helper: returns next byte of the compressed input stream, advancing the
   run/literal cursor */

undefined1 crt_decompress_read_byte(undefined4 *param_1)

{
  undefined1 *puVar1;
  int *piVar2;
  
  puVar1 = (undefined1 *)param_1[2];
  while (puVar1 == (undefined1 *)param_1[3]) {
    piVar2 = (int *)*param_1;
    puVar1 = (undefined1 *)(*piVar2 + (int)piVar2);
    param_1[3] = puVar1 + piVar2[1];
    *param_1 = piVar2 + 2;
  }
  param_1[2] = puVar1 + 1;
  return *puVar1;
}

/**
 * __decompress
 *   ARMCC runtime LZ77-style region decompressor used by scatterloading (RW data init)
 *   address 0x0800E28A, 176 bytes
 *   prototype: void *__decompress(uint32_t *src)
 */
/* ARMCC runtime LZ77-style region decompressor used by scatterloading (RW data init) */

uint * __decompress(uint *param_1)

{
  uint *puVar1;
  undefined1 uVar2;
  char cVar3;
  uint uVar4;
  undefined1 *puVar5;
  uint *puVar6;
  undefined1 *puVar7;
  undefined1 *puVar8;
  undefined1 *puVar9;
  int unaff_r9;
  int iVar10;
  uint *local_30;
  uint *local_2c;
  undefined1 *local_28;
  undefined1 *local_24;
  
  local_30 = param_1 + 2;
  puVar7 = (undefined1 *)0x0;
  local_2c = local_30 + (*param_1 & 0xfffffffe);
  puVar9 = (undefined1 *)0x0;
  local_28 = (undefined1 *)0x0;
  local_24 = (undefined1 *)0x0;
  uVar4 = param_1[1];
  param_1 = param_1 + (*param_1 & 0xfffffffe) + 2;
  puVar6 = param_1 + (uVar4 & 0xfffffffe);
  do {
    if (param_1 == puVar6) {
      return puVar6;
    }
    do {
      cVar3 = crt_decompress_read_byte(&local_30);
      iVar10 = (int)cVar3;
      do {
        while (local_28 == local_24) {
          local_28 = (undefined1 *)(*local_30 + (int)local_30);
          puVar1 = local_30 + 1;
          local_30 = local_30 + 2;
          local_24 = local_28 + *puVar1;
        }
        uVar2 = *local_28;
        puVar5 = local_28;
        puVar8 = puVar7;
        local_28 = local_28 + 1;
        while( true ) {
          if (puVar8 == puVar9) {
            do {
              if (param_1 == puVar6) {
                crt_abort(puVar5);
              }
              puVar8 = (undefined1 *)*param_1;
              if ((uVar4 & 1) != 0) {
                puVar8 = puVar8 + unaff_r9;
              }
              puVar5 = (undefined1 *)param_1[1];
              puVar9 = puVar5 + (int)puVar8;
              param_1 = param_1 + 2;
            } while (puVar8 == puVar9);
          }
          puVar7 = puVar8 + 1;
          *puVar8 = uVar2;
          if (-1 < iVar10) break;
          iVar10 = iVar10 + 1;
          puVar8 = puVar7;
          if (iVar10 == 0) goto LAB_0800e322;
        }
        iVar10 = iVar10 + -1;
      } while (-1 < iVar10);
LAB_0800e322:
    } while (puVar7 != puVar9);
  } while( true );
}

/**
 * crt_abort
 *   Fatal-error stub (decompress overrun): tail-calls the semihosting exit
 *   address 0x0800E33A, 6 bytes
 *   called from: __decompress
 *   prototype: void crt_abort(void)
 */
/* Fatal-error stub (decompress overrun): tail-calls the semihosting exit */

void crt_abort(void)

{
  _sys_exit(1);
  return;
}

/**
 * _sys_exit
 *   Semihosting _sys_exit: loops issuing BKPT 0xAB (never returns)
 *   address 0x0800E340, 16 bytes
 *   called from: crt_abort, _uncertain_crt_exit_alt
 *   prototype: void _sys_exit(int code)
 */
/* Semihosting _sys_exit: loops issuing BKPT 0xAB (never returns) */

void _sys_exit(void)

{
  do {
    software_bkpt(0xab);
  } while( true );
}

/**
 * __rt_lib_init
 *   ARMCC __rt_lib_init: walks the library-init table, each entry (base+off)(base+1) returning the next entry
 *   address 0x0800E354, 32 bytes
 *   called from: crt___rt_entry
 *   prototype: void __rt_lib_init(void)
 */
/* ARMCC __rt_lib_init: walks the library-init table, each entry (base+off)(base+1) returning the
   next entry */

void __rt_lib_init(void)

{
  int *piVar1;
  
  for (piVar1 = &DAT_0800e37c; piVar1 != (int *)&UNK_0800e3ac;
      piVar1 = (int *)(*(code *)(*piVar1 + (int)piVar1))(piVar1 + 1)) {
  }
  return;
}

/**
 * _uncertain_crt_rt_lib_shutdown
 *   Likely ARMCC __rt_lib_shutdown: invokes the fini/exit-function list via crt_run_fnlist
 *   address 0x0800E3CC, 24 bytes
 *   prototype: uint32_t crt_rt_lib_shutdown(uint32_t a)
 */
/* Likely ARMCC __rt_lib_shutdown: invokes the fini/exit-function list via crt_run_fnlist */

undefined4 _uncertain_crt_rt_lib_shutdown(undefined4 param_1)

{
  _uncertain_crt_run_fnlist(&DAT_0800e444,_uncertain_crt___rt_entry_main);
  return param_1;
}

/**
 * _uncertain_crt_run_fnlist
 *   Runs a list of PC-relative function pointers between two bounds (init/fini array runner)
 *   address 0x0800E3EC, 24 bytes
 *   called from: _uncertain_crt_rt_lib_shutdown
 *   prototype: void crt_run_fnlist(int *start, int *end)
 */
/* Runs a list of PC-relative function pointers between two bounds (init/fini array runner) */

undefined4
_uncertain_crt_run_fnlist(int *param_1,int *param_2,undefined4 param_3,undefined4 param_4)

{
  for (; param_1 != param_2; param_1 = param_1 + 1) {
    (*(code *)((int)param_1 + *param_1))();
  }
  return param_4;
}

/**
 * crt___rt_entry
 *   ARMCC __rt_entry body: runs __rt_lib_init, calls APP_main(0), then exits
 *   address 0x0800E404, 26 bytes
 *   called from: _uncertain_crt___rt_entry_main
 *   prototype: int crt___rt_entry(void)
 */
/* ARMCC __rt_entry body: runs __rt_lib_init, calls APP_main(0), then exits */

undefined4 crt___rt_entry(void)

{
  int iVar1;
  
  iVar1 = _uncertain_crt_rt_lib_init_guard();
  if (iVar1 != 0) {
    __rt_lib_init();
  }
  APP_main(0);
  crt_exit();
  return 1;
}

/**
 * _uncertain_crt_rt_lib_init_guard
 *   Returns 1; gate that always forces __rt_lib_init to run (stack/heap init stub)
 *   address 0x0800E41E, 4 bytes
 *   called from: crt___rt_entry
 *   prototype: int crt_rt_lib_init_guard(void)
 */
/* Returns 1; gate that always forces __rt_lib_init to run (stack/heap init stub) */

undefined4 _uncertain_crt_rt_lib_init_guard(void)

{
  return 1;
}

/**
 * crt_exit
 *   Program exit: infinite loop calling _sys_exit
 *   address 0x0800E422, 4 bytes
 *   called from: crt___rt_entry
 *   prototype: void crt_exit(int code)
 */
/* Program exit: infinite loop calling _sys_exit */

void crt_exit(undefined4 param_1)

{
  do {
    _sys_exit(param_1);
  } while( true );
}

/**
 * _uncertain_crt_exit_alt
 *   Alternate exit/abort path: infinite loop calling _sys_exit
 *   address 0x0800E428, 10 bytes
 *   prototype: void crt_exit_alt(int code)
 */
/* Alternate exit/abort path: infinite loop calling _sys_exit */

void _uncertain_crt_exit_alt(undefined4 param_1)

{
  do {
    _sys_exit(param_1);
  } while( true );
}

/**
 * _uncertain_crt___rt_entry_main
 *   Calls crt___rt_entry then spins; registered as the entry/exit callback
 *   address 0x0800E454, 12 bytes
 *   prototype: void crt___rt_entry_main(void)
 */
/* Calls crt___rt_entry then spins; registered as the entry/exit callback */

void _uncertain_crt___rt_entry_main(void)

{
  crt___rt_entry();
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

