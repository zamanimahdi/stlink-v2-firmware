/*
 * app/startup.c - Reset / C runtime entry
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * select_vector_table
 *   Select application vs. loader vector table by the magic 0x47A53C15 at 0x08000100; set DAT_20001504/DAT_20001508 base pointers. Returns 0 if app present.
 *   address 0x08009238, 42 bytes
 *   called from: APP_main, usb_class_reset, app_hw_init, _uncertain_stlink_target_mode_apply, usb_desc_init
 *   prototype: undefined4 select_vector_table(void)
 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Select application vs. loader vector table by the magic 0x47A53C15 at 0x08000100; set
   DAT_20001504/DAT_20001508 base pointers. Returns 0 if app present. */

undefined4 select_vector_table(void)

{
  if (_DAT_08000100 == 0x47a53c15) {
    vt_region_base_ptr = (undefined4 *)0x8000000;
    vt_app_base_ptr = (undefined4 *)&DAT_08000100;
    return 0;
  }
  vt_app_base_ptr = &DAT_0801b100;
  vt_region_base_ptr = &DAT_0801b000;
  return 1;
}

/**
 * check_app_valid
 *   Like select_vector_table but also tests a validity bit of the selected table[1]; returns 1 if the app image is marked valid.
 *   address 0x08009262, 42 bytes
 *   called from: _uncertain_board_detect_variant
 *   prototype: undefined4 check_app_valid(void)
 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Like select_vector_table but also tests a validity bit of the selected table[1]; returns 1 if the
   app image is marked valid. */

undefined4 check_app_valid(void)

{
  vt_app_base_ptr = (undefined4 *)&DAT_08000100;
  vt_region_base_ptr = (undefined4 *)0x8000000;
  if (_DAT_08000100 != 0x47a53c15) {
    vt_app_base_ptr = &DAT_0801b100;
    vt_region_base_ptr = &DAT_0801b000;
  }
  if ((int)(vt_app_base_ptr[1] << 0x1f) < 0) {
    return 1;
  }
  return 0;
}

/**
 * app_hw_init
 *   Top-level app hardware init (called from Reset_Handler): USB off, clock tree (sub_0800C10C), GPIO, mode/LED setup, USB+NVIC init
 *   address 0x0800C048, 92 bytes
 *   called from: APP_Reset_Handler
 *   prototype: undefined8 app_hw_init(void)
 */
/* Top-level app hardware init (called from Reset_Handler): USB off, clock tree (sub_0800C10C),
   GPIO, mode/LED setup, USB+NVIC init */

undefined8 app_hw_init(void)

{
  int iVar1;
  undefined2 local_10;
  undefined1 local_e;
  undefined1 local_d;
  undefined4 uStack_c;
  
  board_id_bit = 0;
  target_mode_active = 0;
  board_detect_pending = 1;
  iVar1 = select_vector_table();
  if (iVar1 == 0) {
    USB_CNTR = 1;
    rcc_set_system_clock();
    GPIO_SetBits(&GPIOA_CRL,0x8000);
    local_10 = 0x8000;
    local_d = 0x14;
    local_e = 1;
    GPIO_Init(&GPIOA_CRL,&local_10);
    _uncertain_stlink_target_mode_apply();
    led_tim1_init();
    usb_interrupt_config();
  }
  return CONCAT44(uStack_c,CONCAT13(local_d,CONCAT12(local_e,local_10)));
}

/**
 * APP_main
 *   Application main: init USB/clocks, anti-clone check, descriptor+USB device init, then main loop running the current SWIM/debug task pointer (DAT_2000121c) keyed off device state DAT_20001528
 *   address 0x0800DF2C, 274 bytes
 *   called from: crt___rt_entry
 *   prototype: void APP_main(void)
 */
/* WARNING: Removing unreachable block (ram,0x0800df5c) */
/* Application main: init USB/clocks, anti-clone check, descriptor+USB device init, then main loop
   running the current SWIM/debug task pointer (DAT_2000121c) keyed off device state DAT_20001528 */

void APP_main(void)

{
  byte bVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  undefined4 uVar7;
  byte bVar8;
  bool bVar9;
  
  bVar8 = 0;
  uVar2 = RTC_GetCounter();
  led_set_mode(4,2000,0);
  iVar3 = select_vector_table();
  if (iVar3 == 0) {
    usb_hw_power_off();
    timer_delay_ticks(10);
  }
  iVar3 = stub_return_neg1();
  if ((0 < iVar3) && (iVar4 = _uncertain_board_detect_variant(), iVar4 == 0)) {
    *(char *)(stlink_info_buf + 8) = (char)iVar3;
  }
  g_trace_active = 0;
  g_connect_phase = '\x01';
  stlink_active_subcmd = 0;
  adc_init_measure();
  usb_desc_init();
  USB_Init();
LAB_0800dfa2:
  if (usb_mode_sel != 2 && usb_mode_sel != 1) goto LAB_0800e032;
  uVar7 = 500;
  uVar5 = 4;
  bVar8 = usb_mode_sel;
  do {
    led_set_mode(uVar5,uVar7,0);
LAB_0800dfd0:
    do {
      if (bVar8 < 2) {
        uVar5 = RTC_GetCounter();
        uVar6 = util_time_diff(uVar2,uVar5);
        if ((2000 < uVar6) && (usb_mode_sel != 2 && usb_mode_sel != 4)) {
          usb_mode_sel = 3;
        }
      }
      else if (bVar8 == 4) {
        if (g_connect_phase == '\x03') {
          set_basepri(8);
          (*swim_state_fn)();
          set_basepri(0);
        }
        if (DAT_200030ad == '\x01') {
          DAT_200030ad = '\0';
          stlink_cmd_process();
          anticlone_background_write();
        }
      }
      if (bVar8 == 0) goto LAB_0800dfa2;
      if (bVar8 != 3) {
LAB_0800e032:
        bVar1 = usb_mode_sel;
        if (usb_mode_sel == 3) {
          _uncertain_stlink_mode_enter(1);
          bVar8 = bVar1;
          goto LAB_0800dfd0;
        }
        bVar9 = bVar8 == 4;
        bVar8 = bVar1;
        if (bVar9) goto LAB_0800dfd0;
      }
      bVar8 = usb_mode_sel;
    } while (usb_mode_sel != 4);
    anticlone_verify_signature();
    _uncertain_stlink_mode_enter(1);
    uVar7 = 0;
    uVar5 = 1;
  } while( true );
}

/**
 * APP_Reset_Handler
 *   Application reset handler: runs scatterload/__main (sub_0800C048) then __rt_entry, then spins
 *   address 0x0800E434, 8 bytes
 *   prototype: void APP_Reset_Handler(void)
 */
/* Application reset handler: runs scatterload/__main (sub_0800C048) then __rt_entry, then spins */

void APP_Reset_Handler(void)

{
  app_hw_init();
  crt___rt_entry();
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

