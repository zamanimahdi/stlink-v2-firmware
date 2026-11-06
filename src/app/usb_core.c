/*
 * app/usb_core.c - USB device core (control transfers)
 */
#include "../include/types.h"
#include "../include/stm32f103_regs.h"
#include "globals.h"

/**
 * UserToPMABufferCopy
 *   ST USB UserToPMABufferCopy: copy user buffer into USB packet-memory (PMA 0x40006000)
 *   address 0x0800C600, 40 bytes
 *   called from: stlink_cmd_send_reply, usart_bridge_send_trace, DataStageIn, stlink_cmd_reply_unknown, stlink_cmd_process, usart_bridge_dma_tc_handler
 *   prototype: void UserToPMABufferCopy(undefined2 *src,int pma_off,int nbytes)
 */
/* ST USB UserToPMABufferCopy: copy user buffer into USB packet-memory (PMA 0x40006000) */

void UserToPMABufferCopy(undefined2 *param_1,int param_2,int param_3)

{
  undefined2 *puVar1;
  int iVar2;
  
  puVar1 = (undefined2 *)(&DAT_40006000 + param_2 * 2);
  for (iVar2 = param_3 + 1 >> 1; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar1 = *param_1;
    puVar1 = puVar1 + 2;
    param_1 = param_1 + 1;
  }
  return;
}

/**
 * PMAToUserBufferCopy
 *   ST USB PMAToUserBufferCopy: copy from USB PMA to user buffer
 *   address 0x0800C628, 30 bytes
 *   called from: stlink_dispatch_command, Out0_Process
 *   prototype: void PMAToUserBufferCopy(undefined2 *dst,int pma_off,int nbytes)
 */
/* ST USB PMAToUserBufferCopy: copy from USB PMA to user buffer */

void PMAToUserBufferCopy(undefined2 *param_1,int param_2,int param_3)

{
  undefined4 *puVar1;
  int iVar2;
  
  puVar1 = (undefined4 *)(&DAT_40006000 + param_2 * 2);
  for (iVar2 = param_3 + 1 >> 1; iVar2 != 0; iVar2 = iVar2 + -1) {
    *param_1 = (short)*puVar1;
    puVar1 = puVar1 + 1;
    param_1 = param_1 + 1;
  }
  return;
}

/**
 * SetEPTxAddr
 *   ST USB SetEPTxAddr: write ADDR_TX in the EP buffer-description table
 *   address 0x0800C73C, 2 bytes
 *   called from: usb_class_reset
 *   prototype: void SetEPTxAddr(int ep,uint addr)
 */
/* ST USB SetEPTxAddr: write ADDR_TX in the EP buffer-description table */

void SetEPTxAddr(int param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = USB_BTABLE;
  *(uint *)(&DAT_40006000 + ((uVar1 & 0xffff) + param_1 * 8) * 2) = param_2 & 0xfffffffe;
  return;
}

/**
 * SetEPRxAddr
 *   ST USB SetEPRxAddr: write ADDR_RX in the EP buffer-description table
 *   address 0x0800C73E, 14 bytes
 *   called from: usb_class_reset
 *   prototype: void SetEPRxAddr(int ep,uint addr)
 */
/* ST USB SetEPRxAddr: write ADDR_RX in the EP buffer-description table */

void SetEPRxAddr(int param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = USB_BTABLE;
  *(uint *)(&DAT_40006008 + ((uVar1 & 0xffff) + param_1 * 8) * 2) = param_2 & 0xfffffffe;
  return;
}

/**
 * GetEPTxAddr
 *   ST USB GetEPTxAddr: read ADDR_TX from the BTABLE entry
 *   address 0x0800C74C, 2 bytes
 *   called from: DataStageIn
 *   prototype: uint GetEPTxAddr(int ep)
 */
/* ST USB GetEPTxAddr: read ADDR_TX from the BTABLE entry */

uint GetEPTxAddr(int param_1)

{
  uint uVar1;
  
  uVar1 = USB_BTABLE;
  return *(uint *)(&DAT_40006000 + ((uVar1 & 0xffff) + param_1 * 8) * 2) & 0xffff;
}

/**
 * GetEPRxAddr
 *   ST USB GetEPRxAddr: read ADDR_RX from the BTABLE entry
 *   address 0x0800C74E, 14 bytes
 *   called from: Out0_Process
 *   prototype: uint GetEPRxAddr(int ep)
 */
/* ST USB GetEPRxAddr: read ADDR_RX from the BTABLE entry */

uint GetEPRxAddr(int param_1)

{
  uint uVar1;
  
  uVar1 = USB_BTABLE;
  return *(uint *)(&DAT_40006008 + ((uVar1 & 0xffff) + param_1 * 8) * 2) & 0xffff;
}

