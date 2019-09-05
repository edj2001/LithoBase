void setFrame( bool state)
{

  if (state){
    
    setLightsDisplayMode();
    setMotorOn();
    
  }else{
    
    setLightsOff();
    setMotorOff();
    
  }
}

