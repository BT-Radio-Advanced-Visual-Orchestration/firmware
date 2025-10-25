# LoRa OTA Examples

This directory contains example code demonstrating how to use the LoRa OTA (Over-The-Air) firmware update functionality.

## Examples

### lora_ota_transmitter.cpp

Example code for transmitting firmware updates to remote devices via LoRa.

**Use Case:** 
- Update firmware on remote collar devices without physical access
- Deploy firmware updates to devices in the field
- Useful for base stations or dongles that send updates to collars

**How to Use:**

1. Build your main firmware:
   ```bash
   pio run
   ```
   This creates `.pio/build/esp32dev/firmware.bin`

2. In production, you would load this binary from SD card or SPIFFS
   
3. Modify the transmitter example with your target device ID

4. Upload the transmitter code to your base station/dongle

5. The transmitter will send the firmware to the target device(s)

**Important Notes:**
- The example uses dummy test data for demonstration
- In production, load actual firmware.bin from storage (SD card, SPIFFS, or LittleFS)
- Ensure LoRa communication range is adequate for the entire update
- Monitor progress through serial output
- Update takes time depending on firmware size and LoRa settings

## Receiver Setup

The main firmware (`src/main.cpp`) already includes LoRa OTA receiver functionality. Any device running the standard firmware can receive updates automatically when:

1. LoRa OTA is initialized and enabled
2. An INIT message is received from a transmitter
3. The device is not currently updating

## Protocol Overview

The LoRa OTA protocol uses a chunked transfer mechanism:

1. **INIT Message**: Transmitter sends firmware metadata (size, version, CRC32)
2. **CHUNK Messages**: Firmware sent in 240-byte chunks with individual CRC32
3. **ACK/NACK**: Receiver acknowledges each chunk or requests retry
4. **COMPLETE**: Final message when all chunks received
5. **Auto-Reboot**: Device validates and reboots with new firmware

## Performance Considerations

- **Chunk Size**: 240 bytes per packet (optimized for LoRa)
- **Transfer Speed**: ~1-2 KB/s depending on LoRa settings and conditions
- **Example**: 500 KB firmware takes ~4-8 minutes
- **Range**: Standard LoRa range (several km line-of-sight)
- **Reliability**: Automatic retry mechanism (up to 3 retries per chunk)

## Security Recommendations

For production deployments:

1. **Authentication**: Verify transmitter identity before accepting updates
2. **Firmware Signing**: Implement cryptographic signatures to validate firmware
3. **Encrypted Transfer**: Use LoRa encryption for sensitive data
4. **Access Control**: Restrict which devices can initiate updates
5. **Rollback**: Implement rollback mechanism for failed updates

## Troubleshooting

### Update Fails to Start
- Check LoRa initialization
- Verify devices are in range
- Ensure receiver is not busy with another task

### Chunk Transmission Failures
- Increase timeout if devices are far apart
- Check for LoRa interference
- Verify chunk size is appropriate for your LoRa settings

### CRC Errors
- Check for electrical interference
- Verify LoRa signal quality (RSSI/SNR)
- Ensure stable power supply during update

### Device Doesn't Reboot After Update
- Check serial output for validation errors
- Verify firmware binary is valid
- Ensure sufficient flash space

## Advanced Usage

### Broadcasting to Multiple Devices

To update multiple devices simultaneously, set target device ID to empty string:

```cpp
loraOta.sendFirmware(firmwareData, firmwareSize, "2.0.0", "");
```

All devices in range will receive and apply the update.

### Selective Updates

To update specific devices, specify the device ID:

```cpp
loraOta.sendFirmware(firmwareData, firmwareSize, "2.0.0", "BRAVO_001");
```

### Loading Firmware from SD Card

```cpp
#include <SD.h>

void sendFirmwareFromSD() {
    if (!SD.begin()) {
        Serial.println("SD initialization failed");
        return;
    }
    
    File file = SD.open("/firmware.bin", FILE_READ);
    if (!file) {
        Serial.println("Failed to open firmware file");
        return;
    }
    
    size_t size = file.size();
    uint8_t* buffer = (uint8_t*)malloc(size);
    if (!buffer) {
        Serial.println("Memory allocation failed");
        file.close();
        return;
    }
    
    file.read(buffer, size);
    file.close();
    
    loraOta.sendFirmware(buffer, size, "2.0.0", "");
    
    free(buffer);
}
```

### Progress Monitoring

```cpp
void loop() {
    loraOta.handle();
    
    if (loraOta.isUpdateInProgress()) {
        Serial.printf("Progress: %d%%\n", loraOta.getProgress());
        Serial.printf("State: %d\n", loraOta.getState());
    }
}
```