/**
 * SetEPTxCount
 *   ST USB SetEPTxCount: write COUNT_TX in the BTABLE entry
 *   address 0x0800C75C, 14 bytes
 *   called from: stlink_cmd_send_reply, usart_bridge_send_trace, DataStageIn, Out0_Process, stlink_cmd_reply_unknown, stlink_cmd_process, usart_bridge_dma_tc_handler
 *   prototype: void SetEPTxCount(int ep,undefined4 count)
 */
/* ST USB SetEPTxCount: write COUNT_TX in the BTABLE entry */

void SetEPTxCount(int param_1,undefined4 param_2)

{
  uint uVar1;
  
  uVar1 = USB_BTABLE;
  *(undefined4 *)(&DAT_40006004 + ((uVar1 & 0xffff) + param_1 * 8) * 2) = param_2;
  return;
}

/**
 * SetEPRxCount
 *   ST USB SetEPRxCount: encode/write COUNT_RX (block-size format) in the BTABLE entry
 *   address 0x0800C76A, 56 bytes
 *   called from: usb_class_reset, Standard_ClearFeature, Out0_Process
 *   prototype: void SetEPRxCount(int ep,uint count)
 */
/* ST USB SetEPRxCount: encode/write COUNT_RX (block-size format) in the BTABLE entry */

void SetEPRxCount(int param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = USB_BTABLE;
  if ((int)param_2 < 0x3f) {
    uVar2 = param_2 >> 1;
    if ((int)(param_2 << 0x1f) < 0) {
      uVar2 = uVar2 + 1;
    }
    uVar2 = (uVar2 & 0xffff) << 10;
  }
  else {
    uVar2 = param_2 >> 5;
    if ((param_2 & 0x1f) == 0) {
      uVar2 = uVar2 - 1;
    }
    uVar2 = (uVar2 & 0xffff) << 10 | 0x8000;
  }
  *(uint *)(&DAT_4000600c + ((uVar1 & 0xffff) + param_1 * 8) * 2) = uVar2;
  return;
}

/**
 * GetEPRxCount
 *   ST USB GetEPRxCount: read COUNT_RX (low 10 bits) from the BTABLE entry
 *   address 0x0800C7A2, 24 bytes
 *   called from: stlink_dispatch_command
 *   prototype: uint GetEPRxCount(int ep)
 */
/* ST USB GetEPRxCount: read COUNT_RX (low 10 bits) from the BTABLE entry */

uint GetEPRxCount(int param_1)

{
  uint uVar1;
  
  uVar1 = USB_BTABLE;
  return *(uint *)(&DAT_4000600c + ((uVar1 & 0xffff) + param_1 * 8) * 2) & 0x3ff;
}

/**
 * SetEPTxAddr_unused
 *   Duplicate/unused SetEPTxAddr (writes ADDR_TX, no callers)
 *   address 0x0800C7B0, 22 bytes
 *   prototype: void SetEPTxAddr_unused(int ep,uint addr)
 */
/* Duplicate/unused SetEPTxAddr (writes ADDR_TX, no callers) */

void SetEPTxAddr_unused(int param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = USB_BTABLE;
  *(uint *)(&DAT_40006000 + ((uVar1 & 0xffff) + param_1 * 8) * 2) = param_2 & 0xfffffffe;
  return;
}

/**
 * GetEPTxAddr_unused
 *   Duplicate/unused GetEPTxAddr (reads ADDR_TX, no callers)
 *   address 0x0800C7C6, 20 bytes
 *   prototype: uint GetEPTxAddr_unused(int ep)
 */
/* Duplicate/unused GetEPTxAddr (reads ADDR_TX, no callers) */

uint GetEPTxAddr_unused(int param_1)

{
  uint uVar1;
  
  uVar1 = USB_BTABLE;
  return *(uint *)(&DAT_40006000 + ((uVar1 & 0xffff) + param_1 * 8) * 2) & 0xffff;
}

/**
 * Standard_GetConfiguration
 *   ST usb_core: GET_CONFIGURATION standard request; returns &pInformation->Current_Configuration
 *   address 0x0800D0CC, 26 bytes
 *   called from: Data_Setup0
 *   prototype: uint8_t *Standard_GetConfiguration(uint16_t Length)
 */
/* ST usb_core: GET_CONFIGURATION standard request; returns &pInformation->Current_Configuration */

int Standard_GetConfiguration(int param_1)

{
  if (param_1 == 0) {
    *(undefined2 *)(usb_pInformation + 0x10) = 1;
    return 0;
  }
  (*(code *)*usb_pUser_Standard_Requests)();
  return usb_pInformation + 10;
}

/**
 * Standard_SetConfiguration
 *   ST usb_core: SET_CONFIGURATION standard request; validates wValue and calls User_SetConfiguration
 *   address 0x0800D0E6, 52 bytes
 *   called from: NoData_Setup0
 *   prototype: RESULT Standard_SetConfiguration(void)
 */
/* ST usb_core: SET_CONFIGURATION standard request; validates wValue and calls User_SetConfiguration
    */

longlong Standard_SetConfiguration(void)

