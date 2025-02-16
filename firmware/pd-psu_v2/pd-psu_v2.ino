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
  //Read the analog reading of both pins
  voltageReading = analogRead(VOLTAGE_PIN);
  currentReading = analogRead(CURRENT_PIN);
  isCCMode = !digitalRead(CCCV_PIN);

  //Convert them to actual voltage / current value
  voltageReading = map(voltageReading, 0, 255, 0, 500) * 4;  //30k - 10k voltage divider
  currentReading = (2.5 - (currentReading * (5.0 / 255)) )/0.185;      //Max range 0 to 5A

  //Cap the range of digits to make sure it won't overflow
  voltageReading = min(voltageReading, 9999);
  currentReading = min(currentReading, 999);

  //Print result on USB Serial
  USBSerial_print(((float)voltageReading/100));
  USBSerial_print(",");
  USBSerial_println(((float)currentReading/100));
  
  //Update OLED display
  PS_Screen(voltageReading, currentReading, isCCMode);
}
