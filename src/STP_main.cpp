#include "STP_global.h"
#include "STP_serial.h"
#include "STP_stepper.h"
#include <readThermo.h>
#include <TeensyThreads.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 adcLC(0x48);   // default address: Addr pin connected to GND
Adafruit_ADS1115 adcLVDT(0x49); // alt address1: Addr pin connected to Vin

extern AccelStepper X, Y, Z;


double GetLCData(int idx) 
{
  double LC_data;
  int noise = 0;
  int16_t adc_data;;
  
  // ADS1115 ADC conversion (16-bits)
  adc_data = adcLC.readADC_SingleEnded(idx) - 20000; // 20000 counts = 2.5 Amplifier ref voltage
  
  noise = (0.0057 * (abs(adc_data))) + 151; //noise calculation function generated from experimental data (see STP_Electronics/client/STP_Electronics_Data2.xlxs)
   
  LC_data = (((adc_data - noise) * 8.192) / 65536);       //Count to voltage conversion
  
  LC_data = LC_data *(28039.20 / LCGAIN);   // Conversion to lbf; 300 lbf = 10.69930809 mV max (5.007 VDC LC supply)
  return(LC_data);                          //                    linear, so 10.69930809 mV / 300 = 35.6643603 uV / lbf = 28039.19632 lbf / V 
}

double GetLVDTData(int idx) 
{
  uint16_t LVDT_raw;
  double LVDT_data;
  
  LVDT_raw = adcLVDT.readADC_SingleEnded(idx); // read
  LVDT_data = LVDT_raw; // + some processing???

  return(LVDT_data);
}

int ReadAndSend(int mode)
{
  if(mode == 0)
  {
    Serial.println("Temp " + String(InternalTemperature.readTemperatureC(),4));
  }
  else if (mode == 2)
  {
    Serial.println("LC " + String(GetLCData(0),4) 
                         + String(GetLCData(1),4)
                         + String(GetLCData(2),4));
  }
  else if (mode == 2)
  {
    Serial.println("CURRENT " + String(X.currentPosition(),4) + " "
                              + String(Y.currentPosition(),4) + " "
                              + String(Z.currentPosition(),4));
  }
  else if(mode == 3) 
  {
    Serial.println("LVDT " + String(GetLVDTData(0),4) 
                           + String(GetLVDTData(1),4)
                           + String(GetLVDTData(2),4));

  }

  return(0);
}

/* 
Thread for running motors, always running in the backgrund, 
Position adjustments occur in main loop, no movement occurs unless
MoveTo is updated elsewhere.
*/
void move_motors() {
  static int cnt = 0;
  extern bool Xdone, Ydone, Zdone, Xupdated, Yupdated, Zupdated;

  while(1) {
    if ((X.distanceToGo() == 0) && (Y.distanceToGo() == 0) && (Z.distanceToGo() == 0)) 
    {
      enableMotors(OFF);
    }

    if (X.distanceToGo() != 0) 
    {
      X.runSpeed();
      if(!(cnt%5))
      {
        ReadAndSend(0);
        Serial.print("PROGRESS: X "); Serial.println(X.distanceToGo());
      }
    }
    else 
    {
      Xdone = true;
    }
    if (Y.distanceToGo() != 0) 
    {
      Y.runSpeed();
      if(!(cnt%5))
      {
        ReadAndSend(0);
        Serial.print("PROGRESS: Y "); Serial.println(Y.distanceToGo()); 
      }
    }
    else 
    {
      Ydone = true;
    }
    if (Z.distanceToGo() != 0) 
    {
      Z.runSpeed();
      if(!(cnt%5))
      {
        ReadAndSend(0);
        Serial.print("PROGRESS: Z "); Serial.println(Z.distanceToGo());
      }
    }
    else 
    {
      Zdone = true;
    }

    if(Xdone == true && Xupdated == true) {
      Xupdated = false;
      Serial.println("DONE X");
    }
    if(Ydone == true && Yupdated == true) {
      Yupdated = false;
      Serial.println("DONE Y");
    }
    if(Zdone == true && Zupdated == true) {
      Zupdated = false;
      Serial.println("DONE Z");
    }
  }
}




void setup() 
{
  Serial.begin(BAUD_RATE);

  pinMode(STEP_ENABLE_PIN, OUTPUT);

  adcLC.setGain(GAIN_ONE);     //   +/- 4.096V  1 bit = 0.125mV
  adcLC.begin();
  adcLVDT.setGain(GAIN_ONE);     //   +/- 4.096V  1 bit = 0.125mV
  adcLVDT.begin();

  enableMotors(OFF);
  threads.setDefaultStackSize(4096);
  threads.addThread(move_motors);
}

void loop() 
{
  unsigned int ret;
  static int indx=0;

  ret = serial_read();

  if(ret)
  {
    parse_command();
  }
  if(!(indx%1000)) /* Teensy temp readings, Every ~10 seconds */
  {
    ReadAndSend(0);  /* Read the teensy CPU temp */
  }
  else
  if(!(indx%100)) /* Thermocouple readings, Every ~1 second */
  {
    ReadThermo(0);  /* Read the motor windings thermocouple */
    ReadAndSend(2); /* Read current position of steppers */
  }
  else
  if(!(indx%10))  /* LVDT readings, ~10 per second */ 
  {
    ReadAndSend(3); /* Read LVDT's */
  }
  else /* LC readings, ~100 per second */
  {
    ReadAndSend(1);  /* Read Load Cells Every Pass */
  }

  delay(7);  /* sets the pace to 100 Hz; there appears to be 3 mS of overhead / pass */

  indx++;
}

/*
LC values:  100 / second 
LVDT values  10 / second 
MT values     1 / second
Curr Steps    1 / second

*/