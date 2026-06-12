/**
 * @file       BmsFault.c
 * @version    2.0.0
 * @brief      State-based fault latches + edge-triggered CAN 0x101 dispatch.
 *
 * @details    Each fault source owns one latch (active / severity / value).
 *             BmsFault_Process picks the highest-priority active latch every
 *             50 ms and emits 0x101 if the chosen (source, value) pair has
 *             changed since the previous transmission. This fixes the bug
 *             where the HMI banner stayed red forever after a transient OT
 *             event because v1 only emitted "raise" events, never "clear".
 *
 *             Concurrency: latches and the TX edge buffer are written from
 *             task context (Check / SetCommError / Reset) and read by Process.
 *             Reset may also be called from the CAN RX ISR (HMI command 0x01),
 *             so all access is wrapped in interrupt-suspend pairs.
 */

/*******************************************************************************
* Includes
*******************************************************************************/
#include "Std_Types.h"
#include "BmsFault.h"
#include "Can_43_FLEXCAN.h"
#include "OsIf.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   CAN identifier for BMS_Fault frame.
 */
#define BMS_FAULT_CAN_ID             (0x101U)

/**
 * @brief   Software PDU handle for BMS_Fault (matches generated CAN config).
 */
#define BMS_FAULT_SW_PDU_HANDLE      (1U)

/**
 * @brief   OC threshold expressed in raw 0.1 mA LSB units.
 */
#define BMS_FAULT_OC_THRESH_RAW      ((sint16)(BMS_FAULT_OC_THRESH_MA * 10))

/**
 * @brief   Number of latch entries (OV, UV, OC, OT, INA_COMM, STM_COMM).
 */
#define BMS_FAULT_NUM_ENTRIES        (6U)

/**
 * @brief   Latch indices. Priority order = array index order (lower wins).
 *          UV gets the second-highest priority after OV (same voltage
 *          protection group).
 */
#define BMS_FAULT_IDX_OV             (0U)
#define BMS_FAULT_IDX_UV             (1U)
#define BMS_FAULT_IDX_OC             (2U)
#define BMS_FAULT_IDX_OT             (3U)
#define BMS_FAULT_IDX_INA_COMM       (4U)
#define BMS_FAULT_IDX_STM_COMM       (5U)

/**
 * @brief   Sentinel for "no previous TX captured yet" so the first Process
 *          tick after Init forces a baseline 0x101 (source = NONE).
 */
#define BMS_FAULT_TX_INVALID         (0xFFU)

/**
 * @brief   Single latch entry.
 */
typedef struct
{
    boolean active;
    uint8   severity;
    uint16  value;
} BmsFault_EntryType;

/*******************************************************************************
* Prototypes
*******************************************************************************/

static void BmsFault_SetEntry(uint8 idx, boolean active, uint8 severity, uint16 value);

/*******************************************************************************
* Variables
*******************************************************************************/

/**
 * @brief   Lookup table mapping latch index to fault source code
 *          (CAN 0x101 byte 0).
 * @details Required because after inserting UV in the middle of the latch
 *          array the source code is no longer (index + 1).
 */
static const uint8 s_idxToSrc[BMS_FAULT_NUM_ENTRIES] =
{
    BMS_FAULT_SRC_OV,        /* 0 */
    BMS_FAULT_SRC_UV,        /* 1 */
    BMS_FAULT_SRC_OC,        /* 2 */
    BMS_FAULT_SRC_OT,        /* 3 */
    BMS_FAULT_SRC_INA_COMM,  /* 4 */
    BMS_FAULT_SRC_STM_COMM   /* 5 */
};

/**
 * @brief   Per-source latches. Index order = priority (lower wins).
 */
static volatile BmsFault_EntryType s_entries[BMS_FAULT_NUM_ENTRIES] =
{
    { FALSE, BMS_FAULT_SEV_PROTECTION, 0U },   /* OV       */
    { FALSE, BMS_FAULT_SEV_PROTECTION, 0U },   /* UV       */
    { FALSE, BMS_FAULT_SEV_PROTECTION, 0U },   /* OC       */
    { FALSE, BMS_FAULT_SEV_WARNING,    0U },   /* OT       */
    { FALSE, BMS_FAULT_SEV_WARNING,    0U },   /* INA_COMM */
    { FALSE, BMS_FAULT_SEV_WARNING,    0U }    /* STM_COMM */
};

