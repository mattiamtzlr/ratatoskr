#pragma once

/* SSD1306 with a resolution of 128x64 pixels */
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <GFX_fonts/Font5x5Fixed.h>

#include <map>

#include "faces.hpp"
#include "util.hpp"

#define SSD1306_NO_SPLASH /* this disables the splash screen */

#define FONT &Font5x5Fixed
#define FONT_WIDTH 5 /* set manually */
#define FONT_HEIGHT 5 /* set manually */

enum TextSize { SMALL = 1, MEDIUM = 2, LARGE = 3, HUGE = 4 };
enum Face { MAD, NEUTRAL, HAPPY, BLINK };
static std::map<Face, uint8_t*> face_map = {
    {HAPPY, happy_bits},
};

class OLED {
   private:
    uint8_t m_width = 128;
    uint8_t m_height = 64;
    uint8_t m_addr = 0x3C;
    uint8_t m_reset = 4;

    bool m_frozen = false;
    Adafruit_SSD1306 m_oled =
        Adafruit_SSD1306(m_width, m_height, &Wire, m_reset);

    unsigned long last_micros = 0;

   public:
    MODE mode = RUN;

    OLED(uint8_t addr = 0x3C);
    void begin();

    void set_text_size(TextSize size);
    void clear();
    void display();
    void freeze();
    void unfreeze();

    void update_status_bar(int16_t gyro_angle, uint16_t left_rpm,
                           uint16_t right_rpm);
    void update_ToFs(uint16_t tof_left, uint16_t tof_front_left,
                     uint16_t tof_front_right, uint16_t tof_right);
    void idle();
};
