#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Alert thresholds based on Section 3.6.3
#define TEMP_LOW_THRESHOLD 16.0
#define TEMP_HIGH_THRESHOLD 28.0

class TemperatureSensor {
private:
    OneWire oneWire;
    DallasTemperature sensors;
    
    float currentTemp;
    unsigned long lastRequestTime;
    bool conversionPending;
    
    // 750ms is the max conversion time for 12-bit resolution on the DS18B20
    const unsigned long conversionDelay = 750; 
    
    // The experimentally determined thermal drift coefficient (g/°C)
    // Based on Report Section 4.5.1
    const float alpha = 0.421; 

public:
    // Constructor (requires the digital pin number the DS18B20 is connected to)
    TemperatureSensor(uint8_t pin);

    // Initialize the sensor
    void begin();

    // Non-blocking state machine to update temperature
    // Call this repeatedly in the main loop
    void update();

    // Get the most recently read temperature
    float getTemperature();

    // Apply the linear thermal drift compensation model
    float compensateWeight(float rawWeight, float refTemp);

    // Check if temperature is outside the calibrated range
    enum AlertState { NORMAL, ALERT_LOW, ALERT_HIGH };
    AlertState getAlertState();
};

#endif