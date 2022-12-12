#include <STP_global.h>
#include <STP_stepper.h>


void enableMotors(int onoff)
{
  if(onoff == ON)
  {
    digitalWrite(STEP_ENABLE_PIN , LOW);               // Enable the driver circuits
  }
  else
  {
    digitalWrite(STEP_ENABLE_PIN , HIGH);               // Enable the driver circuits
  }
}