{
  ushort uVar1;
  uint unaff_r7;
  bool bVar2;
  
  if (*(byte *)(usb_pInformation + 3) <= usb_max_config) {
    uVar1 = (ushort)*(byte *)(usb_pInformation + 2);
    bVar2 = uVar1 == 0;
    if (bVar2) {
      uVar1 = *(ushort *)(usb_pInformation + 4);
    }
    if (bVar2 && uVar1 == 0) {
      *(byte *)(usb_pInformation + 10) = *(byte *)(usb_pInformation + 3);
      (**(code **)(usb_pUser_Standard_Requests + 4))();
      return (ulonglong)unaff_r7 << 0x20;
    }
  }
  return CONCAT44(unaff_r7,2);
}

/**
 * Standard_GetInterface
 *   ST usb_core: GET_INTERFACE standard request; returns &pInformation->Current_AlternateSetting
 *   address 0x0800D11A, 36 bytes
 *   called from: Data_Setup0
 *   prototype: uint8_t *Standard_GetInterface(uint16_t Length)
 */
/* ST usb_core: GET_INTERFACE standard request; returns &pInformation->Current_AlternateSetting */

int Standard_GetInterface(int param_1)

{
  if (param_1 == 0) {
    *(undefined2 *)(usb_pInformation + 0x10) = 1;
    return 0;
  }
  (**(code **)(usb_pUser_Standard_Requests + 8))();
  return usb_pInformation + 0xc;
}

/**
 * Standard_SetInterface
 *   ST usb_core: SET_INTERFACE standard request; calls Get_Interface_Setting then User_SetInterface
 *   address 0x0800D13E, 72 bytes
 *   called from: NoData_Setup0
 *   prototype: RESULT Standard_SetInterface(void)
 */
/* ST usb_core: SET_INTERFACE standard request; calls Get_Interface_Setting then User_SetInterface
    */

undefined4 Standard_SetInterface(void)

{
  uint uVar1;
  bool bVar2;
  
  uVar1 = (**(code **)(usb_pProperty + 0x18))
                    (*(undefined1 *)(usb_pInformation + 5),*(undefined1 *)(usb_pInformation + 3));
  if (*(char *)(usb_pInformation + 10) != '\0') {
    bVar2 = uVar1 == 0;
    if (bVar2) {
      uVar1 = (uint)*(byte *)(usb_pInformation + 4);
    }
    if ((bVar2 && uVar1 == 0) && (*(char *)(usb_pInformation + 2) == '\0')) {
      (**(code **)(usb_pUser_Standard_Requests + 0xc))();
      *(undefined1 *)(usb_pInformation + 0xb) = *(undefined1 *)(usb_pInformation + 5);
      *(undefined1 *)(usb_pInformation + 0xc) = *(undefined1 *)(usb_pInformation + 3);
      return 0;
    }
  }
  return 2;
}

/**
 * Standard_GetStatus
 *   ST usb_core: GET_STATUS standard request; builds StatusInfo word from device/endpoint state
 *   address 0x0800D186, 146 bytes
 *   called from: Data_Setup0
 *   prototype: uint8_t *Standard_GetStatus(uint16_t Length)
 */
/* ST usb_core: GET_STATUS standard request; builds StatusInfo word from device/endpoint state */

undefined2 * Standard_GetStatus(int param_1)

{
  byte *pbVar1;
  byte bVar2;
  
  if (param_1 == 0) {
    pbVar1 = usb_pInformation;
    pbVar1[0x10] = 2;
    pbVar1[0x11] = 0;
    return (undefined2 *)0x0;
  }
  usb_StatusInfo = 0;
  bVar2 = *usb_pInformation;
  if ((bVar2 & 0x7f) == 0) {
    bVar2 = 0;
    if ((int)((uint)usb_pInformation[9] << 0x1a) < 0) {
      bVar2 = 2;
    }
    if ((int)((uint)usb_pInformation[9] << 0x19) < 0) {
      bVar2 = bVar2 | 1;
    }
    goto LAB_0800d1c4;
  }
  if ((bVar2 & 0x7f) == 1) goto LAB_0800d1d0;
  if ((bVar2 & 0x7f) != 2) {
    usb_StatusInfo = 0;
    return (undefined2 *)0x0;
  }
  if ((int)((uint)usb_pInformation[5] << 0x18) < 0) {
    if (((&USB_EP0R)[usb_pInformation[5] & 0xf] & 0x30) == 0x10) {
      bVar2 = 1;
LAB_0800d1c4:
      usb_StatusInfo = (ushort)bVar2;
    }
  }
  else if (((&USB_EP0R)[usb_pInformation[5] & 0xf] & 0x3000) == 0x1000) {
    bVar2 = 1;
    goto LAB_0800d1c4;
  }
  (**(code **)(usb_pUser_Standard_Requests + 0x10))();
LAB_0800d1d0:
  return &usb_StatusInfo;
}

