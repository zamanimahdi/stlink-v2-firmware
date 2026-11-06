/*
 * app/aes.c - AES-128 encryption
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * anticlone_is_active
 *   Return the anti-clone active flag DAT_200030B0.
 *   address 0x08008CFC, 6 bytes
 *   called from: target_connect
 *   prototype: undefined1 anticlone_is_active(void)
 */
/* Return the anti-clone active flag DAT_200030B0. */

undefined1 anticlone_is_active(void)

{
  return anticlone_active;
}

/**
 * anticlone_verify_signature
 *   Anti-clone check: AES-128 encrypt {flash size + chip UID} with key "What are you doi" and compare to the 16-byte signature at 0x08003C00; set DAT_200030B1 on mismatch.
 *   address 0x08008D02, 160 bytes
 *   called from: APP_main, anticlone_init
 *   prototype: void anticlone_verify_signature(void)
 */
/* Anti-clone check: AES-128 encrypt {flash size + chip UID} with key "What are you doi" and compare
   to the 16-byte signature at 0x08003C00; set DAT_200030B1 on mismatch. */

void anticlone_verify_signature(void)

{
  undefined1 *puVar1;
  int iVar2;
  char *pcVar3;
  char *pcVar4;
  char *pcVar5;
  undefined1 local_f0;
  undefined1 local_ef;
  undefined1 local_ee;
  undefined1 local_ed;
  undefined1 auStack_ec [12];
  char local_e0 [16];
  undefined1 auStack_d0 [176];
  undefined1 auStack_20 [24];
  
  __aeabi_memcpy(auStack_20,"What are you doi",0x11);
  aes_buffer_clear(auStack_d0,0xb0);
  local_f0 = FLASH_SIZE;
  local_ef = DAT_1ffff7e1;
  local_ee = DAT_1ffff7e2;
  local_ed = DAT_1ffff7e3;
  iVar2 = 4;
  puVar1 = &local_f0;
  do {
    puVar1[4] = (&DAT_1ffff7e4)[iVar2];
    puVar1[5] = *(undefined1 *)(iVar2 + 0x1ffff7e5);
    puVar1[6] = *(undefined1 *)(iVar2 + 0x1ffff7e6);
    puVar1[7] = *(undefined1 *)(iVar2 + 0x1ffff7e7);
    iVar2 = iVar2 + 4;
    puVar1 = puVar1 + 4;
  } while (iVar2 < 0x10);
  aes_key_expansion(auStack_20,auStack_d0);
  aes_encrypt_block(&local_f0,local_e0,auStack_d0);
  pcVar3 = &DAT_08003c00;
  iVar2 = 0;
  pcVar5 = local_e0;
  do {
    pcVar4 = pcVar3 + 1;
    if (*pcVar5 != *pcVar3) {
      g_trace_active = 1;
    }
    pcVar3 = pcVar3 + 2;
    if (pcVar5[1] != *pcVar4) {
      g_trace_active = 1;
    }
    iVar2 = iVar2 + 2;
    pcVar5 = pcVar5 + 2;
  } while (iVar2 < 0x10);
  return;
}

/**
 * anticlone_reflash_signature
 *   (uncertain) Unlock flash, erase the signature page at 0x08003C00 and reprogram it plus ~0xF7 words copied from 0x0801F824 (brick/restore of the anti-clone block).
 *   address 0x08008DA2, 126 bytes
 *   called from: target_scan_rom_table
 */
/* (uncertain) Unlock flash, erase the signature page at 0x08003C00 and reprogram it plus ~0xF7
   words copied from 0x0801F824 (brick/restore of the anti-clone block). */

void anticlone_reflash_signature(void)

