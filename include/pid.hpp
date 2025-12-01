#pragma once
// PID Controller Class
class PID {
   public:
    PID(float k_p, float k_i, float k_d);

    /**
     * Update function for the PID.
     *
     * @param error and time since the last update.
     * @return the corrected signal.
     */
    float update(float time_step, float error);

    /**
     * Reset the PID.
     *
     * Will set the integral and the previous error to zero.
     *
     */
    void reset();

   private:
    // Coefficients
    float k_p;
    float k_i;
    float k_d;

    // State Variables
    float previous_error;
    float integral;
};