/**
 * Standard_ClearFeature
 *   ST usb_core: CLEAR_FEATURE standard request; clears remote-wakeup or stalls/unstalls an endpoint
 *   address 0x0800D218, 212 bytes
 *   called from: NoData_Setup0
 *   prototype: RESULT Standard_ClearFeature(void)
 */
/* ST usb_core: CLEAR_FEATURE standard request; clears remote-wakeup or stalls/unstalls an endpoint
    */

longlong Standard_ClearFeature(void)

{
  byte bVar1;
  byte *pbVar2;
  ushort uVar3;
  uint uVar4;
  uint uVar5;
  uint in_r3;
  uint uVar6;
  bool bVar7;
  bool bVar8;
  
  uVar3 = *usb_pInformation & 0x7f;
  if ((*usb_pInformation & 0x7f) == 0) {
    usb_pInformation[9] = usb_pInformation[9] & 0xdf;
LAB_0800d2e4:
    return (ulonglong)in_r3 << 0x20;
  }
  bVar7 = uVar3 == 2;
  if (bVar7) {
    uVar3 = *(ushort *)(usb_pInformation + 2);
  }
  bVar8 = bVar7 && uVar3 == 0;
  if (bVar7 && uVar3 == 0) {
    bVar8 = usb_pInformation[4] == 0;
  }
  if (bVar8) {
    bVar1 = usb_pInformation[5];
    uVar4 = (uint)bVar1;
    uVar6 = uVar4 & 0xffffff7f;
    if ((int)(uVar4 << 0x18) < 0) {
      uVar5 = (&USB_EP0R)[uVar6] & 0x30;
    }
    else {
      uVar5 = (&USB_EP0R)[uVar6] & 0x3000;
    }
    if (uVar6 < usb_num_endpoints) {
      pbVar2 = usb_pInformation;
      if (uVar5 != 0) {
        pbVar2 = (byte *)(uint)usb_pInformation[10];
      }
      if (uVar5 != 0 && pbVar2 != (byte *)0x0) {
        if ((int)(uVar4 << 0x18) < 0) {
          _uncertain_usb_ep_clear_ctr_tx(uVar6);
          if (((&USB_EP0R)[uVar6] & 0x30) == 0x10) {
            SetEPTxStatus(uVar6,0x30);
          }
        }
        else {
          if ((bVar1 & 0x7f) != 0) {
            _uncertain_usb_ep_clear_ctr_rx(uVar6);
          }
          if (((&USB_EP0R)[uVar6] & 0x3000) == 0x1000) {
            if ((bVar1 & 0x7f) == 0) {
              SetEPRxCount(0,usb_ep0_buffer_size);
              uVar4 = USB_EP0R;
              USB_EP0R = uVar4 & 0xbf8f ^ 0x3000 | 0x8080;
            }
            else {
              (&USB_EP0R)[uVar6] = (&USB_EP0R)[uVar6] & 0xbf8f ^ 0x3000 | 0x8080;
            }
          }
        }
        (**(code **)(usb_pUser_Standard_Requests + 0x14))();
        goto LAB_0800d2e4;
      }
    }
  }
  return CONCAT44(in_r3,2);
}

/**
 * Standard_SetEndPointFeature
 *   ST usb_core: SET_FEATURE on endpoint; sets EP STALL via USB_EPnR, calls User_SetEndPointFeature
 *   address 0x0800D2EC, 116 bytes
 *   called from: NoData_Setup0
 *   prototype: RESULT Standard_SetEndPointFeature(void)
 */
/* ST usb_core: SET_FEATURE on endpoint; sets EP STALL via USB_EPnR, calls User_SetEndPointFeature
    */

longlong Standard_SetEndPointFeature(void)

{
  uint uVar1;
  uint uVar2;
  uint in_r3;
  uint uVar3;
  uint uVar4;
  
  uVar3 = (uint)*(byte *)(usb_pInformation + 5);
  uVar1 = uVar3 & 0xffffff7f;
  if ((int)(uVar3 << 0x18) < 0) {
    uVar4 = (&USB_EP0R)[uVar1] & 0x30;
  }
  else {
    uVar4 = (&USB_EP0R)[uVar1] & 0x3000;
  }
  if ((uVar1 < usb_num_endpoints) && (*(short *)(usb_pInformation + 2) == 0)) {
    uVar2 = usb_pInformation;
    if (uVar4 != 0) {
      uVar2 = (uint)*(byte *)(usb_pInformation + 10);
    }
    if (uVar4 != 0 && uVar2 != 0) {
      if ((int)(uVar3 << 0x18) < 0) {
        uVar3 = (&USB_EP0R)[uVar1] & 0x8fbf ^ 0x10;
      }
      else {
        uVar3 = (&USB_EP0R)[uVar1] & 0xbf8f ^ 0x1000;
      }
      (&USB_EP0R)[uVar1] = uVar3 | 0x8080;
      (**(code **)(usb_pUser_Standard_Requests + 0x18))();
      return (ulonglong)in_r3 << 0x20;
    }
  }
  return CONCAT44(in_r3,2);
}

