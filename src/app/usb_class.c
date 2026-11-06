/*
 * app/usb_class.c - USB class / bulk endpoints
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * usb_class_set_configured
 *   SET_CONFIGURATION callback: clears/sets EP1, kicks class start, sets device state=CONFIGURED(4)
 *   address 0x0800D932, 26 bytes
 *   prototype: void usb_class_set_configured(void)
 */
/* SET_CONFIGURATION callback: clears/sets EP1, kicks class start, sets device state=CONFIGURED(4)
    */

void usb_class_set_configured(void)

{
  _uncertain_usb_ep_clear_ctr_tx(1);
  _uncertain_usb_ep_clear_ctr_rx(usb_ep0_idx);
  stlink_cmd_reset_status();
  usb_mode_sel = 4;
  return;
}

/**
 * usb_class_reset
 *   Device Reset callback: configures BTABLE and EP0..EP3 (type/addr/counts), sets address 0, enables USB interrupts
 *   address 0x0800D950, 214 bytes
 *   prototype: void usb_class_reset(void)
 */
/* Device Reset callback: configures BTABLE and EP0..EP3 (type/addr/counts), sets address 0, enables
   USB interrupts */

void usb_class_reset(void)

{
  int iVar1;
  
  iVar1 = select_vector_table();
  if (iVar1 == 1) {
    usb_interrupt_config();
    RCC_APB2PeriphClockCmd(0xa0c,1);
    _uncertain_target_io_reinit();
    _uncertain_target_mco_io_config();
    rtc_lse_init();
    *(undefined1 *)(usb_pInformation + 8) = 7;
    *(undefined1 *)(usb_pInformation + 10) = 1;
    *(undefined1 *)(usb_pInformation + 9) = *(undefined1 *)(stlink_info_buf + 7);
    usb_wInterrupt_Mask = 0x8400;
    usb_ep0_idx = 2;
    usb_ep_out_num = 3;
    SetEPRxAddr(0,0x20);
    SetEPRxCount(0,usb_ep0_buffer_size);
    SetEPTxAddr(0,0x60);
    SetEPTxAddr(1,0xa0);
    SetEPRxAddr(usb_ep0_idx,0xe0);
    SetEPRxCount(usb_ep0_idx,usb_ep0_buffer_size);
    SetEPTxAddr(usb_ep_out_num,0x120);
    stlink_cmd_set_status_idle(0);
    usb_mode_sel = 4;
    led_set_mode(3,0,0);
    return;
  }
  *(undefined1 *)(usb_pInformation + 10) = 0;
  usb_ep0_idx = 2;
  usb_ep_out_num = 3;
  usb_hw_power_on();
  USB_ISTR = 0;
  usb_wInterrupt_Mask = 0x8400;
  USB_CNTR = 0x8400;
  *(undefined1 *)(usb_pInformation + 9) = *(undefined1 *)(stlink_info_buf + 7);
  return;
}

