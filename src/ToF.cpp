#include "ToF.hpp"

// =================== ToFVL53 (VL53L4CD) ===================

// constructor: probe + basic config
ToFVL53::ToFVL53(uint8_t mux_channel, const char* label)
  : ToF(mux_channel, label) {
  selectMux_();
  
  // Pololu API: init() -> true on success
  if (vl53_.init()) {
    // (optional) timing setup; defaults are hopefully ok. Uncomment if you want faster updates:
    // vl53_.setRangeTiming(50, 0); // 50 ms budget, continuous mode (0 = unlimited)
    initialized_ = true;
  } else {
    initialized_ = false;
  }
  
  running_ = false;
  last_ = {};
}

bool ToFVL53::start() {
  if (!initialized_) return false;
  selectMux_();
  
  // continuous mode; read() will poll dataReady()
  vl53_.startContinuous();
  running_ = true;
  return true;
}

ToFReading ToFVL53::read() {
  ToFReading r{};
  if (!initialized_ || !running_) return r;
  
  selectMux_();
  if (vl53_.dataReady()) {
    // read(false) = just fetch value; then clear interrupt
    r.mm = vl53_.read(false);
    vl53_.clearInterrupt();
    
    // status 0 = good; also require positive distance
    r.valid = (vl53_.ranging_data.range_status == 0 && r.mm > 0);
    last_ = r;
    return r;
  }
  
  // no fresh sample — return the last one we stored
  return last_;
}

// =================== ToFVL6180 (VL6180X) ===================

// constructor: probe + basic config
ToFVL6180::ToFVL6180(uint8_t mux_channel, const char* label)
  : ToF(mux_channel, label) {
  selectMux_();
  
  // Adafruit API: begin() -> true on success
  if (vl618_.begin()) {
    initialized_ = true;
  } else {
    initialized_ = false;
  }
  
  running_ = false;
  last_ = {};
}

bool ToFVL6180::start() {
  if (!initialized_) return false;
  
  // nothing to "start" for VL6180X; we just poll in read()
  running_ = true;
  return true;
}

ToFReading ToFVL6180::read() {
  ToFReading r{};
  if (!initialized_ || !running_) return r;
  
  selectMux_();
  
  // single-shot read + status check
  uint8_t mm8 = vl618_.readRange();
  uint8_t st  = vl618_.readRangeStatus();
  if (st == 0) {
    r.mm = static_cast<uint16_t>(mm8);
    r.valid = (r.mm > 0);
  } else {
    r.valid = false;
  }
  
  last_ = r;
  return r;
}