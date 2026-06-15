#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>

#define MAX_CALIBRATION_POINTS 10

class ScaleCalibration {
private:
    float m; // Calibration slope (g/count)
    float b; // Calibration intercept (g)
    
    // Arrays to hold calibration data points
    float counts[MAX_CALIBRATION_POINTS];
    float masses[MAX_CALIBRATION_POINTS];
    uint8_t numPoints;

public:
    // Constructor
    ScaleCalibration();

    // Manually set known calibration parameters (e.g., loaded from EEPROM)
    void setParameters(float slope, float intercept);

    // Add a single calibration point (filtered ADC count vs. known certified mass)
    bool addPoint(float adcCount, float knownMass);

    // Clear current calibration points
    void resetPoints();

    // Perform Ordinary Least-Squares Linear Regression to find m and b
    // Returns true if successful, false if not enough points
    bool calculateRegression();

    // Apply the calibration model: W_hat = m * C + b
    float getWeight(float adcCount);

    // Getters for the coefficients
    float getSlope();
    float getIntercept();
};

#endif