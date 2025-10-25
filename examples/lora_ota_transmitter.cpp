/**
 * @file examples/lora_ota_transmitter.cpp
 * @brief Example firmware transmitter for LoRa OTA updates
 * 
 * This example demonstrates how to send firmware updates to remote devices via LoRa.
 * 
 * Hardware Requirements:
 * - ESP32 with LoRa module
 * - SD card or SPIFFS with firmware binary
 * 
 * Usage:
 * 1. Build your firmware binary using: pio run
 * 2. Copy .pio/build/esp32dev/firmware.bin to SD card or SPIFFS
 * 3. Upload this transmitter code to a separate ESP32
 * 4. The transmitter will send the firmware to target devices
 * 
 * NOTE: This is an example. In production, load firmware from SD/SPIFFS.
 */

#include <Arduino.h>
#include "LoRaComm.h"
#include "LoRaOTA.h"

// Device configuration
#define DEVICE_ID "BRAVO_TRANSMITTER"
#define TARGET_DEVICE "BRAVO_001"  // Empty string for broadcast to all devices

// Module instances
LoRaComm lora;
LoRaOTA loraOta(lora);

// Example: Small test firmware (in production, load from SD card or SPIFFS)
// This would be replaced with actual firmware binary read from storage
const uint8_t testFirmware[] = {
    // This is just example data. In real use, load actual firmware.bin
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== LoRa OTA Transmitter ===\n");

    // Initialize LoRa
    Serial.println("Initializing LoRa...");
    if (!lora.begin()) {
        Serial.println("✗ LoRa initialization failed");
        while (1) delay(1000);
    }
    Serial.println("✓ LoRa ready");

    // Initialize LoRa OTA
    Serial.println("Initializing LoRa OTA...");
    if (!loraOta.begin()) {
        Serial.println("✗ LoRa OTA initialization failed");
        while (1) delay(1000);
    }
    Serial.println("✓ LoRa OTA ready");

    // Wait a bit before starting transmission
    Serial.println("\nWaiting 5 seconds before starting firmware transmission...");
    delay(5000);

    // Start firmware transmission
    Serial.println("\n=== Starting Firmware Transmission ===");
    Serial.printf("Target Device: %s\n", TARGET_DEVICE);
    Serial.printf("Firmware Size: %u bytes\n", sizeof(testFirmware));
    Serial.println("Firmware Version: 2.0.0");
    Serial.println();

    // In production, you would load firmware from SD card or SPIFFS:
    /*
    File firmwareFile = SD.open("/firmware.bin", FILE_READ);
    size_t firmwareSize = firmwareFile.size();
    uint8_t* firmwareBuffer = (uint8_t*)malloc(firmwareSize);
    firmwareFile.read(firmwareBuffer, firmwareSize);
    firmwareFile.close();
    
    loraOta.sendFirmware(firmwareBuffer, firmwareSize, "2.0.0", TARGET_DEVICE);
    free(firmwareBuffer);
    */

    // For this example, we'll use the test data
    if (loraOta.sendFirmware(testFirmware, sizeof(testFirmware), "2.0.0", TARGET_DEVICE)) {
        Serial.println("✓ Firmware transmission started");
    } else {
        Serial.println("✗ Failed to start firmware transmission");
        Serial.println("Error: " + loraOta.getLastError());
    }
}

void loop() {
    // Handle LoRa OTA transmission
    loraOta.handle();

    // Display progress
    static uint8_t lastProgress = 0;
    uint8_t progress = loraOta.getProgress();
    
    if (progress != lastProgress && progress % 10 == 0) {
        Serial.printf("Transmission progress: %u%%\n", progress);
        lastProgress = progress;
    }

    // Check if transmission completed
    static bool transmissionComplete = false;
    if (!loraOta.isUpdateInProgress() && !transmissionComplete && lastProgress > 0) {
        transmissionComplete = true;
        Serial.println("\n=== Firmware Transmission Complete ===");
        Serial.println("Target device should now be updating...");
    }

    delay(10);
}
