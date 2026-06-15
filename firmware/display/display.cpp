#include "display.h"

// Initialize LCD at address 0x27 for a 16 columns x 2 rows display
DisplayManager::DisplayManager(uint8_t bzPin) : lcd(0x27, 16, 2) {
    buzzerPin = bzPin;
    lastBuzzerTime = 0;
    alertStartTime = 0;
    pulseCount = 0;
    buzzerActive = false;
    inAlertCycle = false;
}

void DisplayManager::begin() {
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);
    
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Booting..");
}

void DisplayManager::updateWeight(float weight, bool tempWarning) {
    lcd.setCursor(0, 0);
    lcd.print("Wt: ");
    
    // Format weight to 2 decimal places
    lcd.print(weight, 2);
    
    // Append asterisk if outside calibrated thermal range
    if (tempWarning) {
        lcd.print("* g    "); 
    } else {
        lcd.print(" g     "); 
    }
}

void DisplayManager::updateTemperature(float temp, TemperatureSensor::AlertState state) {
    lcd.setCursor(0, 1);
    lcd.print("T: ");
    lcd.print(temp, 1);
    lcd.print(" C ");

    if (state == TemperatureSensor::NORMAL) {
        lcd.print("OK  ");
    } else if (state == TemperatureSensor::ALERT_HIGH) {
        lcd.print("!HI ");
    } else if (state == TemperatureSensor::ALERT_LOW) {
        lcd.print("!LO ");
    }
}

void DisplayManager::handleBuzzer(TemperatureSensor::AlertState state) {
    unsigned long currentMillis = millis();

    // If we are back to normal, silence the buzzer and reset states
    if (state == TemperatureSensor::NORMAL) {
        digitalWrite(buzzerPin, LOW);
        buzzerActive = false;
        inAlertCycle = false;
        return;
    }

    // Start a new 30-second alert cycle if we aren't in one
    if (!inAlertCycle || (currentMillis - alertStartTime >= alertCooldown)) {
        inAlertCycle = true;
        alertStartTime = currentMillis;
        pulseCount = 0;
        buzzerActive = false;
        lastBuzzerTime = currentMillis;
    }

    // Execute the 3-pulse pattern (0.5s ON, 1s OFF)
    if (inAlertCycle && pulseCount < 3) {
        if (!buzzerActive && (currentMillis - lastBuzzerTime >= pulseInterval)) {
            digitalWrite(buzzerPin, HIGH);
            buzzerActive = true;
            lastBuzzerTime = currentMillis;
        } 
        else if (buzzerActive && (currentMillis - lastBuzzerTime >= pulseDuration)) {
            digitalWrite(buzzerPin, LOW);
            buzzerActive = false;
            lastBuzzerTime = currentMillis;
            pulseCount++; // One complete pulse finished
        }
    }
}

void DisplayManager::sleep() {
    lcd.noBacklight();
}

void DisplayManager::wake() {
    lcd.backlight();
}