/**
 * DataStageIn
 *   ST usb_core: fills EP0 TX buffer (UserToPMABufferCopy) for the data-in stage, handles ZLP/short-packet end
 *   address 0x0800D37A, 168 bytes
 *   called from: Data_Setup0, In0_Process
 *   prototype: void DataStageIn(void)
 */
/* ST usb_core: fills EP0 TX buffer (UserToPMABufferCopy) for the data-in stage, handles
   ZLP/short-packet end */

void DataStageIn(void)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined1 uVar6;
  ushort *puVar7;
  ushort uVar8;
  
  uVar2 = usb_pInformation;
  puVar7 = (ushort *)(usb_pInformation + 0x10);
  uVar8 = *puVar7;
  uVar3 = usb_pInformation;
  if (uVar8 == 0) {
    uVar3 = (uint)*(byte *)(usb_pInformation + 8);
  }
  if (uVar8 != 0 || uVar3 != 4) {
    uVar1 = *(ushort *)(usb_pInformation + 0x14);
    if (uVar1 < uVar8) {
      uVar6 = 2;
      uVar8 = uVar1;
    }
    else {
      uVar6 = 4;
      if (uVar1 < uVar8) {
        uVar8 = uVar1;
      }
    }
    uVar4 = (**(code **)(usb_pInformation + 0x18))(uVar8);
    uVar5 = GetEPTxAddr(0);
    UserToPMABufferCopy(uVar4,uVar5,uVar8);
    SetEPTxCount(0,uVar8);
    *puVar7 = *puVar7 - uVar8;
    *(ushort *)(uVar2 + 0x12) = uVar8 + *(short *)(uVar2 + 0x12);
    usb_StatusIn = 0x30;
    usb_StatusOut = 0x3000;
  }
  else if (usb_send_zlp == '\x01') {
    uVar3 = USB_BTABLE;
    *(undefined4 *)(&DAT_40006004 + (uVar3 & 0xffff) * 2) = 0;
    uVar6 = 4;
    usb_StatusIn = 0x30;
    usb_send_zlp = '\0';
  }
  else {
    uVar6 = 7;
    usb_StatusIn = 0x10;
  }
  *(undefined1 *)(usb_pInformation + 8) = uVar6;
  return;
}

/**
 * NoData_Setup0
 *   ST usb_core: dispatches no-data-stage SETUP requests (SET_ADDRESS/CONFIGURATION/FEATURE etc.) to standard or class handlers
 *   address 0x0800D422, 218 bytes
 *   called from: Setup0_Process
 *   prototype: RESULT NoData_Setup0(void)
 */
/* ST usb_core: dispatches no-data-stage SETUP requests (SET_ADDRESS/CONFIGURATION/FEATURE etc.) to
   standard or class handlers */

undefined4 NoData_Setup0(void)

{
  byte bVar1;
  uint uVar2;
  byte bVar3;
  int iVar4;
  ushort uVar5;
  undefined4 in_r3;
  bool bVar6;
  bool bVar7;
  
  bVar3 = usb_pInformation[1];
  bVar1 = *usb_pInformation;
  if ((bVar1 & 0x7f) == 0) {
    if (bVar3 == 9) {
      iVar4 = Standard_SetConfiguration();
      goto LAB_0800d4dc;
    }
    if (bVar3 != 5) {
      if (bVar3 == 3) {
        uVar5 = (ushort)usb_pInformation[3];
        bVar6 = uVar5 == 1;
        if (bVar6) {
          uVar5 = *(ushort *)(usb_pInformation + 4);
        }
        if (bVar6 && uVar5 == 0) {
          usb_pInformation[9] = usb_pInformation[9] | 0x20;
          (**(code **)(usb_pUser_Standard_Requests + 0x1c))();
          goto LAB_0800d458;
        }
      }
      else {
        if (bVar3 == 1) {
          bVar1 = usb_pInformation[3];
        }
        if (((bVar3 == 1 && bVar1 == 1) && (*(short *)(usb_pInformation + 4) == 0)) &&
           ((int)((uint)usb_pInformation[9] << 0x1a) < 0)) goto LAB_0800d4ce;
      }
      goto LAB_0800d4e0;
    }
    if (usb_pInformation[3] < 0x80) {
      uVar5 = (ushort)usb_pInformation[2];
      bVar6 = uVar5 == 0;
      if (bVar6) {
        uVar5 = *(ushort *)(usb_pInformation + 4);
      }
      bVar7 = bVar6 && uVar5 == 0;
      if (bVar6 && uVar5 == 0) {
        bVar7 = usb_pInformation[10] == 0;
      }
      if (bVar7) goto LAB_0800d458;
    }
LAB_0800d4f8:
    bVar3 = 8;
  }
  else {
    if ((bVar1 & 0x7f) == 1) {
      if (bVar3 == 0xb) {
        iVar4 = Standard_SetInterface();
        goto LAB_0800d4dc;
      }
LAB_0800d4e0:
      iVar4 = (**(code **)(usb_pProperty + 0x14))(bVar3);
      if (iVar4 == 3) {
        bVar3 = 9;
        goto LAB_0800d474;
      }
      if (iVar4 != 0) goto LAB_0800d4f8;
    }
    else {
      if ((bVar1 & 0x7f) != 2) goto LAB_0800d4e0;
      if (bVar3 == 1) {
LAB_0800d4ce:
        iVar4 = Standard_ClearFeature();
      }
      else {
        if (bVar3 != 3) goto LAB_0800d4e0;
        iVar4 = Standard_SetEndPointFeature();
      }
LAB_0800d4dc:
      if (iVar4 != 0) goto LAB_0800d4e0;
    }
LAB_0800d458:
    uVar2 = USB_BTABLE;
    *(undefined4 *)(&DAT_40006004 + (uVar2 & 0xffff) * 2) = 0;
    bVar3 = 6;
    usb_StatusIn = 0x30;
  }
LAB_0800d474:
  usb_pInformation[8] = bVar3;
  return in_r3;
}

