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

Update interval: 30 s.

## Hardware

- **Controller**: Canaduino PLC + Arduino Nano ESP32 (ESP32-S3)
- **Battery**: TEMGO 12V 300Ah LiFePO4 (200A internal BMS, BLE app)
- **Distance**: ~10 ft from battery, through one wall + furnace
  (RSSI ~-79 dBm — comfortable for sustained GATT connection)
- **I/O**: none wired. REL1 (D2 / GPIO 5) is `internal: true` +
  `ALWAYS_OFF` so the pin is driven low rather than floating. Add
  REL2/REL3/REL4 the same way if the board variant exposes them.

## Networks

| Slot | Notes |
|---|---|
| Primary (default at boot) | preferred LAN |
| Secondary (toggle via P3) | Starlink fallback |

P3 button toggles network and reboots; selection persists in NVS.
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
| `upload.sh` | OTA helper that injects secrets as substitutions |
| `.gitignore` | ignores `secrets.h`, `.esphome/`, etc. |

## License

MIT
