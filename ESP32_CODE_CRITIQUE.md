# ESP32 Firmware Critique And Dashboard Contract

Target sketch: `esp32-water-vendo-code/esp32-water-vendo-code.ino`

## Findings Fixed In This Pass

1. Cloud logging was disabled in source with `WIFI_ENABLED false`.
   - Production WiFi/Firebase settings now live in local `esp32-water-vendo-code/secrets.h`.
   - Checked-in `secrets.example.h` documents required values without committing device secrets.
2. ESP32 Firestore writes did not match dashboard schema.
   - Firmware used Firestore `timestampValue`; dashboard filters and charts require epoch-millisecond number values.
   - Firmware now uploads numeric millisecond `timestamp` values and dashboard rules validate that shape.
3. Firmware and dashboard disagreed on prices.
   - Hardware coin rules and vend thresholds use PHP 3 regular and PHP 5 cold.
   - Dashboard rules, mock data, samples, UI currency text, and exports now follow 3/5 pricing.
4. Offline sale timing was misleading.
   - Unsynced transactions remain queued until NTP time is available.
   - Flush-time timestamps are marked with `timeSynced=false`; dashboard labels those timestamps as approximate.
5. REST failures lacked enough operator signal.
   - Serial output now distinguishes transport failures from Firestore payload/rule rejection responses.

## Residual Risks

1. Direct Firestore create access has no device identity.
   - Current rules only allow strict create payloads and deny update/delete, but any party able to submit valid writes can add sales rows.
   - Use Firebase device authentication or a relay endpoint if write identity, rate limiting, or revocation becomes required.
2. WiFi and NTP remain runtime dependencies for live reporting.
   - Vending continues offline by design. Queue size is finite and overwrites oldest queued rows when full.
3. Queue persistence is raw ESP32 Preferences data.
   - Queue schema changes reset incompatible stored queue bytes to avoid corrupt uploads.
   - Power-loss and flash-wear behavior should be checked on real hardware under repeated offline sales.
4. Pump volume and coin classifier thresholds are hardware assumptions.
   - `pumpDuration`, coin pulse ranges, external GPIO34 pull-up, cooler relay polarity, and 50-cup refill reset behavior need device calibration.

## Dashboard Write Contract

Each new `waterLogs` document created by ESP32 must contain only:

```text
amount: 3 or 5
isCold: bool
timestamp: epoch milliseconds int
timeSynced: bool
clientUptimeMs: int
```

`timeSynced=false` means ESP32 did not have trusted wall-clock time at vend time. Its `timestamp` is upload/flush time after clock recovery, not exact vend time.

## Firmware Setup

1. Copy `esp32-water-vendo-code/secrets.example.h` to `esp32-water-vendo-code/secrets.h`.
2. Fill WiFi SSID/password, Firebase web API key, and Firebase project ID.
3. Publish rules in `FIRESTORE_SETUP.md`.
4. Compile/upload sketch for intended ESP32 board.
5. Use Serial Monitor at 115200 baud while testing cold and regular sales.
