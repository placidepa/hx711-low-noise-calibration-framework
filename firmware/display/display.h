#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "../temperature/temperature.h"

class DisplayManager {
private:
    LiquidCrystal_I2C lcd;
    uint8_t buzzerPin;

    // Buzzer timing state variables
    unsigned long lastBuzzerTime;
    unsigned long alertStartTime;
    uint8_t pulseCount;
    bool buzzerActive;
    bool inAlertCycle;

    // Based on Section 3.6.3 of the report
    const unsigned long pulseDuration = 500;   // 0.5 seconds
    const unsigned long pulseInterval = 1000;  // 1.0 second between pulses
    const unsigned long alertCooldown = 30000; // 30 seconds before repeating alert

public:
    // Constructor (LCD I2C address is standard 0x27)
    DisplayManager(uint8_t bzPin);

    // Initialize LCD and Buzzer pin
    void begin();

    // Update Row 1: "Wt: XX.XX g" (Appends '*' if temp is out of bounds)
    void updateWeight(float weight, bool tempWarning);

    // Update Row 2: "T: XX.X C OK" or "!HI" / "!LO"
    void updateTemperature(float temp, TemperatureSensor::AlertState state);

    // Non-blocking state machine for the buzzer pattern
    void handleBuzzer(TemperatureSensor::AlertState state);

    // Turn off LCD backlight (used by power manager)
    void sleep();

    // Turn on LCD backlight
    void wake();
};

#endif