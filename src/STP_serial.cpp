#include <STP_global.h>
#include <STP_serial.h>
#include <STP_stepper.h>


/* 
Arduino A5 pin used to process Load Cell data 
*/
AccelStepper X(AccelStepper::DRIVER, X_STEP, X_DIR);
AccelStepper Y(AccelStepper::DRIVER, Y_STEP, Y_DIR);
AccelStepper Z(AccelStepper::DRIVER, Z_STEP, Z_DIR);

char command[20];
uint8_t char_counter;
uint8_t char_offset;
char actuator;
unsigned int steps = 0;
int velocity = 1;
int acceleration = 0;
int currSteps = 1;
unsigned x;
int i = 0;
char c;
bool Xdone = true;
bool Xupdated = false;
bool Ydone = true;
bool Yupdated = false;
bool Zdone = true;
bool Zupdated = false;


extern int ReadAndSend(int);

int serial_read()
{
  unsigned int n = 0;

  //memset(data, 0, sizeof(data));  
  memset(command, 0, sizeof(command));

  char_counter = 0;

  while (Serial.available() > 0)
  {
    command[char_counter++] = Serial.read();

    delayMicroseconds(100);

    n++;
  }
  
  Serial.flush();

  return(n);
}


//Command Structure: Stepper (X, Y, Z) = Steps (unsigned int) = Velocity (signed int) = Acceleration (unsigned int)
void parse_command()
{
  char step[5], vel[5], accel[5], data[10];


 // extern AccelStepper X, Y, Z;

  actuator = command[0];

  if(command[0] == 0x00)
  {
     return;
  }

  Serial.print("CMD: "); Serial.println(command);

  if(String(command) == "HALT") 
  {
    enableMotors(OFF);              // Immediately disable drivers
    X.stop();                       // Stop X, Y, and Z steppers
    X.moveTo(X.currentPosition());  // Update Moveto, to prevent further movement
    Y.stop();
    Y.moveTo(Y.currentPosition());
    Z.stop();
    Z.moveTo(Z.currentPosition());
  }
  else if(command[0] == 'S')
  {
    char_counter = 6;
    actuator = command[4];

    if(command[char_counter] == '-'){
      char_counter++;
      currSteps = -1;
    }
    while(command[char_counter])  // Set step data
    {
      data[char_counter - 6] = command[char_counter];
      char_counter++;
    }
    currSteps = currSteps * atoi(data);
  }
  else if(command[1] == ' ')
  {
    char_counter = 2;
    
    while (command[char_counter] != ' ') 
    {  
      step[char_counter - 2] = command[char_counter];
      char_counter++;
    }
    steps = atoi(step);
    char_counter++; // move to velocity field

    if(command[char_counter] == '-')
    { 
      char_counter++;
      velocity = -1;
    }

    char_offset = char_counter;
    while (command[char_counter] != ' ') 
    {
      vel[char_counter - char_offset] = command[char_counter];
      char_counter++;
    } 
    velocity = velocity * atoi(vel);
    char_counter++; // move to acceleration field
    
    char_offset = char_counter;
    while (command[char_counter]) 
    {  
      accel[char_counter - char_offset] = command[char_counter];
      char_counter++;
    }
    acceleration = atoi(accel);
  }

  switch(actuator)
  {    
    case 'X':
      Serial.flush();

      Serial.println("ACK: X, Steps = " +String(steps) + ", Vel = " + String(velocity) + ", Accel = " + String(acceleration));
      Xdone = false;
      Xupdated = true;
      enableMotors(ON);

      X.setAcceleration(acceleration);
      X.setSpeed(velocity);
      if (velocity < 0) {
        X.moveTo(X.currentPosition() - steps);
      }
      else {
        X.moveTo(X.currentPosition() + steps);
      }
    break;
    
    case 'Y':
      Serial.flush();

      Serial.println("ACK: Y, Steps = " + String(steps) + ", Vel = " + String(velocity) + ", Accel = " + String(acceleration));
      Ydone = false;
      Yupdated = true;
      enableMotors(ON);

      Y.setAcceleration(acceleration);
      Y.setSpeed(velocity);
      if (velocity < 0) {
        Y.moveTo(Y.currentPosition() - steps);
      }
      else {
        Y.moveTo(Y.currentPosition() + steps);
      }
    break;
    
    case 'Z': 
      Serial.flush();

      Serial.println("ACK: Z, Steps = " +String(steps) + ", Vel = " + String(velocity) + ", Accel = " + String(acceleration));
      Zdone = false;
      Zupdated = true;
      enableMotors(ON);

      Z.setAcceleration(acceleration);
      Z.setSpeed(velocity);
      if (velocity < 0) {
        Z.moveTo(Z.currentPosition() - steps);
      }
      else {
        Z.moveTo(Z.currentPosition() + steps);
      }

      if(!(x%5))
      {
          ReadAndSend(0);
          Serial.print("PROGRESS: Z "); Serial.println(x);
      }
    break;

    case 'S':
      Serial.flush();
      Serial.println("ACK: Setting " + String(actuator) + " Current Steps = " + String(currSteps));

      if(actuator == 'X') 
      {
        X.setCurrentPosition(currSteps);
      }
      else if (actuator == 'Y') 
      {
        Y.setCurrentPosition(currSteps);
      }
      else if (actuator == 'Z') 
      {
        Z.setCurrentPosition(currSteps);
      }
    break;

    case 'H':
      // No action needed, halt command handled prior.
    break;

    default:  
      Serial.print("ERROR: Unknown Command: "); Serial.println(command);
    break;
  }
}