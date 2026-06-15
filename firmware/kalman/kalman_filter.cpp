#include "kalman_filter.h"

KalmanFilter::KalmanFilter(float q, float r, float p, float initial_value) {
    Q = q;
    R = r;
    P_est = p;
    x_est = initial_value;
}

float KalmanFilter::update(float z_k) {
    // --- PREDICTION STEP ---
    // A priori state estimate (identity state transition matrix)
    float x_pred = x_est; 
    
    // A priori error covariance
    float P_pred = P_est + Q; 

    // --- UPDATE STEP ---
    // Calculate the Kalman Gain
    float K = P_pred / (P_pred + R); 
    
    // Posteriori state estimate (innovation-weighted blend)
    x_est = x_pred + K * (z_k - x_pred); 
    
    // Posteriori error covariance
    P_est = (1 - K) * P_pred; 

    return x_est;
}

void KalmanFilter::reset(float initial_value) {
    x_est = initial_value;
    P_est = 1.0; // Reset uncertainty
}