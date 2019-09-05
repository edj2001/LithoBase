/*
 *  Motor driver board connections:
 *  
 */
// http://www.4tronix.co.uk/arduino/Stepper-Motors.php
//declare variables for the motor pins
int motorPin1 = D5;    // Blue   - 28BYJ48 pin 1  IN1
int motorPin2 = D6;    // Pink   - 28BYJ48 pin 2  IN2
int motorPin3 = D7;    // Yellow - 28BYJ48 pin 3  IN3
int motorPin4 = D8;    // Orange - 28BYJ48 pin 4  IN4
                        // Red    - 28BYJ48 pin 5 (VCC)

int motorSpeed = 1200;  //variable to set stepper speed
int count = 0;          // count of steps made
int countsperrev = 512; // number of steps per full revolution
int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};

void setOutput(int out)
{
  digitalWrite(motorPin1, bitRead(lookup[out], 0));
  digitalWrite(motorPin2, bitRead(lookup[out], 1));
  digitalWrite(motorPin3, bitRead(lookup[out], 2));
  digitalWrite(motorPin4, bitRead(lookup[out], 3));
}


//set pins to ULN2003 high in sequence from 1 to 4
//delay "motorSpeed" between each pin setting (to determine speed)
void anticlockwise()
{
  for(int i = 0; i < 8; i++)
  {
    setOutput(i);
    delayMicroseconds(motorSpeed);
  }
}

void clockwise()
{
  for(int i = 7; i >= 0; i--)
  {
    setOutput(i);
    delayMicroseconds(motorSpeed);
  }
}

boolean motorState;
uint8_t fmotorSpeed;

void setupMotor()
{
  //declare the motor pins as outputs
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  
}
void setMotorOff()
{
  motorState = false;
}

void setMotorOn()
{
  motorState = true; 
}

void setMotor( bool state, unsigned char value)
{
  // value is 0 to 255
  

  if (state) {
    motorSpeed = value;
    setMotorOn();
  } else
  {
   setMotorOff();
  }
  
}

void handleMotor()
{
  if(count < countsperrev )
    clockwise();
  else if (count == countsperrev * 2)
    count = 0;
  else
    anticlockwise();
  count++;
  
}

boolean getMotorState()
{
  return motorState;
}

uint8_t getMotorSpeed()
{
  return motorSpeed;  
}

