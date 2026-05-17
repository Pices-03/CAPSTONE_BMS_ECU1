# BMS Project

---

## ⚡ Hướng dẫn bảo trì file này

> **Quy tắc:** Mỗi khi thay đổi thiết kế, cập nhật đúng section tương ứng bên dưới.
> Claude Cowork đọc file này làm "nguồn sự thật duy nhất" — nếu file lỗi thời, output sẽ sai.

| Khi bạn thay đổi...                        | Cập nhật section   | Ví dụ cụ thể                                         |
|---------------------------------------------|--------------------|------------------------------------------------------|
| Thêm/bớt/sửa CAN message                   | §3 + §12           | Thêm dòng vào bảng §3, thêm test spec vào §12        |
| Thêm/sửa fault code                        | §4 + §12           | Thêm code + threshold + test case biên               |
| Đổi I2C packet STM32                       | §2 + §12           | Sửa byte map + cập nhật test valid/invalid packet    |
| Đổi cách tính SOC/Prediction               | §5 (timing) + §12  | Ghi rõ công thức mới + cập nhật boundary values     |
| Đổi coding convention / comment style      | §6 + §7            | Sửa rule + template Doxygen                          |
| Thêm module mới                            | §5 (module map) + §12 | Thêm vào cây thư mục + thêm spec hàm mới         |
| Đổi system tick period                     | §2 (GPT config) + §5  | Cập nhật bảng RTD field + sửa ticks trong code   |
| Phát hiện bug mới                          | §9 (backlog)       | Thêm dòng với priority                               |
| Một issue đã fix                           | §9 (backlog)       | Xoá dòng hoặc đánh dấu Done                         |

---

## 1. Project Overview

| Field            | Value                                                      |
|------------------|------------------------------------------------------------|
| Project type     | Academic — AUTOSAR Classic architecture study              |
| Primary focus    | MCAL configuration + CDD (Custom Device Driver) authoring  |
| MCU              | NXP S32K144 (Cortex-M4F, 112 MHz)                         |
| AUTOSAR stack    | NXP RTD (Real-Time Drivers) — AUTOSAR Classic R21/R22      |
| IDE / Toolchain  | S32 Design Studio (S32DS), GCC ARM                         |
| OS               | None — bare-metal, timer-based scheduler via GPT/LPIT      |
| CAN baudrate     | 500 kbps — FlexCAN0 — PTE4 (RX) / PTE5 (TX)               |
| I2C bus          | LPI2C0 — PTA2 (SDA) / PTA3 (SCL) — 100 kHz                |
| System tick      | LPIT_0 CH_0 — 50 ms period — 48 MHz clock                  |
| **Demo setup**   | **S32K144 5V rail + resistor (current) → tiến tới 1 cell pin thật** |
| **Pack topology**| **1S Li-ion** (cell ≡ pack; chuyển sang nS chỉ cần đổi OCV table + OV threshold) |
| **Temperature**  | **STM32 slave** (1 NTC) — KHÔNG có ESP32 trong project        |
| **SOC algorithm**| **Coulomb Counting** (OCV lookup là dead-code, re-enable bằng `BMS_SOC_DEMO_MODE=STD_OFF`) |

---

## 2. Hardware Peripherals

| Device           | Interface   | Address | Role                                 |
|------------------|-------------|---------|--------------------------------------|
| INA219           | I2C LPI2C0  | 0x40    | Bus voltage, current, power monitor  |
| STM32 (slave)    | I2C LPI2C0  | 0x10    | Temperature sensor (NTC resistor)    |
| FlexCAN0         | CAN         | —       | Vehicle communication bus            |
| LPIT_0 CH_0      | Internal    | —       | System tick 50 ms — GPT MCAL         |

### GPT / LPIT Configuration (RTD S32DS)
| RTD Field                  | Value                                              |
|----------------------------|----------------------------------------------------|
| GptHwIp                    | LPIT                                               |
| GptModuleRef               | /Gpt/Gpt/GptChannelConfigSet/Lpit_0/LpitChannel_0  |
| GptChannelMode             | GPT_CH_MODE_CONTINUOUS                             |
| GptChannelTickFrequency    | 48000000 (FIRC 48 MHz)                             |
| GptChannelTickValueMax     | 4294967295 (32-bit max — LPIT hardware resolution) |
| GptNotification            | BmsScheduler_Tick                                  |
| GptLpit → LPIT Hardware    | LPIT_0                                             |
| GptLpit → LpitChannel      | CH_0                                               |
| GptHwConfiguration_0       | LPIT_0_CH_0 — GptIsrEnable ✅ GptChannelIsUsed ✅  |

