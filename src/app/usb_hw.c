/*
 * app/usb_hw.c - USB peripheral / packet memory access
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * usb_interrupt_config
 *   Configure USB/system interrupts: NVIC priority grouping, VTOR (app table at 0x08004000), clear/enable USB IRQs, enable peripheral IRQs
 *   address 0x0800C2A4, 164 bytes
 *   called from: app_hw_init, usb_class_reset
 *   prototype: undefined4 usb_interrupt_config(void)
 */
/* Configure USB/system interrupts: NVIC priority grouping, VTOR (app table at 0x08004000),
   clear/enable USB IRQs, enable peripheral IRQs */

undefined4 usb_interrupt_config(void)

{
  __disable_irq();
  NVIC_SetVectorTable(0x8000000,0x4000);
  __enable_irq();
  NVIC_SetPriorityGrouping(0x500);
  USB_ISTR = 0;
  NVIC_SetPriorityGrouping(0x500);
  NVIC_Init(&stack0xfffffff8);
  NVIC_Init(&stack0xfffffff8);
  NVIC_ClearPendingIRQ(0x19);
  NVIC_ClearPendingIRQ(0x1b);
  NVIC_Init(&stack0xfffffff8);
  NVIC_Init(&stack0xfffffff8);
  return 0x1010119;
}

/**
 * SetEPTxStatus
 *   ST USB SetEPTxStatus: toggle STAT_TX bits of USB_EPnR to requested state, preserve CTR/toggle-only bits
 *   address 0x0800C66A, 50 bytes
 *   called from: usart_bridge_send_trace, Standard_ClearFeature, stlink_cmd_process, stlink_cmd_reply_unknown, usart_bridge_dma_tc_handler
 *   prototype: void SetEPTxStatus(int ep,int status)
 */
/* ST USB SetEPTxStatus: toggle STAT_TX bits of USB_EPnR to requested state, preserve
   CTR/toggle-only bits */

void SetEPTxStatus(int param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = (&USB_EP0R)[param_1] & 0x8fbf;
  if (param_2 << 0x1b < 0) {
    uVar1 = uVar1 ^ 0x10;
  }
  if (param_2 << 0x1a < 0) {
    uVar1 = uVar1 ^ 0x20;
  }
  (&USB_EP0R)[param_1] = uVar1 | 0x8080;
  return;
}

/**
 * _uncertain_usb_ep_clear_ctr_rx
 *   Clear/ack EP CTR_RX on USB_EPnR (used in bulk OUT path)
 *   address 0x0800C6F4, 36 bytes
 *   called from: Standard_ClearFeature, usb_class_set_configured
 *   prototype: void usb_ep_clear_ctr_rx(int ep)
 */
/* Clear/ack EP CTR_RX on USB_EPnR (used in bulk OUT path) */

void _uncertain_usb_ep_clear_ctr_rx(int param_1)

{
  if ((int)((&USB_EP0R)[param_1] << 0x11) < 0) {
    (&USB_EP0R)[param_1] = (&USB_EP0R)[param_1] & 0x8f8f | 0xc080;
  }
  return;
}

/**
 * _uncertain_usb_ep_clear_ctr_tx
 *   Clear/ack EP CTR_TX on USB_EPnR (used in bulk IN path)
 *   address 0x0800C718, 36 bytes
 *   called from: Standard_ClearFeature, usb_class_set_configured
 *   prototype: void usb_ep_clear_ctr_tx(int ep)
 */
/* Clear/ack EP CTR_TX on USB_EPnR (used in bulk IN path) */

void _uncertain_usb_ep_clear_ctr_tx(int param_1)

{
  if ((int)((&USB_EP0R)[param_1] << 0x19) < 0) {
    (&USB_EP0R)[param_1] = (&USB_EP0R)[param_1] & 0x8f8f | 0x80c0;
  }
  return;
}

/**
 * usb_hw_set_device_address
 *   Programs all USB_EPnR EA fields then writes USB_DADDR with address | EF enable (ST SetDeviceAddress)
 *   address 0x0800D854, 56 bytes
 *   called from: In0_Process
 *   prototype: void usb_hw_set_device_address(uint8_t addr)
 */
/* Programs all USB_EPnR EA fields then writes USB_DADDR with address | EF enable (ST
   SetDeviceAddress) */

void usb_hw_set_device_address(uint param_1)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  
  uVar1 = (uint)usb_num_endpoints;
  uVar2 = 0;
  if (uVar1 != 0) {
    puVar3 = &USB_EP0R;
    do {
      uVar4 = uVar2 & 0xff;
      uVar2 = uVar2 + 1;
      *puVar3 = uVar4 | *puVar3 & 0x8f8f | 0x8080;
      puVar3 = puVar3 + 1;
    } while (uVar2 < uVar1);
  }
  USB_DADDR = param_1 | 0x80;
  return;
}

/**
 * usb_hw_power_on
 *   USB peripheral power-on: pulls up D+, clears CNTR FRES/PDWN, resets ISTR, enables RESET/CTR interrupt mask (0x1C00)
 *   address 0x0800DB64, 34 bytes
 *   called from: usb_class_reset
 *   prototype: void usb_hw_power_on(void)
 */
