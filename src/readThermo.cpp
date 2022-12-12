// this example is public domain. enjoy!
// https://learn.adafruit.com/thermocouple/

#include <max6675.h>
#include <Wire.h>
#include <readThermo.h>

int thermoDO  = 39; // Teensy MISO1 alt
int thermoCLK = 27; // Teensy SCK1

int thermoX_CS  = 36;
int thermoY_CS  = 37;
int thermoZ_CS  = 38;


MAX6675 thermocoupleX(thermoCLK, thermoX_CS, thermoDO);
MAX6675 thermocoupleY(thermoCLK, thermoY_CS, thermoDO);
MAX6675 thermocoupleZ(thermoCLK, thermoZ_CS, thermoDO);



void ReadThermo(int idx)
{
  // basic readout test, just print the current temp
  if(idx == 0) 
  {
    Serial.println("MT " + String(thermocoupleX.readCelsius()) 
                         + String(thermocoupleX.readCelsius()) 
                         + String(thermocoupleX.readCelsius()));
  }
  else if(idx == 1)
  {
    Serial.println("MT X: " + String(thermocoupleX.readCelsius()));
  }
  else if(idx == 2)
  {
    Serial.println("MT Y: " + String(thermocoupleX.readCelsius()));
  } 
  else if(idx == 3)
  {
    Serial.println("MT Z: " + String(thermocoupleX.readCelsius()));
  } 
}