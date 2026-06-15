#include "hx711_driver.h"

#ifdef HX711_DRIVER_ARDUINO_FALLBACK
void pinMode(uint8_t pin, uint8_t mode) {
    (void)pin;
    (void)mode;
}

void digitalWrite(uint8_t pin, uint8_t value) {
    (void)pin;
    (void)value;
}

int digitalRead(uint8_t pin) {
    (void)pin;
    return LOW;
}

void delayMicroseconds(unsigned int us) {
    (void)us;
}

void yield(void) {
}
#endif

HX711::HX711(uint8_t dout, uint8_t pd_sck, uint8_t gain) {
    dataPin = dout;
    clockPin = pd_sck;
    
    // Determine the number of extra clock pulses for the NEXT reading
    // Gain 128 requires 1 pulse (25 total)
    // Gain 32 requires 2 pulses (26 total)
    // Gain 64 requires 3 pulses (27 total)
    gainPulses = 1; 
    if (gain == 64) gainPulses = 3;
    if (gain == 32) gainPulses = 2;
}

void HX711::begin() {
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, INPUT);
    digitalWrite(clockPin, LOW);
}

bool HX711::isReady() {
    // HX711 pulls DOUT low when data is ready for retrieval
    return digitalRead(dataPin) == LOW;
}

long HX711::readRaw() {
    // Wait until data is ready
    while (!isReady()) {
        yield(); 
    }

    uint8_t data[3] = {0}; // Array to hold the 3 bytes (24 bits)
    uint8_t filler = 0x00;

    // 1. Shift in the 24 bits of data
    for (int8_t i = 2; i >= 0; i--) {
        for (int8_t j = 7; j >= 0; j--) {
            digitalWrite(clockPin, HIGH);
            delayMicroseconds(1); // Ensure pulse is long enough for the HX711
            
            // Read the bit and write it to our array
            bitWrite(data[i], j, digitalRead(dataPin));
            
            digitalWrite(clockPin, LOW);
            delayMicroseconds(1);
        }
    }

    // 2. Pulse the clock to set the gain for the NEXT reading
    for (uint8_t i = 0; i < gainPulses; i++) {
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(1);
        digitalWrite(clockPin, LOW);
        delayMicroseconds(1);
    }

    // 3. Sign-extend the 24-bit two's complement value to 32 bits
    // If the Most Significant Bit (bit 23) is 1, the number is negative
    if (data[2] & 0x80) {
        filler = 0xFF; // Pad the top 8 bits with 1s
    } else {
        filler = 0x00; // Pad the top 8 bits with 0s
    }

    // 4. Reconstruct the 32-bit signed integer using bitwise OR and left-shifts
    unsigned long value = (static_cast<unsigned long>(filler) << 24)
                        | (static_cast<unsigned long>(data[2]) << 16)
                        | (static_cast<unsigned long>(data[1]) << 8)
                        | (static_cast<unsigned long>(data[0]));

    return static_cast<long>(value);
}
