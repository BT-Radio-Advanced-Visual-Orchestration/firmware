/**
 * @file LoRaOTA.h
 * @brief LoRa-based OTA (Over-The-Air) update module for B.R.A.V.O firmware
 * 
 * This module handles wireless firmware updates via LoRa communication for remote
 * collar and dongle updates without requiring WiFi connectivity.
 * 
 * Features:
 * - Chunked firmware transfer over LoRa
 * - CRC validation for data integrity
 * - Acknowledgment and retry mechanism
 * - Support for large firmware binaries
 * - Rollback capability on failed updates
 */

#ifndef LORA_OTA_H
#define LORA_OTA_H

#include <Arduino.h>
#include <Update.h>
#include "LoRaComm.h"

// Protocol constants
#define LORA_OTA_CHUNK_SIZE         240  // Max payload per packet (LoRa limit ~250 bytes)
#define LORA_OTA_MAX_RETRIES        3    // Maximum retries per chunk
#define LORA_OTA_TIMEOUT_MS         5000 // Timeout for acknowledgment (ms)
#define LORA_OTA_PROTOCOL_VERSION   1    // Protocol version

// Message types
enum LoRaOTAMessageType {
    LORA_OTA_INIT       = 0x01,  // Initialize update session
    LORA_OTA_CHUNK      = 0x02,  // Firmware chunk data
    LORA_OTA_ACK        = 0x03,  // Acknowledgment
    LORA_OTA_NACK       = 0x04,  // Negative acknowledgment (retry)
    LORA_OTA_COMPLETE   = 0x05,  // Update complete
    LORA_OTA_ABORT      = 0x06,  // Abort update
    LORA_OTA_STATUS     = 0x07   // Status request/response
};

// Update states
enum LoRaOTAState {
    LORA_OTA_IDLE           = 0,
    LORA_OTA_RECEIVING      = 1,
    LORA_OTA_TRANSMITTING   = 2,
    LORA_OTA_VALIDATING     = 3,
    LORA_OTA_ERROR          = 4
};

// Firmware metadata
struct FirmwareMetadata {
    uint32_t totalSize;      // Total firmware size in bytes
    uint16_t totalChunks;    // Total number of chunks
    uint32_t crc32;          // CRC32 checksum of entire firmware
    char version[16];        // Firmware version string
    uint8_t protocolVersion; // Protocol version
};

// Chunk header
struct ChunkHeader {
    uint8_t messageType;     // Message type
    uint16_t chunkIndex;     // Chunk sequence number
    uint16_t chunkSize;      // Size of data in this chunk
    uint32_t chunkCRC;       // CRC32 of chunk data
};

class LoRaOTA {
public:
    /**
     * @brief Constructor for LoRaOTA
     * @param loraComm Reference to LoRaComm instance
     */
    LoRaOTA(LoRaComm& loraComm);

    /**
     * @brief Initialize LoRaOTA module
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Handle incoming LoRa OTA messages (call in loop)
     * Should be called regularly to process incoming firmware updates
     */
    void handle();

    /**
     * @brief Start firmware update as receiver
     * Prepares the device to receive a firmware update
     * @return true if ready to receive, false otherwise
     */
    bool beginUpdate();

    /**
     * @brief Send firmware update to another device (transmitter mode)
     * @param firmwareData Pointer to firmware binary data
     * @param firmwareSize Size of firmware in bytes
     * @param version Version string of the firmware
     * @param targetDeviceId Target device ID (empty for broadcast)
     * @return true if transmission started successfully
     */
    bool sendFirmware(const uint8_t* firmwareData, size_t firmwareSize, 
                      const char* version, const String& targetDeviceId = "");

    /**
     * @brief Abort current update operation
     */
    void abortUpdate();

    /**
     * @brief Get current update state
     * @return Current LoRaOTAState
     */
    LoRaOTAState getState();

    /**
     * @brief Get update progress percentage
     * @return Progress 0-100
     */
    uint8_t getProgress();

    /**
     * @brief Check if update is in progress
     * @return true if update active, false otherwise
     */
    bool isUpdateInProgress();

    /**
     * @brief Get last error message
     * @return Error message string
     */
    String getLastError();

    /**
     * @brief Enable/disable LoRa OTA functionality
     * @param enable true to enable, false to disable
     */
    void setEnabled(bool enable);

    /**
     * @brief Check if LoRa OTA is enabled
     * @return true if enabled, false otherwise
     */
    bool isEnabled();

private:
    LoRaComm& lora;
    LoRaOTAState state;
    bool enabled;
    bool initialized;
    String lastError;

    // Receiver state
    FirmwareMetadata rxMetadata;
    uint16_t rxCurrentChunk;
    uint8_t rxRetryCount;
    unsigned long rxLastReceiveTime;

    // Transmitter state
    const uint8_t* txFirmwareData;
    size_t txFirmwareSize;
    uint16_t txCurrentChunk;
    uint8_t txRetryCount;
    unsigned long txLastSendTime;
    String txTargetDevice;

    // Private methods
    bool processMessage(const uint8_t* data, size_t length);
    bool handleInitMessage(const uint8_t* data, size_t length);
    bool handleChunkMessage(const uint8_t* data, size_t length);
    bool handleAckMessage(const uint8_t* data, size_t length);
    bool handleNackMessage(const uint8_t* data, size_t length);
    bool handleCompleteMessage(const uint8_t* data, size_t length);
    bool handleAbortMessage(const uint8_t* data, size_t length);

    bool sendInitMessage(const FirmwareMetadata& metadata);
    bool sendChunk(uint16_t chunkIndex);
    bool sendAck(uint16_t chunkIndex);
    bool sendNack(uint16_t chunkIndex);
    bool sendComplete();
    bool sendAbort(const String& reason);

    bool writeChunkToFlash(uint16_t chunkIndex, const uint8_t* data, size_t length);
    bool finalizeUpdate();
    uint32_t calculateCRC32(const uint8_t* data, size_t length);
    bool validateChunkCRC(const uint8_t* data, size_t length, uint32_t expectedCRC);
};

#endif // LORA_OTA_H
