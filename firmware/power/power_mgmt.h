#ifndef POWER_MGMT_H
#define POWER_MGMT_H

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

class PowerManager {
private:
    uint8_t hx711PowerPin;
    uint8_t lcdPowerPin;

    unsigned long lastActivityTime;
    float lastRecordedWeight;

    // Thresholds defined in Report Section 3.7.2
    const unsigned long sleepTimeoutMs = 10000; // 10 seconds
    const float activityThresholdGrams = 0.5;   // 0.5 g difference

    // Configures the Watchdog Timer for a 500 ms interrupt
    void setupWatchdog();

public:
    // Constructor requires the GPIO pins controlling the MOSFET switches
    PowerManager(uint8_t hx711Pin, uint8_t lcdPin);

    // Initialize pins and power states
    void begin();

    // Monitors weight changes and triggers sleep if idle
    // Returns true if the system was just woken up (useful for re-initializing displays)
    bool update(float currentWeight);

    // Executes the power-down sequence and puts the MCU to sleep
    void enterSleep();

    // Wakes up peripherals after the MCU is awakened by the WDT
    void wakePeripherals();
};

#endif