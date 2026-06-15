#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H

class KalmanFilter {
private:
    float x_est; // State estimate (current filtered weight)
    float P_est; // Error covariance
    float Q;     // Process noise covariance
    float R;     // Measurement noise covariance

public:
    /**
     * Constructor
     * Default parameters derived from experimental noise characterization:
     * q = 1e-4 (Process noise)
     * r = 1e-2 (Measurement noise)
     */
    KalmanFilter(float q = 1e-4, float r = 1e-2, float p = 1.0, float initial_value = 0.0);
    
    /**
     * Executes the Prediction and Update steps of the Kalman Filter
     * @param z_k The raw measurement from the HX711 ADC
     * @return The updated optimal state estimate (filtered weight)
     */
    float update(float z_k);
    
    /**
     * Resets the filter state (useful after a tare or calibration event)
     */
    void reset(float initial_value = 0.0);
};

#endif