> **Lưu ý:** `GptChannelTickValueMax = 4294967295` là giới hạn phần cứng 32-bit, KHÔNG phải period.
> Period thực (50 ms = 2400000 ticks) truyền vào `Gpt_StartTimer()` trong code.

### INA219 Configuration
- Shunt resistor: 0.1 Ω (macro `INA219_SHUNT_R_OHM`)
- Max current: 3.2 A (macro `INA219_IMAX_A`)
- Current LSB: ≈ 0.0977 mA/bit — derived from `IMAX_A / 32768`
- Calibration register: ≈ 4194 (0x1062) — derived from `0.04096 / (CURRENT_LSB × R_shunt)`
- Config register: **`0x319F`** (32V range, **PG=±320 mV**, 128-sample averaging, continuous mode)
  - Trước đây dùng `0x219F` với PG=±40 mV → saturate trên 0.4 A → KHÔNG khớp Imax 3.2 A đã calibrate.
  - Đổi sang PG=±320 mV cho phép đo full range 3.2 A đúng với calibration register.

### STM32 Slave Packet Format (6 bytes)
```
Byte[0] = 0xAA       Header
Byte[1] = 0x01       Mode
Byte[2] = TEMP_H     Temperature × 10 >> 8
Byte[3] = TEMP_L     Temperature × 10 & 0xFF
Byte[4] = STATUS     0x00=OK, 0x01=OutOfRange
Byte[5] = CRC        XOR of Byte[0..4]
```

---

## 3. CAN Message Table

| ID    | Name              | DLC | Period  | Source  | Signal layout                                                              |
|-------|-------------------|-----|---------|---------|----------------------------------------------------------------------------|
| 0x100 | BMS_ElecMeasure   | 8   | 50 ms   | INA219  | B0-B1: V (0.001V LSB), B2-B3: I signed (0.1mA LSB), B4-B5: P (0.1mW LSB), B6: status, B7: rsvd |
| 0x101 | BMS_Fault         | 4   | Event   | MCU     | B0: fault source, B1: severity, B2-B3: fault value                        |
| 0x200 | BMS_Temperature   | 4   | 200 ms  | STM32   | B0-B2: temp raw ((T+40)/0.5), B3: status                                   |
| 0x210 | BMS_Voltage       | 4   | 100 ms  | INA219  | B0-B1: pack voltage (0.001V LSB), B2: status, B3: rsvd                     |
| 0x300 | BMS_SOC           | 6   | 100 ms  | MCU     | B0: SOC (0.5%LSB), B1: SOH, B2: state (0x01=chg/0x02=dchg), B3-B4: rsvd, B5: method |
| 0x400 | BMS_Prediction    | 6   | 500 ms  | MCU     | B0-B1: TTE (min), B2-B3: TTF (min), B4: avg power (0.5W LSB), B5: valid flags |
| 0x710 | HMI_CMD (RX)      | 3   | On demand | HMI  | B0: CMD (0x01=reset fault, 0x02=calib SOC, 0x03=snapshot)                  |

---

## 4. Fault Table

| Code  | Source     | Threshold           | Severity  | Action          |
|-------|------------|---------------------|-----------|-----------------|
| 0x00  | NONE       | (clear event)       | —         | TX 0x101 (auto-clear, falling edge) |
| 0x01  | OV         | > 5500 mV (1S demo, tolerant 5V rail) | Protection | TX 0x101 |
| 0x02  | OC         | > 3000 mA           | Protection | TX 0x101        |
| 0x03  | OT         | > 55 °C (real T)    | Warning   | TX 0x101        |
| 0x04  | INA_COMM   | INA219 I2C timeout  | Warning   | TX 0x101        |
| 0x05  | STM_COMM   | STM32 packet error  | Warning   | TX 0x101        |

### Cơ chế gửi 0x101 (v2, state-based)

- Mỗi source có 1 latch riêng (`s_entries[5]` trong BmsFault.c).
- Check / SetCommError set latch = TRUE khi điều kiện vi phạm, FALSE khi normalised.
- `BmsFault_Process()` mỗi tick 50 ms: pick latch active có priority cao nhất.
- Chỉ TX 0x101 khi **(source, value) thay đổi** so với lần TX trước (edge-triggered).
- → Khi tất cả fault clear → TX 1 frame `src=0x00` (falling edge) → HMI tự xóa banner.
- → Khi switch giữa 2 fault (vd OT → OV) → TX 1 frame mới.