/* USB peripheral power-on: pulls up D+, clears CNTR FRES/PDWN, resets ISTR, enables RESET/CTR
   interrupt mask (0x1C00) */

longlong usb_hw_power_on(void)

{
  uint unaff_r7;
  
  gpio_pa15_write(1);
  USB_CNTR = 1;
  USB_CNTR = 0;
  USB_ISTR = 0;
  usb_wInterrupt_Mask = 0x1c00;
  USB_CNTR = 0x1c00;
  return (ulonglong)unaff_r7 << 0x20;
}

/**
 * usb_hw_power_off
 *   USB peripheral reconnect/reset: asserts FRES, clears ISTR, toggles D+ pull-up, restarts (CNTR=3)
 *   address 0x0800DB86, 24 bytes
 *   called from: APP_main
 *   prototype: uint32_t usb_hw_power_off(void)
 */
/* USB peripheral reconnect/reset: asserts FRES, clears ISTR, toggles D+ pull-up, restarts (CNTR=3)
    */

undefined4 usb_hw_power_off(void)

{
  USB_CNTR = 1;
  USB_ISTR = 0;
  gpio_pa15_write();
  USB_CNTR = 3;
  return 0;
}

/**
 * usb_hw_ctr_lp
 *   ST usb_int CTR_LP: low-priority Correct-Transfer handler; services EP0 (Setup/In/Out) and other EP OUT/IN callbacks
 *   address 0x0800DBA8, 320 bytes
 *   called from: USB_Istr
 *   prototype: uint32_t usb_hw_ctr_lp(void)
 */
/* ST usb_int CTR_LP: low-priority Correct-Transfer handler; services EP0 (Setup/In/Out) and other
   EP OUT/IN callbacks */

uint usb_hw_ctr_lp(void)

{
  undefined4 uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint in_r3;
  uint local_28;
  
  local_28 = in_r3 & 0xffff0000;
  do {
    while( true ) {
      uVar3 = USB_ISTR;
      usb_wIstr = (undefined2)uVar3;
      if (-1 < (int)(uVar3 << 0x10)) {
        return local_28;
      }
      uVar2 = uVar3 & 0xf;
      usb_EPindex = (undefined1)uVar2;
      if ((uVar3 & 0xf) == 0) break;
      iVar4 = (&USB_EP0R)[uVar2];
      local_28 = CONCAT22(local_28._2_2_,(short)iVar4);
      if (iVar4 << 0x10 < 0) {
        (&USB_EP0R)[uVar2] = (&USB_EP0R)[uVar2] & 0xf8f | 0x80;
        (**(code **)(&usb_pEpInt_IN + uVar2 * 4))();
      }
      else if (iVar4 << 0x18 < 0) {
        (&USB_EP0R)[uVar2] = (&USB_EP0R)[uVar2] & 0x8f0f | 0x8000;
        (**(code **)(&usb_pEpInt_OUT + uVar2 * 4))();
      }
    }
    uVar1 = USB_EP0R;
    usb_StatusIn = (ushort)uVar1 & 0x30;
    usb_StatusOut = (ushort)uVar1 & 0x3000;
    uVar2 = USB_EP0R;
    USB_EP0R = uVar2 & 0xbfbf ^ 0x2020 | 0x8080;
    uVar2 = USB_EP0R;
    if (-1 < (int)(uVar3 << 0x1b)) {
      USB_EP0R = uVar2 & 0x8f0f | 0x8000;
      In0_Process();
      goto LAB_0800dc78;
    }
    local_28 = CONCAT22(local_28._2_2_,(short)uVar2);
    if ((int)(uVar2 << 0x14) < 0) {
      uVar3 = USB_EP0R;
      USB_EP0R = uVar3 & 0xf8f | 0x80;
      Setup0_Process();
      goto LAB_0800dc78;
    }
  } while (-1 < (int)(uVar2 << 0x10));
  uVar3 = USB_EP0R;
  USB_EP0R = uVar3 & 0xf8f | 0x80;
  Out0_Process();
LAB_0800dc78:
  uVar3 = USB_EP0R;
  uVar3 = uVar3 & 0xbfbf;
  if ((int)((uint)usb_StatusOut << 0x13) < 0) {
    uVar3 = uVar3 ^ 0x1000;
  }
  if ((int)((uint)usb_StatusOut << 0x12) < 0) {
    uVar3 = uVar3 ^ 0x2000;
  }
  if ((int)((uint)usb_StatusIn << 0x1b) < 0) {
    uVar3 = uVar3 ^ 0x10;
  }
  if ((int)((uint)usb_StatusIn << 0x1a) < 0) {
    uVar3 = uVar3 ^ 0x20;
  }
  USB_EP0R = uVar3 | 0x8080;
  return local_28;
}

/**
 * usb_hw_ctr_hp
 *   ST usb_int CTR_HP: high-priority Correct-Transfer handler loop; dispatches per-endpoint IN/OUT callbacks
 *   address 0x0800DCE8, 126 bytes
 *   prototype: void usb_hw_ctr_hp(void)
 */
