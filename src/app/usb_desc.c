/*
 * app/usb_desc.c - USB descriptors
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * usb_desc_build_serial
 *   Builds the USB serial-number string descriptor (and board-id ASCII) from the 96-bit STM32 Unique Device ID
 *   address 0x0800DDA0, 194 bytes
 *   called from: usb_desc_init
 *   prototype: void usb_desc_build_serial(int ascii_mode)
 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Builds the USB serial-number string descriptor (and board-id ASCII) from the 96-bit STM32 Unique
   Device ID */

void usb_desc_build_serial(int param_1)

{
  int iVar1;
  char *pcVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  char cVar6;
  uint uVar7;
  
  if (param_1 == 0) {
    if (_Unique_device_ID_31_0 != 0) {
      usb_serial_string = (char)_Unique_device_ID_31_0;
      DAT_2000141c = (char)(_Unique_device_ID_31_0 >> 8);
      DAT_2000142a = (char)Unique_device_ID_95_64;
      DAT_20001420 = (char)(_Unique_device_ID_31_0 >> 0x18);
      DAT_2000141e = (char)(_Unique_device_ID_31_0 >> 0x10);
      DAT_20001424 = (char)(_Unique_device_ID_63_32 >> 8);
      DAT_20001422 = (char)_Unique_device_ID_63_32;
      DAT_20001426 = (char)(_Unique_device_ID_63_32 >> 0x10);
      DAT_20001428 = (char)(_Unique_device_ID_63_32 >> 0x18);
      DAT_2000142c = (char)(Unique_device_ID_95_64 >> 8);
      DAT_2000142e = (char)(Unique_device_ID_95_64 >> 0x10);
      DAT_20001430 = (char)(Unique_device_ID_95_64 >> 0x18);
      DAT_20001418 = 0x1a;
      DAT_20001490 = 0x1a;
      return;
    }
  }
  else {
    iVar1 = 0;
    pcVar2 = &usb_serial_string;
    uVar3 = 0;
    do {
      uVar5 = 0xf0000000 >> (uVar3 & 0xff);
      uVar4 = iVar1 * -4 + 0x1c;
      uVar7 = (uVar5 & _Unique_device_ID_31_0) >> (uVar4 & 0xff);
      if (uVar7 < 10) {
        cVar6 = (char)uVar7 + '0';
      }
      else if (uVar7 < 0x10) {
        cVar6 = (char)uVar7 + '7';
      }
      else {
        cVar6 = 'F';
      }
      *pcVar2 = cVar6;
      uVar7 = (uVar5 & _Unique_device_ID_63_32) >> (uVar4 & 0xff);
      if (uVar7 < 10) {
        cVar6 = (char)uVar7 + '0';
      }
      else if (uVar7 < 0x10) {
        cVar6 = (char)uVar7 + '7';
      }
      else {
        cVar6 = 'F';
      }
      pcVar2[0x10] = cVar6;
      uVar4 = (Unique_device_ID_95_64 & uVar5) >> (uVar4 & 0xff);
      if (uVar4 < 10) {
        cVar6 = (char)uVar4 + '0';
      }
      else if (uVar4 < 0x10) {
        cVar6 = (char)uVar4 + '7';
      }
      else {
        cVar6 = 'F';
      }
      pcVar2[0x20] = cVar6;
      iVar1 = iVar1 + 1;
      uVar3 = uVar3 + 4;
      pcVar2 = pcVar2 + 2;
    } while (iVar1 < 8);
  }
  return;
}

/**
 * usb_desc_init
 *   Initialises descriptor RAM fields (bcdUSB/VID bytes/config/string lengths) and fills serial from UID
 *   address 0x0800DE62, 72 bytes
 *   called from: APP_main
 *   prototype: void usb_desc_init(void)
 */
/* Initialises descriptor RAM fields (bcdUSB/VID bytes/config/string lengths) and fills serial from
   UID */

void usb_desc_init(void)

{
  int iVar1;
  
  DAT_200014e0 = 0;
  DAT_200014e2 = 0;
  DAT_200014e6 = 0x48;
  DAT_200014e7 = 0x37;
  DAT_20001450 = 1;
  DAT_20001467 = 2;
  DAT_2000146e = 0x83;
  DAT_20001470 = 0x40;
  iVar1 = select_vector_table();
  usb_desc_build_serial(iVar1 != 1);
  DAT_2000144e = 0x27;
  DAT_200014f4 = 0x27;
  DAT_20001450 = (char)DAT_20001520;
  return;
}

