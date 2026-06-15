#include "eeprom_manager.h"

EEPROMManager::EEPROMManager() {
    // Constructor
}

uint8_t EEPROMManager::calculateChecksum(const CalibrationMemory& data) {
    uint8_t checksum = 0;
    
    // Create a byte pointer to read the struct sequentially
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&data);
    
    // Sum all bytes EXCEPT the last one (which is the checksum itself)
    size_t sizeToSum = sizeof(CalibrationMemory) - sizeof(uint8_t);
    for (size_t i = 0; i < sizeToSum; i++) {
        checksum += ptr[i];
    }
    
    return checksum;
}

void EEPROMManager::saveParameters(float slope, float intercept, float refTemp) {
    CalibrationMemory memoryBlock;
    
    memoryBlock.validityFlag = EEPROM_VALIDITY_FLAG;
    memoryBlock.m = slope;
    memoryBlock.b = intercept;
    memoryBlock.t_ref = refTemp;
    
    // Calculate and assign the checksum
    memoryBlock.checksum = calculateChecksum(memoryBlock);
    
    // Write the entire struct to EEPROM address 0
    EEPROM.put(EEPROM_START_ADDRESS, memoryBlock);
}

bool EEPROMManager::loadParameters(float &slope, float &intercept, float &refTemp) {
    CalibrationMemory memoryBlock;
    
    // Read the entire struct from EEPROM
    EEPROM.get(EEPROM_START_ADDRESS, memoryBlock);
    
    // 1. Check the validity flag (prevents reading blank 0xFF or zeroed 0x00 memory)
    if (memoryBlock.validityFlag != EEPROM_VALIDITY_FLAG) {
        return false;
    }
    
    // 2. Verify data integrity using the checksum
    uint8_t expectedChecksum = calculateChecksum(memoryBlock);
    if (memoryBlock.checksum != expectedChecksum) {
        return false; // Data corrupted
    }
    
    // Data is valid; pass it back to the referenced variables
    slope = memoryBlock.m;
    intercept = memoryBlock.b;
    refTemp = memoryBlock.t_ref;
    
    return true;
}

void EEPROMManager::formatEEPROM() {
    // Overwriting the start address with 0x00 invalidates the 0xA5A5 flag
    EEPROM.write(EEPROM_START_ADDRESS, 0x00);
    EEPROM.write(EEPROM_START_ADDRESS + 1, 0x00);
}