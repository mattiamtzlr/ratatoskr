#include "ToF.hpp"

// gate I2C to just our branch on the mux
void ToF::tcaSelect_() {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << ch_);
  Wire.endTransmission();
  delay(1); // tiny settle helps (idk ive seen others do this)
}

bool ToF::begin() {
  tcaSelect_();

  if (type_ == Type::Long_VL53L4CD) {
    // default I2C addr for VL53L4CD is 0x29 (mux isolates so it's fine)
    if (!vl53_.begin(0x29, &Wire)) return false;
    ok_ = true;
  } else {
    // VL6180X init is just begin() on the default addr behind the mux
    if (!vl618_.begin()) return false;
    ok_ = true;
  }
  running_ = false;
  last_ = {};
  return ok_;
}

bool ToF::start() {
  if (!ok_) return false;
  tcaSelect_();

  if (type_ == Type::Long_VL53L4CD) {
    running_ = vl53_.startRanging(); // continuous mode
  } else {
    // VL6180X doesn’t need a start; we just mark it as running for symmetry
    running_ = true;
  }
  return running_;
}

ToF::Reading ToF::read() {
  Reading r{};
  if (!ok_ || !running_) return r;

  tcaSelect_();

  if (type_ == Type::Long_VL53L4CD) {
    // non-blocking: only when a fresh sample is ready
    if (vl53_.dataReady()) {
      vl53_.clearInterrupt();
      if (!vl53_.read()) {
        r.valid = false;
        last_ = r;
        return r;
      }
      r.mm = vl53_.distance();
      // basic sanity window
      r.valid = (r.mm > 0 && r.mm < 2000);
      last_ = r;
      return r;
    } else {
      // no new sample yet; return last known (could be invalid)
      return last_;
    }
  } else {     
    // VL6180X: single-shot read + status check
    uint8_t mm_u8 = vl618_.readRange();
    uint8_t st = vl618_.readRangeStatus();
    if (st == VL6180X_ERROR_NONE) {
      r.mm = static_cast<uint16_t>(mm_u8);
      r.valid = (r.mm > 0 && r.mm <= 255);
    } else {
      r.valid = false;
    }
    last_ = r;
    return r;
  }
}
