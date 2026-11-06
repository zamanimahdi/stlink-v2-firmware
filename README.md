# ST-LINK/V2 Firmware

Firmware for the **ST-LINK/V2** in-circuit debugger and programmer — the USB probe built
around an **STM32F103 (Cortex-M3)** that connects a host PC to a target microcontroller and
drives it over **SWD**, **JTAG** or **SWIM**. This repository holds the firmware source,
organised by module, plus a project that builds the application image that **runs on the
ST-LINK/V2 hardware**.

- USB VID `0x0483` / PID `0x3748`
- MCU: STM32F103, Cortex-M3 at 72 MHz
- Application base `0x08004000` (vector table: SP `0x20003910`, reset `0x0800E435`)

---

## How it works

The probe enumerates as a USB device. The host sends **16-byte command packets** on a bulk
OUT endpoint and reads replies on a bulk IN endpoint. A command selects a working mode and
performs an operation on the target:

- **Enumeration & transport** — a USB-FS device stack (control endpoint 0, bulk endpoints for
  commands/replies, a third bulk IN endpoint for SWO trace). The USB serial number is the
  chip's 96-bit unique ID.
- **Command dispatch** — the top-level handler decodes the command byte and, for the debug and
  SWIM groups, a sub-command byte, then calls the matching handler.
- **Target access** — every memory/register/control operation has two back-ends chosen by the
  active mode: **SWD** is bit-banged on GPIO (SWCLK = PB13, SWDIO = PB14); **JTAG** is clocked
  through the SPI peripherals (SPI1/SPI2) with DMA. **SWIM** (for STM8 targets) is an
  interrupt-driven state machine using TIM2/TIM4 and DMA for pulse-width framing.
- **Debug core** — target control uses the standard Cortex-M debug registers: `DHCSR`
  (`0xA05F0003` halt, `0xA05F0001` run, `0xA05F0005` single-step), `DEMCR`, `AIRCR` for reset,
  the **FPB** unit for hardware breakpoints and the **DWT** unit for watchpoints; core
  registers move through `DCRSR`/`DCRDR`. Writes are read back and verified.
- **Status LED** — a bi-colour LED on PA9 driven by TIM1 channel 2 shows connection/activity.

### USB command set

| Command | Meaning |
|---|---|
| `0xF1` | GET_VERSION — ST-Link / JTAG / SWIM versions + USB VID/PID |
| `0xF2` | DEBUG — read/write memory (8/16/32-bit) and core registers, run / halt / step, breakpoints, watchpoints, SWO trace |
| `0xF3` | DFU control (`0x07` = exit to the bootloader) |
| `0xF4` | SWIM — STM8 single-wire transactions |
| `0xF5` | GET_CURRENT_MODE |
| `0xF6` | USART system-bootloader bridge to the target |
| `0xF7` | GET_TARGET_VOLTAGE — ADC channel 0 (PA0) referenced to VREFINT |
| `0xF8` | read board-identification pins (PC13 / PC14) |

---

## Functional overview

The application is organised into these modules (396 functions in total):

| Module | Responsibility |
|---|---|
| `stlink_cmd` | USB command dispatcher and all command handlers |
| `swd` | Serial Wire Debug line protocol (bit-banged) and DAP/AP access |
| `jtag` | JTAG transport over SPI + DMA, TAP state machine, IDCODE scan |
| `swim` | SWIM single-wire interface for STM8 targets |
| `usb_core` / `usb_hw` | USB device core (control transfers) and the USB peripheral / packet-memory layer |
| `usb_class` / `usb_desc` | Bulk-endpoint class layer and USB descriptors |
| `aes` | AES-128 used by the device authenticity check |
| `adc` | Target-voltage measurement |
| `rcc` `gpio` `flash` `dma` `timer` `nvic` `systick` | On-chip peripheral drivers |
| `led` | Status LED control |
| `crt` / `startup` | C runtime, reset path and interrupt handlers |

Each module file carries a one-line description above every function, and
[`src/function_index.csv`](src/function_index.csv) lists every function with its address,
module and description.

---

## Repository layout

```
src/                     firmware source, by module
  app/                     one file per module (swd, jtag, swim, usb_*, stlink_cmd, ...)
    globals.h              SRAM variables used by the application
  include/
    stm32f103_regs.h       peripheral register definitions
    sram_globals.h         RAM globals
    types.h                fixed-width type aliases
  function_index.csv       every function: address, module, name, description

rebuild/                 project that builds the running image (STM32CubeIDE)
  Src/
    stlink_app.s           the application in assembly
    sig.s  blob1.s  blob2.s configuration / data blocks
  STM32F103CBTX_FLASH.ld   linker script (fixed section addresses)
  .project  .cproject      STM32CubeIDE project files
```

---

## Building and running on ST-LINK/V2

The project under [`rebuild/`](rebuild/) builds the application image that runs on ST-LINK/V2
hardware (STM32F103).

**STM32CubeIDE:** `File → Import → Existing Projects into Workspace`, select `rebuild/`, then
Build. The output is in `Debug/` (`.elf`, `.hex`, `.bin`).

**Command line** (arm-none-eabi toolchain):

```bash
cd rebuild
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c -x assembler-with-cpp Src/stlink_app.s -o stlink_app.o
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c -x assembler-with-cpp Src/sig.s   -o sig.o
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c -x assembler-with-cpp Src/blob1.s -o blob1.o
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c -x assembler-with-cpp Src/blob2.s -o blob2.o
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -nostartfiles -nostdlib \
    -T STM32F103CBTX_FLASH.ld stlink_app.o sig.o blob1.o blob2.o -o stlink.elf
arm-none-eabi-objcopy -O binary --gap-fill 0xff stlink.elf stlink.bin
```

Flash the resulting image to an ST-LINK/V2 (STM32F103) over SWD; the application starts at
`0x08004000` beneath the ST-Link bootloader. The build uses `-nostartfiles -nostdlib` because
the startup code and C runtime are part of the application itself.

The `src/` tree is an annotated reference describing each function and how the probe works;
the runnable image is produced by the `rebuild/` project above.

## Contact

If you'd like to get in touch, feel free to email **timahdizamani@gmail.com**.

## License

Released under the [MIT License](LICENSE).
