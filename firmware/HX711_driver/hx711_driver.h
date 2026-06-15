#ifndef HX711_DRIVER_H
#define HX711_DRIVER_H

#if defined(ARDUINO)
#include <Arduino.h>
#else
#define HX711_DRIVER_ARDUINO_FALLBACK
#endif

#ifdef HX711_DRIVER_ARDUINO_FALLBACK
#include <stdint.h>

#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define bitWrite(value, bit, bitvalue) \
    ((bitvalue) ? ((value) |= (1UL << (bit))) : ((value) &= ~(1UL << (bit))))

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t value);
int digitalRead(uint8_t pin);
void delayMicroseconds(unsigned int us);
void yield(void);
#endif

class HX711 {
private:
    uint8_t dataPin;
    uint8_t clockPin;
    uint8_t gainPulses;

public:
    // Constructor
    HX711(uint8_t dout, uint8_t pd_sck, uint8_t gain = 128);
    
    // Setup pins
    void begin();
    
    // Check if ADC is ready
    bool isReady();
    
    // The bit-bang routine to extract the 24-bit reading
    long readRaw();
};

#endif
