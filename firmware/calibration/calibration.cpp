#include "calibration.h"

ScaleCalibration::ScaleCalibration() {
    m = 0.0;
    b = 0.0;
    numPoints = 0;
}

void ScaleCalibration::setParameters(float slope, float intercept) {
    m = slope;
    b = intercept;
}

bool ScaleCalibration::addPoint(float adcCount, float knownMass) {
    if (numPoints < MAX_CALIBRATION_POINTS) {
        counts[numPoints] = adcCount;
        masses[numPoints] = knownMass;
        numPoints++;
        return true;
    }
    return false; // Array is full
}

void ScaleCalibration::resetPoints() {
    numPoints = 0;
}

bool ScaleCalibration::calculateRegression() {
    if (numPoints < 2) {
        return false; // Need at least 2 points for a line
    }

    double sumC = 0.0;
    double sumW = 0.0;
    double sumCW = 0.0;
    double sumC2 = 0.0;

    for (uint8_t i = 0; i < numPoints; i++) {
        sumC += counts[i];
        sumW += masses[i];
        sumCW += (counts[i] * masses[i]);
        sumC2 += (counts[i] * counts[i]);
    }

    double N = (double)numPoints;

    // Equation 3.19: m = (N * Σ(CW) - ΣC * ΣW) / (N * Σ(C^2) - (ΣC)^2)
    double denominator = (N * sumC2) - (sumC * sumC);
    
    // Prevent division by zero if all counts are identical
    if (denominator == 0.0) {
        return false; 
    }

    m = (float)(((N * sumCW) - (sumC * sumW)) / denominator);

    // Equation 3.20: b = (ΣW - m * ΣC) / N
    b = (float)((sumW - (m * sumC)) / N);

    return true;
}

float ScaleCalibration::getWeight(float adcCount) {
    // Equation 3.18: W_hat = m * C + b
    return (m * adcCount) + b;
}

float ScaleCalibration::getSlope() {
    return m;
}

float ScaleCalibration::getIntercept() {
    return b;
}