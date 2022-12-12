#include <Arduino.h>
#include <AccelStepper.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <InternalTemperature.h>





#define LCGAIN 217  // TBD by experimentation of load cells



#define X_STEP 2
#define X_DIR  5
#define X_LIM  9

#define Y_STEP 3
#define Y_DIR  6
#define Y_LIM 10

#define Z_STEP 4
#define Z_DIR  7
#define Z_LIM 11

#define SERIAL_NO_DATA 0xff

#define CMD_STATUS_REPORT '?'
#define CMD_FEED_HOLD '!'
#define CMD_CYCLE_START '~'
#define CMD_RESET 0x18 // ctrl-x

#define BAUD_RATE 115200

#define OFF 0
#define ON  1

#define STEP_ENABLE_PIN 8 