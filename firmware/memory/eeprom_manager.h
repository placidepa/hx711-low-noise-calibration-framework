#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <Arduino.h>
#include <EEPROM.h>

// Standard flag to verify EEPROM has been formatted/written by our system
#define EEPROM_VALIDITY_FLAG 0xA5A5
#define EEPROM_START_ADDRESS 0

// Data structure mirroring the EEPROM memory map
struct CalibrationMemory {
    uint16_t validityFlag; // 2 bytes (0xA5A5)
    float m;               // 4 bytes (Calibration slope)
    float b;               // 4 bytes (Calibration intercept)
    float t_ref;           // 4 bytes (Reference temperature)
    uint8_t checksum;      // 1 byte  (Data integrity verification)
};

class EEPROMManager {
private:
    // Calculates a simple additive checksum over the struct bytes
    uint8_t calculateChecksum(const CalibrationMemory& data);

public:
    EEPROMManager();

    // Saves the parameters to EEPROM, automatically handling the flag and checksum
    void saveParameters(float slope, float intercept, float refTemp);

    // Loads parameters from EEPROM. 
    // Returns true if the validity flag and checksum are correct.
    bool loadParameters(float &slope, float &intercept, float &refTemp);

    // Wipes the EEPROM validity flag to force a recalibration on next boot
    void formatEEPROM();
};

#endif