/**
 * Data_Setup0
 *   ST usb_core: dispatches data-stage SETUP requests (GET_DESCRIPTOR/STATUS/CONFIG/INTERFACE and class setups), picks CopyRoutine
 *   address 0x0800D500, 422 bytes
 *   called from: Setup0_Process
 *   prototype: void Data_Setup0(void)
 */
/* ST usb_core: dispatches data-stage SETUP requests (GET_DESCRIPTOR/STATUS/CONFIG/INTERFACE and
   class setups), picks CopyRoutine */

void Data_Setup0(void)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  ushort uVar5;
  code *pcVar6;
  short sVar7;
  uint uVar8;
  bool bVar9;
  bool bVar10;
  
  bVar2 = usb_pInformation[1];
  if (bVar2 == 6) {
    if ((*usb_pInformation & 0x7f) != 0) goto LAB_0800d580;
    bVar2 = usb_pInformation[2];
    if (bVar2 == 1) {
      pcVar6 = *(code **)(usb_pProperty + 0x1c);
    }
    else if (bVar2 == 2) {
      pcVar6 = *(code **)(usb_pProperty + 0x20);
    }
    else {
      if (bVar2 != 3) goto LAB_0800d580;
      pcVar6 = *(code **)(usb_pProperty + 0x24);
    }
LAB_0800d622:
    pbVar1 = usb_pInformation;
    if (pcVar6 == (code *)0x0) goto LAB_0800d580;
    pbVar1[0x12] = 0;
    pbVar1[0x13] = 0;
    *(code **)(pbVar1 + 0x18) = pcVar6;
    (*pcVar6)(0);
    iVar3 = 0;
  }
  else {
    if (bVar2 == 0) {
      sVar7 = *(short *)(usb_pInformation + 2);
      bVar9 = sVar7 == 0;
      if (bVar9) {
        sVar7 = *(short *)(usb_pInformation + 6);
      }
      if ((bVar9 && sVar7 == 2) && (usb_pInformation[4] == 0)) {
        bVar2 = *usb_pInformation;
        sVar7 = 0x7f;
        bVar9 = (bVar2 & 0x7f) == 0;
        if (bVar9) {
          sVar7 = *(short *)(usb_pInformation + 4);
        }
        if (bVar9 && sVar7 == 0) {
LAB_0800d5ca:
          pcVar6 = (code *)0x800d187;
          goto LAB_0800d622;
        }
        if ((bVar2 & 0x7f) == 1) {
          iVar3 = (**(code **)(usb_pProperty + 0x18))(usb_pInformation[5],0);
          if ((iVar3 == 0) && (usb_pInformation[10] != 0)) goto LAB_0800d5ca;
        }
        else if ((bVar2 & 0x7f) == 2) {
          bVar2 = usb_pInformation[5];
          uVar8 = bVar2 & 0xf;
          if ((int)((uint)bVar2 << 0x18) < 0) {
            uVar4 = (&USB_EP0R)[uVar8] & 0x30;
          }
          else {
            uVar4 = (&USB_EP0R)[uVar8] & 0x3000;
          }
          if (((uVar8 < usb_num_endpoints) && ((bVar2 & 0x70) == 0)) && (uVar4 != 0))
          goto LAB_0800d5ca;
        }
      }
    }
    else if (bVar2 == 8) {
      if ((*usb_pInformation & 0x7f) == 0) {
        pcVar6 = (code *)0x800d0cd;
        goto LAB_0800d622;
      }
    }
    else {
      bVar9 = bVar2 == 10;
      if (bVar9) {
        bVar2 = *usb_pInformation & 0x7f;
      }
      if ((bVar9 && bVar2 == 1) && (usb_pInformation[10] != 0)) {
        uVar5 = *(ushort *)(usb_pInformation + 2);
        bVar9 = uVar5 == 0;
        if (bVar9) {
          uVar5 = (ushort)usb_pInformation[4];
        }
        bVar10 = bVar9 && uVar5 == 0;
        if (bVar9 && uVar5 == 0) {
          bVar10 = *(short *)(usb_pInformation + 6) == 1;
        }
        if ((bVar10) &&
           (iVar3 = (**(code **)(usb_pProperty + 0x18))(usb_pInformation[5],0), iVar3 == 0)) {
          pcVar6 = (code *)0x800d11b;
          goto LAB_0800d622;
        }
      }
    }
LAB_0800d580:
    iVar3 = (**(code **)(usb_pProperty + 0x10))(usb_pInformation[1]);
    if (iVar3 == 3) {
      bVar2 = 9;
      goto LAB_0800d650;
    }
  }
  pbVar1 = usb_pInformation;
  uVar8 = (uint)*(ushort *)(usb_pInformation + 0x10);
  if (uVar8 == 0xffff) {
    bVar2 = 9;
  }
  else {
    if (iVar3 != 2 && uVar8 != 0) {
      if (-1 < (int)((uint)*usb_pInformation << 0x18)) {
        usb_pInformation[8] = 3;
        usb_StatusOut = 0x3000;
        return;
      }
      uVar5 = *(ushort *)(usb_pInformation + 6);
      if (uVar5 < uVar8) {
        *(ushort *)(usb_pInformation + 0x10) = uVar5;
      }
      else if (uVar8 < uVar5) {
        uVar4 = (uint)*(byte *)(usb_pProperty + 0x2c);
        if (uVar8 < uVar4) {
          usb_send_zlp = 0;
        }
        else if (uVar8 == uVar4 * (uVar8 / uVar4)) {
          usb_send_zlp = 1;
        }
      }
      *(ushort *)(pbVar1 + 0x14) = (ushort)*(byte *)(usb_pProperty + 0x2c);
      DataStageIn();
      return;
    }
    bVar2 = 8;
  }
