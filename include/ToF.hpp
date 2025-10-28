#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_VL53L4CD.h"
#include "Adafruit_VL6180X.h"

// super small wrapper for one ToF sensor behind a TCA9548A channel.
// we have two kinds:
//  - VL6180X (short range)
//  - VL53L4CD (long range)
//
// defaults:
//  - we use Wire (no Wire1 stuff)
//  - TCA address is 0x70 (change the #define if your board is different)

#define TCA_ADDR 0x70

class ToF {
public:
  enum class Type : uint8_t { Short_VL6180X, Long_VL53L4CD };

  struct Reading {
    uint16_t mm = 0;
    bool valid = false;
  };

  // keep it simple: only the mux channel, the sensor type, and a label for prints
  ToF(uint8_t tca_channel, Type type, const char* label)
  : ch_(tca_channel), type_(type), label_(label) {}

  // probe the sensor on its mux channel
  bool begin();

  // start continuous ranging for VL53; no-op for VL6180X (we just poll it)
  bool start();

  // get one distance reading (mm + valid flag)
  Reading read();

  // last reading validity (just a tiny helper)
  bool isValid() const { return last_.valid; }

  const char* label() const { return label_; }
  uint8_t channel() const { return ch_; }
  Type type() const { return type_; }

private:
  // select one TCA9548A channel (one-hot mask)
  void tcaSelect_();

  // drivers (we only use the one that matches type_)
  Adafruit_VL53L4CD vl53_;
  Adafruit_VL6180X  vl618_;

  uint8_t ch_;
  Type type_;
  const char* label_;

  bool ok_ = false;
  bool running_ = false;
  Reading last_{};
};
