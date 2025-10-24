/**
 * @file LoRaOTA.cpp
 * @brief LoRa-based OTA update module implementation
 */

#include "LoRaOTA.h"

// CRC32 lookup table for efficient calculation
static const uint32_t crc32_table[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

LoRaOTA::LoRaOTA(LoRaComm& loraComm) 
    : lora(loraComm), 
      state(LORA_OTA_IDLE),
      enabled(true),
      initialized(false),
      rxCurrentChunk(0),
      rxRetryCount(0),
      rxLastReceiveTime(0),
      txFirmwareData(nullptr),
      txFirmwareSize(0),
      txCurrentChunk(0),
      txRetryCount(0),
      txLastSendTime(0) {
    memset(&rxMetadata, 0, sizeof(rxMetadata));
}

bool LoRaOTA::begin() {
    if (initialized) {
        return true;
    }

    Serial.println("LoRaOTA: Initializing...");
    initialized = true;
    state = LORA_OTA_IDLE;
    
    Serial.println("LoRaOTA: Ready for firmware updates via LoRa");
    return true;
}

void LoRaOTA::handle() {
    if (!initialized || !enabled) {
        return;
    }

    // Check for incoming LoRa messages
    if (lora.available()) {
        uint8_t buffer[256];
        int length = lora.receiveData(buffer, sizeof(buffer));
        
        if (length > 0) {
            processMessage(buffer, length);
        }
    }

    // Handle timeout in receiving state
    if (state == LORA_OTA_RECEIVING) {
        if (millis() - rxLastReceiveTime > LORA_OTA_TIMEOUT_MS) {
            if (rxRetryCount < LORA_OTA_MAX_RETRIES) {
                Serial.println("LoRaOTA: Timeout waiting for chunk, requesting retry");
                sendNack(rxCurrentChunk);
                rxRetryCount++;
                rxLastReceiveTime = millis();
            } else {
                lastError = "Timeout: No response from sender";
                abortUpdate();
            }
        }
    }

    // Handle transmitting state
    if (state == LORA_OTA_TRANSMITTING) {
        if (millis() - txLastSendTime > LORA_OTA_TIMEOUT_MS) {
            if (txRetryCount < LORA_OTA_MAX_RETRIES) {
                Serial.printf("LoRaOTA: Retrying chunk %d\n", txCurrentChunk);
                sendChunk(txCurrentChunk);
                txRetryCount++;
                txLastSendTime = millis();
            } else {
                lastError = "Timeout: No acknowledgment from receiver";
                abortUpdate();
            }
        }
    }
}

bool LoRaOTA::beginUpdate() {
    if (state != LORA_OTA_IDLE) {
        lastError = "Update already in progress";
        return false;
    }

    Serial.println("LoRaOTA: Ready to receive firmware update");
    state = LORA_OTA_RECEIVING;
    rxCurrentChunk = 0;
    rxRetryCount = 0;
    rxLastReceiveTime = millis();
    memset(&rxMetadata, 0, sizeof(rxMetadata));
    
    return true;
}

bool LoRaOTA::sendFirmware(const uint8_t* firmwareData, size_t firmwareSize, 
                           const char* version, const String& targetDeviceId) {
    if (state != LORA_OTA_IDLE) {
        lastError = "Update already in progress";
        return false;
    }

    if (!firmwareData || firmwareSize == 0) {
        lastError = "Invalid firmware data";
        return false;
    }

    Serial.printf("LoRaOTA: Starting firmware transmission (%u bytes)\n", firmwareSize);
    
    // Prepare metadata
    FirmwareMetadata metadata;
    metadata.totalSize = firmwareSize;
    metadata.totalChunks = (firmwareSize + LORA_OTA_CHUNK_SIZE - 1) / LORA_OTA_CHUNK_SIZE;
    metadata.crc32 = calculateCRC32(firmwareData, firmwareSize);
    metadata.protocolVersion = LORA_OTA_PROTOCOL_VERSION;
    strncpy(metadata.version, version, sizeof(metadata.version) - 1);
    metadata.version[sizeof(metadata.version) - 1] = '\0';

    // Store transmission state
    txFirmwareData = firmwareData;
    txFirmwareSize = firmwareSize;
    txCurrentChunk = 0;
    txRetryCount = 0;
    txTargetDevice = targetDeviceId;
    state = LORA_OTA_TRANSMITTING;

    // Send initialization message
    if (!sendInitMessage(metadata)) {
        lastError = "Failed to send init message";
        abortUpdate();
        return false;
    }

    txLastSendTime = millis();
    return true;
}

void LoRaOTA::abortUpdate() {
    Serial.println("LoRaOTA: Aborting update");
    
    if (state == LORA_OTA_RECEIVING) {
        Update.abort();
    }
    
    sendAbort(lastError);
    
    state = LORA_OTA_IDLE;
    txFirmwareData = nullptr;
    txFirmwareSize = 0;
}

LoRaOTAState LoRaOTA::getState() {
    return state;
}

uint8_t LoRaOTA::getProgress() {
    if (state == LORA_OTA_RECEIVING && rxMetadata.totalChunks > 0) {
        return (rxCurrentChunk * 100) / rxMetadata.totalChunks;
    } else if (state == LORA_OTA_TRANSMITTING && txFirmwareSize > 0) {
        uint16_t totalChunks = (txFirmwareSize + LORA_OTA_CHUNK_SIZE - 1) / LORA_OTA_CHUNK_SIZE;
        return (txCurrentChunk * 100) / totalChunks;
    }
    return 0;
}

bool LoRaOTA::isUpdateInProgress() {
    return state != LORA_OTA_IDLE && state != LORA_OTA_ERROR;
}

String LoRaOTA::getLastError() {
    return lastError;
}

void LoRaOTA::setEnabled(bool enable) {
    enabled = enable;
}

bool LoRaOTA::isEnabled() {
    return enabled;
}

// Private methods

bool LoRaOTA::processMessage(const uint8_t* data, size_t length) {
    if (length < 1) {
        return false;
    }

    uint8_t messageType = data[0];

    switch (messageType) {
        case LORA_OTA_INIT:
            return handleInitMessage(data, length);
        case LORA_OTA_CHUNK:
            return handleChunkMessage(data, length);
        case LORA_OTA_ACK:
            return handleAckMessage(data, length);
        case LORA_OTA_NACK:
            return handleNackMessage(data, length);
        case LORA_OTA_COMPLETE:
            return handleCompleteMessage(data, length);
        case LORA_OTA_ABORT:
            return handleAbortMessage(data, length);
        default:
            Serial.printf("LoRaOTA: Unknown message type: 0x%02X\n", messageType);
            return false;
    }
}

bool LoRaOTA::handleInitMessage(const uint8_t* data, size_t length) {
    if (length < sizeof(FirmwareMetadata) + 1) {
        Serial.println("LoRaOTA: Invalid INIT message size");
        return false;
    }

    // Copy metadata
    memcpy(&rxMetadata, data + 1, sizeof(FirmwareMetadata));

    Serial.println("LoRaOTA: Received firmware update request");
    Serial.printf("  Version: %s\n", rxMetadata.version);
    Serial.printf("  Size: %u bytes\n", rxMetadata.totalSize);
    Serial.printf("  Chunks: %u\n", rxMetadata.totalChunks);
    Serial.printf("  CRC32: 0x%08X\n", rxMetadata.crc32);

    // Begin ESP32 Update
    if (!Update.begin(rxMetadata.totalSize)) {
        lastError = "Failed to begin update";
        Serial.println("LoRaOTA: " + lastError);
        sendNack(0);
        return false;
    }

    state = LORA_OTA_RECEIVING;
    rxCurrentChunk = 0;
    rxRetryCount = 0;
    rxLastReceiveTime = millis();

    // Send ACK to start receiving chunks
    sendAck(0);
    return true;
}

bool LoRaOTA::handleChunkMessage(const uint8_t* data, size_t length) {
    if (state != LORA_OTA_RECEIVING) {
        Serial.println("LoRaOTA: Received chunk but not in receiving state");
        return false;
    }

    if (length < sizeof(ChunkHeader) + 1) {
        Serial.println("LoRaOTA: Invalid CHUNK message size");
        sendNack(rxCurrentChunk);
        return false;
    }

    ChunkHeader header;
    memcpy(&header, data + 1, sizeof(ChunkHeader));

    // Validate chunk index
    if (header.chunkIndex != rxCurrentChunk) {
        Serial.printf("LoRaOTA: Expected chunk %u, got %u\n", rxCurrentChunk, header.chunkIndex);
        sendNack(rxCurrentChunk);
        return false;
    }

    // Validate chunk data
    const uint8_t* chunkData = data + 1 + sizeof(ChunkHeader);
    if (!validateChunkCRC(chunkData, header.chunkSize, header.chunkCRC)) {
        Serial.printf("LoRaOTA: CRC mismatch for chunk %u\n", header.chunkIndex);
        sendNack(rxCurrentChunk);
        rxRetryCount++;
        return false;
    }

    // Write chunk to flash
    if (!writeChunkToFlash(header.chunkIndex, chunkData, header.chunkSize)) {
        lastError = "Failed to write chunk to flash";
        abortUpdate();
        return false;
    }

    Serial.printf("LoRaOTA: Chunk %u/%u received and written\n", 
                  rxCurrentChunk + 1, rxMetadata.totalChunks);

    rxCurrentChunk++;
    rxRetryCount = 0;
    rxLastReceiveTime = millis();

    // Send ACK
    sendAck(header.chunkIndex);

    // Check if all chunks received
    if (rxCurrentChunk >= rxMetadata.totalChunks) {
        return finalizeUpdate();
    }

    return true;
}

bool LoRaOTA::handleAckMessage(const uint8_t* data, size_t length) {
    if (state != LORA_OTA_TRANSMITTING) {
        return false;
    }

    if (length < 3) {
        return false;
    }

    uint16_t ackedChunk;
    memcpy(&ackedChunk, data + 1, sizeof(uint16_t));

    if (ackedChunk != txCurrentChunk) {
        Serial.printf("LoRaOTA: ACK for wrong chunk (expected %u, got %u)\n", 
                      txCurrentChunk, ackedChunk);
        return false;
    }

    Serial.printf("LoRaOTA: Chunk %u acknowledged\n", ackedChunk);
    txCurrentChunk++;
    txRetryCount = 0;

    uint16_t totalChunks = (txFirmwareSize + LORA_OTA_CHUNK_SIZE - 1) / LORA_OTA_CHUNK_SIZE;
    
    // Check if all chunks sent
    if (txCurrentChunk >= totalChunks) {
        sendComplete();
        Serial.println("LoRaOTA: Firmware transmission complete");
        state = LORA_OTA_IDLE;
        txFirmwareData = nullptr;
        return true;
    }

    // Send next chunk
    if (!sendChunk(txCurrentChunk)) {
        lastError = "Failed to send chunk";
        abortUpdate();
        return false;
    }

    txLastSendTime = millis();
    return true;
}

bool LoRaOTA::handleNackMessage(const uint8_t* data, size_t length) {
    if (state != LORA_OTA_TRANSMITTING) {
        return false;
    }

    if (length < 3) {
        return false;
    }

    uint16_t nackedChunk;
    memcpy(&nackedChunk, data + 1, sizeof(uint16_t));

    Serial.printf("LoRaOTA: Chunk %u NACK received, retrying\n", nackedChunk);
    
    txRetryCount++;
    if (txRetryCount >= LORA_OTA_MAX_RETRIES) {
        lastError = "Too many retries";
        abortUpdate();
        return false;
    }

    // Resend the chunk
    sendChunk(nackedChunk);
    txLastSendTime = millis();
    return true;
}

bool LoRaOTA::handleCompleteMessage(const uint8_t* data, size_t length) {
    Serial.println("LoRaOTA: Update complete message received");
    return true;
}

bool LoRaOTA::handleAbortMessage(const uint8_t* data, size_t length) {
    Serial.println("LoRaOTA: Update aborted by sender");
    
    if (state == LORA_OTA_RECEIVING) {
        Update.abort();
    }
    
    state = LORA_OTA_IDLE;
    return true;
}

bool LoRaOTA::sendInitMessage(const FirmwareMetadata& metadata) {
    uint8_t buffer[sizeof(FirmwareMetadata) + 1];
    buffer[0] = LORA_OTA_INIT;
    memcpy(buffer + 1, &metadata, sizeof(FirmwareMetadata));
    
    return lora.sendData(buffer, sizeof(buffer));
}

bool LoRaOTA::sendChunk(uint16_t chunkIndex) {
    if (!txFirmwareData || chunkIndex >= ((txFirmwareSize + LORA_OTA_CHUNK_SIZE - 1) / LORA_OTA_CHUNK_SIZE)) {
        return false;
    }

    size_t offset = chunkIndex * LORA_OTA_CHUNK_SIZE;
    size_t chunkSize = min((size_t)LORA_OTA_CHUNK_SIZE, txFirmwareSize - offset);
    const uint8_t* chunkData = txFirmwareData + offset;

    // Prepare chunk header
    ChunkHeader header;
    header.messageType = LORA_OTA_CHUNK;
    header.chunkIndex = chunkIndex;
    header.chunkSize = chunkSize;
    header.chunkCRC = calculateCRC32(chunkData, chunkSize);

    // Build packet
    uint8_t buffer[sizeof(ChunkHeader) + LORA_OTA_CHUNK_SIZE + 1];
    buffer[0] = LORA_OTA_CHUNK;
    memcpy(buffer + 1, &header, sizeof(ChunkHeader));
    memcpy(buffer + 1 + sizeof(ChunkHeader), chunkData, chunkSize);

    size_t packetSize = 1 + sizeof(ChunkHeader) + chunkSize;
    
    Serial.printf("LoRaOTA: Sending chunk %u/%u (%u bytes)\n", 
                  chunkIndex + 1, 
                  (txFirmwareSize + LORA_OTA_CHUNK_SIZE - 1) / LORA_OTA_CHUNK_SIZE,
                  chunkSize);

    return lora.sendData(buffer, packetSize);
}

bool LoRaOTA::sendAck(uint16_t chunkIndex) {
    uint8_t buffer[3];
    buffer[0] = LORA_OTA_ACK;
    memcpy(buffer + 1, &chunkIndex, sizeof(uint16_t));
    
    return lora.sendData(buffer, sizeof(buffer));
}

bool LoRaOTA::sendNack(uint16_t chunkIndex) {
    uint8_t buffer[3];
    buffer[0] = LORA_OTA_NACK;
    memcpy(buffer + 1, &chunkIndex, sizeof(uint16_t));
    
    return lora.sendData(buffer, sizeof(buffer));
}

bool LoRaOTA::sendComplete() {
    uint8_t buffer[1];
    buffer[0] = LORA_OTA_COMPLETE;
    
    return lora.sendData(buffer, sizeof(buffer));
}

bool LoRaOTA::sendAbort(const String& reason) {
    Serial.println("LoRaOTA: Sending abort message: " + reason);
    uint8_t buffer[1];
    buffer[0] = LORA_OTA_ABORT;
    
    return lora.sendData(buffer, sizeof(buffer));
}

bool LoRaOTA::writeChunkToFlash(uint16_t chunkIndex, const uint8_t* data, size_t length) {
    size_t written = Update.write(const_cast<uint8_t*>(data), length);
    
    if (written != length) {
        lastError = "Flash write failed";
        Serial.printf("LoRaOTA: Flash write error - expected %u bytes, wrote %u\n", length, written);
        return false;
    }

    return true;
}

bool LoRaOTA::finalizeUpdate() {
    Serial.println("LoRaOTA: Finalizing update...");
    state = LORA_OTA_VALIDATING;

    if (!Update.end(true)) {
        lastError = "Update finalization failed";
        Serial.println("LoRaOTA: " + lastError);
        state = LORA_OTA_ERROR;
        return false;
    }

    Serial.println("LoRaOTA: ✓ Firmware update successful!");
    Serial.println("LoRaOTA: Device will reboot in 3 seconds...");
    
    sendComplete();
    
    delay(3000);
    ESP.restart();
    
    return true;
}

uint32_t LoRaOTA::calculateCRC32(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < length; i++) {
        uint8_t index = (crc ^ data[i]) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[index];
    }
    
    return ~crc;
}

bool LoRaOTA::validateChunkCRC(const uint8_t* data, size_t length, uint32_t expectedCRC) {
    uint32_t actualCRC = calculateCRC32(data, length);
    return actualCRC == expectedCRC;
}