/* ST usb_int CTR_HP: high-priority Correct-Transfer handler loop; dispatches per-endpoint IN/OUT
   callbacks */

undefined4 usb_hw_ctr_hp(void)

{
  uint uVar1;
  undefined4 in_r3;
  
  while (uVar1 = USB_ISTR, usb_wIstr = (undefined2)uVar1, (int)(uVar1 << 0x10) < 0) {
    USB_ISTR = 0x7fff;
    uVar1 = uVar1 & 0xf;
    usb_EPindex = (undefined1)uVar1;
    if ((int)((&USB_EP0R)[uVar1] << 0x10) < 0) {
      (&USB_EP0R)[uVar1] = (&USB_EP0R)[uVar1] & 0xf8f | 0x80;
      (**(code **)(&usb_pEpInt_IN + uVar1 * 4))();
    }
    else if ((int)((&USB_EP0R)[uVar1] << 0x18) < 0) {
      (&USB_EP0R)[uVar1] = (&USB_EP0R)[uVar1] & 0x8f0f | 0x8000;
      (**(code **)(&usb_pEpInt_OUT + uVar1 * 4))();
    }
  }
  return in_r3;
}

/**
 * APP_USB_HP_CAN1_TX_IRQHandler
 *   USB high-priority IRQ: inlined CTR_HP loop servicing per-endpoint IN/OUT correct-transfer callbacks
 *   address 0x0800E0D4, 4 bytes
 *   prototype: void APP_USB_HP_CAN1_TX_IRQHandler(void)
 */
/* USB high-priority IRQ: inlined CTR_HP loop servicing per-endpoint IN/OUT correct-transfer
   callbacks */

undefined4 APP_USB_HP_CAN1_TX_IRQHandler(void)

{
  uint uVar1;
  undefined4 in_r3;
  
  while (uVar1 = USB_ISTR, usb_wIstr = (undefined2)uVar1, (int)(uVar1 << 0x10) < 0) {
    USB_ISTR = 0x7fff;
    uVar1 = uVar1 & 0xf;
    usb_EPindex = (undefined1)uVar1;
    if ((int)((&USB_EP0R)[uVar1] << 0x10) < 0) {
      (&USB_EP0R)[uVar1] = (&USB_EP0R)[uVar1] & 0xf8f | 0x80;
      (**(code **)(&usb_pEpInt_IN + uVar1 * 4))();
    }
    else if ((int)((&USB_EP0R)[uVar1] << 0x18) < 0) {
      (&USB_EP0R)[uVar1] = (&USB_EP0R)[uVar1] & 0x8f0f | 0x8000;
      (**(code **)(&usb_pEpInt_OUT + uVar1 * 4))();
    }
  }
  return in_r3;
}

/**
 * APP_USB_LP_CAN1_RX0_IRQHandler
 *   USB low-priority IRQ: handles RESET (calls pProperty->Reset) and CTR (CTR_LP), sets device state
 *   address 0x0800E0D8, 4 bytes
 *   prototype: void APP_USB_LP_CAN1_RX0_IRQHandler(void)
 */
/* USB low-priority IRQ: handles RESET (calls pProperty->Reset) and CTR (CTR_LP), sets device state
    */

undefined4 APP_USB_LP_CAN1_RX0_IRQHandler(void)

{
  undefined4 uVar1;
  undefined4 in_r3;
  
  uVar1 = USB_ISTR;
  usb_wIstr = (ushort)uVar1;
  if ((int)((uint)(usb_wIstr & usb_wInterrupt_Mask) << 0x15) < 0) {
    USB_ISTR = 0xfbff;
    (*DAT_200013c8)();
  }
  if (((int)((uint)(usb_wIstr & usb_wInterrupt_Mask) << 0x10) < 0) &&
     (usb_hw_ctr_lp(), usb_mode_sel == '\0')) {
    usb_mode_sel = '\x01';
  }
  return in_r3;
}

/**
 * USB_Istr
 *   ST usb_istr: reads ISTR, handles RESET (pProperty->Reset) and CTR (CTR_LP), updates device state
 *   address 0x0800E214, 60 bytes
 *   prototype: uint32_t USB_Istr(void)
 */
/* ST usb_istr: reads ISTR, handles RESET (pProperty->Reset) and CTR (CTR_LP), updates device state
    */

undefined4 USB_Istr(void)

{
  undefined4 uVar1;
  undefined4 in_r3;
  
  uVar1 = USB_ISTR;
  usb_wIstr = (ushort)uVar1;
  if ((int)((uint)(usb_wIstr & usb_wInterrupt_Mask) << 0x15) < 0) {
    USB_ISTR = 0xfbff;
    (*DAT_200013c8)();
  }
  if (((int)((uint)(usb_wIstr & usb_wInterrupt_Mask) << 0x10) < 0) &&
     (usb_hw_ctr_lp(), usb_mode_sel == '\0')) {
    usb_mode_sel = '\x01';
  }
  return in_r3;
}