{
  undefined4 *puVar1;
  int iVar2;
  
  FLASH_Unlock();
  FLASH_ErasePage(&DAT_08003c00);
  FLASH_ProgramWord(&DAT_08003c20,0xffffff4f);
  FLASH_ProgramWord(&DAT_08003c00,0xc2b92595);
  FLASH_ProgramWord(&DAT_08003c04,0xc51411d7);
  FLASH_ProgramWord(&DAT_08003c08,0x686df7c2);
  FLASH_ProgramWord(&DAT_08003c0c,0x5b58012b);
  FLASH_ProgramWord(&DAT_08003c10,0xffffffff);
  FLASH_ProgramWord(&DAT_08003c14,0xffffffff);
  FLASH_ProgramWord(&DAT_08003c18,0xffffffff);
  FLASH_ProgramWord(&DAT_08003c1c,0xffffffff);
  puVar1 = &DAT_0801f824;
  iVar2 = 0xf7;
  do {
    FLASH_ProgramWord(puVar1 + -0x6f00,*puVar1);
    puVar1 = puVar1 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}

/**
 * anticlone_start
 *   Begin anti-clone monitoring: clear counters (DAT_200030B2/B4), set active flag DAT_200030B0=1 and snapshot the timer (DAT_200030B8).
 *   address 0x08008E20, 66 bytes
 *   called from: target_scan_rom_table
 */
/* WARNING: Removing unreachable block (ram,0x08008e30) */
/* WARNING: Removing unreachable block (ram,0x08008e38) */
/* WARNING: Removing unreachable block (ram,0x08008e3e) */
/* WARNING: Removing unreachable block (ram,0x08008e4e) */
/* WARNING: Removing unreachable block (ram,0x08008e46) */
/* Begin anti-clone monitoring: clear counters (DAT_200030B2/B4), set active flag DAT_200030B0=1 and
   snapshot the timer (DAT_200030B8). */

void anticlone_start(void)

{
  anticlone_word_index = 0;
  anticlone_page_counter = 0;
  anticlone_active = 1;
  anticlone_timestamp = RTC_GetCounter();
  return;
}

/**
 * anticlone_init
 *   Reset anti-clone state (DAT_200030B0=0, DAT_200030B1=0) and run the signature verification.
 *   address 0x08008E62, 10 bytes
 *   called from: target_connect, target_scan_rom_table
 */
/* Reset anti-clone state (DAT_200030B0=0, DAT_200030B1=0) and run the signature verification. */

void anticlone_init(void)

{
  anticlone_active = 0;
  g_trace_active = 0;
  anticlone_verify_signature();
  return;
}

/**
 * anticlone_background_write
 *   Anti-clone background task: while active, periodically program flash bytes into 0x0801F400 pacing by elapsed timer ticks; sets fault flag on timeout.
 *   address 0x08008E6C, 140 bytes
 *   called from: APP_main
 */
/* Anti-clone background task: while active, periodically program flash bytes into 0x0801F400 pacing
   by elapsed timer ticks; sets fault flag on timeout. */

undefined4 anticlone_background_write(void)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 in_r3;
  
  if (anticlone_active == '\x01') {
    if (anticlone_page_counter < 0xd3) {
      uVar2 = RTC_GetCounter();
      uVar1 = util_time_diff(anticlone_timestamp,uVar2);
      if (4000 < uVar1) {
        anticlone_timestamp = uVar2;
        FLASH_Unlock();
        if (0x1ff < anticlone_word_index) {
          anticlone_page_counter = anticlone_page_counter + 1;
          FLASH_ErasePage(&DAT_0801f400);
          anticlone_word_index = 0;
        }
        FLASH_ProgramHalfWord(&DAT_0801f400 + anticlone_word_index,anticlone_page_counter);
        anticlone_word_index = anticlone_word_index + 1;
      }
    }
    else {
      if (anticlone_timeout == 0) {
        anticlone_timestamp = RTC_GetCounter();
        uVar1 = RTC_GetCounter();
        anticlone_timeout = (uVar1 & 0x1fff) + 10000;
        return in_r3;
      }
      uVar2 = RTC_GetCounter();
      uVar1 = util_time_diff(anticlone_timestamp,uVar2);
      if (anticlone_timeout < uVar1) {
        g_trace_active = 1;
        return in_r3;
      }
    }
  }
  return in_r3;
}

/**
 * aes_buffer_clear
 *   Zero a byte buffer (unrolled) - used to clear AES key/state before expansion
 *   address 0x0800B90C, 56 bytes
 *   called from: anticlone_verify_signature
 *   prototype: undefined4 *aes_buffer_clear(undefined4 *buf,uint len)
 */
