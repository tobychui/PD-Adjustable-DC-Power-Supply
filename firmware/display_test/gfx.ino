void drawPixel(uint8_t x, uint8_t y, bool on) {
  uint8_t page = y / 8;
  uint8_t bit = y % 8;
  uint8_t column = x;

  // Set the page and column address
  I2CWriteCommand(0xB0 + page);         // Set page address
  I2CWriteCommand(0x00 | (column & 0x0F));  // Set lower column address
  I2CWriteCommand(0x10 | (column >> 4));   // Set higher column address

  // Read the current data
  uint8_t data;
  I2CStart();
  I2CSend(OLED_I2C_ADDRESS << 1);      // I2C address + Write bit
  I2CSend(0x00);                       // Co = 0, D/C# = 0 (Command mode)
  I2CSend(0x00);                       // Dummy command to read data
  I2CStop();
  
  // Assume data is read correctly, here data should be handled
  // Modify data based on the `on` parameter
  if (on) {
    data |= (1 << bit); // Set the pixel bit
  } else {
    data &= ~(1 << bit); // Clear the pixel bit
  }

  // Write the modified data back to the display
  I2CStart();
  I2CSend(OLED_I2C_ADDRESS << 1);      // I2C address + Write bit
  I2CSend(0x40);                       // Co = 0, D/C# = 1 (Data mode)
  I2CSend(data);                       // Write the modified data
  I2CStop();
}

void drawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool fill) {
  for (uint8_t i = 0; i < width; i++) {
    for (uint8_t j = 0; j < height; j++) {
      if (fill || (i == 0 || i == width - 1 || j == 0 || j == height - 1)) {
        drawPixel(x + i, y + j, true); // Draw the pixel
      }
    }
  }
}