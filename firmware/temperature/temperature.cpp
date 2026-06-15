#include "temperature.h"

TemperatureSensor::TemperatureSensor(uint8_t pin) : oneWire(pin), sensors(&oneWire) {
    currentTemp = 25.0; // Default safe assumption before first read
    lastRequestTime = 0;
    conversionPending = false;
}

void TemperatureSensor::begin() {
    sensors.begin();
    // Disable blocking mode. This allows the MCU to do other things (like reading
    // the HX711 and running the Kalman filter) while the DS18B20 calculates the temp.
    sensors.setWaitForConversion(false); 
}

void TemperatureSensor::update() {
    unsigned long currentMillis = millis();

    // If we aren't waiting for a conversion, start one
    if (!conversionPending) {
        sensors.requestTemperatures();
        lastRequestTime = currentMillis;
        conversionPending = true;
    } 
    // If we are waiting, check if 750ms has passed
    else if (currentMillis - lastRequestTime >= conversionDelay) {
        // Read the temperature (index 0 assumes only one sensor on the bus)
        float tempC = sensors.getTempCByIndex(0);
        
        // Guard against read errors (DS18B20 returns DEVICE_DISCONNECTED_C if error)
        if (tempC != DEVICE_DISCONNECTED_C) {
            currentTemp = tempC;
        }
        
        // Reset flag to start a new conversion on the next loop
        conversionPending = false;
    }
}

float TemperatureSensor::getTemperature() {
    return currentTemp;
}

float TemperatureSensor::compensateWeight(float rawWeight, float refTemp) {
    // Apply the first-order linear correction model:
    // W_compensated = W_raw - alpha * (T - T_ref)
    
    float tempDifference = currentTemp - refTemp;
    float thermalDrift = alpha * tempDifference;
    
    return rawWeight - thermalDrift;
}

TemperatureSensor::AlertState TemperatureSensor::getAlertState() {
    if (currentTemp < TEMP_LOW_THRESHOLD) {
        return ALERT_LOW;
    } else if (currentTemp > TEMP_HIGH_THRESHOLD) {
        return ALERT_HIGH;
    } else {
        return NORMAL;
    }
}