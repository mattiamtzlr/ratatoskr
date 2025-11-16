#include <utility>

// PID Controller Class
class PID {
   public:
    PID();

    std::pair<int, int> update(float time_step, float leftDistance,
                               float rightDistance, float leftEncoder,
                               float rightEncoder);

    void reset();

   private:
    // PID coefficients
    double distanceKp;
    double distanceKi;
    double distanceKd;

    double encoderKp;
    double encoderKi;
    double encoderKd;

    // PID state variables
    double distancePrevError;
    double distanceIntegral;
    double encoderPrevError;
    double encoderIntegral;

    double basePWM_forward;
    int corrected_left_PWM;
    int corrected_right_PWM;
};
