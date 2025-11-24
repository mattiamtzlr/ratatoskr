// PID Controller Class
class PID {
   public:
    PID(float k_p, float k_i, float k_d);

    float update(float time_step, float error);
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