/* Zero a byte buffer (unrolled) - used to clear AES key/state before expansion */

undefined4 * aes_buffer_clear(undefined4 *param_1,uint param_2)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  uint uVar3;
  
  do {
    uVar3 = param_2;
    param_2 = uVar3 - 0x10;
    if (0xf < uVar3) {
      *param_1 = 0;
      param_1[1] = 0;
      param_1[2] = 0;
      param_1[3] = 0;
      param_1 = param_1 + 4;
    }
  } while (0xf < uVar3 && param_2 != 0);
  if ((param_2 & 8) != 0) {
    *param_1 = 0;
    param_1[1] = 0;
    param_1 = param_1 + 2;
  }
  puVar2 = param_1;
  if ((int)(uVar3 << 0x1d) < 0) {
    puVar2 = param_1 + 1;
    *param_1 = 0;
  }
  puVar1 = puVar2;
  if ((param_2 & 2) != 0) {
    puVar1 = (undefined4 *)((int)puVar2 + 2);
    *(undefined2 *)puVar2 = 0;
  }
  puVar2 = puVar1;
  if ((int)(uVar3 * -0x80000000) < 0) {
    puVar2 = (undefined4 *)((int)puVar1 + 1);
    *(undefined1 *)puVar1 = 0;
  }
  return puVar2;
}

/**
 * aes_key_expansion
 *   AES-128 key schedule: expand 16-byte key to round keys using S-box DAT_0800BC28 and Rcon DAT_0800BD28
 *   address 0x0800B914, 110 bytes
 *   called from: anticlone_verify_signature
 *   prototype: void aes_key_expansion(uint *key,uint *roundkeys)
 */
/* AES-128 key schedule: expand 16-byte key to round keys using S-box DAT_0800BC28 and Rcon
   DAT_0800BD28 */

void aes_key_expansion(uint *param_1,uint *param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint *puVar5;
  int iVar6;
  
  uVar4 = *param_1;
  uVar2 = param_1[1];
  uVar3 = param_1[2];
  uVar1 = param_1[3];
  *param_2 = uVar4;
  param_2[1] = uVar2;
  param_2[2] = uVar3;
  param_2[3] = uVar1;
  iVar6 = 10;
  puVar5 = &DAT_0800bd28;
  do {
    uVar4 = *puVar5 ^ ((uint)(byte)(&DAT_0800bc28)[uVar1 >> 0x18] |
                      (uint)(byte)(&DAT_0800bc28)[(uVar1 & 0xffff) >> 8] << 0x10 |
                      (uint)(byte)(&DAT_0800bc28)[(uVar1 & 0xffffff) >> 0x10] << 0x18 |
                      (uint)(byte)(&DAT_0800bc28)[uVar1 & 0xff] << 8) ^ uVar4;
    uVar2 = uVar2 ^ uVar4;
    uVar3 = uVar3 ^ uVar2;
    param_2[4] = uVar4;
    uVar1 = uVar1 ^ uVar3;
    param_2[5] = uVar2;
    param_2[6] = uVar3;
    iVar6 = iVar6 + -1;
    param_2[7] = uVar1;
    param_2 = param_2 + 4;
    puVar5 = puVar5 + 1;
  } while (iVar6 != 0);
  return;
}

/**
 * aes_encrypt_block
 *   AES-128 encrypt one 16-byte block (9 main rounds + final) using S-box DAT_0800BC28; anti-clone challenge cipher
 *   address 0x0800B982, 676 bytes
 *   called from: anticlone_verify_signature
 *   prototype: void aes_encrypt_block(uint *in,uint *out,uint *roundkeys)
 */
/* AES-128 encrypt one 16-byte block (9 main rounds + final) using S-box DAT_0800BC28; anti-clone
   challenge cipher */

