#include "../HX711_driver/hx711_driver.h"
#include "../kalman/kalman_filter.h"
#include "../calibration/calibration.h"
#include "../memory/eeprom_manager.h"
#include "../temperature/temperature.h"
#include "../power/power_mgmt.h"
#include "../display/display.h"

// Pin Definitions based on ATmega328P layout
#define DOUT_PIN 3
#define SCK_PIN 4
#define ONE_WIRE_PIN 2
#define BUZZER_PIN 5
#define HX711_POWER_PIN 6
#define LCD_POWER_PIN 7

// Object Instantiation
HX711 scale(DOUT_PIN, SCK_PIN, 128);
KalmanFilter kalman;
ScaleCalibration calModel;
EEPROMManager memory;
TemperatureSensor tempSensor(ONE_WIRE_PIN);
PowerManager power(HX711_POWER_PIN, LCD_POWER_PIN);
DisplayManager display(BUZZER_PIN);

// Global State
float refTemperature = 25.0;

void setup() {
    Serial.begin(9600);
    
    // Initialize Hardware
    power.begin();
    display.begin();
    scale.begin();
    tempSensor.begin();

    // Load Calibration Parameters from EEPROM
    float m, b, tRef;
    if (memory.loadParameters(m, b, tRef)) {
        calModel.setParameters(m, b);
        refTemperature = tRef;
    } else {
        // Fallback defaults if EEPROM is blank or corrupted
        calModel.setParameters(0.0000604, 7.50); // Experimental defaults from report
        refTemperature = 25.0;
    }
}

void loop() {
    // 1. Asynchronously update temperature
    tempSensor.update();

    // 2. Read weight when HX711 data is ready (10 Hz)
    if (scale.isReady()) {
        long rawCount = scale.readRaw();

        // 3. Apply discrete-time Kalman filter to the raw ADC count
        float filteredCount = kalman.update((float)rawCount);

        // 4. Convert filtered count to mass using linear regression model
        float rawWeight = calModel.getWeight(filteredCount);

        // 5. Apply temperature drift compensation
        float compensatedWeight = tempSensor.compensateWeight(rawWeight, refTemperature);

        // 6. Check power manager for sleep conditions
        bool justWokeUp = power.update(compensatedWeight);
        if (justWokeUp) {
            display.wake();
            // Re-initialize any display states if necessary
        }

        // 7. Update User Interface
        TemperatureSensor::AlertState tState = tempSensor.getAlertState();
        bool isWarning = (tState != TemperatureSensor::NORMAL);
        
        display.updateWeight(compensatedWeight, isWarning);
        display.updateTemperature(tempSensor.getTemperature(), tState);
        display.handleBuzzer(tState);
    }
}