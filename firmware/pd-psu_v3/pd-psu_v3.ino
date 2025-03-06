/*
    PD Power Supply
    Author: 

    This is the firmware for the PD power supply 
    that reads the voltage and current of the supply and display the 
    power information on the SSD1306 OLED display.

    This firmware is designed for CH552G with CH55xduino board definations
    Recommend config
    24Mhz (Internal), 5V

*/
#include <Serial.h>

/* Hardware Definations */
#define CURRENT_PIN 14  //0 - 5A
#define VOLTAGE_PIN 32  //0 - 20V
#define CCCV_PIN 15     //CC = LOW, CV = HIGH
#define OLED_SCL 30
#define OLED_SDA 31

/* Global Variables */
uint16_t voltageReading;
uint16_t currentReading;
bool isCCMode;

/* Function Prototypes */
void setupOLED();
void initializeOLED();
void clearScreen();
void PS_Screen(uint16_t, uint16_t, bool);

void setup() {
  //Disable internal pull-up resistors for input pins
  pinMode(CURRENT_PIN, INPUT);
  pinMode(VOLTAGE_PIN, INPUT);
  pinMode(CCCV_PIN, INPUT);

  // Initialize I2C pins for OLED
  setupOLED();
  initializeOLED();

  //Clear the OLED screen
  clearScreen();
}

void loop() {
  //Get Output Voltage
  voltageReading = 0;
  for (int i = 0; i < 10; i++) {
    voltageReading += map(analogRead(VOLTAGE_PIN), 0, 255, 0, 500) * 4;  //Convert ADC to Volt
  }

  //Get output current
  currentReading = analogRead(CURRENT_PIN);
  currentReading = (((float)currentReading * (5.0 / 255.0)) - 2.5) / 0.185 * 100;  //Max range -5A to 5A
  isCCMode = !digitalRead(CCCV_PIN);

  //Cap the range of digits to make sure it won't overflow
  voltageReading = min(voltageReading, 99999);
  currentReading = min(currentReading, 999);

  //Print result on USB Serial
  //USBSerial_print(((float)voltageReading / 1000));
  //USBSerial_print(",");
  //USBSerial_println(((float)currentReading / 100));
  USBSerial_println((int)isCCMode);
  //Update OLED display
  PS_Screen(voltageReading/10, currentReading, isCCMode);
}
