#pragma once

/* SSD1306 with a resolution of 128x64 pixels */
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#define CHAR_WIDTH 9 /* in pt */

enum TextSize { SMALL = 1, MEDIUM = 2, LARGE = 3 };

class OLED {
   private:
    uint8_t m_width = 128;
    uint8_t m_height = 64;
    uint8_t m_addr = 0x3C;
    uint8_t m_reset = 4;

    bool m_frozen = false;
    Adafruit_SSD1306 m_oled = Adafruit_SSD1306(m_width, m_height, &Wire, m_reset);

   public:
    OLED(uint8_t addr = 0x3C);
    void begin();

    void set_text_size(TextSize size);
    void clear();
    void display();
    void freeze();
    void unfreeze();

    void update_status_bar(String text);
    void update_ToFs();
    void idle();
};
