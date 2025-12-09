#include "oled.hpp"

#include "esp_logger.hpp"

using namespace ESPLogger;

OLED::OLED(uint8_t addr) : m_addr(addr) {}

void OLED::begin() {
    if (!m_oled.begin(SSD1306_SWITCHCAPVCC, m_addr)) {
        log("OLED initialisation failed, aborting.");
        while (true);
    }

    m_oled.clearDisplay();
    m_oled.setTextColor(WHITE);
    m_oled.setFont(FONT);
    m_oled.setTextWrap(false);

    set_text_size(MEDIUM);
    m_oled.setCursor( /* center text */
        12, ((m_height - FONT_HEIGHT * MEDIUM) / 2) + (FONT_HEIGHT * MEDIUM)
    );
    m_oled.println("ratatoskr");
    m_oled.display();
    delay(1000);
}

void OLED::clear() {
    if (!m_frozen) m_oled.clearDisplay();
}

void OLED::display() {
    if (!m_frozen) m_oled.display();
}

void OLED::freeze() { m_frozen = true; }
void OLED::unfreeze() { m_frozen = false; }

void OLED::set_text_size(TextSize size) { m_oled.setTextSize((int)size); }

void OLED::set_cursor(int line, LinePos pos) {
    int x_pos = 0;
    switch (pos) {
        case LEFT:
            x_pos = 0;
            break;
        case MID_LEFT:
            x_pos = m_width / 2 - (FONT_WIDTH * MEDIUM * 5) + 2;
            break;
        case MID:
            x_pos = m_width / 2 - (int)floor(FONT_WIDTH * MEDIUM * 1.5) - 2;
            break;
        case MID_RIGHT:
            x_pos = m_width / 2 + 10;
            break;
        case RIGHT:
            x_pos = m_width - (FONT_WIDTH * MEDIUM * 3 + 5);
            break;
    }

    int y_pos = line * FONT_HEIGHT * MEDIUM + (5 * (line == 2));
    m_oled.setCursor(x_pos, y_pos);
}

void OLED::update_status_bar(int16_t gyro_angle, uint16_t left_rpm,
                             uint16_t right_rpm) {
    set_cursor(1, LEFT);
    m_oled.printf("%03d", right_rpm);

    set_cursor(1, MID);
    m_oled.printf("%03d", gyro_angle);

    set_cursor(1, RIGHT);
    m_oled.printf("%03d", left_rpm);
}

void OLED::update_ToFs(uint16_t tof_left, uint16_t tof_front_left,
                       uint16_t tof_front_right, uint16_t tof_right) {
    set_cursor(2, LEFT);
    m_oled.printf("%03d", tof_right);

    set_cursor(2, RIGHT);
    m_oled.printf("%03d", tof_left);

    set_cursor(4, MID_LEFT);
    m_oled.printf("%03d", tof_front_right);

    set_cursor(4, MID_RIGHT);
    m_oled.printf("%03d", tof_front_left);
}

void OLED::draw_face(Face face) {
    m_oled.drawXBitmap(0, 0, face_map[face], m_width, m_height, WHITE);
}
