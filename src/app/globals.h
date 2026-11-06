/* app: shared declarations */
#pragma once
#include "../include/sram_globals.h"

/* Function prototypes */
int dap_read_reg(int apndp, int reg_addr, uint32_t *out, uint32_t unused);  /* 0x080040EC */
int dap_read_reg_checked(int apndp, int reg_addr, uint32_t *out);  /* 0x08004132 */
int dap_write_reg(int apndp, int reg_addr, uint32_t data, uint32_t unused);  /* 0x08004146 */
int dap_write_reg_checked(int apndp, int reg_addr, uint32_t data);  /* 0x08004192 */
uint64_t dap_check_ctrl_stat(int is_write);  /* 0x080041A6 */
void dap_read_rdbuff(void);  /* 0x08004208 */
uint64_t dap_finish_transfer(uint32_t v, int status, uint32_t a, uint32_t b);  /* 0x0800425E */
int jtag_shift_short(uint32_t w0, uint32_t w1, uint32_t a, uint32_t b);  /* 0x08004282 */
uint64_t jtag_shift_ir_dr(int mode, uint32_t ir, uint32_t dr_lo, uint32_t dr_hi);  /* 0x080042B0 */
uint32_t dap_ack_to_status(uint32_t ack_bits);  /* 0x08004312 */
uint64_t dap_identify_ap(uint8_t *is_known);  /* 0x0800438A */
uint64_t dap_read_part_id(uint32_t *out_partno);  /* 0x0800443C */
uint64_t target_connect(int ap_index, uint32_t mode_arg, uint32_t conn_type, uint32_t reset_vector);  /* 0x080044A0 */
uint64_t dap_power_up_debug(void);  /* 0x080045DA */
int rom_read_component_id(int base, uint32_t *out_partno, uint8_t *is_cm, int *out_class);  /* 0x08004650 */
uint32_t target_scan_rom_table(void);  /* 0x080046E0 */
uint32_t target_part_is_special(void);  /* 0x08004838 */
uint32_t target_slot_check(uint32_t ap_index);  /* 0x08004866 */
int target_select_ap(uint32_t ap_index);  /* 0x0800488E */
void target_disconnect(int ap_index);  /* 0x0800499C */
void dap_select_ap(uint32_t ap_bank);  /* 0x080049E4 */
int dap_select_dpbank(uint32_t dpbanksel);  /* 0x08004A3C */
uint64_t mem_ap_set_csw(uint32_t size_mode, uint32_t ap_sel, uint32_t a, uint32_t b);  /* 0x08004A86 */
uint32_t target_drive_nrst(int mode);  /* 0x08004B06 */
void mem_read_word(uint32_t addr, uint32_t *out, uint32_t ap);  /* 0x08004B3C */
uint64_t cortexm_write_reg_verify(uint32_t value, uint32_t reg, uint32_t a, int b);  /* 0x08004B50 */
uint64_t cortexm_mem_ap_access_retry(uint32_t a, uint32_t b);  /* 0x08004C14 */
int dap_get_last_rw_status(uint32_t *out_status, uint32_t *out_value);  /* 0x08004D14 */
void cortexm_read_all_regs(int mode);  /* 0x08004D78 */
void cortexm_check_halted(uint8_t *halted);  /* 0x08004DBC */
void dwt_set_watchpoint(int comp_idx, uint32_t addr, int func, int mask_idx, uint32_t data, int enable);  /* 0x08004DFC */
void fpb_set_breakpoint(uint32_t addr, int comp_idx, int half, uint32_t a);  /* 0x08004EC0 */
void cortexm_reset_and_halt(uint8_t *ok, uint32_t a, uint32_t b, uint32_t c);  /* 0x08004F24 */
void cortexm_step(uint8_t *ok);  /* 0x08004FFE */
void cortexm_halt(void);  /* 0x08005078 */
void mem_read_32(uint32_t addr, uint32_t *buf, uint32_t count, uint32_t *out_fail_addr, uint32_t ap);  /* 0x080050FC */
uint64_t mem_read_32_noinc(int addr, int buf, uint32_t count, int *out_fail_addr, uint32_t ap);  /* 0x080051DE */
void mem_read_16(uint32_t addr, uint16_t *buf, uint32_t count, uint32_t *out_fail_addr, uint32_t ap);  /* 0x080052A0 */
void mem_read_8(uint32_t addr, uint8_t *out, uint32_t ap);  /* 0x0800539A */
void mem_write_32(uint32_t addr, uint32_t *buf, uint32_t count, uint32_t ap);  /* 0x08005402 */
void mem_write_32_noinc(uint32_t addr, uint32_t *buf, uint32_t count, uint32_t ap);  /* 0x08005474 */
void mem_write_16(uint32_t addr, uint16_t *buf, uint32_t count, uint32_t ap);  /* 0x08005504 */
void mem_write_8(uint32_t addr, int value, uint32_t ap);  /* 0x0800557E */
void cortexm_write_dhcsr_word(void);  /* 0x080055F2 */
uint64_t stlink_send_reply(uint32_t ep, uint8_t *reply, uint32_t len, int keep);  /* 0x08005620 */
uint64_t stlink_cmd_set_freq(int set, int cmd_idx);  /* 0x0800566C */
void stlink_cmd_get_conn_status(uint32_t ep);  /* 0x080056AC */
void stlink_cmd_rw_dap_reg(int cmd_idx, int is_read);  /* 0x08005714 */
uint64_t stlink_cmd_set_led_ack(uint32_t ep);  /* 0x0800592C */
uint8_t stlink_cmd_get_fw_build(uint32_t ep);  /* 0x08005950 */
uint32_t stlink_cmd_get_version(uint32_t ep);  /* 0x080059A0 */
uint64_t stlink_cmd_get_param_block(int cmd_idx, uint32_t a, uint32_t b, uint32_t c);  /* 0x08005A2C */
uint32_t stlink_cmd_jtag_raw(int cmd_idx, uint32_t a, uint32_t b, uint32_t c);  /* 0x08005AC2 */
void stlink_cmd_get_caps(uint32_t ep);  /* 0x08005B3A */
uint32_t stlink_cmd_read_trace_buf(int cmd_idx, uint32_t a, uint32_t b, uint32_t ep);  /* 0x08005B88 */
void stlink_cmd_get_rw_status(uint32_t ep, uint32_t a, uint32_t b, uint32_t c);  /* 0x08005C20 */
uint64_t cortexm_run_core(int ap_index);  /* 0x08005C50 */
uint32_t apkey_table_lookup(char *key, uint8_t *slot);  /* 0x08005C98 */
void ap_tables_reset(void);  /* 0x08005D50 */
uint64_t cmd_check_allowed(char *key, int cmd, uint8_t *out_slot);  /* 0x08005DA4 */
uint32_t apkey_table_add(uint8_t *key, uint8_t flag, int slot);  /* 0x08005E20 */
uint32_t apkey_table_remove(int slot);  /* 0x08005E84 */
uint64_t stlink_cmd_debug_connect(int cmd_idx, uint32_t sub, uint32_t a, uint32_t b);  /* 0x08005ECC */
uint32_t swd_set_clock_delay(uint32_t delay_count);  /* 0x08007630 */
uint32_t swd_init_gpio(void);  /* 0x0800763A */
uint64_t jtag_init_gpio(void);  /* 0x080076F2 */
uint32_t swd_bitbang_request_fast(uint32_t request, uint8_t *ack);  /* 0x0800773E */
uint32_t swd_bitbang_send_request(uint32_t request, uint8_t *ack);  /* 0x080077A4 */
int swd_bitbang_read_word(uint32_t request, uint32_t *out, uint32_t no_retry);  /* 0x080079B2 */
uint32_t swd_bitbang_write_word(uint32_t request, uint32_t value);  /* 0x08007C52 */
undefined4 swd_bb_write_word(int delay_sel, uint data);  /* 0x08007D62 */
uint swd_read_access(byte request, uint *data_out, char no_retry);  /* 0x08007E5A */
undefined4 swd_write_access(byte request, uint data);  /* 0x08007EB2 */
undefined4 jtag_set_frequency(int freq_code);  /* 0x0800828C */
uint jtag_reverse_bits(byte *p);  /* 0x08008438 */
int jtag_tap_goto_state(int tms_a, uint tms_b);  /* 0x08008692 */
undefined1 anticlone_is_active(void);  /* 0x08008CFC */
void anticlone_verify_signature(void);  /* 0x08008D02 */
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *init);  /* 0x08008FE2 */
bool GPIO_ReadInputDataBit(int GPIOx, uint pin);  /* 0x0800909E */
void GPIO_SetBits(int GPIOx, undefined4 pins);  /* 0x080090AC */
void GPIO_ResetBits(int GPIOx, undefined4 pins);  /* 0x080090B0 */
void GPIO_WriteBit(int GPIOx, undefined4 pins, int val);  /* 0x080090B4 */
void GPIO_PinRemapConfig(uint remap, int newstate);  /* 0x080090BE */
void set_msp_if_privileged(undefined4 sp);  /* 0x0800922C */
void nop_function(void);  /* 0x08009234 */
undefined4 select_vector_table(void);  /* 0x08009238 */
undefined4 check_app_valid(void);  /* 0x08009262 */
undefined4 stlink_set_mode(int mode);  /* 0x08009360 */
undefined4 adc_check_voltage(void);  /* 0x0800938A */
int util_time_diff(int a, int b);  /* 0x080093FC */
undefined4 timer_delay_ticks(uint ticks);  /* 0x08009400 */
void __scatterload_zeroinit(int *table);  /* 0x08009416 */
void led_set_result(undefined1 ok);  /* 0x08009438 */
undefined1 led_get_result(void);  /* 0x0800943E */
void led_tim1_init(void);  /* 0x08009444 */
uint led_set_mode(uint mode, uint arg, int count);  /* 0x08009498 */
void led_blink_activity(void);  /* 0x08009624 */
undefined4 *__aeabi_memclr(int dst, uint n);  /* 0x0800962C */
uint stlink_dispatch_command(int ep);  /* 0x08009700 */
uint stlink_cmd_process(int chan, undefined4 p2, undefined4 p3, undefined4 p4);  /* 0x08009772 */
void stlink_cmd_send_reply(int chan, void *data, uint len, int last);  /* 0x08009854 */
void stlink_cmd_set_status_busy(int chan);  /* 0x08009886 */
void stlink_cmd_set_status_idle(int chan);  /* 0x08009894 */
int stlink_cmd_is_first_block(uint chan);  /* 0x080098A2 */
void stlink_cmd_reset_status(void);  /* 0x080098B6 */
void stlink_cmd_reply_unknown(int chan);  /* 0x080098BE */
void *__aeabi_memcpy(void *dst, const void *src, uint n);  /* 0x0800991C */
void *__aeabi_memset(int dst, uint len, uint c);  /* 0x08009994 */
void swim_dma_init(void);  /* 0x08009A08 */
void swim_init(void);  /* 0x08009B0E */
void swim_state_read_start(void);  /* 0x08009D42 */
void swim_state_write_start(void);  /* 0x08009E18 */
void swim_state_read_prep(void);  /* 0x08009EAC */
char swim_read_bits(undefined1 *out_byte);  /* 0x08009F90 */
void swim_state_read_loop(void);  /* 0x0800A154 */
void swim_state_write_block(void);  /* 0x0800A240 */
void swim_state_write_wait(void);  /* 0x0800A290 */
void swim_state_write_resync(void);  /* 0x0800A2F8 */
void swim_state_write_next(void);  /* 0x0800A33C */
void swim_state_write_second(void);  /* 0x0800A3C8 */
undefined4 swim_state_write_advance(void);  /* 0x0800A404 */
void swim_state_entry_seq(void);  /* 0x0800A52C */
void swim_state_wait_sync(void);  /* 0x0800A574 */
void swim_state_sync_done(void);  /* 0x0800A5AC */
void swim_state_start_frame(void);  /* 0x0800A618 */
void swim_build_byte_pattern(int byte);  /* 0x0800A708 */
void swim_send_pattern_dma(void);  /* 0x0800A774 */
void swim_send_header_dma(void *table);  /* 0x0800A800 */
void swim_build_timing_tables(int mode);  /* 0x0800A89C */
void swim_rx_complete_dispatch(void);  /* 0x0800A934 */
void swim_dma_ch7_err_isr(void);  /* 0x0800A9DC */
void swim_dma_ch1_isr(void);  /* 0x0800AA1E */
void swim_capture_complete(void);  /* 0x0800AA56 */
void swim_abort_to_idle(void);  /* 0x0800AA88 */
undefined8 swim_deinit(void);  /* 0x0800AAA8 */
void swim_wait_line_high(void);  /* 0x0800AB80 */
undefined8 stlink_debug_ap_access_batch(undefined1 *ops, uint count);  /* 0x0800ACEC */
undefined4 usb_bulk_debug_access(int ep,undefined4,undefined4,undefined4);  /* 0x0800ADEC */
void stlink_debug_get_result_ptr(undefined2 *len,undefined4 **buf);  /* 0x0800AE7E */
void usart_bridge_dma_init(undefined4 baud);  /* 0x0800AEA8 */
undefined4 usart_bridge_init(void);  /* 0x0800AEFA */
longlong usart_bridge_set_baud(int baud,uint len);  /* 0x0800B062 */
void usart_bridge_close(void);  /* 0x0800B122 */
ushort usart_bridge_poll_rx(void);  /* 0x0800B1D2 */
undefined4 usart_bridge_send_trace(void);  /* 0x0800B22E */
void usart_bridge_dma_tc_handler(void);  /* 0x0800B270 */
undefined4 spi_reset_pulse(undefined *spi_base);  /* 0x0800B338 */
void gpio_cfg_or_merge(ushort *dst,ushort *src);  /* 0x0800B380 */
void SPI_Cmd(ushort *spi,int enable);  /* 0x0800B3AE */
void spi_cr2_bits_config(int spi,ushort mask,int enable);  /* 0x0800B3C2 */
void spi_ssoe_config(int spi,int enable);  /* 0x0800B3D2 */
void DMA_DeInit(uint *chan);  /* 0x0800B3E8 */
void DMA_Init(uint *chan,uint *init);  /* 0x0800B462 */
void DMA_Cmd(uint *chan,int enable);  /* 0x0800B498 */
void DMA_ITConfig(uint *chan,uint it,int enable);  /* 0x0800B4AA */
uint DMA_GetCurrDataCounter(int chan);  /* 0x0800B4BA */
undefined8 jtag_spi_dma_xfer(undefined4,undefined1 *out,undefined4,undefined4);  /* 0x0800B4D0 */
undefined4 jtag_spi_wait_ready(void);  /* 0x0800B604 */
void RCC_DeInit(void);  /* 0x0800B6A0 */
void RCC_HSEConfig(int state);  /* 0x0800B6E0 */
undefined4 RCC_WaitForHSEStartUp(void);  /* 0x0800B712 */
void RCC_PLLConfig(uint src,uint mul);  /* 0x0800B740 */
void RCC_PLLCmd(undefined4 enable);  /* 0x0800B750 */
void RCC_SYSCLKConfig(uint sw);  /* 0x0800B756 */
uint RCC_GetSYSCLKSource(void);  /* 0x0800B764 */
void RCC_HCLKConfig(uint hpre);  /* 0x0800B76E */
void RCC_PCLK1Config(uint ppre1);  /* 0x0800B778 */
void RCC_PCLK2Config(int ppre2);  /* 0x0800B786 */
void RCC_USBCLKConfig(undefined4 div);  /* 0x0800B796 */
void RCC_LSEConfig(uint bits);  /* 0x0800B79C */
void RCC_RTCCLKCmd(undefined4 enable);  /* 0x0800B7A2 */
void RCC_GetClocksFreq(uint *clocks);  /* 0x0800B7A8 */
void RCC_AHBPeriphClockCmd(uint p,int en);  /* 0x0800B826 */
void RCC_APB2PeriphClockCmd(uint p,int en);  /* 0x0800B838 */
void RCC_APB1PeriphClockCmd(uint p,int en);  /* 0x0800B84A */
void RCC_APB2PeriphResetCmd(uint p,int en);  /* 0x0800B85C */
void RCC_APB1PeriphResetCmd(uint p,int en);  /* 0x0800B86E */
void RCC_BackupResetCmd(undefined4 enable);  /* 0x0800B880 */
void RCC_MCOConfig(undefined1 mco);  /* 0x0800B886 */
uint RCC_GetFlagStatus(uint flag);  /* 0x0800B88C */
undefined4 *aes_buffer_clear(undefined4 *buf,uint len);  /* 0x0800B90C */
void aes_key_expansion(uint *key,uint *roundkeys);  /* 0x0800B914 */
void aes_encrypt_block(uint *in,uint *out,uint *roundkeys);  /* 0x0800B982 */
void FLASH_SetLatency(uint latency);  /* 0x0800BD50 */
void FLASH_PrefetchBufferCmd(uint state);  /* 0x0800BD66 */
void FLASH_Unlock(void);  /* 0x0800BD72 */
void FLASH_ErasePage(undefined4 addr);  /* 0x0800BD84 */
void FLASH_ProgramWord(undefined2 *addr,undefined4 data);  /* 0x0800BDC2 */
void FLASH_ProgramHalfWord(undefined2 *addr,undefined2 data);  /* 0x0800BE02 */
undefined8 FLASH_WaitForLastOperation(int timeout,undefined4,undefined4,int);  /* 0x0800BE34 */
undefined4 RTC_GetCounter(void);  /* 0x0800BEB8 */
void RTC_SetPrescaler(undefined4 presc);  /* 0x0800BEC4 */
void RTC_WaitForLastTask(void);  /* 0x0800BEE4 */
void RTC_WaitForSynchro(void);  /* 0x0800BEEE */
void ADC_Init(int adc,uint *init);  /* 0x0800BF0C */
void ADC_Cmd(int adc,int enable);  /* 0x0800BF48 */
void ADC_ResetCalibration(int adc);  /* 0x0800BF5A */
uint ADC_GetResetCalibrationStatus(int adc);  /* 0x0800BF64 */
void ADC_StartCalibration(int adc);  /* 0x0800BF6A */
uint ADC_GetCalibrationStatus(int adc);  /* 0x0800BF74 */
void ADC_SoftwareStartConvCmd(int adc,int enable);  /* 0x0800BF7E */
void ADC_RegularChannelConfig(int adc,int ch,int rank,int smp);  /* 0x0800BF90 */
uint ADC_GetConversionValue(int adc);  /* 0x0800C00E */
void ADC_TempSensorVrefintCmd(int enable);  /* 0x0800C014 */
bool ADC_GetFlagStatus(uint *adc,uint flag);  /* 0x0800C038 */
undefined8 app_hw_init(void);  /* 0x0800C048 */
undefined4 target_mco_io_config(void);  /* 0x0800C0A4 */
void rtc_lse_init(void);  /* 0x0800C0D2 */
void rcc_set_system_clock(void);  /* 0x0800C10C */
undefined8 target_io_reinit(void);  /* 0x0800C18E */
undefined4 stlink_mode_leave(int mode);  /* 0x0800C1C6 */
undefined4 stlink_mode_enter(int mode,undefined4,undefined4,undefined4);  /* 0x0800C202 */
undefined8 board_detect_variant(void);  /* 0x0800C252 */
undefined4 stub_return_neg1(void);  /* 0x0800C294 */
undefined4 usb_interrupt_config(void);  /* 0x0800C2A4 */
void gpio_pa15_write(int high);  /* 0x0800C348 */
undefined8 stlink_target_mode_apply(undefined4,undefined4,undefined4,undefined4);  /* 0x0800C35C */
undefined8 gpio_pb0_init(void);  /* 0x0800C444 */
undefined4 swd_jtag_pins_init(void);  /* 0x0800C478 */
void TIM1_reset(void);  /* 0x0800C524 */
void tim1_oc2_load_config(undefined2 *cfg);  /* 0x0800C53E */
void TIM_OCStructInit(undefined2 *oc);  /* 0x0800C580 */
void TIM1_Cmd(undefined4 enable);  /* 0x0800C594 */
void TIM1_CtrlPWMOutputs(undefined4 enable);  /* 0x0800C59C */
void tim1_dier_config(ushort bits,int enable);  /* 0x0800C5A4 */
void TIM1_OnePulseMode(undefined4 enable);  /* 0x0800C5B8 */
void tim1_oc2_mode(short mode);  /* 0x0800C5BE */
void tim1_oc2_preload(undefined4 enable);  /* 0x0800C5C8 */
void UserToPMABufferCopy(undefined2 *src,int pma_off,int nbytes);  /* 0x0800C600 */
void PMAToUserBufferCopy(undefined2 *dst,int pma_off,int nbytes);  /* 0x0800C628 */
void SetEPTxStatus(int ep,int status);  /* 0x0800C66A */
void usb_ep_clear_ctr_rx(int ep);  /* 0x0800C6F4 */
void usb_ep_clear_ctr_tx(int ep);  /* 0x0800C718 */
void SetEPTxAddr(int ep,uint addr);  /* 0x0800C73C */
void SetEPRxAddr(int ep,uint addr);  /* 0x0800C73E */
uint GetEPTxAddr(int ep);  /* 0x0800C74C */
uint GetEPRxAddr(int ep);  /* 0x0800C74E */
void SetEPTxCount(int ep,undefined4 count);  /* 0x0800C75C */
void SetEPRxCount(int ep,uint count);  /* 0x0800C76A */
uint GetEPRxCount(int ep);  /* 0x0800C7A2 */
void SetEPTxAddr_unused(int ep,uint addr);  /* 0x0800C7B0 */
uint GetEPTxAddr_unused(int ep);  /* 0x0800C7C6 */
uint swap_bytes_u16(uint v);  /* 0x0800C7FC */
void swim_scheduler_set_mode(int mode,undefined2,undefined4,undefined4);  /* 0x0800C808 */
void swim_init_counters(int mode);  /* 0x0800C836 */
undefined4 swim_command_dispatch(undefined4 cmd);  /* 0x0800C880 */
void swim_read_setup(int len,undefined4 addr,undefined4);  /* 0x0800C8DC */
void swim_write_setup(int len,undefined4 addr,undefined4);  /* 0x0800C920 */
void NVIC_SetPriorityGrouping(uint group);  /* 0x0800CD20 */
void NVIC_Init(byte *init);  /* 0x0800CD2A */
void __disable_irq(void);  /* 0x0800CDAA */
void __enable_irq(void);  /* 0x0800CDAE */
int set_basepri(int prio);  /* 0x0800CDB2 */
void NVIC_TriggerSWInterrupt(undefined4 irqn);  /* 0x0800CDB8 */
void NVIC_ClearPendingIRQ(uint irqn);  /* 0x0800CDBE */
void NVIC_SetVectorTable(uint base,uint off);  /* 0x0800CDD0 */
void SysTick_SetReload(undefined4 reload);  /* 0x0800CE00 */
void SysTick_CounterCmd(int cmd);  /* 0x0800CE06 */
void SysTick_ITConfig(int enable);  /* 0x0800CE20 */
undefined4 TIM_DeInit(undefined2 *tim);  /* 0x0800CE3C */
void TIM_TimeBaseInit(ushort *tim,ushort *init);  /* 0x0800CE8C */
undefined4 USART_DeInit(undefined2 *usart);  /* 0x0800CEB0 */
void usart_init(uint16_t *USARTx, USART_InitTypeDef *init);  /* 0x0800CF38 */
void usart_clock_config(uint32_t USARTx, uint16_t *clk_init);  /* 0x0800CFC8 */
void usart_cr3_bit_config(uint32_t USARTx, uint16_t mask, int enable);  /* 0x0800CFE0 */
void util_bitband_write(uint32_t val);  /* 0x0800D030 */
void util_toggle_pulse(void);  /* 0x0800D03C */
void nvic_enable_irq(void);  /* 0x0800D09A */
void nvic_disable_irq(void);  /* 0x0800D09E */
uint8_t *Standard_GetConfiguration(uint16_t Length);  /* 0x0800D0CC */
RESULT Standard_SetConfiguration(void);  /* 0x0800D0E6 */
uint8_t *Standard_GetInterface(uint16_t Length);  /* 0x0800D11A */
RESULT Standard_SetInterface(void);  /* 0x0800D13E */
uint8_t *Standard_GetStatus(uint16_t Length);  /* 0x0800D186 */
RESULT Standard_ClearFeature(void);  /* 0x0800D218 */
RESULT Standard_SetEndPointFeature(void);  /* 0x0800D2EC */
void DataStageIn(void);  /* 0x0800D37A */
RESULT NoData_Setup0(void);  /* 0x0800D422 */
void Data_Setup0(void);  /* 0x0800D500 */
RESULT Setup0_Process(void);  /* 0x0800D6B4 */
RESULT In0_Process(void);  /* 0x0800D71C */
RESULT Out0_Process(void);  /* 0x0800D76A */
void usb_hw_set_device_address(uint8_t addr);  /* 0x0800D854 */
void usb_class_set_configured(void);  /* 0x0800D932 */
void usb_class_reset(void);  /* 0x0800D950 */
void usb_hw_power_on(void);  /* 0x0800DB64 */
uint32_t usb_hw_power_off(void);  /* 0x0800DB86 */
uint32_t usb_hw_ctr_lp(void);  /* 0x0800DBA8 */
void usb_hw_ctr_hp(void);  /* 0x0800DCE8 */
void usb_desc_build_serial(int ascii_mode);  /* 0x0800DDA0 */
void usb_desc_init(void);  /* 0x0800DE62 */
void APP_main(void);  /* 0x0800DF2C */
void USB_Init(void);  /* 0x0800E060 */
void APP_SysTick_Handler(void);  /* 0x0800E080 */
void APP_WWDG_IRQHandler(void);  /* 0x0800E084 */
void APP_PVD_IRQHandler(void);  /* 0x0800E086 */
void APP_TAMPER_IRQHandler(void);  /* 0x0800E088 */
void APP_RTC_IRQHandler(void);  /* 0x0800E08A */
void APP_FLASH_IRQHandler(void);  /* 0x0800E08C */
void APP_RCC_IRQHandler(void);  /* 0x0800E08E */
void APP_EXTI0_IRQHandler(void);  /* 0x0800E090 */
void APP_EXTI1_IRQHandler(void);  /* 0x0800E094 */
void APP_EXTI2_IRQHandler(void);  /* 0x0800E096 */
void APP_EXTI3_IRQHandler(void);  /* 0x0800E098 */
void APP_EXTI4_IRQHandler(void);  /* 0x0800E09A */
void APP_DMA1_Channel1_IRQHandler(void);  /* 0x0800E09C */
void APP_DMA1_Channel2_IRQHandler(void);  /* 0x0800E0A0 */
void APP_DMA1_Channel3_IRQHandler(void);  /* 0x0800E0A6 */
void APP_DMA1_Channel4_IRQHandler(void);  /* 0x0800E0AE */
uint16_t APP_DMA1_Channel5_IRQHandler(void);  /* 0x0800E0B8 */
void APP_DMA1_Channel6_IRQHandler(void);  /* 0x0800E0C8 */
void APP_DMA1_Channel7_IRQHandler(void);  /* 0x0800E0D0 */
void APP_USB_HP_CAN1_TX_IRQHandler(void);  /* 0x0800E0D4 */
void APP_USB_LP_CAN1_RX0_IRQHandler(void);  /* 0x0800E0D8 */
void APP_EXTI9_5_IRQHandler(void);  /* 0x0800E0DC */
void APP_TIM1_BRK_IRQHandler(void);  /* 0x0800E0DE */
void APP_TIM1_UP_IRQHandler(void);  /* 0x0800E0E0 */
void APP_TIM1_TRG_COM_IRQHandler(void);  /* 0x0800E116 */
uint32_t APP_TIM1_CC_IRQHandler(void);  /* 0x0800E118 */
void APP_TIM2_IRQHandler(void);  /* 0x0800E170 */
void APP_TIM3_IRQHandler(void);  /* 0x0800E172 */
void APP_TIM4_IRQHandler(void);  /* 0x0800E174 */
void APP_I2C1_EV_IRQHandler(void);  /* 0x0800E178 */
void APP_I2C1_ER_IRQHandler(void);  /* 0x0800E17A */
void APP_I2C2_EV_IRQHandler(void);  /* 0x0800E17C */
void APP_I2C2_ER_IRQHandler(void);  /* 0x0800E17E */
void APP_SPI1_IRQHandler(void);  /* 0x0800E180 */
void APP_SPI2_IRQHandler(void);  /* 0x0800E182 */
void APP_USART1_IRQHandler(void);  /* 0x0800E184 */
void APP_USART2_IRQHandler(void);  /* 0x0800E186 */
void APP_USART3_IRQHandler(void);  /* 0x0800E188 */
void APP_EXTI15_10_IRQHandler(void);  /* 0x0800E18A */
void APP_RTC_Alarm_IRQHandler(void);  /* 0x0800E18C */
void APP_USBWakeUp_IRQHandler(void);  /* 0x0800E18E */
uint32_t USB_Istr(void);  /* 0x0800E214 */
uint8_t crt_decompress_read_byte(uint32_t *state);  /* 0x0800E268 */
void *__decompress(uint32_t *src);  /* 0x0800E28A */
void crt_abort(void);  /* 0x0800E33A */
void _sys_exit(int code);  /* 0x0800E340 */
void __rt_lib_init(void);  /* 0x0800E354 */
uint32_t crt_rt_lib_shutdown(uint32_t a);  /* 0x0800E3CC */
void crt_run_fnlist(int *start, int *end);  /* 0x0800E3EC */
int crt___rt_entry(void);  /* 0x0800E404 */
int crt_rt_lib_init_guard(void);  /* 0x0800E41E */
void crt_exit(int code);  /* 0x0800E422 */
void crt_exit_alt(int code);  /* 0x0800E428 */
void APP_Reset_Handler(void);  /* 0x0800E434 */
void crt___rt_entry_main(void);  /* 0x0800E454 */
void APP_NMI_Handler(void);  /* 0x0800E460 */
void APP_HardFault_Handler(void);  /* 0x0800E464 */
void APP_MemManage_Handler(void);  /* 0x0800E468 */
void APP_BusFault_Handler(void);  /* 0x0800E46C */
void APP_UsageFault_Handler(void);  /* 0x0800E470 */
void APP_SVC_Handler(void);  /* 0x0800E474 */
void APP_DebugMon_Handler(void);  /* 0x0800E478 */
void APP_PendSV_Handler(void);  /* 0x0800E47C */
void APP_ADC1_2_IRQHandler(void);  /* 0x0800E480 */
void APP_CAN1_RX1_IRQHandler(void);  /* 0x0800E484 */
void APP_CAN1_SCE_IRQHandler(void);  /* 0x0800E488 */
