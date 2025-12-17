/* Deek-Robot 8x8 LED Matrix (red) w/ HT16K33 Backpack controlled using Adafruit
 * LED backpack library */

#pragma once

#include <Adafruit_GFX.h>

#include <cstdint>

#include "Adafruit_LEDBackpack.h"
#include "util.hpp"

class LEDMatrix {
   private:
    int16_t m_h = 8;
    int16_t m_w = 8;
    uint8_t m_addr = 0x70;
    Adafruit_8x8matrix m_matrix = Adafruit_8x8matrix();

   public:
    /**
     * Constructor for LEDMatrix
     */
    LEDMatrix(uint8_t addr);

    /**
     * 0 -> side-left
     * 1 -> top-left
     * 2 -> top-right
     * 3 -> side-right
     * @params id of the wall to be drawn
     */
    void drawWall(uint8_t id);

    /**
     * 0 -> side-left
     * 1 -> top-left
     * 2 -> top-right
     * 3 -> side-right
     * @params id of the wall to be cleared
     */
    void clearWall(uint8_t id);

    /**
     * Clear LED-Matrix.
     */
    void clearAll();
};
