# LoRa FOTA Deployment Guide

## Overview

This guide provides step-by-step instructions for deploying and using the LoRa Firmware Over-The-Air (FOTA) update system in the B.R.A.V.O firmware.

## What is LoRa FOTA?

LoRa FOTA enables wireless firmware updates to remote devices using LoRa communication, eliminating the need for:
- WiFi connectivity
- Physical access to devices
- Manual firmware uploads

This is particularly useful for:
- Devices deployed in remote locations
- Field operations without WiFi infrastructure
- Updating multiple devices simultaneously
- Emergency firmware patches

## System Architecture

### Components

1. **Transmitter Device**: 
   - Base station or dongle
   - Has access to firmware binary (SD card, SPIFFS, or LittleFS)
   - Sends firmware updates via LoRa

2. **Receiver Device(s)**:
   - Collar devices or other remote units
   - Automatically receive and apply updates
   - Reboot with new firmware

### Protocol Flow

```
Transmitter                    Receiver
    |                              |
    |-------- INIT (metadata) ---->|
    |                              | (Prepare flash)
    |<----------- ACK -------------|
    |                              |
    |------- CHUNK #0 (240B) ----->|
    |                              | (Validate & write)
    |<----------- ACK -------------|
    |                              |
    |------- CHUNK #1 (240B) ----->|
    |                              | (Validate & write)
    |<----------- ACK -------------|
    |                              |
    ... (continue for all chunks) ...
    |                              |
    |------- CHUNK #N (last) ----->|
    |                              | (Validate & write)
    |<----------- ACK -------------|
    |                              |
    |-------- COMPLETE ----------->|
    |                              | (Validate & reboot)
```

## Quick Start

### 1. Build Firmware

```bash
cd firmware
pio run
```

Binary location: `.pio/build/esp32dev/firmware.bin`

### 2. Prepare Receiver

The standard firmware already includes LoRa OTA receiver functionality - no changes needed!

### 3. Send Update

Using the example transmitter:

```cpp
loraOta.sendFirmware(firmwareData, firmwareSize, "2.0.0", "");
```

## Performance

| Firmware Size | Transfer Time (SF7) | Transfer Time (SF10) |
|--------------|---------------------|----------------------|
| 100 KB       | ~1 minute          | ~3 minutes           |
| 500 KB       | ~4 minutes         | ~8 minutes           |
| 1 MB         | ~8 minutes         | ~15 minutes          |

## Troubleshooting

### Update Doesn't Start
- Check LoRa initialization
- Verify devices are in range
- Check frequency band matches

### Frequent CRC Errors
- Reduce distance between devices
- Check for interference
- Increase spreading factor

### Device Doesn't Reboot
- Check serial output for errors
- Verify firmware binary is valid
- Ensure sufficient flash space

## Best Practices

1. **Test Before Deployment**: Start with small firmware on test device
2. **Plan Update Windows**: Schedule during low-activity periods
3. **Monitor Updates**: Log all attempts and track success rates
4. **Staged Rollouts**: Update small groups first
5. **Security**: Implement firmware signing and device authentication

## Protocol Specification

### Message Types

- **INIT (0x01)**: Firmware metadata (size, version, CRC32)
- **CHUNK (0x02)**: 240-byte firmware chunks with CRC32
- **ACK (0x03)**: Acknowledge successful chunk
- **NACK (0x04)**: Request chunk retry
- **COMPLETE (0x05)**: All chunks received
- **ABORT (0x06)**: Cancel update

### Chunk Format

```
[Type] [ChunkHeader] [Data (240 bytes)] [CRC32]
```

### Reliability Features

- CRC32 validation per chunk and for entire firmware
- Automatic retry (up to 3 times per chunk)
- Timeout handling (5 seconds default)
- ACK/NACK protocol for reliable transfer

## Security Recommendations

For production deployments:

1. **Firmware Signing**: Verify firmware with cryptographic signature
2. **Authentication**: Verify transmitter identity before accepting updates
3. **Encrypted Transfer**: Use LoRa encryption for sensitive data
4. **Access Control**: Restrict update initiation to authorized devices
5. **Rollback Support**: Implement ability to revert to previous firmware

## Advanced Usage

### Broadcast to Multiple Devices

```cpp
// Empty target ID = broadcast
loraOta.sendFirmware(data, size, "2.0.0", "");
```

### Target Specific Device

```cpp
loraOta.sendFirmware(data, size, "2.0.0", "BRAVO_001");
```

### Load from SD Card

```cpp
File file = SD.open("/firmware.bin", FILE_READ);
size_t size = file.size();
uint8_t* buffer = (uint8_t*)malloc(size);
file.read(buffer, size);
file.close();

loraOta.sendFirmware(buffer, size, "2.0.0", "");
free(buffer);
```

### Monitor Progress

```cpp
void loop() {
    loraOta.handle();
    
    if (loraOta.isUpdateInProgress()) {
        Serial.printf("Progress: %d%%\n", loraOta.getProgress());
    }
}
```

## Resources

- **Main Documentation**: README.md
- **Examples**: examples/README.md
- **Example Code**: examples/lora_ota_transmitter.cpp
- **Protocol Details**: include/LoRaOTA.h