> **Lưu ý v1 → v2:** Trước đây `BmsFault` chỉ gửi event "raise", không có "clear". Khi T giảm dưới 55°C, HMI vẫn thấy banner đỏ vĩnh viễn vì không nhận được frame nào báo "fault đã hết". Đã refactor thành state-based với edge-triggered TX (backlog #23).

---

## 5. Software Architecture

### Hiện trạng (As-Is) — cấu trúc đã refactor xong (layout split src/include, đồng nhất với RTD)
```
CAPSTONE/
├── src/
│   └── main.c                                  Init only — gọi BmsScheduler_Init() + Run()
│
├── Application/
│   ├── BmsScheduler/
│   │   ├── src/BmsScheduler.c                  GPT/LPIT-driven 50ms tick + task dispatch
│   │   └── include/BmsScheduler.h
│   ├── BmsApp/
│   │   ├── src/BmsApp.c                        CAN signal pack + TX + CanIf callbacks
│   │   └── include/BmsApp.h
│   ├── BmsSoc/
│   │   ├── src/BmsSoc.c                        Coulomb-counting SOC + TTE/TTF (OCV dead code for real-pack mode)
│   │   └── include/
│   │       ├── BmsSoc.h                        Public API: Init/Update/GetSocRaw/SetSocPct/PredictionPack
│   │       └── BmsSoc_Cfg.h                    Demo mode, initial SOC, nominal capacity, dead zone
│   └── BmsFault/
│       ├── src/BmsFault.c                      Fault detect/latch + 0x101 dispatch
│       └── include/BmsFault.h
│
├── CDD/
│   ├── CDD_INA219/
│   │   ├── src/CDD_INA219.c                    Driver implementation
│   │   └── include/
│   │       ├── CDD_INA219.h                    Public API
│   │       ├── CDD_INA219_Types.h              INA219_ReturnType enum
│   │       └── CDD_INA219_Cfg.h                Address, channel, calib values
│   └── CDD_STM32Temp/
│       ├── src/CDD_STM32Temp.c
│       └── include/
│           ├── CDD_STM32Temp.h
│           ├── CDD_STM32Temp_Types.h           STM32_Temp_ReturnType enum
│           └── CDD_STM32Temp_Cfg.h             Slave addr, packet size, channel
│
├── CDD_I2C/                                    CDD I2C tự viết (MCAL extension)
├── RTD/                                        KHÔNG sửa — NXP RTD generated
├── board/                                      KHÔNG sửa — RTD generated
├── generate/                                   KHÔNG sửa — RTD generated
├── include/                                    Header tổng
└── project_context.md                          <- file này
```

> **Lưu ý S32DS — phải UPDATE include paths sau khi đổi layout:**
>
> Project Properties → C/C++ General → Paths and Symbols → tab **Includes**
> (chọn GNU C, tick "Add to all configurations" và "Add to all languages").
> Add 6 path mới (TRỎ TỚI `include/`, không phải module folder gốc):
>
>     /CAPSTONE/Application/BmsScheduler/include
>     /CAPSTONE/Application/BmsApp/include
>     /CAPSTONE/Application/BmsSoc/include
>     /CAPSTONE/Application/BmsFault/include
>     /CAPSTONE/CDD/CDD_INA219/include
>     /CAPSTONE/CDD/CDD_STM32Temp/include
>
> Source Location tab vẫn giữ `/CAPSTONE/Application` và `/CAPSTONE/CDD` —
> S32DS đệ quy xuống `src/` của từng module. Xoá các include path cũ
> (`.../BmsApp`, không còn header trực tiếp ở đó).

---

### Include Dependency (one-way: Application -> CDD -> MCAL)
```
main.c
  └── BmsScheduler.h

BmsScheduler.c
  ├── BmsApp.h
  ├── BmsSoc.h
  ├── BmsFault.h
  └── Gpt.h                 (MCAL — tu RTD/generate)

BmsApp.c
  ├── CDD_INA219.h
  ├── CDD_STM32Temp.h
  └── Can_43_FLEXCAN.h

BmsFault.c
  ├── CDD_INA219_Types.h    (chi can type, khong can full API)
  └── CDD_STM32Temp_Types.h

CDD_INA219.c
  ├── CDD_INA219.h          (own header — luon include chinh minh truoc)
  └── CDD_I2c.h             (tu CDD_I2C/)

CDD_STM32Temp.c
  ├── CDD_STM32Temp.h
  └── CDD_I2c.h
```

### Scheduler Timing (timer-based, LPIT_0 CH_0 ISR every 50 ms)
```
BmsScheduler_Tick()     ← called from LPIT ISR (interrupt context)
    sets s_tick50msFlag = TRUE

BmsScheduler_Run()      ← called from main() superloop (task context)
    checks s_tick50msFlag — returns immediately if not set
    clears flag, increments s_tickCount

    every tick    (50 ms)  : BmsFault_Process()        ← highest priority, always first
                             BmsApp_Task50ms()          ← INA219 read → BmsSoc_Update(I) → pack + TX 0x100
    tickCount % 2 (100 ms) : BmsApp_Task100ms()         ← BmsSoc_GetSocRaw → pack + TX 0x300, 0x210
    tickCount % 4 (200 ms) : BmsApp_Task200ms()         ← STM32 read → pack + TX 0x200
    tickCount % 10 (500 ms): BmsApp_Task500ms()         ← Prediction → pack + TX 0x400
                             resets tickCount = 0
```

> **s_tick50msFlag** là `volatile boolean` — set từ ISR, đọc từ main loop.
> **KHÔNG** dùng busy-wait delay. CPU rảnh giữa các tick có thể dùng cho polling CAN.

---

## 6. Coding Convention (must follow for all edits)

### 6.1 Formatting
- Indent: **4 spaces** — no tab characters
- Max line width: **250 characters**
- Blank line at end of every file
- `/* */` only — no `//` comments anywhere

### 6.2 Variable Naming

| Scope          | Prefix     | Style    | Example                        |
|----------------|------------|----------|--------------------------------|
| Global         | `g_`       | camelCase | `g_faultPending`              |
| Static (file)  | `s_`       | camelCase | `s_ina219TxBuf`               |
| Local          | none       | camelCase | `voltMv`, `currRaw`           |
| Pointer param  | `p`        | camelCase | `pOutTemperature`             |

- Local variables: declared at **top of function**, each on its own line, always initialized
- No `uint32_t a, b, c;` style (one declaration per line)

### 6.3 Unsigned Literals
```c
/* All unsigned constants must have U suffix */
#define FAULT_OV_THRESH_MV   (12600U)
uint8 idx = 0U;
```

### 6.4 File Structure Sections

**Header file (.h) order:**
```c
/*******************************************************************************
* Definitions
*******************************************************************************/

/*******************************************************************************
* API
*******************************************************************************/
```

**Source file (.c) order:**
```c
/*******************************************************************************
* Definitions
*******************************************************************************/

/*******************************************************************************
* Prototypes
*******************************************************************************/

/*******************************************************************************
* Variables
*******************************************************************************/

/*******************************************************************************
* Code
*******************************************************************************/
```

### 6.5 Preprocessor Guards
```c
#ifndef _INA219_H_
#define _INA219_H_
/* ... */
#endif /* _INA219_H_ */
```

### 6.6 No-jump Rules
- No `goto`, no `continue`
- `break` only inside `switch`
- Single `return` at end of function (use early assignment, not early return)
- No recursion

### 6.7 Static Scope
- All file-scope functions and variables must be `static`
- Justify every non-static global with a comment

---

## 7. Doxygen Comment Style (V-model aware)

> **Nguyên tắc:** Mỗi comment phải đủ để người viết test case KHÔNG cần đọc code —
> chỉ đọc comment là biết đầu vào hợp lệ, đầu ra kỳ vọng, điều kiện lỗi.

---

### 7.1 File header (mọi .c và .h)

```c
/**
 * @file       INA219.c
 * @version    1.0.0
 * @brief      CDD driver for INA219 power monitor via I2C (LPI2C0).
 *
 * @details    Provides register-level read/write access to INA219 for:
 *               - Bus voltage measurement (0–26V, 4 mV/LSB after shift)
 *               - Shunt voltage measurement (±40 mV, 10 µV/LSB)
 *               - Current measurement (signed, 0.09765625 mA/LSB)
 *               - Power measurement (0.001953125 W/LSB)
 *             Hardware: LPI2C0, PTA2(SDA)/PTA3(SCL), 100 kHz, addr 0x40.
 *             Platform: NXP S32K144, AUTOSAR RTD, bare-metal (no OS).
 *
 * @note       Caller must ensure I2c_Init() has been called before any
 *             INA219_xxx() function. Not interrupt-safe — do not call
 *             from ISR context.
 */
```

---

### 7.2 Hàm thông thường (có I/O rõ ràng)

```c
/**
 * @brief   Integrate one scheduler tick (50 ms) of current draw into the
 *          Coulomb-counting SOC integrator.
 *
 * @details Per tick:
 *            delta_Ah = curr_A × (BMS_SOC_TICK_MS / 1000 / 3600)
 *            remain_Ah -= delta_Ah         (positive = discharge subtracts)
 *            soc_raw   = remain_Ah / nominal × 200, clamped to [0, 200]
 *          Current |i| < BMS_SOC_CURRENT_DEAD_ZONE_MA (5 mA) is treated as
 *          zero to prevent ADC noise from drifting the integrator.
 *
 * @pre     BmsSoc_Init() has been called at boot.
 *          Caller has just read a fresh INA219 current sample (50 ms cadence).
 *
 * @param[in]  curr_mA   Pack current in milliamps (signed).
 *                        Positive    : discharge (subtracts capacity)
 *                        Negative    : charge    (adds capacity)
 *                        Range tied to INA219 signed register (±32 A logical)
 *
 * @param[out] — (no output param; integrator state cached in module statics)
 *
 * @return  void
 *
 * @post    s_remainCapacity_mAh updated and clamped to [0, NominalCapacity].
 *          s_socRaw re-derived; readable via BmsSoc_GetSocRaw().
 *          s_updateCnt incremented (debug counter).
 *
 * @note    Algorithm: pure Coulomb counting (no voltage feedback in demo).
 *          Drift over hours is limited by the boundary clamp; for true
 *          accuracy under real-pack mode the future enhancement is OCV
 *          re-calibration when the pack rests for N consecutive ticks.
 */
void BmsSoc_Update(float32 curr_mA);
```

---

### 7.3 Hàm CDD — có I2C và error return

```c
/**
 * @brief   Read bus voltage from INA219 register 0x02.
 *
 * @details Two-step I2C transaction:
 *            Step 1 — Write 1 byte (register pointer 0x02) to slave 0x40.
 *            Step 2 — Read 2 bytes (raw register value, big-endian).
 *          Conversion: raw_16bit >> 3, then × 0.004 V/LSB.
 *          Valid conversion is indicated by CNVR bit (bit1) of raw value.
 *
 * @pre     I2c_Init() has been called. INA219_Init() has been called.
 *          I2C bus is idle (no pending transaction on channel 0).
 *
 * @param[out] pOutVoltage_V  Pointer to float32 to receive result.
 *                             On E_OK    : *pOutVoltage_V in range [0.0, 26.0] V
 *                             On E_NOT_OK: *pOutVoltage_V is unchanged (not written)
 *                             Must not be NULL_PTR.
 *
 * @return  E_OK      I2C transaction complete, conversion valid.
 * @return  E_NOT_OK  I2C timeout (bus hung or slave not responding).
 *
 * @post    On success: s_ina219RxBuf[0..1] holds last raw register bytes.
 *          On failure: s_ina219RxBuf is unchanged. Bus state unknown.
 *
 * @note    Blocking-poll with timeout = 0xFFFF iterations (~1.3 ms at 80MHz).
 *          If timeout fires, caller should set fault flag and skip this reading.
 */
Std_ReturnType INA219_ReadBusVoltage_V(float32 *pOutVoltage_V);
```

---

### 7.4 Hàm Init

```c
/**
 * @brief   Initialize INA219 power monitor.
 *
 * @details Performs the following register writes via I2C:
 *            1. Config register 0x00 ← 0x8000  (software reset)
 *            2. Calibration register 0x05 ← 4096 (R=0.1Ω, Imax=3.2A)
 *            3. Config register 0x00 ← 0x219F  (32V, Gain/1, 128-samp, continuous)
 *          After this function returns, INA219 continuously converts and
 *          updates internal registers every ~68 ms (128-sample averaging).
 *
 * @pre     I2c_Init() must have been called before this function.
 *          LPI2C0 bus must be idle.
 *
 * @param   void
 * @return  void
 *
 * @post    INA219 is in continuous shunt+bus conversion mode.
 *          Internal calibration register = 4096.
 *          Subsequent INA219_ReadXxx() calls will return valid data
 *          after the first conversion cycle (~68 ms).
 *
 * @note    No error is reported if I2C write fails during init —
 *          failure will surface on first INA219_ReadXxx() call.
 *          Call once at startup. Re-calling reinitializes the chip.
 */
void INA219_Init(void);
```

---

### 7.5 Callback

```c
/**
 * @brief   CAN TX confirmation callback — called by FlexCAN driver.
 *
 * @details Invoked by Can_43_FLEXCAN ISR after a mailbox transmission
 *          completes successfully. Increments TX counter and sets TX flag
 *          for the main loop timeout polling.
 *
 * @param[in]  CanTxPduId   PDU ID of the confirmed frame (unused, logged only).
 *                           Type: PduIdType (uint16, AUTOSAR).
 *
 * @return  void
 *
 * @post    g_canTxFlag = TRUE
 *          g_canTxConfirmCnt incremented by 1 (wraps at UINT8_MAX).
 *
 * @note    Called from interrupt context. Do NOT call directly.
 *          Conforms to CanIf_TxConfirmation() prototype (AUTOSAR SWS_CANIF).
 */
void CanIf_TxConfirmation(PduIdType CanTxPduId);
```

---

### 7.6 Macro / define

```c
/**
 * @brief   Overvoltage protection threshold (1S Li-ion + 5V demo rig).
 * @details = 5500 mV — relaxed above the 4.2 V cell limit so the demo
 *          (powered from a 5 V resistor rig with ~0.5 V sag) does not
 *          alarm. For strict 1S protection, set to 4200U; for 3S to 12600U.
 *          Fault code 0x01 is raised when bus voltage exceeds this value.
 *          Unit: millivolts (mV).
 */
#define BMS_FAULT_OV_THRESH_MV   (5500U)
```

---

### 7.7 Enum

```c
/**
 * @brief   Return codes for STM32 temperature sensor driver.
 * @details Used as return type for STM32_TempSensor_Read().
 *          Values are ordered by error severity (0 = success).
 */
typedef enum
{
    STM32_TEMP_OK          = 0U,  /**< Packet received and all checks passed.           */
    STM32_TEMP_ERR_TIMEOUT = 1U,  /**< I2C bus did not respond within 0xFFFF iterations.*/
    STM32_TEMP_ERR_HEADER  = 2U,  /**< Byte[0] != 0xAA — slave desync or bus noise.     */
    STM32_TEMP_ERR_CRC     = 3U,  /**< XOR of Byte[0..4] != Byte[5] — data corrupted.  */
    STM32_TEMP_ERR_RANGE   = 4U   /**< STATUS byte (Byte[4]) = 0x01 — sensor out of range. */
} STM32_Temp_ReturnType;
```

---

## 8. AUTOSAR-Specific Rules (MCAL / CDD layer)

### MemMap sections (required for all module-level variables)
```c
#define INA219_START_SEC_VAR_INIT_8
#include "INA219_MemMap.h"
static I2c_DataType s_ina219TxBuf[3U] = {0U, 0U, 0U};
#define INA219_STOP_SEC_VAR_INIT_8
#include "INA219_MemMap.h"
```

### AUTOSAR types — use instead of stdint
| Use this       | Not this  |
|----------------|-----------|
| `uint8`        | `uint8_t` |
| `uint16`       | `uint16_t`|
| `uint32`       | `uint32_t`|
| `sint16`       | `int16_t` |
| `float32`      | `float`   |
| `boolean`      | `bool`    |
| `TRUE/FALSE`   | `1/0`     |
| `NULL_PTR`     | `NULL`    |
| `STD_ON/STD_OFF` | `1/0`   |

### Return types
- Driver functions return `Std_ReturnType` (`E_OK` / `E_NOT_OK`) or a module-specific enum
- `void` return only for init functions and callbacks with no meaningful error path

### Include order (in .c files)
```c
#include "Std_Types.h"       /* AUTOSAR base types — always first */
#include "INA219.h"          /* Own header */
#include "CDD_I2c.h"         /* MCAL dependency */
/* No OS, no RTE includes needed in this project */
```

---

## 9. Known Issues / Refactor Backlog

| # | File                       | Issue                                                              | Priority | Status |
|---|----------------------------|--------------------------------------------------------------------|----------|--------|
| 1 | CDD_INA219.c               | `INA219_I2C_ADDR` macro unused — struct hardcodes `0x40U`          | High     | ✅ Done |
| 2 | CDD_INA219.c               | `WriteReg`/`ReadReg` swallow I2C timeout silently — no error return | High     | ✅ Done |
| 3 | BmsFault.c (từ main.c)     | `g_faultPending` set từ RX callback không có critical section      | High     | ✅ Done |
| 4 | CDD_STM32Temp.h            | Missing `extern "C"` guard                                         | Medium   | ✅ Done |
| 5 | All Application files      | Module-level vars thiếu MemMap sections                            | Medium   | ✅ Done (CDD); Application chỉ static, không cần MemMap |
| 6 | BmsFault.c / BmsApp.c      | Magic numbers `0x01U/0x02U/0x03U` chưa có named macros             | Medium   | ✅ Done |
| 7 | BmsApp.c                   | CAN signal packing cần tách thành `BmsApp_PackXxx()` riêng         | Medium   | ✅ Done |
| 8 | All files                  | Thiếu Doxygen file header block (@file @version @brief @details)   | Low      | ✅ Done |
| 9 | All files                  | Naming chưa nhất quán với convention (g_, s_ prefix)               | Low      | ✅ Done |
| 10 | main.c                    | Busy-wait delay → đã thay bằng GPT/LPIT timer (BmsScheduler)        | High     | ✅ Done |
| 11 | Architecture              | File naming CDD chưa có prefix `CDD_` → cần rename toàn bộ        | Medium   | ✅ Done |
| 12 | CDD_INA219 / CDD_STM32Temp | Thiếu `_Types.h` — ReturnType enum đang nằm trong `.h` chính       | Medium   | ✅ Done |
| 13 | S32DS .cproject           | Cần add Source Locations + Include Paths cho Application/, CDD/    | High     | ✅ Done (user đã add) |
| 14 | .mex Platform_Cfg         | IntCtrl thiếu LPIT0_Ch0_IRQn entry → ISR không được NVIC enable    | High     | ✅ Done (add PlatformIsrConfig_2 trong .mex) |
| 15 | .mex Gpt                  | GptEnableDisableNotificationApi = false → callback dispatch bị compile-out trong Gpt_ProcessCommonInterrupt | High     | ✅ Done (tick trong Config Tool → STD_ON) |
| 16 | Vector_Table.s            | INT_CTRL_IP_ENABLE_VTOR_CONFIG=STD_OFF → runtime install handler thất bại vì VTOR chỉ flash → slot 48 vẫn undefined_handler | High     | ✅ Done (patch tĩnh `.long LPIT_0_CH_0_ISR` slot 48) |
| 17 | CDD_INA219_Cfg.h          | CONFIG_VAL `0x219F` PG=±40 mV saturate trên 0.4 A → KHÔNG khớp Imax 3.2 A calibrate | High | ✅ Done (đổi `0x319F` PG=±320 mV) |
| 18 | CDD_INA219_Cfg.h          | CALIB_VAL hard-code 4096 lệch với CURRENT_LSB stated → 2.4% drift | Medium | ✅ Done (derive động từ R + Imax) |
| 19 | CDD_INA219                 | Thiếu API Reset / IsConnected / ReadAll → khó fault recovery + đọc đồng bộ | Medium | ✅ Done (thêm 3 public API) |
| 20 | BmsSoc                     | OCV-only → sai số dưới tải (IR drop), không có cập nhật theo dòng | High | ✅ Done (thay bằng Coulomb Counting + OCV làm seed dead-code cho real-pack mode) |
| 21 | BmsApp HMI calib SOC      | Chỉ ghi vào SDU, không update state → bị overwrite tick sau | Medium | ✅ Done (wire HMI 0x02 → `BmsSoc_SetSocPct`) |
| 22 | BmsFault                  | Mã fault 0x03 dùng chung OT thật và I2C comm error → HMI báo nhầm "Temperature warning" khi chỉ là timeout | High | ✅ Done (tách 0x04 INA_COMM, 0x05 STM_COMM; `RaiseCommError` thêm tham số source) |
| 23 | BmsFault                  | Event-based: chỉ TX khi raise, KHÔNG TX khi clear → HMI banner đỏ kẹt mãi sau khi OT đã giảm dưới 55°C | High | ✅ Done (refactor sang state-based per-source latches + edge-triggered TX gồm cả falling edge) |
| 24 | BmsApp                    | INA219/STM32 read OK không clear latch comm fault → HMI strip vàng kẹt | High | ✅ Done (mỗi sensor read OK pair với `SetCommError(src, FALSE, 0)`) |
| 25 | BmsFault / BmsSoc         | Threshold + OCV table còn cho 3S Li-ion → không khớp demo 1 cell + 5V rig → OV alarm spam | High | ✅ Done (OV=5500mV, OCV 3000–4200 mV cho 1S, OC=3000mA giữ nguyên) |
| 26 | BmsApp 0x300 byte 5       | Method byte luôn 0x00 → HMI hiển thị "Voltage-based" dù đang Coulomb counting | Low | ✅ Done (pack `BMS_SOC_METHOD_COULOMB = 0x01`) |
| 27 | BmsSoc TTE threshold     | DISCHG_MIN=50 mA quá cao cho demo (~17 mA) → TTE/TTF luôn invalid | Medium | ✅ Done (giảm xuống 5 mA, khớp dead-zone) |
| 28 | BmsSoc state float32     | Float32 23-bit mantissa → ULP rounding tích lũy trong tích phân Coulomb counting. ISO 26262 yêu cầu deterministic. | High | ✅ Done (refactor sang `sint64 s_remain_nC` — tích phân integer thuần qua `SMULL`. Float chỉ giữ ở debug mirror + PredictionPack stateless approximation) |
| 29 | INA219 Current_LSB lệch  | LSB = 97.66 µA → multiply float non-clean → khó scaling integer | Medium | ✅ Done (đổi sang LSB = 100 µA exact, Cal = 4096) |
| 30 | CDD_INA219 thiếu API integer | Chỉ trả float → mọi caller phải cast lại | Medium | ✅ Done (thêm `ReadCurrent_uA / ReadVoltage_mV / ReadPower_uW / ReadShuntVoltage_uV`, giữ float API legacy) |
| 31 | BmsFault.CheckTemp dùng float compare | `temp_C > 55.0f` không deterministic, ISO 26262 không ưa | Low | ✅ Done (so sánh `temp_raw > BMS_FAULT_OT_THRESH_RAW` = 190 integer) |

---

### Lưu ý cấu hình S32DS / RTD bắt buộc khi setup project lại

Nếu bạn (hoặc người sau) regenerate project từ .mex, đảm bảo các setting sau, nếu không IRQ-driven scheduler sẽ KHÔNG hoạt động:

| Setting | Vị trí | Giá trị bắt buộc | Lý do |
|---|---|---|---|
| `GptEnableDisableNotificationApi` | S32 Config Tool → Component **Gpt** → tab **GptConfigurationOfOptApiServices** | ☑ tick (true) | Mở callback chain `Gpt_ProcessCommonInterrupt` → `BmsScheduler_Tick()` |
| `GptIsrEnable` cho LPIT_0_CH_0 | S32 Config Tool → Component **Gpt** → tab **GptHwConfiguration** struct 0 | ☑ tick (true) | Generate ISR code cho channel |
| `GptChannelIsUsed` cho LPIT_0_CH_0 | Tương tự | ☑ tick (true) | Allocate channel |
| Platform interrupt config | S32 Config Tool → Component **Platform** → **IntCtrlConfig → PlatformIsrConfig** | Add: `IsrName=LPIT0_Ch0_IRQn`, `IsrHandler=LPIT_0_CH_0_ISR`, `IsrEnabled=true` | Enable NVIC bit 16 cho LPIT0_Ch0 (IRQ#48) |
| `Project_Settings/Startup_Code/Vector_Table.s` slot 48 | File source (không qua Config Tool) | `.long LPIT_0_CH_0_ISR` (thay `.long undefined_handler`) | Vì `INT_CTRL_IP_ENABLE_VTOR_CONFIG=STD_OFF`, runtime install handler chỉ làm việc nếu vector table relocate sang RAM. Patch tĩnh là workaround. Bắt buộc kèm `.globl LPIT_0_CH_0_ISR` ở đầu file. |

---

## 10. What Claude Should NOT Touch

- Files under `MCAL/` — generated by S32DS RTD configurator, do not edit manually
- `Can_43_FLEXCAN_PBcfg.c`, `I2c_PBcfg.c`, `Port_PBcfg.c` and similar RTD PBcfg files
- `Mcu_PBcfg.c` and clock configuration
- `Gpt_PBcfg.c`, `Gpt_Cfg.h` — generated by RTD after GPT config
- ARXML files

---

## 12. V-model Test Specification

> **Placeholder** — Sẽ bổ sung sau khi refactor code ổn định.
> Mỗi hàm cần có: Input range → Công thức tính expected (từ spec, không từ code) → Output kỳ vọng → Side effects.

---

## 11. Glossary

| Term      | Meaning                                          |
|-----------|--------------------------------------------------|
| CDD       | Custom Device Driver (AUTOSAR layer below BSW)   |
| MCAL      | Microcontroller Abstraction Layer                |
| RTD       | NXP Real-Time Drivers (AUTOSAR BSW stack)        |
| LPI2C0    | Low-Power I2C peripheral 0 on S32K144            |
| OCV       | Open Circuit Voltage (used for SOC estimation)   |
| TTE       | Time To Empty (battery prediction)               |
| TTF       | Time To Full (battery prediction)                |
| OV/OC/OT  | Overvoltage / Overcurrent / Overtemperature      |
| HOH       | Hardware Object Handle (CAN AUTOSAR term)        |
| LPIT      | Low Power Interrupt Timer — hardware timer on S32K144, used for system tick |
| GPT       | General Purpose Timer — AUTOSAR MCAL module abstracting LPIT/FTM/LPTMR      |
| SDU       | Service Data Unit (CAN payload buffer)           |