/**
 * @brief   Last (source, value) successfully written to the CAN driver.
 * @details Used by Process to detect edges. The sentinel BMS_FAULT_TX_INVALID
 *          guarantees the first Process call after Init transmits.
 */
static volatile uint8  s_lastTxSrc = BMS_FAULT_TX_INVALID;
static volatile uint16 s_lastTxVal = 0U;

/**
 * @brief   Static SDU for CAN 0x101 (BMS_Fault).
 */
static uint8 s_faultSdu[BMS_FAULT_DLC] = {0U, 0U, 0U, 0U};

/*******************************************************************************
* Code
*******************************************************************************/

/**
 * @brief   Atomic latch update (helper).
 */
static void BmsFault_SetEntry(uint8 idx, boolean active, uint8 severity, uint16 value)
{
    if (idx < BMS_FAULT_NUM_ENTRIES)
    {
        OsIf_SuspendAllInterrupts();
        s_entries[idx].active   = active;
        s_entries[idx].severity = severity;
        s_entries[idx].value    = value;
        OsIf_ResumeAllInterrupts();
    }
}

/**
 * @brief   See BmsFault.h.
 */
void BmsFault_Init(void)
{
    uint8 i = 0U;

    OsIf_SuspendAllInterrupts();
    for (i = 0U; i < BMS_FAULT_NUM_ENTRIES; i++)
    {
        s_entries[i].active = FALSE;
        s_entries[i].value  = 0U;
    }
    s_lastTxSrc    = BMS_FAULT_TX_INVALID;
    s_lastTxVal    = 0U;
    s_faultSdu[0U] = BMS_FAULT_SRC_NONE;
    s_faultSdu[1U] = BMS_FAULT_SEV_WARNING;
    s_faultSdu[2U] = 0U;
    s_faultSdu[3U] = 0U;
    OsIf_ResumeAllInterrupts();
}

/**
 * @brief   See BmsFault.h.
 */
void BmsFault_Process(void)
{
    uint8       chosenSrc = BMS_FAULT_SRC_NONE;
    uint8       chosenSev = BMS_FAULT_SEV_WARNING;
    uint16      chosenVal = 0U;
    uint8       i         = 0U;
    boolean     found     = FALSE;
    Can_PduType pdu       = {0U, 0U, 0U, NULL_PTR};

    /* Priority order = array index order:
     * OV > UV > OC > OT > INA_COMM > STM_COMM. */
    OsIf_SuspendAllInterrupts();
    for (i = 0U; (i < BMS_FAULT_NUM_ENTRIES) && (found == FALSE); i++)
    {
        if (s_entries[i].active == TRUE)
        {
            chosenSrc = s_idxToSrc[i];
            chosenSev = s_entries[i].severity;
            chosenVal = s_entries[i].value;
            found     = TRUE;
        }
    }
    OsIf_ResumeAllInterrupts();

    /* Edge-triggered TX: send on every change of (source, value). This covers
     * the rising edge (NONE -> fault), the falling edge (fault -> NONE) and
     * switches between two faults (e.g. OT -> OV). */
    if ((chosenSrc != s_lastTxSrc) || (chosenVal != s_lastTxVal))
    {
        s_faultSdu[0U] = chosenSrc;
        s_faultSdu[1U] = chosenSev;
        s_faultSdu[2U] = (uint8)(chosenVal >> 8U);
        s_faultSdu[3U] = (uint8)(chosenVal & 0xFFU);

        pdu.id          = BMS_FAULT_CAN_ID;
        pdu.swPduHandle = BMS_FAULT_SW_PDU_HANDLE;
        pdu.length      = BMS_FAULT_DLC;
        pdu.sdu         = s_faultSdu;
        (void)Can_43_FLEXCAN_Write(
            Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_Fault, &pdu);

        s_lastTxSrc = chosenSrc;
        s_lastTxVal = chosenVal;
    }
}

