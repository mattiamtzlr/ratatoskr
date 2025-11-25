// PID Controller Class
class PID {
   public:
    PID(float time_step, float k_p, float k_i, float k_d, float lower,
        float upper);

    float update(float error);
    void reset();
    // Bounds
    const float UPPER_BOUND;
    const float LOWER_BOUND;
    float TIME_STEP;

   private:
    // Coefficients
    float k_p;
    float k_i;
    float k_d;

    // State Variables
    float previous_error;
    float integral;

    // Output
    float corrected_signal;
};
