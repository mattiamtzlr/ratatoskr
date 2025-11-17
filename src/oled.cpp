#include "oled.hpp"

OLED::OLED(uint8_t addr) : m_addr(addr) {}

void OLED::begin() {
    if (!m_oled.begin(SSD1306_SWITCHCAPVCC, m_addr)) {
        Serial.println("OLED initialisation failed!");
        while (true);
    }
    m_oled.display();
    m_oled.setTextColor(WHITE);
    m_oled.setFont(&FreeMonoBold9pt7b);
    set_text_size(SMALL);
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

void OLED::update_status_bar(String text) {
    clear();
    m_oled.setCursor(0, 15);
    m_oled.setTextWrap(false);
    m_oled.println(text);
    display();
}

void OLED::update_ToFs() {
    clear();
    m_oled.setCursor(0, 20);
    display();
}

void OLED::idle() {
    clear();
    set_text_size(LARGE);
    m_oled.setCursor(20, 20);
    m_oled.print(">OwO<");
    display();
    delay(3000);
    clear();
    m_oled.setCursor(20, 20);
    m_oled.print(">-w-<");
    display();
    delay(500);
}
