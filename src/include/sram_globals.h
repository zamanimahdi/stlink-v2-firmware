/* Global variables in SRAM identified while reading the code (bootloader and application
   share the same SRAM, each uses it for its own variables). */
#pragma once
#include <stdint.h>
#include "types.h"

extern undefined1 usart_bridge_active;  /* 0x20000000: USART bootloader-bridge active flag */
extern undefined2 usart_bridge_rx_len;  /* 0x20000004: USART bridge RX buffer half-length (double-buffer) */
extern undefined2 usb_trace_ep_maxpkt;  /* 0x2000000C: Max packet size used when flushing bridge RX to USB EP3 trace */
extern int usart_bridge_cur_target;  /* 0x2000001C: Currently selected USART bridge target handle */
extern uint8_t g_target_connected;  /* 0x20001020: Flag: a debug target is currently connected/attached */
extern uint8_t g_conn_lost;  /* 0x20001021: Flag: target connection lost / reconnect-in-progress */
extern uint8_t g_debug_mode_bitbang;  /* 0x20001022: Debug-access backend selector: 1 = bit-bang SWD path, 0 = SPI/DMA (JTAG/hardware) path */
extern uint8_t[] g_ap_state;  /* 0x20001023: Per-AP connection state (1/2/3 = SWD/JTAG/reset-under-reset style connect) */
extern uint8_t g_apkey_count;  /* 0x20001024: Number of active entries in the access-key table */
extern uint8_t g_rom_detected;  /* 0x20001025: Flag set once the ROM table / core has been detected */
extern ushort stlink_mem_offset;  /* 0x20001026: Offset into stlink_data_buf for memory read/write streaming. */
extern ushort stlink_mem_remaining;  /* 0x20001028: Remaining byte count for memory read/write streaming. */
extern uint32_t g_dp_select_cache;  /* 0x2000102C: Cached DP SELECT register value (0xFFF0 = invalid) */
extern uint8_t[] g_ap_csw_cache;  /* 0x20001030: Per-AP cached MEM-AP CSW access-size byte (0xFF = invalid) */
extern uint32_t[] g_ap_tar_cache;  /* 0x20001054: Per-AP cached MEM-AP TAR / access-state value */
extern uint32_t g_detected_partid;  /* 0x20001078: ROM-table component/part id detected during target scan */
extern uint32_t g_last_rw_value;  /* 0x2000107C: Last read/written DAP/DHCSR value (GETLASTRWSTATUS) */
extern uint stlink_reg_tmp2;  /* 0x20001080: Scratch for second register/data word. */
extern uint stlink_tgt_addr;  /* 0x20001084: Current target memory address for RW_MEM operations. */
extern uint stlink_ap_access;  /* 0x20001088: Access-port / transfer attribute word for memory ops. */
extern uint32_t g_current_ap;  /* 0x2000108C: Currently selected access-port index */
extern uint32_t g_requested_ap;  /* 0x20001090: Access-port index requested for the next operation */
extern uint32_t g_reset_vector;  /* 0x20001094: Cached connect-under-reset vector/address (0xFFFFFFFF = unset) */
extern uint32_t g_trace_count;  /* 0x20001098: Number of trace/SWO bytes available to read (capped at 0xBFE) */
extern uint stlink_coreid;  /* 0x20001124: Read core/DP IDCODE value. */
extern uint stlink_dpid;  /* 0x20001128: Second DP/AP ID word. */
extern uint[3] stlink_last_rw_status;  /* 0x2000112C: Default last-RW status record (status word + address). */
extern uint[3][] stlink_ap_status;  /* 0x20001138: Per-AP last-RW status records (status + fault address), indexed by AP id. */
extern undefined1 swim_capture_done_flag;  /* 0x200011B4: Set by swim_capture_complete when a line capture has latched */
extern byte swim_status;  /* 0x200011B5: SWIM operation status/result (1 = running/OK, other = error code). */
extern byte swim_retry_level;  /* 0x200011B6: SWIM retry / header-byte stage counter. */
extern undefined1 swim_error_flag;  /* 0x200011B7: SWIM DMA/line error flag */
extern ushort[] swim_capture_buf;  /* 0x200011B8: TIM2/DMA input-capture timestamps for SWIM bit decoding (DAT_200011ba..cc). */
extern undefined2 swim_captured_period;  /* 0x200011D4: Latched TIM4 capture value (SWIM sync/bit period) */
extern ushort swim_byte_count;  /* 0x200011D6: Total byte count for the SWIM block transfer. */
extern ushort swim_byte_index;  /* 0x200011D8: Current byte index within the SWIM block transfer. */
extern undefined2 swim_timing_table;  /* 0x200011DA: Base of derived SWIM bit-timing constants (DC/DD/DE/E0/E2/E4) */
extern undefined2 swim_bit_len_cur;  /* 0x200011E6: Current SWIM bit length (plus _EA high / _EC low timings) */
extern undefined2 swim_sync_period;  /* 0x200011EE: Measured SWIM sync base period used to derive bit timings */
extern code* swim_state_fn;  /* 0x2000121C: SWIM interrupt state-machine current handler pointer (in SRAM); advanced by swim_state_* functions. */
extern byte* swim_src_ptr;  /* 0x20001220: Source data pointer for SWIM write/verify stream. */
extern undefined4 swim_header_table_ptr;  /* 0x20001228: Pointer/handle to SWIM command-header compare table */
extern undefined2 * swim_bit_buf_ptr;  /* 0x2000122C: Pointer to SWIM TIM4 compare pattern buffer currently being built */
extern undefined2[] swim_pattern_table0;  /* 0x2000127C: SWIM compare pattern table 0 (also 20001254/20001268 tables 1/2) */
extern ushort target_clk_prescaler;  /* 0x200012A0: SPI baud-prescaler bits selecting SWD/JTAG shift frequency. */
extern ushort jtag_last_tap_state;  /* 0x200012A2: Cached last JTAG TAP state (0xFFFF=unknown) used by jtag_tap_goto_state. */
extern uint* spi_status_reg_ptr;  /* 0x20001304: Pointer to the active SPI status register used by jtag_spi_xfer_byte. */
extern uint8_t[9][12] g_ap_slot_table;  /* 0x20001358: Per-AP active-slot table (9 entries, 12 bytes each) */
extern Device_Property usb_Device_Property;  /* 0x200013C4: RAM copy of the Device_Property vtable; first slot (Init) called by USB_Init */
extern uint16_t usb_ep0_buffer_size;  /* 0x200013F0: EP0 TX/RX buffer size constant (0x40) passed to SetEPRxCount */
extern User_Standard_Requests usb_User_Standard_Requests;  /* 0x200013F4: RAM copy of the User_Standard_Requests callback vtable */
extern char[] usb_serial_string;  /* 0x2000141A: Serial-number string-descriptor characters built from the STM32 UID */
extern uint32_t[] g_ap_rombase_cache;  /* 0x2000149C: Per-AP cached ROM-table base/IDCODE (0xFFFFFFFF = invalid) */
extern byte[] swd_dp_read_req_table;  /* 0x200014C0: Table of SWD DP read request bytes (indexed by register), base for swd_read_reg. */
extern byte[] swd_dp_write_req_table;  /* 0x200014C4: Table of SWD DP write request bytes, base for swd_write_reg. */
extern uint32_t g_swd_reconnect_pending;  /* 0x200014C8: Bit-bang SWD auto-reconnect trigger/flag */
extern uint32_t g_swd_reconnect_val;  /* 0x200014CC: Scratch value used by the bit-bang SWD auto-reconnect sequence */
extern uint32_t g_swd_clock_delay;  /* 0x200014D0: Bit-bang SWD/JTAG clock half-period busy-loop count (interface frequency) */
extern byte[] stlink_info_buf;  /* 0x200014F0: Firmware/version/mode info data buffer region (overlaps data buffer area). */
extern undefined4 swd_ap_select;  /* 0x200014F8: Last selected SWD AP index */
extern undefined4 swd_ap_addr;  /* 0x200014FC: Current SWD access-port/address selected in debug access batch */
extern int swd_ap_state;  /* 0x20001500: SWD/AP access state (0x80=OK) in debug access batch */
extern undefined4* vt_app_base_ptr;  /* 0x20001504: Selected application vector-table base pointer. */
extern undefined4* vt_region_base_ptr;  /* 0x20001508: Selected image region base pointer (app vs loader). */
extern uint8_t usb_num_endpoints;  /* 0x20001524: Number of configured endpoints (loop bound for EPnR scans) */
extern uint8_t usb_max_config;  /* 0x20001525: Max valid configuration value for SET_CONFIGURATION validation */
extern byte usb_mode_sel;  /* 0x20001528: USB/app mode select used by main loop (sub_0800DF2C). */
extern byte usb_ep0_idx;  /* 0x20001529: Index of the control endpoint register (EP0R) used when re-arming EP0. */
extern uint8_t usb_ep_out_num;  /* 0x2000152A: Data OUT endpoint number used by the class */
extern undefined1 stlink_current_mode;  /* 0x2000152B: Current ST-Link mode byte returned by GET_CURRENT_MODE. */
extern undefined1 led_cmd_result;  /* 0x2000152C: Last command result byte consumed by the LED blink IRQ. */
extern uint8_t[] g_reply_buf;  /* 0x20001530: Command reply/status buffer sent back to the host on EP1-IN */
extern undefined4[] debug_ap_result_buf;  /* 0x20002D30: Result buffer for batched SWD AP accesses (len DAT_200030C0<<3) */
extern byte[64][N] stlink_ep2out_buf;  /* 0x20002F30: Per-slot 64-byte EP2 bulk-OUT staging buffer. */
extern code*[8] usb_pEpInt_OUT;  /* 0x20002F6C: Table of per-endpoint OUT (correct-transfer) callback pointers */
extern code*[8] usb_pEpInt_IN;  /* 0x20002F88: Table of per-endpoint IN (correct-transfer) callback pointers */
extern uint8_t[10][5] g_apkey_table;  /* 0x20002FA8: Access-key table: 10 entries of 4-byte key + 1 flag byte */
extern Device_Info usb_Device_Info;  /* 0x20003058: ST USB core pInformation storage struct (bmRequestType/bRequest/wValue/wIndex/wLength/ControlState/Current_Feature/Configuration...) */
extern Device_Info* usb_pInformation;  /* 0x20003074: Pointer to the active USB Device_Info (control-pipe state); set to &usb_Device_Info in USB_Init */
extern Device_Property* usb_pProperty;  /* 0x20003078: Pointer to Device_Property vtable (Init/Reset/Status IN/OUT/Class setups/Get*Descriptor/MaxPacketSize) */
extern User_Standard_Requests* usb_pUser_Standard_Requests;  /* 0x2000307C: Pointer to User_Standard_Requests vtable (Get/Set Configuration/Interface/Status/Feature/Address callbacks) */
extern undefined1 stlink_active_subcmd;  /* 0x20003088: Currently executing debug/SWIM subcommand opcode. */
extern undefined1 swim_sched_mode;  /* 0x2000308C: SWIM/command scheduler mode (plus DAT_2000308D param) set by swim_scheduler_set_mode */
extern uint stlink_bulk_remaining;  /* 0x20003090: Remaining byte count for the current bulk read/write transfer. */
extern uint stlink_bulk_offset;  /* 0x20003092: Byte offset into the current bulk transfer buffer. */
extern uint stlink_bulk_total;  /* 0x20003094: Total length of the current bulk transfer. */
extern uint stlink_bulk_addr;  /* 0x20003098: Target address for the current bulk transfer. */
extern byte[16][N] stlink_cmd_buf;  /* 0x2000309C: Per-slot 16-byte command buffer; [0]=command byte (F1-F8/debug), [1]=subcommand, [2..]=params; indexed by slot*0x10. */
extern byte stlink_cmd_subcmd;  /* 0x2000309D: Command buffer byte 1 (subcommand selector within F2/F4 groups). */
extern uint8_t[] g_cmd_block;  /* 0x2000309E: Current 16-byte USB command block fields (command byte, sub-command, address/data words) */
extern byte[N] stlink_cmd_reply_state;  /* 0x200030AC: Per-slot reply/transfer state: 0 idle, 1 busy/more, 2 sending fragment, 3 last/error. */
extern byte[N] stlink_cmd_pending;  /* 0x200030AE: Per-slot flag: a new 16-byte command is pending (copied from EP2 OUT buffer). */
extern undefined1 anticlone_active;  /* 0x200030B0: Anti-clone monitoring active flag. */
extern uint8_t g_trace_active;  /* 0x200030B1: Flag: SWO/trace capture is currently active */
extern undefined1 anticlone_page_counter;  /* 0x200030B2: Anti-clone background write page/row counter. */
extern uint anticlone_word_index;  /* 0x200030B4: Anti-clone background write word index within the page. */
extern undefined4 anticlone_timestamp;  /* 0x200030B8: Last free-running timer snapshot for pacing anti-clone background writes. */
extern uint anticlone_timeout;  /* 0x200030BC: Computed timeout threshold for the anti-clone background task. */
extern uint usb_bulk_count;  /* 0x200030C0: USB bulk debug-access element count */
extern uint usb_bulk_remaining;  /* 0x200030C4: Bytes remaining to accumulate for the bulk debug-access block */
extern uint usb_bulk_offset;  /* 0x200030C8: Current offset into the accumulated bulk debug-access block */
extern uint32_t g_trace_remaining;  /* 0x200030CC: Bytes remaining to stream from the trace buffer */
extern uint32_t g_trace_offset;  /* 0x200030D0: Current offset into the trace buffer being streamed */
extern undefined4 adc_voltage_sample;  /* 0x200030EC: Last ADC1 conversion result (target voltage related). */
extern int swim_retry_count_a;  /* 0x200030F4: SWIM retry counter A (reset on new command) */
extern int swim_retry_count_b;  /* 0x200030F6: SWIM retry counter B */
extern uint16_t usb_StatusInfo;  /* 0x200030F8: GET_STATUS reply word returned by Standard_GetStatus */
extern uint16_t usb_StatusOut;  /* 0x200030FC: Saved EP0 RX status bits (STAT_RX) to restore after CTR processing */
extern uint16_t usb_StatusIn;  /* 0x200030FE: Saved EP0 TX status bits (STAT_TX) to restore after CTR processing */
extern undefined1 board_id_bit;  /* 0x20003100: Detected board-ID/variant bit (GPIOB IDR bit3) */
extern undefined1 target_mode_active;  /* 0x20003101: Flag: target debug IO currently connected/active */
extern undefined1 board_detect_pending;  /* 0x20003102: Flag: board detection not yet performed */
extern uint8_t g_connect_phase;  /* 0x20003103: State/phase marker used during the debug-connect command sequence */
extern uint16_t usb_wInterrupt_Mask;  /* 0x20003104: Cached USB CNTR interrupt-enable mask (e.g. 0x8400/0x1C00) */
extern uint16_t usb_wIstr;  /* 0x20003106: Last-read USB ISTR snapshot */
extern uint8_t usb_send_zlp;  /* 0x20003108: DataStageIn flag: send a terminating zero-length packet */
extern uint8_t usb_EPindex;  /* 0x20003109: Endpoint index of the ISTR event being serviced */
