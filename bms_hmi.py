#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
BMS Head Unit — PC Dashboard (v4 — real 1S Li-ion + UV protection)
Giao tiếp CAN với S32K144 qua USB-CAN adapter (CANable/PEAK/KVASER).

Changes vs v3 (đồng bộ firmware mới):
  - Sửa parse 0x100: DLC=8, status ở B6 (BMS_INA219_STATUS_OK = 0x03).
  - Sửa parse 0x200: stm32_ok lấy từ B3 (trước đang nhầm sang B1 = temp_raw).
  - Parse 0x210: DLC=4, B3 chứa warn flags (bit 0 UV, bit 1 OV) — MỚI.
  - Parse 0x300: DLC=6, B2=warn flags, B3=method (trước method ở B2).
  - Parse 0x400: valid_flags ở B5, SOC% ở B4 (trước HMI đọc B4 làm valid).
  - Thêm fault code 0x06 = UV (Undervoltage), severity PROTECTION (banner đỏ).
  - Cell voltage value đổi màu + badge "▼ LOW" / "▲ HIGH" khi UV/OV warn active.
  - CalibrateSOC default 50 % thay vì 95 % (bỏ BMS_SOC_INITIAL_PCT).
"""

import sys, time, struct, math
from dataclasses import dataclass, field
from typing import Optional
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QGridLayout, QLabel, QPushButton, QComboBox, QLineEdit,
    QGroupBox, QFrame, QDialog, QDialogButtonBox, QTextEdit,
    QSplitter, QCheckBox, QMessageBox, QProgressBar, QSizePolicy,
    QInputDialog
)
from PyQt5.QtCore import Qt, QTimer, QThread, pyqtSignal, QObject, QRectF, QPointF
from PyQt5.QtGui import (
    QPainter, QColor, QPen, QBrush, QFont, QLinearGradient, QPalette
)

# =============================================================================
# Design tokens — central place to tune the whole UI
# =============================================================================
FONT_MONO       = "Consolas"
FONT_DISPLAY    = "Segoe UI"

FS_TITLE        = 26
FS_GROUP        = 15
FS_FIELD_LABEL  = 13
FS_FIELD_VALUE  = 34
FS_FIELD_UNIT   = 13
FS_BADGE        = 16
FS_STATUS       = 14
FS_DOT          = 14
FS_PRED_LABEL   = 14
FS_PRED_VALUE   = 18
FS_BATTERY_PCT  = 38
FS_BATTERY_STATE= 16
FS_GAUGE_VAL    = 26
FS_GAUGE_LABEL  = 13
FS_BUTTON       = 14

# Palette
COL_BG          = "#0B1220"
COL_BG_CARD     = "rgba(12,20,34,0.85)"
COL_BORDER      = "#1B2940"
COL_TITLE       = "#5AA0FF"
COL_TEXT_HI     = "#F2F6FF"
COL_TEXT_MED    = "#9BB5D0"
COL_TEXT_DIM    = "#566980"
COL_OK          = "#26D070"
COL_WARN        = "#F2B033"
COL_CRIT        = "#FF4D4D"
COL_ACCENT_BLUE = "#3A95FF"
COL_ACCENT_PUR  = "#B47CFF"

# Voltage warn flag bits — đồng bộ với BmsApp.c BMS_VOLT_WARN_UV/_OV
VOLT_WARN_UV = 0x01
VOLT_WARN_OV = 0x02

# Firmware status byte values — đồng bộ với BmsApp.h
INA219_STATUS_OK = 0x03
TEMP_STATUS_OK   = 0x01


@dataclass
class BMSData:
    bus_voltage: float = 0.0
    current: float = 0.0
    power: float = 0.0
    ts_elec: float = 0.0
    fault_source: int = 0
    fault_level: int = 0
    fault_value: int = 0
    fault_active: bool = False
    ts_fault: float = 0.0
    temp_C: float = 0.0
    stm32_ok: bool = False
    ts_temp: float = 0.0
    pack_voltage: float = 0.0
    ina219_ok: bool = False
    volt_warn_flags: int = 0       # NEW: bit 0 UV, bit 1 OV (từ 0x210 B3 / 0x300 B2)
    ts_volt: float = 0.0
    soc: float = 0.0
    bms_state: int = 0
    soc_coulomb: bool = False
    ts_soc: float = 0.0
    time_to_empty: int = 0
    time_to_full: int = 0
    tte_valid: bool = False
    ttf_valid: bool = False
    ts_pred: float = 0.0
    rx_count: int = 0
    fault_log: list = field(default_factory=list)


def format_minutes(total_min):
    """Adaptive format cho TTE/TTF — đọc rõ ở mọi range từ phút tới ngày."""
    if total_min < 1:
        return "< 1m"
    if total_min < 60:
        return f"{int(total_min)}m"
    if total_min < 1440:
        h, m = divmod(int(total_min), 60)
        return f"{h}h {m:02d}m"
    d, rest = divmod(int(total_min), 1440)
    h = rest // 60
    return f"{d}d {h}h"


# v4: thêm 0x06 UV. Source codes khớp BmsFault.h:
#   0x01 OV, 0x02 OC, 0x03 OT, 0x04 INA_COMM, 0x05 STM_COMM, 0x06 UV
FAULT_SOURCE_MAP = {
    0: "—",
    1: "⚡ Overvoltage",
    2: "🔥 Overcurrent",
    3: "🌡 Overtemperature",
    4: "🔌 INA219 Comm",
    5: "🔌 STM32 Comm",
    6: "🪫 Undervoltage",
}

# Hardware-critical = banner đỏ. UV cùng nhóm với OV/OC/OT (severity PROTECTION).
FAULT_HW_CRITICAL = {1, 2, 3, 6}
FAULT_COMM        = {4, 5}


def parse_can_frame(can_id, data, bms):
    """Parse incoming CAN frame từ S32K144 firmware (v4 — real pack mode).

    Byte layouts khớp BmsApp.c hiện tại (project_context.md §3):
      - 0x100  DLC 8: V(B0-1) + I_signed(B2-3) + P(B4-5) + status(B6) + rsvd(B7)
      - 0x101  DLC 4: src(B0) + sev(B1) + val_MSB(B2) + val_LSB(B3)
      - 0x200  DLC 4: temp_raw x3 (B0-2) + status(B3)
      - 0x210  DLC 4: V_mV(B0-1) + status(B2) + warn_flags(B3)
      - 0x300  DLC 6: SOC_raw(B0) + state(B1) + warn_flags(B2) + method(B3) + rsvd(B4-5)
      - 0x400  DLC 6: TTE(B0-1) + TTF(B2-3) + SOC%(B4) + valid_flags(B5)

    Health logic:
      - ina219_ok: lấy từ B6 của 0x100 (= INA219_STATUS_OK) hoặc B2 của 0x210.
        Đồng thời comm-error 0x04 trên 0x101 sẽ override = False.
      - stm32_ok: lấy từ B3 của 0x200 (TEMP_STATUS_OK = 0x01).
    """
    now = time.time()
    try:
        if can_id == 0x100 and len(data) >= 8:
            bms.bus_voltage = struct.unpack_from('>H', data, 0)[0] * 0.001
            bms.current     = struct.unpack_from('>h', data, 2)[0] * 0.1
            bms.power       = struct.unpack_from('>H', data, 4)[0] * 0.1
            bms.ina219_ok   = (data[6] == INA219_STATUS_OK)
            bms.ts_elec     = now
            bms.rx_count   += 1

        elif can_id == 0x101 and len(data) >= 4:
            bms.fault_source = data[0]
            bms.fault_level  = data[1]
            bms.fault_value  = struct.unpack_from('>H', data, 2)[0]
            bms.fault_active = bms.fault_source != 0
            bms.ts_fault     = now
            # Comm-error fault overrides health flag (latches until cleared).
            if bms.fault_source == 0x04:        # INA_COMM
                bms.ina219_ok = False
            elif bms.fault_source == 0x05:      # STM_COMM
                bms.stm32_ok = False
            elif bms.fault_source == 0x00:      # NONE = all clear (falling edge)
                bms.ina219_ok = True
                bms.stm32_ok = True
            if bms.fault_source != 0:
                bms.fault_log.insert(0, {
                    'time': time.strftime('%H:%M:%S'),
                    'source_code': bms.fault_source,
                    'source': FAULT_SOURCE_MAP.get(bms.fault_source, "?"),
                    'level': "PROTECTION" if bms.fault_level else "WARNING",
                    'value': bms.fault_value})
                if len(bms.fault_log) > 50:
                    bms.fault_log.pop()
            bms.rx_count += 1

        elif can_id == 0x200 and len(data) >= 4:
            # B0-B2: temp_raw triplicated (legacy redundancy). B3: status.
            bms.temp_C   = data[0] * 0.5 - 40.0
            bms.stm32_ok = (data[3] == TEMP_STATUS_OK)
            bms.ts_temp  = now
            bms.rx_count += 1

        elif can_id == 0x210 and len(data) >= 4:
            # B0-B1: cell voltage mV. B2: status. B3: warn flags (UV/OV).
            bms.pack_voltage    = struct.unpack_from('>H', data, 0)[0] * 0.001
            bms.ina219_ok       = (data[2] == INA219_STATUS_OK)
            bms.volt_warn_flags = data[3]
            bms.ts_volt         = now
            bms.rx_count       += 1

        elif can_id == 0x300 and len(data) >= 4:
            # B0: SOC raw. B1: state. B2: warn flags. B3: method. B4-5 rsvd.
            bms.soc             = data[0] * 0.5
            bms.bms_state       = data[1]
            bms.volt_warn_flags = data[2]
            bms.soc_coulomb     = (data[3] == 0x01)
            bms.ts_soc          = now
            bms.rx_count       += 1

        elif can_id == 0x400 and len(data) >= 6:
            # B0-1 TTE, B2-3 TTF, B4 SOC%, B5 valid_flags
            bms.time_to_empty = struct.unpack_from('>H', data, 0)[0]
            bms.time_to_full  = struct.unpack_from('>H', data, 2)[0]
            v = data[5]
            bms.tte_valid = bool(v & 1)
            bms.ttf_valid = bool(v & 2)
            bms.ts_pred = now
            bms.rx_count += 1
    except Exception:
        pass


class CANWorker(QObject):
    data_updated     = pyqtSignal(object)
    connection_error = pyqtSignal(str)
    log_message      = pyqtSignal(str)
    connected_signal = pyqtSignal(bool)

    def __init__(self, interface, channel, bitrate):
        super().__init__()
        self.interface = interface
        self.channel   = channel
        self.bitrate   = bitrate
        self._running  = False
        self.bms       = BMSData()
        self._bus      = None

    def start_listening(self):
        self._running = True
        try:
            import can
            kw = dict(interface=self.interface, channel=self.channel, bitrate=self.bitrate)
            if self.interface == 'slcan':
                kw['ttyBaudrate'] = 115200
            self._bus = can.interface.Bus(**kw)
            self.connected_signal.emit(True)
            self.log_message.emit(
                f"[OK] Kết nối: {self.interface} / {self.channel} @ {self.bitrate} bps")
        except Exception as e:
            self.connection_error.emit(str(e))
            self._running = False
            return

        import can
        while self._running:
            try:
                msg = self._bus.recv(timeout=0.5)
                if msg is None:
                    continue
                parse_can_frame(msg.arbitration_id, bytes(msg.data), self.bms)
                self.data_updated.emit(self.bms)
                self.log_message.emit(
                    f"[RX {time.strftime('%H:%M:%S.%f')[:-3]}] "
                    f"ID=0x{msg.arbitration_id:03X} DLC={msg.dlc} "
                    f"Data={' '.join(f'{b:02X}' for b in msg.data)}")
            except can.CanOperationError:
                continue
            except Exception as e:
                if not self._running:
                    break
                err = str(e).lower()
                if any(k in err for k in ('format', 'framing', 'timeout', 'invalid', 'decode')):
                    continue
                if any(k in err for k in ('closed', 'shutdown', 'disconnect', 'access')):
                    self.connection_error.emit(f"Bus lỗi: {e}")
                    break
                continue

        if self._bus:
            try:
                self._bus.shutdown()
            except Exception:
                pass
        self.connected_signal.emit(False)

    def send_cmd(self, cmd, param, token):
        """Send HMI command 0x710 to S32K144 (DLC=2: cmd + param)."""
        if self._bus is None:
            return False
        try:
            import can
            msg = can.Message(arbitration_id=0x710,
                              data=[cmd & 0xFF, param & 0xFF],
                              is_extended_id=False)
            self._bus.send(msg)
            self.log_message.emit(
                f"[TX {time.strftime('%H:%M:%S')}] "
                f"0x710 CMD=0x{cmd:02X} PARAM=0x{param:02X} TOKEN={token}")
            return True
        except Exception as e:
            self.connection_error.emit(str(e))
            return False

    def stop(self):
        self._running = False


class BatteryWidget(QWidget):
    BODY_FRAC   = 0.70
    BODY_TOP    = 0.06
    STATE_FRAC  = 0.18
    GAP_PX      = 8

    def __init__(self, parent=None):
        super().__init__(parent)
        self.soc = 0.0
        self.state = 0
        self.setMinimumSize(200, 340)

    def update_data(self, soc, state):
        self.soc = max(0.0, min(100.0, soc))
        self.state = state
        self.update()

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        w, h = self.width(), self.height()

        bx = w * 0.12
        by = h * self.BODY_TOP
        bw = w * 0.76
        bh = h * self.BODY_FRAC

        tip_w = bw * 0.36
        tip_h = max(6.0, h * 0.04)
        p.setPen(Qt.NoPen)
        p.setBrush(QBrush(QColor(170, 175, 185)))
        p.drawRoundedRect(int(bx + (bw - tip_w) / 2), int(by - tip_h + 2),
                          int(tip_w), int(tip_h), 3, 3)

        bc = {0: QColor(70, 85, 110), 1: QColor(38, 208, 112),
              2: QColor(242, 176, 51), 3: QColor(255, 77, 77)}.get(self.state, QColor(70, 85, 110))
        p.setPen(QPen(bc, 4))
        p.setBrush(QBrush(QColor(18, 24, 36)))
        p.drawRoundedRect(QRectF(bx, by, bw, bh), 12, 12)

        m = 8
        ih = (bh - 2 * m) * (self.soc / 100.0)
        iy = by + m + (bh - 2 * m) * (1 - self.soc / 100.0)
        fc = (QColor(38, 208, 112) if self.soc > 60
              else QColor(242, 176, 51) if self.soc > 25
              else QColor(255, 77, 77))
        if self.state == 1:
            fc = fc.lighter(115)
        g = QLinearGradient(0, iy + ih, 0, iy)
        g.setColorAt(0, fc.darker(140))
        g.setColorAt(1, fc.lighter(140))
        p.setPen(Qt.NoPen)
        p.setBrush(QBrush(g))
        p.drawRoundedRect(QRectF(bx + m, iy, bw - 2 * m, ih), 8, 8)

        p.setPen(QPen(QColor(12, 18, 28, 180), 1))
        for i in range(1, 5):
            sy = by + m + (bh - 2 * m) / 5 * i
            p.drawLine(int(bx + m + 2), int(sy), int(bx + bw - m - 2), int(sy))

        p.setPen(QPen(QColor(255, 255, 255)))
        p.setFont(QFont(FONT_MONO, FS_BATTERY_PCT, QFont.Bold))
        p.drawText(QRectF(bx, by, bw, bh), Qt.AlignCenter, f"{self.soc:.0f}%")

        sl = {0: "⏸  IDLE", 1: "⚡  CHARGE", 2: "▼  DISCHARGE", 3: "⚠  FAULT"}.get(self.state, "")
        sc = {0: QColor(140, 155, 175), 1: QColor(38, 208, 112),
              2: QColor(242, 176, 51), 3: QColor(255, 77, 77)}.get(self.state, QColor(140, 155, 175))
        chip_y = by + bh + self.GAP_PX
        chip_h = h * self.STATE_FRAC
        p.setPen(QPen(sc))
        p.setFont(QFont(FONT_MONO, FS_BATTERY_STATE, QFont.Bold))
        p.drawText(QRectF(0, chip_y, w, chip_h),
                   Qt.AlignHCenter | Qt.AlignTop, sl)
        p.end()


class ArcGauge(QWidget):
    def __init__(self, label, unit, min_val, max_val, warn_val=None, crit_val=None, parent=None):
        super().__init__(parent)
        self.label = label
        self.unit = unit
        self.min_val = min_val
        self.max_val = max_val
        self.warn_val = warn_val
        self.crit_val = crit_val
        self.value = min_val
        self.setMinimumSize(200, 180)

    def set_value(self, v):
        self.value = max(self.min_val, min(self.max_val, v))
        self.update()

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        w, h = self.width(), self.height()
        cx, cy = w / 2, h * 0.66
        r = min(w * 0.44, h * 0.62)
        ratio = max(0.0, min(1.0, (self.value - self.min_val) / max(1e-9, self.max_val - self.min_val)))
        p.setPen(QPen(QColor(38, 50, 68), 12, Qt.SolidLine, Qt.RoundCap))
        rect = QRectF(cx - r, cy - r, 2 * r, 2 * r)
        p.drawArc(rect, int(210 * 16), int(-240 * 16))
        ac = (QColor(255, 77, 77) if self.crit_val and self.value >= self.crit_val
              else QColor(242, 176, 51) if self.warn_val and self.value >= self.warn_val
              else QColor(38, 208, 112))
        p.setPen(QPen(ac, 12, Qt.SolidLine, Qt.RoundCap))
        p.drawArc(rect, int(210 * 16), int(-240 * ratio * 16))
        ang = math.radians(210 - 240 * ratio)
        nx = cx + (r - 6) * math.cos(ang)
        ny = cy - (r - 6) * math.sin(ang)
        p.setPen(Qt.NoPen)
        p.setBrush(QBrush(QColor(255, 255, 255)))
        p.drawEllipse(QPointF(nx, ny), 7, 7)
        p.setPen(QPen(QColor(242, 246, 255)))
        p.setFont(QFont(FONT_MONO, FS_GAUGE_VAL, QFont.Bold))
        p.drawText(QRectF(cx - 70, cy - 28, 140, 44), Qt.AlignCenter, f"{self.value:.1f}")
        p.setFont(QFont(FONT_MONO, FS_GAUGE_LABEL, QFont.Bold))
        p.setPen(QPen(QColor(155, 181, 208)))
        p.drawText(QRectF(cx - 60, cy + 20, 120, 20), Qt.AlignCenter, self.unit)
        p.setFont(QFont(FONT_MONO, FS_GAUGE_LABEL, QFont.Bold))
        p.setPen(QPen(QColor(180, 200, 225)))
        p.drawText(QRectF(cx - 80, cy + 38, 160, 22), Qt.AlignCenter, self.label)
        p.end()


class DataField(QWidget):
    """Big-number card: icon + label on top, big bold value, unit on bottom."""
    def __init__(self, icon, label, unit="", fmt="{:.2f}", parent=None):
        super().__init__(parent)
        self._fmt = fmt
        lay = QVBoxLayout(self)
        lay.setSpacing(2)
        lay.setContentsMargins(12, 10, 12, 10)
        head = QLabel(f"{icon}  {label}")
        head.setAlignment(Qt.AlignCenter)
        head.setStyleSheet(
            f"color:{COL_TEXT_MED}; font:700 {FS_FIELD_LABEL}px '{FONT_MONO}'; "
            "letter-spacing:1px;")
        self.val_lbl = QLabel("—")
        self.val_lbl.setAlignment(Qt.AlignCenter)
        self.val_lbl.setStyleSheet(
            f"color:{COL_TEXT_HI}; font:900 {FS_FIELD_VALUE}px '{FONT_MONO}'; background:transparent;")
        self.unit_lbl = QLabel(unit)
        self.unit_lbl.setAlignment(Qt.AlignCenter)
        self.unit_lbl.setStyleSheet(
            f"color:{COL_TEXT_DIM}; font:700 {FS_FIELD_UNIT}px '{FONT_MONO}';")
        lay.addWidget(head)
        lay.addWidget(self.val_lbl)
        lay.addWidget(self.unit_lbl)
        self.setStyleSheet(
            f"DataField{{background:#101A2C; border:1px solid {COL_BORDER}; border-radius:10px;}}")
        self.setMinimumHeight(120)

    def set_value(self, value, color=COL_TEXT_HI):
        self.val_lbl.setText(self._fmt.format(value))
        self.val_lbl.setStyleSheet(
            f"color:{color}; font:900 {FS_FIELD_VALUE}px '{FONT_MONO}'; background:transparent;")

    def set_unit_text(self, txt, color=COL_TEXT_DIM):
        """Override unit row -- used to display UV/OV warning badge."""
        self.unit_lbl.setText(txt)
        self.unit_lbl.setStyleSheet(
            f"color:{color}; font:700 {FS_FIELD_UNIT}px '{FONT_MONO}';")

    def set_stale(self):
        self.val_lbl.setStyleSheet(
            f"color:#2E3D4E; font:900 {FS_FIELD_VALUE}px '{FONT_MONO}'; background:transparent;")


class FaultBanner(QFrame):
    """Red flashing banner — only shown for HARDWARE-CRITICAL faults (OV/UV/OC/OT)."""
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setFixedHeight(64)
        self.setVisible(False)
        self._flash = False
        lay = QHBoxLayout(self)
        lay.setContentsMargins(20, 10, 20, 10)
        self.icon_lbl = QLabel("⚠")
        self.icon_lbl.setStyleSheet(f"font-size:34px; color:{COL_WARN};")
        self.icon_lbl.setFixedWidth(48)
        self.msg_lbl = QLabel("—")
        self.msg_lbl.setStyleSheet(
            f"color:#FFE060; font:900 18px '{FONT_MONO}'; background:transparent;")
        self.level_lbl = QLabel("")
        self.level_lbl.setStyleSheet(
            f"color:{COL_CRIT}; font:900 16px '{FONT_MONO}'; background:transparent;")
        lay.addWidget(self.icon_lbl)
        lay.addWidget(self.msg_lbl, 1)
        lay.addWidget(self.level_lbl)
        t = QTimer(self)
        t.timeout.connect(self._tick)
        t.start(500)

    def show_fault(self, src_label, level, value_text):
        self.msg_lbl.setText(f"{src_label}   ●   {value_text}")
        self.level_lbl.setText("🔴 PROTECTION" if level else "🟡 WARNING")
        self.setVisible(True)

    def clear(self):
        self.setVisible(False)

    def _tick(self):
        self._flash = not self._flash
        if self.isVisible():
            bg = "#2D1010" if self._flash else "#1A0808"
            bd = "#CC2020" if self._flash else "#881010"
            self.setStyleSheet(
                f"FaultBanner{{background:{bg}; border:2px solid {bd}; border-radius:8px;}}")


class CommNotice(QFrame):
    """Yellow non-alarming strip — for I2C comm errors (INA219/STM32)."""
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setFixedHeight(42)
        self.setVisible(False)
        self.setStyleSheet(
            "CommNotice{background:#251A05; border:1px solid #6E4E10; border-radius:6px;}")
        lay = QHBoxLayout(self)
        lay.setContentsMargins(18, 6, 18, 6)
        self.icon = QLabel("🔌")
        self.icon.setStyleSheet("font-size:22px;")
        self.text = QLabel("—")
        self.text.setStyleSheet(
            f"color:#FFC85A; font:700 14px '{FONT_MONO}'; background:transparent;")
        lay.addWidget(self.icon)
        lay.addWidget(self.text, 1)

    def show_comm(self, src_label, value_text):
        self.text.setText(f"{src_label}  —  {value_text}")
        self.setVisible(True)

    def clear(self):
        self.setVisible(False)


class ConnectDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Kết nối CAN Bus")
        self.setFixedSize(460, 310)
        self.setStyleSheet(f"""
            QDialog{{background:{COL_BG};}}
            QLabel{{color:{COL_TEXT_MED}; font:14px '{FONT_MONO}';}}
            QComboBox,QLineEdit{{background:#1A2438; border:1px solid #2A3A55;
                border-radius:5px; color:{COL_TEXT_HI}; font:14px '{FONT_MONO}';
                padding:7px 10px; min-height:30px;}}
            QPushButton{{background:#1E3A6E; border:1px solid #2E5090;
                border-radius:5px; color:#8BBCFF; font:bold 14px '{FONT_MONO}'; padding:9px 22px;}}
            QPushButton:hover{{background:#274F90;}}
        """)
        lay = QVBoxLayout(self)
        lay.setSpacing(16)
        lay.setContentsMargins(30, 24, 30, 24)
        title = QLabel("🔌  CAN BUS CONNECTION")
        title.setStyleSheet(
            f"color:{COL_TITLE}; font:bold 18px '{FONT_MONO}'; letter-spacing:2px;")
        lay.addWidget(title)
        sep = QFrame()
        sep.setFrameShape(QFrame.HLine)
        sep.setStyleSheet("color:#1E2D40;")
        lay.addWidget(sep)
        grid = QGridLayout()
        grid.setSpacing(12)
        grid.addWidget(QLabel("Interface:"), 0, 0)
        self.cb_iface = QComboBox()
        self.cb_iface.addItems(["slcan", "socketcan", "pcan", "kvaser", "virtual"])
        self.cb_iface.setCurrentText("slcan")
        grid.addWidget(self.cb_iface, 0, 1)
        grid.addWidget(QLabel("Channel:"), 1, 0)
        self.le_channel = QLineEdit("COM11")
        grid.addWidget(self.le_channel, 1, 1)
        grid.addWidget(QLabel("Bitrate (bps):"), 2, 0)
        self.cb_bitrate = QComboBox()
        self.cb_bitrate.addItems(["125000", "250000", "500000", "1000000"])
        self.cb_bitrate.setCurrentText("500000")
        grid.addWidget(self.cb_bitrate, 2, 1)
        lay.addLayout(grid)
        self.hint_lbl = QLabel("")
        self.hint_lbl.setStyleSheet(f"color:{COL_TEXT_DIM}; font:12px '{FONT_MONO}';")
        lay.addWidget(self.hint_lbl)
        self.cb_iface.currentTextChanged.connect(self._hint)
        self._hint("slcan")
        lay.addStretch()
        btns = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        btns.button(QDialogButtonBox.Ok).setText("Kết nối")
        btns.button(QDialogButtonBox.Cancel).setText("Hủy")
        btns.accepted.connect(self.accept)
        btns.rejected.connect(self.reject)
        lay.addWidget(btns)

    def _hint(self, iface):
        h = {"slcan": "CANable Win: COM3/COM11  |  Linux: /dev/ttyUSB0",
             "socketcan": "CANable Linux: can0, vcan0", "pcan": "PEAK USB: PCAN_USBBUS1",
             "kvaser": "Kvaser: 0, 1, 2", "virtual": "Test không cần phần cứng: 0"}
        c = {"slcan": "COM11", "socketcan": "can0", "pcan": "PCAN_USBBUS1", "kvaser": "0", "virtual": "0"}
        self.hint_lbl.setText(h.get(iface, ""))
        self.le_channel.setText(c.get(iface, ""))

    def get_params(self):
        return self.cb_iface.currentText(), self.le_channel.text().strip(), int(self.cb_bitrate.currentText())


BG_BY_STATE = {0: "#0B1220", 1: "#08180F", 2: "#1A1206", 3: "#190A0A"}


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("BMS Head Unit — S32K144 CAN Monitor")
        self.resize(1500, 920)
        self._worker = None
        self._thread = None
        self._connected = False
        self._cmd_token = 0
        self._last_state = -1
        self._stale = 3.0
        self._bms = BMSData()
        self._build_ui()
        self._apply_bg(0)
        self._timer = QTimer(self)
        self._timer.timeout.connect(self._refresh)
        self._timer.start(100)

    def _build_ui(self):
        self.setStyleSheet(self._css())
        c = QWidget()
        self.setCentralWidget(c)
        root = QVBoxLayout(c)
        root.setSpacing(0)
        root.setContentsMargins(0, 0, 0, 0)
        root.addWidget(self._topbar())
        self.fault_banner = FaultBanner()
        root.addWidget(self.fault_banner)
        self.comm_notice = CommNotice()
        root.addWidget(self.comm_notice)
        sp = QSplitter(Qt.Horizontal)
        sp.setHandleWidth(3)
        sp.setStyleSheet("QSplitter::handle{background:#182030;}")
        sp.addWidget(self._left())
        sp.addWidget(self._center())
        sp.addWidget(self._right())
        sp.setSizes([260, 820, 420])
        root.addWidget(sp, 1)
        sb = self.statusBar()
        sb.setStyleSheet(
            f"QStatusBar{{background:#070D16; color:{COL_TEXT_DIM}; "
            f"font:{FS_STATUS}px '{FONT_MONO}'; border-top:1px solid #182030;}}")
        self._lbl_status = QLabel("  ⬤  Chưa kết nối")
        self._lbl_status.setStyleSheet(
            f"color:#385566; font:700 {FS_STATUS}px '{FONT_MONO}';")
        self._lbl_rx = QLabel("RX: 0")
        self._lbl_rx.setStyleSheet(
            f"color:#4A7860; font:700 {FS_STATUS}px '{FONT_MONO}';")
        sb.addWidget(self._lbl_status)
        sb.addPermanentWidget(self._lbl_rx)

    def _topbar(self):
        bar = QFrame()
        bar.setFixedHeight(72)
        bar.setStyleSheet("QFrame{background:#060B14; border-bottom:2px solid #1B2840;}")
        lay = QHBoxLayout(bar)
        lay.setContentsMargins(22, 10, 22, 10)
        title = QLabel("⚡  BMS HEAD UNIT")
        title.setStyleSheet(
            f"color:{COL_TITLE}; font:bold {FS_TITLE}px '{FONT_MONO}'; letter-spacing:5px;")
        self.btn_connect = QPushButton("🔌  KẾT NỐI")
        self.btn_connect.setFixedHeight(44)
        self.btn_connect.setStyleSheet(self._btn("#1E3A6E", "#2E5090", "#8BBCFF"))
        self.btn_connect.clicked.connect(self._on_connect)
        self.btn_disconnect = QPushButton("✕  NGẮT")
        self.btn_disconnect.setFixedHeight(44)
        self.btn_disconnect.setEnabled(False)
        self.btn_disconnect.setStyleSheet(self._btn("#3A1010", "#601010", "#FF8080"))
        self.btn_disconnect.clicked.connect(self._on_disconnect)
        self.lbl_badge = QLabel("⏸  IDLE")
        self.lbl_badge.setAlignment(Qt.AlignCenter)
        self.lbl_badge.setFixedSize(160, 42)
        self.lbl_badge.setStyleSheet(
            f"background:#1A2840; color:#5878A8; font:bold {FS_BADGE}px '{FONT_MONO}'; "
            "border:1px solid #2A3D60; border-radius:6px;")
        lay.addWidget(title)
        lay.addStretch()
        lay.addWidget(self.lbl_badge)
        lay.addSpacing(18)
        lay.addWidget(self.btn_connect)
        lay.addWidget(self.btn_disconnect)
        return bar

    def _left(self):
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 8, 14)
        lay.setSpacing(14)

        g = self._grp("🔋  PIN")
        gl = QVBoxLayout()
        gl.setContentsMargins(8, 8, 8, 8)
        self.battery = BatteryWidget()
        gl.addWidget(self.battery, 0, Qt.AlignCenter)
        g.setLayout(gl)
        lay.addWidget(g)

        g3 = self._grp("⏱  DỰ ĐOÁN")
        gl3 = QGridLayout()
        gl3.setSpacing(10)
        gl3.setContentsMargins(10, 10, 10, 10)
        self.lbl_tte = self._ilab("—")
        self.lbl_ttf = self._ilab("—")
        rows = [
            ("⏳  Hết",  self.lbl_tte),
            ("🔌  Đầy", self.lbl_ttf),
        ]
        for i, (lbl, wgt) in enumerate(rows):
            l = QLabel(lbl)
            l.setStyleSheet(
                f"color:{COL_TEXT_MED}; font:700 {FS_PRED_LABEL}px '{FONT_MONO}';")
            gl3.addWidget(l, i, 0)
            gl3.addWidget(wgt, i, 1)
        g3.setLayout(gl3)
        lay.addWidget(g3)

        lay.addStretch()
        return w

    def _center(self):
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(8, 14, 8, 14)
        lay.setSpacing(12)

        # ---- 0x100 — Điện (50 ms) ----
        g1 = self._grp("⚡  ĐIỆN   (0x100  •  50 ms)")
        gl1 = QGridLayout()
        gl1.setSpacing(12)
        gl1.setContentsMargins(10, 8, 10, 8)
        self.f_busv = DataField("🔋", "ĐIỆN ÁP", "V", "{:.3f}")
        self.f_cur  = DataField("🔌", "DÒNG", "mA", "{:+.1f}")
        self.f_pwr  = DataField("⚡", "CÔNG SUẤT", "mW", "{:.1f}")
        self.g_v = ArcGauge("Voltage", "V", 0, 6, 4.2, 4.5)        # 1S Li-ion scale (OV=4.2V)
        self.g_i = ArcGauge("Current", "mA", -500, 500, 300, 400)  # demo tải nhẹ (OC=400 mA)
        gl1.addWidget(self.f_busv, 0, 0)
        gl1.addWidget(self.f_cur, 0, 1)
        gl1.addWidget(self.f_pwr, 0, 2)
        gl1.addWidget(self.g_v, 0, 3)
        gl1.addWidget(self.g_i, 0, 4)
        g1.setLayout(gl1)
        lay.addWidget(g1)

        # ---- 0x210 — Cell voltage + status panel ----
        g2 = self._grp("🔋  CELL VOLTAGE   (0x210  •  100 ms)")
        gl2 = QGridLayout()
        gl2.setSpacing(12)
        gl2.setContentsMargins(10, 8, 10, 8)
        self.f_cellv = DataField("①", "CELL 1", "V", "{:.3f}")
        st_card = QFrame()
        st_card.setStyleSheet(
            f"QFrame{{background:#101A2C; border:1px solid {COL_BORDER}; "
            "border-radius:10px;}}")
        st_lay = QVBoxLayout(st_card)
        st_lay.setContentsMargins(14, 10, 14, 10)
        st_lay.setSpacing(8)
        st_title = QLabel("HEALTH")
        st_title.setAlignment(Qt.AlignCenter)
        st_title.setStyleSheet(
            f"color:{COL_TEXT_MED}; font:bold {FS_FIELD_LABEL}px '{FONT_MONO}'; "
            "letter-spacing:2px;")
        self.dot_ina = self._dot("INA219")
        self.dot_stm = self._dot("STM32")
        st_lay.addWidget(st_title)
        st_lay.addWidget(self.dot_ina)
        st_lay.addWidget(self.dot_stm)
        gl2.addWidget(self.f_cellv, 0, 0)
        gl2.addWidget(st_card, 0, 1)
        gl2.setColumnStretch(0, 2)
        gl2.setColumnStretch(1, 1)
        g2.setLayout(gl2)
        lay.addWidget(g2)

        # ---- bottom row: Temp + SoC ----
        bot = QHBoxLayout()
        bot.setSpacing(12)

        g3 = self._grp("🌡  NHIỆT ĐỘ   (0x200  •  200 ms)")
        gl3 = QGridLayout()
        gl3.setSpacing(12)
        gl3.setContentsMargins(10, 8, 10, 8)
        self.f_temp = DataField("🌡", "CELL TEMP", "°C", "{:.1f}")
        self.g_t = ArcGauge("Temp", "°C", -40, 80, 55, 65)
        gl3.addWidget(self.f_temp, 0, 0)
        gl3.addWidget(self.g_t, 0, 1)
        self.lbl_stm_status = self._dot("STM32")
        gl3.addWidget(self.lbl_stm_status, 1, 0, 1, 2)
        g3.setLayout(gl3)
        bot.addWidget(g3, 3)

        g4 = self._grp("📊  SOC   (0x300  •  100 ms)")
        gl4 = QGridLayout()
        gl4.setSpacing(12)
        gl4.setContentsMargins(10, 8, 10, 8)
        self.g_soc = ArcGauge("SOC", "%", 0, 100, 20, 10)
        self.f_soc = DataField("📊", "SOC", "%", "{:.1f}")
        self.lbl_meth = QLabel("Method: —")
        self.lbl_meth.setStyleSheet(
            f"color:{COL_TEXT_MED}; font:700 13px '{FONT_MONO}';"
            f"background:#101A2C; border:1px solid {COL_BORDER}; "
            "border-radius:8px; padding:10px;")
        self.lbl_meth.setAlignment(Qt.AlignCenter)
        gl4.addWidget(self.g_soc, 0, 0, 2, 1)
        gl4.addWidget(self.f_soc, 0, 1)
        gl4.addWidget(self.lbl_meth, 1, 1)
        g4.setLayout(gl4)
        bot.addWidget(g4, 3)
        lay.addLayout(bot)
        return w

    def _right(self):
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(8, 14, 14, 14)
        lay.setSpacing(12)

        g1 = self._grp("📤  ĐIỀU KHIỂN   (0x710)")
        cl = QVBoxLayout()
        cl.setSpacing(10)
        cl.setContentsMargins(10, 10, 10, 10)
        b_rst = QPushButton("⚡   RESET FAULT")
        b_cal = QPushButton("🔧   CALIBRATE SOC")
        b_snap = QPushButton("📷   SNAPSHOT")
        b_rst.setStyleSheet(self._btn("#3A1010", "#601010", "#FF9090"))
        b_cal.setStyleSheet(self._btn("#10203A", "#102050", "#80B0FF"))
        b_snap.setStyleSheet(self._btn("#103020", "#104530", "#80DFA0"))
        for b in [b_rst, b_cal, b_snap]:
            b.setFixedHeight(46)
            cl.addWidget(b)
        b_rst.clicked.connect(self._cmd_reset_fault)
        b_cal.clicked.connect(self._cmd_calibrate_soc)
        b_snap.clicked.connect(self._cmd_snapshot)
        row = QHBoxLayout()
        row.setSpacing(6)
        self.le_cmd = QLineEdit("0x00")
        self.le_cmd.setFixedWidth(78)
        self.le_param = QLineEdit("0x00")
        self.le_param.setFixedWidth(78)
        b_cust = QPushButton("📨  Send")
        b_cust.setFixedHeight(36)
        b_cust.setStyleSheet(self._btn("#202820", "#303E30", "#80C080"))
        b_cust.clicked.connect(self._send_custom)
        for x in [QLabel("CMD"), self.le_cmd, QLabel("PARAM"), self.le_param, b_cust]:
            if isinstance(x, QLabel):
                x.setStyleSheet(
                    f"color:{COL_TEXT_MED}; font:700 13px '{FONT_MONO}';")
            row.addWidget(x)
        cl.addLayout(row)
        self.lbl_token = QLabel("Token: 0")
        self.lbl_token.setStyleSheet(
            f"color:{COL_TEXT_DIM}; font:700 12px '{FONT_MONO}';")
        cl.addWidget(self.lbl_token)
        self.lbl_cmd_feedback = QLabel("—")
        self.lbl_cmd_feedback.setAlignment(Qt.AlignCenter)
        self.lbl_cmd_feedback.setStyleSheet(
            f"color:{COL_TEXT_DIM}; font:700 13px '{FONT_MONO}'; "
            f"padding:6px 10px; border:1px dashed {COL_BORDER}; border-radius:5px;")
        cl.addWidget(self.lbl_cmd_feedback)
        self._fb_timer = QTimer(self)
        self._fb_timer.setSingleShot(True)
        self._fb_timer.timeout.connect(self._clear_cmd_feedback)
        g1.setLayout(cl)
        lay.addWidget(g1)

        g2 = self._grp("🚨  FAULT LOG   (0x101)")
        fl = QVBoxLayout()
        fl.setContentsMargins(10, 10, 10, 10)
        self.fault_text = QTextEdit()
        self.fault_text.setReadOnly(True)
        self.fault_text.setFont(QFont(FONT_MONO, 12, QFont.Bold))
        self.fault_text.setStyleSheet(
            f"background:#0C0E14; color:#FFA0A0; border:1px solid #2A1520; border-radius:6px; "
            f"font-weight:700;")
        self.fault_text.setMaximumHeight(200)
        b_cf = QPushButton("🗑  Xóa")
        b_cf.setFixedHeight(30)
        b_cf.setStyleSheet(self._btn("#1A1010", "#301010", "#A07070"))
        b_cf.clicked.connect(self.fault_text.clear)
        fl.addWidget(self.fault_text)
        fl.addWidget(b_cf)
        g2.setLayout(fl)
        lay.addWidget(g2)

        g3 = self._grp("📡  CAN RAW LOG")
        ll = QVBoxLayout()
        ll.setContentsMargins(10, 10, 10, 10)
        self.log_text = QTextEdit()
        self.log_text.setReadOnly(True)
        self.log_text.setFont(QFont(FONT_MONO, 10))
        self.log_text.setStyleSheet(
            "background:#08080E; color:#3A8A5A; border:1px solid #101E18; border-radius:6px;")
        self.cb_auto = QCheckBox("Auto-scroll")
        self.cb_auto.setChecked(True)
        self.cb_auto.setStyleSheet(
            f"color:{COL_TEXT_MED}; font:700 12px '{FONT_MONO}';")
        b_cl = QPushButton("🗑  Xóa")
        b_cl.setFixedHeight(30)
        b_cl.setStyleSheet(self._btn("#0E1810", "#162A18", "#6AA070"))
        b_cl.clicked.connect(self.log_text.clear)
        lc = QHBoxLayout()
        lc.addWidget(self.cb_auto)
        lc.addStretch()
        lc.addWidget(b_cl)
        ll.addWidget(self.log_text, 1)
        ll.addLayout(lc)
        g3.setLayout(ll)
        lay.addWidget(g3, 1)
        return w

    def _grp(self, title):
        g = QGroupBox(title)
        g.setStyleSheet(f"""QGroupBox{{color:{COL_TITLE};
            font:bold {FS_GROUP}px '{FONT_MONO}'; letter-spacing:1px;
            border:1px solid {COL_BORDER}; border-radius:9px; margin-top:18px; padding-top:10px;
            background:{COL_BG_CARD};}}
            QGroupBox::title{{subcontrol-origin:margin; left:14px; padding:2px 8px;
            background:{COL_BG};}}""")
        return g

    def _dot(self, name):
        l = QLabel(f"●  {name}")
        l.setStyleSheet(
            f"color:#3A4F66; font:700 {FS_DOT}px '{FONT_MONO}';")
        return l

    def _ilab(self, text):
        l = QLabel(text)
        l.setStyleSheet(
            f"color:#9BC8F0; font:900 {FS_PRED_VALUE}px '{FONT_MONO}';")
        l.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        return l

    def _btn(self, bg, bgh, fg):
        return (
            f"QPushButton{{background:{bg}; border:1px solid {bgh}; border-radius:6px;"
            f"color:{fg}; font:bold {FS_BUTTON}px '{FONT_MONO}'; padding:8px 18px;"
            f"letter-spacing:1px;}}"
            f"QPushButton:hover{{background:{bgh};}}"
            f"QPushButton:disabled{{color:#283040; border-color:#1A2030;}}"
        )

    def _css(self):
        return (f"QWidget{{font-family:'{FONT_MONO}','Courier New',monospace;}} "
                f"QLabel{{color:{COL_TEXT_MED};}}"
                f"QLineEdit{{background:#101A2C; border:1px solid {COL_BORDER}; border-radius:5px;"
                f"color:{COL_TEXT_HI}; font:700 13px '{FONT_MONO}'; padding:6px 10px;}}"
                f"QComboBox{{background:#101A2C; border:1px solid {COL_BORDER}; border-radius:5px;"
                f"color:{COL_TEXT_HI}; font:700 13px '{FONT_MONO}'; padding:6px 10px;}}"
                "QScrollBar:vertical{background:#0A1018; width:10px;}"
                "QScrollBar::handle:vertical{background:#2A3850; border-radius:5px;}")

    def _apply_bg(self, state):
        self.centralWidget().setStyleSheet(f"background:{BG_BY_STATE.get(state, BG_BY_STATE[0])};")

    def _set_dot(self, lbl, name, active, acolor):
        lbl.setText(f"●  {name}")
        lbl.setStyleSheet(
            f"color:{acolor if active else '#3A4F66'}; font:700 {FS_DOT}px '{FONT_MONO}';")

    def _on_connect(self):
        dlg = ConnectDialog(self)
        if dlg.exec_() != QDialog.Accepted:
            return
        iface, chan, brate = dlg.get_params()
        self._thread = QThread()
        self._worker = CANWorker(iface, chan, brate)
        self._worker.moveToThread(self._thread)
        self._thread.started.connect(self._worker.start_listening)
        self._worker.data_updated.connect(self._on_data)
        self._worker.log_message.connect(self._on_log)
        self._worker.connection_error.connect(self._on_error)
        self._worker.connected_signal.connect(self._on_connected)
        self._thread.start()

    def _on_disconnect(self):
        if self._worker:
            self._worker.stop()
        if self._thread:
            self._thread.quit()
            self._thread.wait(2000)
        self._worker = None
        self._thread = None

    def _on_connected(self, ok):
        self._connected = ok
        self.btn_connect.setEnabled(not ok)
        self.btn_disconnect.setEnabled(ok)
        if ok:
            self._lbl_status.setText("  ⬤  Đã kết nối")
            self._lbl_status.setStyleSheet(
                f"color:{COL_OK}; font:700 {FS_STATUS}px '{FONT_MONO}';")
        else:
            self._lbl_status.setText("  ⬤  Đã ngắt")
            self._lbl_status.setStyleSheet(
                f"color:#385566; font:700 {FS_STATUS}px '{FONT_MONO}';")

    def _on_data(self, bms):
        self._bms = bms

    def _on_log(self, msg):
        self.log_text.append(msg)
        if self.cb_auto.isChecked():
            self.log_text.verticalScrollBar().setValue(self.log_text.verticalScrollBar().maximum())
        if self.log_text.document().blockCount() > 2000:
            cur = self.log_text.textCursor()
            cur.movePosition(cur.Start)
            cur.select(cur.LineUnderCursor)
            cur.removeSelectedText()
            cur.deleteChar()

    def _on_error(self, msg):
        self._lbl_status.setText(f"  ✕  {msg[:80]}")
        self._lbl_status.setStyleSheet(
            f"color:{COL_CRIT}; font:700 {FS_STATUS}px '{FONT_MONO}';")

    def _refresh(self):
        bms = self._bms
        now = time.time()
        if bms.bms_state != self._last_state:
            self._apply_bg(bms.bms_state)
            self._last_state = bms.bms_state
        sl = {0: ("⏸  IDLE", "#5878A8"), 1: ("⚡  CHARGE", COL_OK),
              2: ("▼  DISCHARGE", COL_WARN), 3: ("⚠  FAULT", COL_CRIT)}
        stxt, scol = sl.get(bms.bms_state, ("—", "#5878A8"))
        self.lbl_badge.setText(stxt)
        self.lbl_badge.setStyleSheet(
            f"background:#1A2840; color:{scol}; font:bold {FS_BADGE}px '{FONT_MONO}';"
            "border:1px solid #2A3D60; border-radius:6px;")

        # --- electrical (0x100) ---
        stale = (now - bms.ts_elec) > self._stale
        if stale:
            for f in [self.f_busv, self.f_cur, self.f_pwr]:
                f.set_stale()
        else:
            self.f_busv.set_value(bms.bus_voltage)
            if   bms.bms_state == 1: cur_col = COL_OK
            elif bms.bms_state == 2: cur_col = COL_WARN
            else:                    cur_col = COL_TEXT_MED
            self.f_cur.set_value(bms.current, cur_col)
            self.f_pwr.set_value(bms.power)
            self.g_v.set_value(bms.bus_voltage)
            self.g_i.set_value(bms.current)

        # --- cell voltage + warn flags (0x210) ---
        stale = (now - bms.ts_volt) > self._stale
        uv_active = bool(bms.volt_warn_flags & VOLT_WARN_UV)
        ov_active = bool(bms.volt_warn_flags & VOLT_WARN_OV)
        if stale:
            self.f_cellv.set_stale()
            self.f_cellv.set_unit_text("V", COL_TEXT_DIM)
        else:
            # Cell voltage value coloring tùy theo warn flag.
            if ov_active:
                self.f_cellv.set_value(bms.pack_voltage, COL_CRIT)
                self.f_cellv.set_unit_text("V  ▲ OVER-VOLTAGE", COL_CRIT)
            elif uv_active:
                self.f_cellv.set_value(bms.pack_voltage, COL_CRIT)
                self.f_cellv.set_unit_text("V  ▼ UNDER-VOLTAGE", COL_CRIT)
            else:
                self.f_cellv.set_value(bms.pack_voltage, COL_TEXT_HI)
                self.f_cellv.set_unit_text("V", COL_TEXT_DIM)

        ina_alive = bms.ina219_ok and not stale
        self._set_dot(self.dot_ina, "INA219", ina_alive, COL_OK)

        # --- temperature (0x200) ---
        stale_t = (now - bms.ts_temp) > self._stale
        if stale_t:
            self.f_temp.set_stale()
            stm_alive = False
        else:
            tc = COL_OK if bms.temp_C < 45 else COL_WARN if bms.temp_C < 60 else COL_CRIT
            self.f_temp.set_value(bms.temp_C, tc)
            self.g_t.set_value(bms.temp_C)
            stm_alive = bms.stm32_ok
        self._set_dot(self.dot_stm, "STM32", stm_alive, COL_OK)
        self.lbl_stm_status.setText("●  STM32 ONLINE" if stm_alive else "●  STM32 OFFLINE")
        self.lbl_stm_status.setStyleSheet(
            f"color:{COL_OK if stm_alive else COL_CRIT}; "
            f"font:700 {FS_DOT}px '{FONT_MONO}';")

        # --- SoC (0x300) ---
        stale = (now - bms.ts_soc) > self._stale
        if not stale:
            sc2 = COL_OK if bms.soc > 40 else COL_WARN if bms.soc > 20 else COL_CRIT
            self.f_soc.set_value(bms.soc, sc2)
            self.g_soc.set_value(bms.soc)
            self.battery.update_data(bms.soc, bms.bms_state)
            self.lbl_meth.setText(
                "🧮  Coulomb" if bms.soc_coulomb else "📐  Voltage-based")

        # --- prediction (0x400) ---
        stale = (now - bms.ts_pred) > self._stale
        if not stale:
            if bms.tte_valid:
                self.lbl_tte.setText(format_minutes(bms.time_to_empty))
            else:
                self.lbl_tte.setText("—")
            if bms.ttf_valid:
                self.lbl_ttf.setText(format_minutes(bms.time_to_full))
            else:
                self.lbl_ttf.setText("—")

        # --- fault routing ---
        if bms.fault_active:
            label  = FAULT_SOURCE_MAP.get(bms.fault_source, "?")
            valstr = self._format_fault_value(bms.fault_source, bms.fault_value)
            if bms.fault_source in FAULT_HW_CRITICAL:
                self.fault_banner.show_fault(label, bms.fault_level, valstr)
                self.comm_notice.clear()
            elif bms.fault_source in FAULT_COMM:
                self.comm_notice.show_comm(label, valstr)
                self.fault_banner.clear()
            else:
                self.fault_banner.clear()
                self.comm_notice.clear()
        else:
            self.fault_banner.clear()
            self.comm_notice.clear()

        if bms.fault_log:
            self.fault_text.clear()
            for e in bms.fault_log[:25]:
                vs = self._format_fault_value(e.get('source_code', 0), e['value'])
                self.fault_text.append(f"[{e['time']}]  {e['level']}  {e['source']}   {vs}")

        self._lbl_rx.setText(f"RX: {bms.rx_count}")

    def _send(self, cmd, param):
        if not self._connected or self._worker is None:
            QMessageBox.warning(self, "Chưa kết nối", "Vui lòng kết nối CAN trước.")
            return False
        self._cmd_token = (self._cmd_token + 1) & 0xFF
        ok = self._worker.send_cmd(cmd, param, self._cmd_token)
        self.lbl_token.setText(f"Token: {self._cmd_token}")
        return ok

    def _flash_feedback(self, text, color):
        self.lbl_cmd_feedback.setText(text)
        self.lbl_cmd_feedback.setStyleSheet(
            f"color:{color}; font:bold 13px '{FONT_MONO}'; "
            f"padding:6px 10px; border:1px solid {color}; border-radius:5px;")
        self._fb_timer.start(3000)

    def _clear_cmd_feedback(self):
        self.lbl_cmd_feedback.setText("—")
        self.lbl_cmd_feedback.setStyleSheet(
            f"color:{COL_TEXT_DIM}; font:700 13px '{FONT_MONO}'; "
            f"padding:6px 10px; border:1px dashed {COL_BORDER}; border-radius:5px;")

    def _cmd_reset_fault(self):
        if self._send(0x01, 0):
            self.fault_banner.clear()
            self.comm_notice.clear()
            self._flash_feedback("✓  Reset Fault đã gửi", COL_OK)

    def _cmd_calibrate_soc(self):
        if not self._connected:
            QMessageBox.warning(self, "Chưa kết nối", "Vui lòng kết nối CAN trước.")
            return
        # Default 50 % nếu chưa có SOC (bỏ BMS_SOC_INITIAL_PCT=95 cũ).
        cur = int(round(self._bms.soc)) if self._bms.soc else 50
        val, ok = QInputDialog.getInt(
            self, "Calibrate SOC",
            "Nhập giá trị SOC mới (0 – 100 %):",
            value=cur, min=0, max=100, step=1)
        if not ok:
            return
        if self._send(0x02, val):
            self._flash_feedback(f"✓  Calib SOC = {val}%", COL_ACCENT_BLUE)

    def _cmd_snapshot(self):
        if self._send(0x03, 0):
            self._flash_feedback(
                f"✓  Snapshot @ {time.strftime('%H:%M:%S')}", "#80DFA0")

    @staticmethod
    def _format_fault_value(source_code, value):
        """Convert raw 16-bit value trong CAN 0x101 byte 2-3 sang text dễ đọc."""
        if source_code == 1:        # OV  -- value = volt_mV
            return f"{value/1000.0:.3f} V"
        if source_code == 6:        # UV  -- value = volt_mV
            return f"{value/1000.0:.3f} V"
        if source_code == 2:        # OC  -- value = curr_raw (0.1 mA LSB)
            return f"{value/10.0:.1f} mA"
        if source_code == 3:        # OT  -- value = temp_raw (0..255)
            return f"{value*0.5-40.0:.1f} °C"
        if source_code in (4, 5):   # COMM -- value = driver errCode
            return f"err=0x{value:02X}"
        return f"val={value}"

    def _send_custom(self):
        try:
            cmd = int(self.le_cmd.text(), 16)
            param = int(self.le_param.text(), 16)
        except ValueError:
            QMessageBox.warning(self, "Lỗi", "CMD/PARAM phải là hex (vd: 0x01)")
            return
        self._send(cmd, param)

    def closeEvent(self, event):
        self._on_disconnect()
        event.accept()


def main():
    app = QApplication(sys.argv)
    app.setApplicationName("BMS Head Unit")
    app.setStyle("Fusion")
    pal = QPalette()
    pal.setColor(QPalette.Window, QColor(11, 18, 32))
    pal.setColor(QPalette.WindowText, QColor(190, 210, 235))
    pal.setColor(QPalette.Base, QColor(10, 16, 24))
    pal.setColor(QPalette.AlternateBase, QColor(16, 24, 36))
    pal.setColor(QPalette.Text, QColor(190, 210, 235))
    pal.setColor(QPalette.Button, QColor(20, 32, 50))
    pal.setColor(QPalette.ButtonText, QColor(160, 185, 215))
    pal.setColor(QPalette.Highlight, QColor(30, 80, 160))
    pal.setColor(QPalette.HighlightedText, QColor(220, 235, 255))
    app.setPalette(pal)
    win = MainWindow()
    win.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