void aes_encrypt_block(uint *param_1,uint *param_2,uint *param_3)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  byte bVar5;
  byte bVar6;
  byte bVar7;
  byte bVar8;
  byte bVar9;
  byte bVar10;
  byte bVar11;
  byte bVar12;
  uint *puVar13;
  uint *puVar14;
  int iVar15;
  uint uVar16;
  uint uVar17;
  uint uVar18;
  uint uVar19;
  uint uVar20;
  uint uVar21;
  uint uVar22;
  uint uVar23;
  uint uVar24;
  uint uVar25;
  uint uVar26;
  uint uVar27;
  uint uVar28;
  uint uVar29;
  uint uVar30;
  uint uVar31;
  uint uVar32;
  uint uVar33;
  uint uVar34;
  uint uVar35;
  
  uVar16 = *param_1 ^ *param_3;
  uVar20 = param_1[1] ^ param_3[1];
  uVar22 = param_1[2] ^ param_3[2];
  uVar27 = param_3[3] ^ param_1[3];
  iVar15 = 9;
  puVar13 = param_3 + 4;
  do {
    puVar14 = puVar13;
    uVar30 = (uint)(byte)(&DAT_0800bc28)[(uVar20 & 0xffffff) >> 0x10] << 0x10 |
             (uint)(byte)(&DAT_0800bc28)[uVar16 >> 0x18] << 0x18;
    uVar31 = uVar30 | (uint)(byte)(&DAT_0800bc28)[(uVar22 & 0xffff) >> 8] << 8;
    uVar32 = (byte)(&DAT_0800bc28)[uVar27 & 0xff] | uVar31;
    uVar33 = (uint)(byte)(&DAT_0800bc28)[(uVar22 & 0xffffff) >> 0x10] << 0x10 |
             (uint)(byte)(&DAT_0800bc28)[uVar20 >> 0x18] << 0x18;
    uVar34 = uVar33 | (uint)(byte)(&DAT_0800bc28)[(uVar27 & 0xffff) >> 8] << 8;
    uVar23 = uVar16 & 0xff;
    uVar35 = (byte)(&DAT_0800bc28)[uVar23] | uVar34;
    uVar24 = (uint)(byte)(&DAT_0800bc28)[(uVar27 & 0xffffff) >> 0x10] << 0x10 |
             (uint)(byte)(&DAT_0800bc28)[uVar22 >> 0x18] << 0x18;
    uVar25 = uVar24 | (uint)(byte)(&DAT_0800bc28)[(uVar16 & 0xffff) >> 8] << 8;
    uVar29 = uVar20 & 0xff;
    uVar17 = (uint)(byte)(&DAT_0800bc28)[(uVar16 & 0xffffff) >> 0x10] << 0x10 |
             (uint)(byte)(&DAT_0800bc28)[uVar27 >> 0x18] << 0x18;
    uVar18 = uVar17 | (uint)(byte)(&DAT_0800bc28)[(uVar20 & 0xffff) >> 8] << 8;
    uVar21 = uVar22 & 0xff;
    uVar26 = (byte)(&DAT_0800bc28)[uVar29] | uVar25;
    uVar28 = (byte)(&DAT_0800bc28)[uVar21] | uVar18;
    uVar19 = (uVar32 >> 7 & 0x1010101) * 0x1b;
    uVar16 = ((uVar19 ^ uVar32 ^ (uVar32 & 0x7f7f7f7f) << 1) << 8 |
             (uVar19 ^ uVar32 ^ (uint)(byte)(&DAT_0800bc28)[uVar16 >> 0x18] << 0x19) >> 0x18) ^
             uVar19 ^ (uVar32 & 0x7f7f7f7f) << 1 ^ (uVar30 >> 0x10 | uVar32 << 0x10) ^
             (uVar31 >> 8 | (uint)(byte)(&DAT_0800bc28)[uVar27 & 0xff] << 0x18) ^ *puVar14;
    uVar19 = (uVar35 >> 7 & 0x1010101) * 0x1b;
    uVar20 = ((uVar19 ^ uVar35 ^ (uVar35 & 0x7f7f7f7f) << 1) << 8 |
             (uVar19 ^ uVar35 ^ (uint)(byte)(&DAT_0800bc28)[uVar20 >> 0x18] << 0x19) >> 0x18) ^
             uVar19 ^ (uVar35 & 0x7f7f7f7f) << 1 ^ (uVar33 >> 0x10 | uVar35 << 0x10) ^
             (uVar34 >> 8 | (uint)(byte)(&DAT_0800bc28)[uVar23] << 0x18) ^ puVar14[1];
    uVar19 = (uVar26 >> 7 & 0x1010101) * 0x1b;
    uVar22 = puVar14[2] ^
             ((uVar19 ^ uVar26 ^ (uVar26 & 0x7f7f7f7f) << 1) << 8 |
             (uVar19 ^ uVar26 ^ (uint)(byte)(&DAT_0800bc28)[uVar22 >> 0x18] << 0x19) >> 0x18) ^
             uVar19 ^ (uVar26 & 0x7f7f7f7f) << 1 ^ (uVar24 >> 0x10 | uVar26 << 0x10) ^
             (uVar25 >> 8 | (uint)(byte)(&DAT_0800bc28)[uVar29] << 0x18);
    uVar19 = (uVar28 >> 7 & 0x1010101) * 0x1b;
    iVar15 = iVar15 + -1;
    uVar27 = puVar14[3] ^
             ((uVar19 ^ uVar28 ^ (uVar28 & 0x7f7f7f7f) << 1) << 8 |
             (uVar19 ^ uVar28 ^ (uint)(byte)(&DAT_0800bc28)[uVar27 >> 0x18] << 0x19) >> 0x18) ^
             uVar19 ^ (uVar28 & 0x7f7f7f7f) << 1 ^ (uVar17 >> 0x10 | uVar28 << 0x10) ^
             (uVar18 >> 8 | (uint)(byte)(&DAT_0800bc28)[uVar21] << 0x18);
    puVar13 = puVar14 + 4;
  } while (iVar15 != 0);
  bVar1 = (&DAT_0800bc28)[(uVar22 & 0xffffff) >> 0x10];
  bVar2 = (&DAT_0800bc28)[uVar20 >> 0x18];
  bVar3 = (&DAT_0800bc28)[(uVar27 & 0xffff) >> 8];
  bVar6 = (&DAT_0800bc28)[(uVar27 & 0xffffff) >> 0x10];
  bVar4 = (&DAT_0800bc28)[uVar16 & 0xff];
  uVar17 = puVar14[5];
  bVar5 = (&DAT_0800bc28)[uVar22 >> 0x18];
  bVar7 = (&DAT_0800bc28)[(uVar16 & 0xffffff) >> 0x10];
  bVar8 = (&DAT_0800bc28)[(uVar16 & 0xffff) >> 8];
  bVar9 = (&DAT_0800bc28)[uVar20 & 0xff];
  uVar18 = puVar14[6];
  bVar10 = (&DAT_0800bc28)[uVar27 >> 0x18];
  bVar11 = (&DAT_0800bc28)[(uVar20 & 0xffff) >> 8];
  bVar12 = (&DAT_0800bc28)[uVar22 & 0xff];
  uVar19 = puVar14[7];
  *param_2 = puVar14[4] ^
             ((uint)(byte)(&DAT_0800bc28)[(uVar20 & 0xffffff) >> 0x10] << 0x10 |
              (uint)(byte)(&DAT_0800bc28)[uVar16 >> 0x18] << 0x18 |
              (uint)(byte)(&DAT_0800bc28)[(uVar22 & 0xffff) >> 8] << 8 |
             (uint)(byte)(&DAT_0800bc28)[uVar27 & 0xff]);
  param_2[1] = ((uint)bVar1 << 0x10 | (uint)bVar2 << 0x18 | (uint)bVar3 << 8 | (uint)bVar4) ^ uVar17
  ;
  param_2[2] = uVar18 ^ ((uint)bVar9 | (uint)bVar6 << 0x10 | (uint)bVar5 << 0x18 | (uint)bVar8 << 8)
  ;
  param_2[3] = uVar19 ^ ((uint)bVar12 |
                        (uint)bVar7 << 0x10 | (uint)bVar10 << 0x18 | (uint)bVar11 << 8);
  return;
}

