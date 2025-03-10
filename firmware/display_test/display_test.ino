#include <SoftI2C.h>
#include "display_char.h"

// Address of the I2C OLED
#define OLED_I2C_ADDRESS 0x3C



// Commands for SSD1306
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_SEGREMAP            0xA1
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETCONTRAST         0x81
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_DISPLAYON           0xAF

static uint16_t display_num[10][16] = display16X16_number;
static uint16_t display_A[16] = display16X16_A;
static uint16_t display_C[16] = display16X16_C;
static uint16_t display_V[16] = display16X16_V;
static uint16_t display_blank[16] = display16X16_blank;
uint8_t voltage_dec_3;
uint8_t voltage_dec_2;
uint8_t voltage_dec_1;
uint8_t voltage_dec_0;
uint8_t current_dec_2;
uint8_t current_dec_1;
uint8_t current_dec_0;
uint16_t* display_addr_list[16];
void I2CWriteCommand(uint8_t command) {
  I2CStart();
  I2CSend(OLED_I2C_ADDRESS << 1); // I2C address + Write bit
  I2CSend(0x00); // Co = 0, D/C# = 0
  I2CSend(command);
  I2CStop();
}

void I2CWriteData(uint8_t data) {
  I2CStart();
  I2CSend(OLED_I2C_ADDRESS << 1); // I2C address + Write bit
  I2CSend(0x40); // Co = 0, D/C# = 1
  I2CSend(data);
  I2CStop();
}

void initializeOLED() {
  I2CWriteCommand(SSD1306_DISPLAYOFF);
  I2CWriteCommand(SSD1306_SETDISPLAYCLOCKDIV);
  I2CWriteCommand(0x80); // Suggested value
  I2CWriteCommand(SSD1306_SETMULTIPLEX);
  I2CWriteCommand(0x3F);
  I2CWriteCommand(SSD1306_SETDISPLAYOFFSET);
  I2CWriteCommand(0x00);
  I2CWriteCommand(SSD1306_SETSTARTLINE | 0x0);
  I2CWriteCommand(SSD1306_CHARGEPUMP);
  I2CWriteCommand(0x14);
  I2CWriteCommand(SSD1306_MEMORYMODE);
  I2CWriteCommand(0x00);
  I2CWriteCommand(SSD1306_SEGREMAP | 0x1);
  I2CWriteCommand(SSD1306_COMSCANDEC);
  I2CWriteCommand(SSD1306_SETCOMPINS);
  I2CWriteCommand(0x12);
  I2CWriteCommand(SSD1306_SETCONTRAST);
  I2CWriteCommand(0xCF);
  I2CWriteCommand(SSD1306_SETPRECHARGE);
  I2CWriteCommand(0xF1);
  I2CWriteCommand(SSD1306_SETVCOMDETECT);
  I2CWriteCommand(0x40);
  I2CWriteCommand(SSD1306_DISPLAYALLON_RESUME);
  I2CWriteCommand(SSD1306_NORMALDISPLAY);
  I2CWriteCommand(SSD1306_DISPLAYON);
}

void clearScreen() {
  for (uint8_t i = 0; i < 8; i++) {
    I2CWriteCommand(0xB0 + i); // Set page address
    I2CWriteCommand(0x00); // Set lower column address
    I2CWriteCommand(0x10); // Set higher column address
    for (uint8_t j = 0; j < 128; j++) {
      I2CWriteData(0x00);
    }
  }
}

void nextRow() {

}

void lightScreen() {
  for (uint8_t i = 0; i < 8; i++) {
    I2CWriteCommand(0xB0 + i); // Set page address
    I2CWriteCommand(0x00); // Set lower column address
    I2CWriteCommand(0x10); // Set higher column address
    for (uint8_t j = 0; j < 128; j++) {
      if (j % 4 == 0) {
        I2CWriteData(0xAA);
      } else {
        I2CWriteData(0x80);
      }
    }
  }
}

void PS_Screen(uint16_t voltage_num, uint16_t current_num, bool CC_mode) {
  voltage_dec_3 = (voltage_num / 1000) % 10;
  voltage_dec_2 = (voltage_num / 100) % 10;
  voltage_dec_1 = (voltage_num / 10) % 10;
  voltage_dec_0 = voltage_num % 10;
  current_dec_2 = (current_num / 100) % 10;
  current_dec_1 = (current_num / 10) % 10;
  current_dec_0 = current_num % 10;
  display_addr_list[0] = (!CC_mode) ? display_C : display_blank;
  display_addr_list[1] = (!CC_mode) ? display_V : display_blank;
  display_addr_list[2] = display_blank;
  display_addr_list[3] = display_num[voltage_dec_3];
  display_addr_list[4] = display_num[voltage_dec_2];
  display_addr_list[5] = display_num[voltage_dec_1];
  display_addr_list[6] = display_num[voltage_dec_0];
  display_addr_list[7] = display_V;
  display_addr_list[8] = (!CC_mode) ? display_blank : display_C;
  display_addr_list[9] = (!CC_mode) ? display_blank : display_C;
  display_addr_list[10] = display_blank;
  display_addr_list[11] = display_blank;
  display_addr_list[12] = display_num[current_dec_2];
  display_addr_list[13] = display_num[current_dec_1];
  display_addr_list[14] = display_num[current_dec_0];
  display_addr_list[15] = display_A;

  for (uint8_t i = 0; i < 8; i++) {
    I2CWriteCommand(0xB0 + i); // Set page address
    I2CWriteCommand(0x00); // Set lower column address
    I2CWriteCommand(0x10); // Set higher column address
    for (uint8_t j = 0; j < 128; j++) {

      if (i % 4 == 3 && (j == 79 || j == 80)) {
        I2CWriteData(0xA0);
      }
      else {
        uint16_t push_temp = 0;
        uint8_t sel_col = 15 - j % 16;
        for (uint8_t k = 0; k < 4; k++) {
          uint8_t bit = ((display_addr_list[(j >> 4) + (i / 4 * 8)])[((i % 4) << 2) + k] >> (sel_col)) & 0x0001;
          push_temp |= (bit << (2 * k + 1));
        }
        I2CWriteData(push_temp);
      }
    }
  }
}

void drawRectangle(uint8_t, uint8_t, uint8_t, uint8_t, bool);

void setup() {
  // Initialize I2C pins
  scl_pin = 30; // P3.0
  sda_pin = 31; // P3.1
  I2CInit();
  initializeOLED();
}

void loop() {
  // Clear the display
  clearScreen();
  delay(500);
  PS_Screen(8888, 888, 1);
  delay(500);
}