LAB_0800d650:
  usb_pInformation[8] = bVar2;
  return;
}

/**
 * Setup0_Process
 *   ST usb_core: reads the 8-byte SETUP packet from PMA into pInformation and calls Data_Setup0 or NoData_Setup0
 *   address 0x0800D6B4, 104 bytes
 *   called from: usb_hw_ctr_lp
 *   prototype: RESULT Setup0_Process(void)
 */
/* ST usb_core: reads the 8-byte SETUP packet from PMA into pInformation and calls Data_Setup0 or
   NoData_Setup0 */

longlong Setup0_Process(void)

{
  uint uVar1;
  undefined1 *puVar2;
  undefined2 uVar3;
  int iVar4;
  uint unaff_r7;
  
  uVar1 = USB_BTABLE;
  iVar4 = (uint)*(ushort *)(&DAT_40006008 + (uVar1 & 0xffff) * 2) * 2;
  if (usb_pInformation[8] != '\t') {
    *usb_pInformation = (&DAT_40006000)[iVar4];
    usb_pInformation[1] = (&DAT_40006001)[iVar4];
    uVar3 = swap_bytes_u16(*(undefined2 *)(&DAT_40006004 + iVar4));
    *(undefined2 *)(usb_pInformation + 2) = uVar3;
    uVar3 = swap_bytes_u16(*(undefined2 *)(&DAT_40006008 + iVar4));
    puVar2 = usb_pInformation;
    *(undefined2 *)(usb_pInformation + 4) = uVar3;
    *(undefined2 *)(puVar2 + 6) = *(undefined2 *)(&DAT_4000600c + iVar4);
  }
  usb_pInformation[8] = 1;
  if (*(short *)(usb_pInformation + 6) == 0) {
    NoData_Setup0();
  }
  else {
    Data_Setup0();
  }
  SetEPRxCount(0,usb_ep0_buffer_size);
  if (usb_pInformation[8] == '\b') {
    usb_StatusOut = 0x1000;
    usb_StatusIn = 0x10;
  }
  if (usb_pInformation[8] == '\t') {
    return CONCAT44(unaff_r7,1);
  }
  return (ulonglong)unaff_r7 << 0x20;
}

/**
 * In0_Process
 *   ST usb_core: EP0 IN transaction handler; advances IN data stage, applies pending SET_ADDRESS
 *   address 0x0800D71C, 78 bytes
 *   called from: usb_hw_ctr_lp
 *   prototype: RESULT In0_Process(void)
 */
/* ST usb_core: EP0 IN transaction handler; advances IN data stage, applies pending SET_ADDRESS */

longlong In0_Process(undefined4 param_1,undefined4 param_2,byte param_3)

