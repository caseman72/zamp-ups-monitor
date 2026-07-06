# Zamp UPS Monitor (TEMGO BMS via JBD BLE)

ESPHome-based monitor for the **TEMGO 12V 300Ah LiFePO4** battery powering
the Zamp 2000W inverter UPS in the house. Runs on the Canaduino PLC +
Arduino Nano ESP32 (ESP32-S3) hardware previously used as the Starlink
monitor — same board, same OTA flow.

The TEMGO's "Smart Touch Screen" is a TUYA-flavored UI on top of a
**JBD BMS** (the Chinese white-label staple, also called "Xiaoxiang
BMS"). The BMS speaks the standard JBD GATT protocol over BLE
(service `0xFF00`, notify char `0xFF01`, write char `0xFF02`), so we
talk to it directly with the well-maintained
[`syssi/esphome-jbd-bms`](https://github.com/syssi/esphome-jbd-bms)
external component — no cloud, no encryption, no Tuya account.

## TEMGO BLE identity (recorded for posterity)

- Advertised name: `FSDD02016J-016`
- MAC: `10:A5:62:90:E5:75`
- Hardware behind the touchscreen: JBD BMS (4-cell LFP variant)

## Sensors exposed (auto-discovered into HA over MQTT)

| Group | Entities |
|---|---|
| Pack | `Pack Voltage`, `Current`, `Power`, `SOC`, `Capacity Remaining`, `Nominal Capacity`, `Charging Cycles`, `Battery Strings` |
| Cells | `Cell {1..4} Voltage`, `Min/Max/Delta/Average Cell Voltage` |
| Temps | `Temperature 1`, `Temperature 2` |
| State | `Charging`, `Discharging`, `Balancing`, `BMS Online` (binary), `Operation Status` (text), `Device Model` (text), `BMS Errors` (text), `Errors Bitmask`, `Balancer Status Bitmask` |
| Diag  | `WiFi RSSI`, `IP Address`, `WiFi Network` |

Update interval: 61 s (skewed by 1 s from revel's 60 s — co-prime so they only co-publish on broker once per LCM = 61 min).

## Hardware

- **Controller**: Canaduino PLC + Arduino Nano ESP32 (ESP32-S3)
- **Battery**: TEMGO 12V 300Ah LiFePO4 (200A internal BMS, BLE app)
- **Distance**: ~10 ft from battery, through one wall + furnace
  (RSSI ~-79 dBm — comfortable for sustained GATT connection)
- **I/O**: REL1–REL3 (D2/D3/D4 = GPIO 5/6/7) are wired as dry contacts
  across the three buttons of a Velux KLI 311 remote — see
  "Skylight cover" below.

## Skylight cover (Velux KLI 311)

The three relays act as dry contacts, each wired across one button's
pads on the KLI 311 remote (powered by a fixed 3.3V buck converter —
was 2xAAA — sharing a supply with the rest of the install; the relay
contacts just short the button pads): REL1 = open, REL2 = stop,
REL3 = close. Exposed to HA as a
`time_based` cover named **Skylight** — a ~400 ms tap makes the INTEGRA
window run to its end on its own, and `open_duration` (72 s) /
`close_duration` (70 s) let HA estimate position and drive partial
opens. One-way remote, so position is estimated, never measured.

## Networks

Single network — `farmland` LAN. P3 is unused.

P5 button safe-reboots.

## MQTT

- Broker: HiveMQ Cloud (TLS, port 8883)
- Topic prefix: `zamp-ups`
- Log topic: `zamp-ups/logs`
- HA auto-discovery enabled (MAC-based unique IDs)

Same MQTT credentials as the other Canaduinos (see `secrets.h`).

## Setup

1. Copy `secrets.example.h` to `secrets.h` and fill in WiFi / MQTT / OTA
   values.

2. The board was originally the **Starlink monitor** at
   `starlink-monitor.local`. First OTA push from the new dir:

   ```bash
   ./upload.sh starlink-monitor.local
   ```

   After it reboots it advertises as `zamp-monitor.local`, so future
   updates use the no-arg form:

   ```bash
   ./upload.sh
   ```

## Single-connection caveat

The JBD BMS has **one BLE connection slot**. If the TEMGO phone app is
open and connected, the canaduino's `ble_client` will fail to claim
the slot. Close the app to cede control; reopen it briefly to take
control back (the canaduino reconnects automatically when the app
disconnects).

## Files

| File | Purpose |
|---|---|
| `zamp-monitor.yaml` | ESPHome config (JBD BMS via BLE) |
| `secrets.example.h` | template for `secrets.h` |
| `secrets.h` | actual credentials (gitignored) |
| `upload.sh` | OTA helper that injects secrets as substitutions; uses the esphome pinned in the Python 3.13 venv at `/Volumes/machfour-2tb/.venvs/esphome/` (espressif32 rejects 3.14) — override with `ESPHOME=...` |
| `.gitignore` | ignores `secrets.h`, `.esphome/`, etc. |

## License

MIT
