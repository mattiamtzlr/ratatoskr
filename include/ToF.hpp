#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <VL53L4CD.h>         // Pololu long-range ToF (VL53L4CD)
#include <Adafruit_VL6180X.h> // Adafruit short-range ToF (VL6180X)

// we're on a TCA9548A mux, so we select exactly one channel before any I2C call.
#define TCA_ADDR 0x70

struct ToFReading {
  uint16_t mm = 0;
  bool valid = false;
};

class ToF {
public:
  // common API every sensor supports
  virtual bool start() = 0;       // start ranging (VL53 continuous; VL618 polled)
  virtual ToFReading read() = 0;  // one reading (returns last if no fresh sample)

  bool isValid() const { return last_.valid; }
  const char* label() const { return label_; }
  uint8_t channel() const { return ch_; }
  bool isInitialized() const { return initialized_; }

protected:
  ToF(uint8_t mux_channel, const char* label)
    : ch_(mux_channel), label_(label) {}

  // helper: select our mux branch
  void selectMux_() const {
    Wire.beginTransmission(TCA_ADDR);
    Wire.write(1 << ch_);
    Wire.endTransmission();
    delay(1); // tiny settle cuz thats what others do
  }

  // shared state
  uint8_t ch_;
  const char* label_;
  bool initialized_ = false;
  bool running_ = false;
  ToFReading last_{};
};

// ----------------- Long range: VL53L4CD (Pololu) -----------------
class ToFVL53 : public ToF {
public:
  // constructor now performs init/probe + basic config
  ToFVL53(uint8_t mux_channel, const char* label);

  bool start() override;
  ToFReading read() override;

private:
  VL53L4CD vl53_; // Pololu API instance
};

// ----------------- Short range: VL6180X (Adafruit) ----------------
class ToFVL6180 : public ToF {
public:
  // constructor now performs init/probe + basic config
  ToFVL6180(uint8_t mux_channel, const char* label);

  bool start() override;
  ToFReading read() override;

private:
  Adafruit_VL6180X vl618_; // Adafruit API instance
};