#include <Wire.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include "secrets.h"

#define BAUD_RATE 9600
#define RX D7
#define TX D8
#define SIZE_BYTES 9
#define TOUCH_PIN D0

SoftwareSerial sensorSerial(RX, TX);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Tabelas de valores CRC
const byte auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};

const byte auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40};

byte bufferBytes[SIZE_BYTES];
const byte baud_rate[13] = {0x01, 0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x01, 0xC2, 0x00, 0xF3, 0x0F}; // Controle baud rate
const byte open_laser[11] = {0x01, 0x10, 0x00, 0x03, 0x00, 0x01, 0x02, 0x00, 0x01, 0x67, 0xA3};            // Open Beam
const byte close_laser[11] = {0x01, 0x10, 0x00, 0x03, 0x00, 0x01, 0x02, 0x00, 0x00, 0xA6, 0x63};           // Close Laser
const byte trig_single[8] = {0x01, 0x03, 0x00, 0x0F, 0x00, 0x02, 0xF4, 0x08};                              // Measure once
const byte trig_successive[8] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xCB};                          // Successive measurment (5Hz)
const byte read_measure_data[8] = {0x01, 0x03, 0x00, 0x0F, 0x00, 0x02, 0xF4, 0x08};                        // Read measuring distance (don't know what to do with this)

int isTouch;
boolean isVisibleLaser = true;

unsigned long lastTimeReadDistance = 0;
unsigned long currentTime = millis();
const long timeout = 10000;
String message = "Aguarde...";

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Ryatec Sistemas");

  Serial.begin(BAUD_RATE);
  sensorSerial.begin(BAUD_RATE, SWSERIAL_8N1);
  sensorSerial.setTimeout(0.001);

  pinMode(TOUCH_PIN, INPUT);

  if (!sensorSerial)
  {
    Serial.println("Invalid EspSoftwareSerial pin configuration, check config");
    while (1)
    {
      delay(1000);
    }
  }
  if (isVisibleLaser)
  {
    sensorSerial.write(open_laser, sizeof(open_laser));
  }
  // connectWifi();
}

void loop()
{
  currentTime = millis();
  isTouch = digitalRead(TOUCH_PIN);
  lcd.setCursor(0, 1);

  int remainingTime = (int)(timeout - (currentTime - lastTimeReadDistance)) / 1000;
  if (remainingTime == 0 && !isVisibleLaser)
  {
    isVisibleLaser = true;
  }

  if (currentTime - lastTimeReadDistance >= timeout)
  {
    lcd.print("Clique para medir");
    if (isVisibleLaser)
    {
      sensorSerial.write(open_laser, sizeof(open_laser));
      isVisibleLaser = false;
    }
  }
  else
  {
    lcd.print("Aguarde: " + String(remainingTime));
  }

  if (isTouch == HIGH && currentTime - lastTimeReadDistance >= timeout)
  {
    clearSerialBuffer(sensorSerial);
    delay(10);
    sensorSerial.write(trig_single, sizeof(trig_single));
    uint32_t distance = readMeasuringDistance(sensorSerial);

    lcd.clear();
    Serial.print("Distância medida: ");
    if (distance > 1000)
    {
      message = String(distance / 1000.0) + " m";
      lcd.print(message);
      Serial.println(message);
    }
    else
    {
      message = String(distance / 10.0) + " cm";
      lcd.print(message);
      Serial.println(message);
    }

    lastTimeReadDistance = currentTime;
    sensorSerial.write(close_laser, sizeof(close_laser));
    isVisibleLaser = false;
  }
  delay(1000);
}

uint32_t readMeasuringDistance(SoftwareSerial &serial)
{
  uint32_t measuredDistance = 0;

  // Send request to sensor
  serial.write(read_measure_data, sizeof(read_measure_data));
  delay(200); // Wait for the sensor to respond

  // Read response from sensor
  while (serial.available() < 9)
    ;                                // Wait until all bytes are received
  byte slaveAddress = serial.read(); // Read slave address
  byte functionCode = serial.read(); // Read function code
  byte byteCount = serial.read();    // Read byte count
  byte highByte1 = serial.read();    // Read high byte of register 1
  byte lowByte1 = serial.read();     // Read low byte of register 1
  byte highByte2 = serial.read();    // Read high byte of register 2
  byte lowByte2 = serial.read();     // Read low byte of register 2
  byte crcHigh = serial.read();      // Read CRC high byte
  byte crcLow = serial.read();       // Read CRC low byte

  // Verify CRC (not implemented in this example)

  // Combine received bytes into 32-bit value (big-endian)
  measuredDistance |= (uint32_t)highByte1 << 24;
  measuredDistance |= (uint32_t)lowByte1 << 16;
  measuredDistance |= (uint32_t)highByte2 << 8;
  measuredDistance |= (uint32_t)lowByte2;

  return measuredDistance;
}

void clearSerialBuffer(SoftwareSerial &serial)
{
  while (serial.available() > 0)
  {
    serial.read();
  }
}

void connectWifi()
{
  // Connect to Wi-Fi network with SSID and password
  lcd.setCursor(0, 0);
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  // Print local IP address
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.print("Success.");
}