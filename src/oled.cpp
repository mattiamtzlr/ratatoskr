#include "oled.hpp"

OLED::OLED(uint8_t addr) : m_addr(addr) {}

void OLED::begin() {
    if (!m_oled.begin(SSD1306_SWITCHCAPVCC, m_addr)) {
        Serial.println("OLED initialisation failed!");
        while (true);
    }

    m_oled.clearDisplay();
    m_oled.setTextColor(WHITE);
    m_oled.setFont(FONT);
    m_oled.setTextWrap(false);

    set_text_size(MEDIUM);
    m_oled.setCursor(
        12, ((m_height - FONT_HEIGHT * MEDIUM) / 2) + (FONT_HEIGHT * MEDIUM));
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

void OLED::update_status_bar(int16_t gyro_angle, uint16_t left_rpm,
                             uint16_t right_rpm) {
    /* write on line 1 on the left */
    m_oled.setCursor(0,
                     FONT_HEIGHT * MEDIUM);
    m_oled.printf("%03d", right_rpm);

    /* write on line 1 in the middle */
    m_oled.setCursor(m_width / 2 - floor(FONT_WIDTH * MEDIUM * 1.5) - 2,
                     FONT_HEIGHT * MEDIUM);
    m_oled.printf("%03d", gyro_angle);

    /* write on line 1 on the right */
    m_oled.setCursor(m_width - (FONT_WIDTH * MEDIUM * 3 + 5),
                     FONT_HEIGHT * MEDIUM);
    m_oled.printf("%03d", left_rpm);
}

void OLED::update_ToFs(uint16_t tof_left, uint16_t tof_front_left,
                       uint16_t tof_front_right, uint16_t tof_right) {
    /* write on line 2 on the left */
    m_oled.setCursor(0,
                     FONT_HEIGHT * MEDIUM * 2 + 5);
    m_oled.printf("%03d", tof_right);

    /* write on line 2 on the right*/
    m_oled.setCursor(m_width - (FONT_WIDTH * MEDIUM * 3 + 5),
                     FONT_HEIGHT * MEDIUM * 2 + 5);
    m_oled.printf("%03d", tof_left);

    /* write on line 4 in the middle to the left */
    m_oled.setCursor(m_width / 2 - (FONT_WIDTH * MEDIUM * 5) + 2,
                     FONT_HEIGHT * MEDIUM * 4);
    m_oled.printf("%03d", tof_front_right);

    /* write on line 4 in the middle to the right*/
    m_oled.setCursor(m_width / 2 + 10,
                     FONT_HEIGHT * MEDIUM * 4);
    m_oled.printf("%03d", tof_front_left);
}

void OLED::idle() {
    m_oled.drawXBitmap(0, 0, face_map[HAPPY], m_width, m_height, WHITE);
    /*set_text_size(HUGE);
    m_oled.setCursor(15, FONT_HEIGHT * HUGE + 5);
    m_oled.print(">OwO<");
    m_oled.setCursor(20, 20);
    m_oled.print(">-w-<");*/
}