/**
 * @brief   See BmsFault.h.
 */
void BmsFault_CheckElec(uint16 volt_mV, sint16 curr_raw)
{
    boolean ovActive = FALSE;
    boolean uvActive = FALSE;
    boolean ocActive = FALSE;
    uint16  ovValue  = 0U;
    uint16  uvValue  = 0U;
    uint16  ocValue  = 0U;

    if (volt_mV > BMS_FAULT_OV_THRESH_MV)
    {
        ovActive = TRUE;
        ovValue  = volt_mV;
    }
    else if (volt_mV < BMS_FAULT_UV_THRESH_MV)
    {
        /* Undervoltage: pack below 3.0 V for 1S Li-ion. Continuing to
         * discharge under this threshold permanently damages the cell. */
        uvActive = TRUE;
        uvValue  = volt_mV;
    }
    else
    {
        /* Voltage inside the safe range -- both latches cleared below. */
    }

    if (curr_raw > BMS_FAULT_OC_THRESH_RAW)
    {
        ocActive = TRUE;
        ocValue  = (uint16)curr_raw;
    }

    BmsFault_SetEntry(BMS_FAULT_IDX_OV, ovActive, BMS_FAULT_SEV_PROTECTION, ovValue);
    BmsFault_SetEntry(BMS_FAULT_IDX_UV, uvActive, BMS_FAULT_SEV_PROTECTION, uvValue);
    BmsFault_SetEntry(BMS_FAULT_IDX_OC, ocActive, BMS_FAULT_SEV_PROTECTION, ocValue);
}

/**
 * @brief   See BmsFault.h.
 */
void BmsFault_CheckTemp(float32 temp_C, uint8 temp_raw)
{
    boolean otActive = FALSE;
    uint16  otValue  = 0U;

    (void)temp_C;     /* Float view kept in signature for backward compatibility. */

    /* Safety-critical compare uses INTEGER (temp_raw vs OT_RAW = 190).
     * The previous implementation compared `temp_C > 55.0f`, which carries
     * ULP rounding; ISO 26262 recommends deterministic integers for safety
     * thresholds. */
    if (temp_raw > (uint8)BMS_FAULT_OT_THRESH_RAW)
    {
        otActive = TRUE;
        otValue  = (uint16)temp_raw;
    }

    BmsFault_SetEntry(BMS_FAULT_IDX_OT, otActive, BMS_FAULT_SEV_WARNING, otValue);
}

/**
 * @brief   See BmsFault.h.
 */
void BmsFault_SetCommError(uint8 source, boolean active, uint8 errCode)
{
    uint8  idx = BMS_FAULT_NUM_ENTRIES;   /* sentinel = "no valid index" */
    uint16 val = 0U;

    if (source == BMS_FAULT_SRC_INA_COMM)
    {
        idx = BMS_FAULT_IDX_INA_COMM;
    }
    else if (source == BMS_FAULT_SRC_STM_COMM)
    {
        idx = BMS_FAULT_IDX_STM_COMM;
    }
    else
    {
        /* Ignore unsupported sources -- defensive against caller misuse. */
    }

    if (idx < BMS_FAULT_NUM_ENTRIES)
    {
        val = (active == TRUE) ? (uint16)errCode : 0U;
        BmsFault_SetEntry(idx, active, BMS_FAULT_SEV_WARNING, val);
    }
}

/**
 * @brief   See BmsFault.h.
 */
void BmsFault_Reset(void)
{
    uint8 i = 0U;

    OsIf_SuspendAllInterrupts();
    for (i = 0U; i < BMS_FAULT_NUM_ENTRIES; i++)
    {
        s_entries[i].active = FALSE;
        s_entries[i].value  = 0U;
    }
    OsIf_ResumeAllInterrupts();
    /* Do NOT touch s_lastTxSrc -- let Process emit the falling-edge frame. */
}
