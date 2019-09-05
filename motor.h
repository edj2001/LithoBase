boolean motorState;
uint8_t motorSpeed;

void setupMotor()
{
  
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
  
}

boolean getMotorState()
{
  return motorState;
}

uint8_t getMotorSpeed()
{
  return motorSpeed;  
}

