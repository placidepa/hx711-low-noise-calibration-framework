#include "power_mgmt.h"

// Global flag set by the Watchdog Timer Interrupt
volatile bool wdt_woke_up = false;

// Watchdog Interrupt Service Routine (ISR)
ISR(WDT_vect) {
    wdt_woke_up = true;
}

PowerManager::PowerManager(uint8_t hx711Pin, uint8_t lcdPin) {
    hx711PowerPin = hx711Pin;
    lcdPowerPin = lcdPin;
    lastActivityTime = 0;
    lastRecordedWeight = 0.0;
}

void PowerManager::begin() {
    pinMode(hx711PowerPin, OUTPUT);
    pinMode(lcdPowerPin, OUTPUT);
    wakePeripherals(); // Ensure peripherals are on at boot
    lastActivityTime = millis();
}

void PowerManager::setupWatchdog() {
    // Disable interrupts while configuring the WDT
    cli(); 
    
    // Reset watchdog
    wdt_reset(); 
    
    // Clear WDT reset flag
    MCUSR &= ~(1 << WDRF); 
    
    // Set WDCE (Watchdog Change Enable) and WDE (Watchdog System Reset Enable)
    WDTCSR |= (1 << WDCE) | (1 << WDE); 
    
    // Configure for 500ms timeout (WDP2 and WDP0 set) and enable interrupt mode (WDIE)
    WDTCSR = (1 << WDIE) | (1 << WDP2) | (1 << WDP0); 
    
    // Re-enable interrupts
    sei(); 
}

bool PowerManager::update(float currentWeight) {
    bool justWokeUp = false;

    // Check if the weight change exceeds the 0.5g threshold
    if (abs(currentWeight - lastRecordedWeight) > activityThresholdGrams) {
        lastActivityTime = millis();
        lastRecordedWeight = currentWeight;
    }

    // If idle time exceeds 10 seconds, enter sleep
    if ((millis() - lastActivityTime) > sleepTimeoutMs) {
        enterSleep();
        
        // When execution resumes here, the MCU has woken up.
        // The main loop should now do a quick HX711 read to check for activity.
        justWokeUp = true; 
        lastActivityTime = millis(); // Reset timer to give the system time to evaluate
    }

    return justWokeUp;
}

void PowerManager::enterSleep() {
    // 1. Power down peripherals via MOSFETs
    digitalWrite(hx711PowerPin, LOW);
    digitalWrite(lcdPowerPin, LOW);

    // 2. Configure Watchdog to wake us up in 500 ms
    setupWatchdog();

    // 3. Enter Power-save/Power-down mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    
    // Optional: disable BOD (Brown-out Detection) during sleep to save more power
    #if defined(BODS) && defined(BODSE)
        MCUCR |= (1 << BODS) | (1 << BODSE);
        MCUCR &= ~(1 << BODSE);
    #endif

    // Put MCU to sleep here. Execution stops until the WDT fires.
    sleep_cpu(); 

    // --- WAKE UP POINT ---
    // MCU resumes execution here after the WDT ISR completes.
    
    sleep_disable();
    
    // Disable Watchdog until we need to sleep again
    cli();
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = 0x00; 
    sei();

    // 4. Restore power to peripherals so the main loop can check for activity
    wakePeripherals();
}

void PowerManager::wakePeripherals() {
    digitalWrite(hx711PowerPin, HIGH);
    digitalWrite(lcdPowerPin, HIGH);
}