{
  byte bVar1;
  uint unaff_r7;
  bool bVar2;
  
  bVar1 = usb_pInformation[8];
  if (bVar1 == 2 || bVar1 == 4) {
    DataStageIn();
    bVar1 = usb_pInformation[8];
  }
  else {
    if (bVar1 == 6) {
      bVar1 = usb_pInformation[1];
      bVar2 = bVar1 == 5;
      if (bVar2) {
        bVar1 = *usb_pInformation;
        param_3 = 0x7f;
      }
      if (bVar2 && (bVar1 & param_3) == 0) {
        usb_hw_set_device_address(usb_pInformation[3]);
        (**(code **)(usb_pUser_Standard_Requests + 0x20))();
      }
      (**(code **)(usb_pProperty + 8))();
    }
    bVar1 = 8;
  }
  usb_pInformation[8] = bVar1;
  SetEPRxCount(0,usb_ep0_buffer_size);
  if (usb_pInformation[8] == 8) {
    usb_StatusOut = 0x1000;
    usb_StatusIn = 0x10;
  }
  if (usb_pInformation[8] != 9) {
    return (ulonglong)unaff_r7 << 0x20;
  }
  return CONCAT44(unaff_r7,1);
}

/**
 * Out0_Process
 *   ST usb_core: EP0 OUT transaction handler; receives data-out stage (PMAToUserBufferCopy) and completes control transfer
 *   address 0x0800D76A, 234 bytes
 *   called from: usb_hw_ctr_lp
 *   prototype: RESULT Out0_Process(void)
 */
/* ST usb_core: EP0 OUT transaction handler; receives data-out stage (PMAToUserBufferCopy) and
   completes control transfer */

longlong Out0_Process(void)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  undefined1 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  ushort *puVar7;
  ushort uVar8;
  uint unaff_r7;
  
  iVar3 = usb_pInformation;
  cVar1 = *(char *)(usb_pInformation + 8);
  if (cVar1 != '\x02' && cVar1 != '\x04') {
    if (cVar1 == '\x03' || cVar1 == '\x05') {
      puVar7 = (ushort *)(usb_pInformation + 0x10);
      uVar8 = *puVar7;
      if (*(code **)(usb_pInformation + 0x18) != (code *)0x0 && uVar8 != 0) {
        if (*(ushort *)(usb_pInformation + 0x14) < uVar8) {
          uVar8 = *(ushort *)(usb_pInformation + 0x14);
        }
        uVar5 = (**(code **)(usb_pInformation + 0x18))(uVar8);
        *puVar7 = *puVar7 - uVar8;
        *(ushort *)(iVar3 + 0x12) = uVar8 + *(short *)(iVar3 + 0x12);
        uVar6 = GetEPRxAddr(0);
        PMAToUserBufferCopy(uVar5,uVar6,uVar8);
      }
      if (*puVar7 != 0) {
        usb_StatusOut = 0x3000;
        SetEPTxCount(0,0);
        usb_StatusIn = 0x30;
      }
      if (*puVar7 < *(ushort *)(iVar3 + 0x14)) {
        if (*puVar7 != 0) {
          uVar4 = 5;
          goto LAB_0800d7e8;
        }
        *(undefined1 *)(usb_pInformation + 8) = 6;
        uVar2 = USB_BTABLE;
        *(undefined4 *)(&DAT_40006004 + (uVar2 & 0xffff) * 2) = 0;
        usb_StatusIn = 0x30;
      }
      else {
        uVar4 = 3;
LAB_0800d7e8:
        *(undefined1 *)(usb_pInformation + 8) = uVar4;
      }
      uVar4 = *(undefined1 *)(usb_pInformation + 8);
      goto LAB_0800d818;
    }
    if (cVar1 == '\a') {
      (**(code **)(usb_pProperty + 0xc))();
    }
  }
  uVar4 = 8;
LAB_0800d818:
  *(undefined1 *)(usb_pInformation + 8) = uVar4;
  SetEPRxCount(0,usb_ep0_buffer_size);
  if (*(char *)(usb_pInformation + 8) == '\b') {
    usb_StatusOut = 0x1000;
    usb_StatusIn = 0x10;
  }
  if (*(char *)(usb_pInformation + 8) != '\t') {
    return (ulonglong)unaff_r7 << 0x20;
  }
  return CONCAT44(unaff_r7,1);
}

/**
 * USB_Init
 *   ST USB_Init: sets pInformation/pProperty/pUser pointers and calls pProperty->Init
 *   address 0x0800E060, 20 bytes
 *   called from: APP_main
 *   prototype: void USB_Init(void)
 */
/* ST USB_Init: sets pInformation/pProperty/pUser pointers and calls pProperty->Init */

void USB_Init(void)

{
  usb_pInformation = &usb_Device_Info;
  DAT_20003060 = 2;
  usb_pUser_Standard_Requests = &usb_User_Standard_Requests;
  usb_pProperty = &usb_Device_Property;
                    /* WARNING: Could not recover jumptable at 0x0800e072. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*usb_Device_Property)();
  return;
}

