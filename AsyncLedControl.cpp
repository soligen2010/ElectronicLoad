
#include "AsyncLedControl.h"


AsyncLedControlFade::AsyncLedControlFade (int pin, bool  offState)
      :AsyncLedControlLight(pin,offState)
{
  dimLevelLow = 0;
  dimLevelHigh = maxPWM;

}

AsyncLedControlLight::AsyncLedControlLight (int pin, bool  offState)
{
  ledPin = (pin);
  nextState = (OFF);
  currentState = (OFF);
  ledOffState = (offState);
  nextStateChangeTime = (0);
  blinkIntervalOn = (DEFAULT_INTERVAL);
  blinkIntervalOff = (DEFAULT_INTERVAL);

  pinMode(pin,OUTPUT);
  digitalWrite(pin,offState);

}

void AsyncLedControlLight::turnOnDim(uint16_t level, uint16_t onDelay)
{
  nextStateChangeTime = millis() + onDelay;
  if (ledOffState == HIGH) {
    dimLevelHigh = maxPWM - level;
    dimLevelLow = maxPWM - level;
  } else {
    dimLevelHigh = level;
    dimLevelLow = level;    
  }
  nextState = ON_DIM;
}

void AsyncLedControlLight::turnOn(uint16_t onDelay)
{
  nextStateChangeTime = millis() + onDelay;
  nextState = ON;
}

void AsyncLedControlLight::turnOff(uint16_t onDelay)
{
  nextStateChangeTime = millis() + onDelay;
  nextState = OFF;
}

void AsyncLedControlFade::fadeOn(uint16_t interval, uint16_t onDelay)
{
  nextStateChangeTime = millis() + onDelay;
  if ((currentState != PULSE) && (currentState != BLINK_DIM) && (currentState != ON_DIM)) {
    blinkIntervalOn = interval;
    dimLevelLow = 0;
    dimLevelHigh = maxPWM;
  }
  if (currentState != ON) {nextState = FADE_ON;}
}

void AsyncLedControlFade::fadeOnDim(uint16_t level, uint16_t interval, uint16_t onDelay)
{
  if (level == 0 ) {
    fadeOff(interval, onDelay);
  } else {
    nextStateChangeTime = millis() + onDelay;
    dimLevelFadeOn = (ledOffState == HIGH) ? maxPWM - level : level;
    if ((currentState != PULSE) && (currentState != BLINK_DIM) && (currentState != ON_DIM)) {
      blinkIntervalOn = interval;
      if (ledOffState == HIGH) {
        dimLevelLow = dimLevelFadeOn;
        dimLevelHigh = maxPWM;
      } else {
        dimLevelLow = 0;
        dimLevelHigh = dimLevelFadeOn;
      }
    }
    if (currentState != ON && currentState != ON_DIM) {nextState = FADE_ON_DIM;}
  }
}

void AsyncLedControlFade::fadeOff(uint16_t interval, uint16_t onDelay)
{
  nextStateChangeTime = millis() + onDelay;
  if ((currentState != PULSE) && (currentState != BLINK_DIM) && (currentState != ON_DIM)) {
    blinkIntervalOff = interval;
    dimLevelLow = 0;
    dimLevelHigh = maxPWM;
  }
  if (currentState != OFF) {nextState = FADE_OFF;}
}


void AsyncLedControlLight::blink(uint16_t intervalOn, uint16_t intervalOff, uint16_t onDelay)
{
  nextStateChangeTime = millis() + onDelay;
  if (ledOffState == HIGH) {
    blinkIntervalOn = intervalOff;
    blinkIntervalOff = intervalOn;
  } else {
    blinkIntervalOn = intervalOn;
    blinkIntervalOff = intervalOff; 
  }   
  nextState = BLINK;
}

void AsyncLedControlLight::blinkDim(uint16_t low, uint16_t high, uint16_t intervalOn, uint16_t intervalOff, uint16_t onDelay)
{
  nextStateChangeTime = millis() + onDelay;
  if (((low < high) && (ledOffState == LOW)) || ((low > high) && (ledOffState == HIGH)))  {
    dimLevelLow=low;
    dimLevelHigh=high;
  }
  else{
    dimLevelLow=high;
    dimLevelHigh=low;
  }
  if (ledOffState == HIGH) {
    dimLevelHigh = maxPWM - dimLevelHigh;
    dimLevelLow = maxPWM - dimLevelLow;
    blinkIntervalOn = intervalOff;
    blinkIntervalOff = intervalOn;
  } else {
    blinkIntervalOn = intervalOn;
    blinkIntervalOff = intervalOff; 
  }
  nextState = BLINK_DIM;
}

void AsyncLedControlFade::pulse(uint16_t intervalOn, uint16_t intervalOff, uint16_t low, uint16_t high, uint16_t onDelay)
{
  nextStateChangeTime = millis() + onDelay;
  if (((low < high) && (ledOffState == LOW)) || ((low > high) && (ledOffState == HIGH)))  {
    dimLevelLow=low;
    dimLevelHigh=high;
  }
  else{
    dimLevelLow=high;
    dimLevelHigh=low;
  }
  if (ledOffState == HIGH) {
    dimLevelHigh = maxPWM - dimLevelHigh;
    dimLevelLow = maxPWM - dimLevelLow;
    blinkIntervalOn = intervalOff;
    blinkIntervalOff = intervalOn;
  } else {
    blinkIntervalOn = intervalOn;
    blinkIntervalOff = intervalOff;    
  }
  if (dimLevelHigh > maxPWM) {dimLevelHigh = maxPWM;}
  nextState = PULSE;
}

AsyncLedControlLight::ledState AsyncLedControlLight::getCurrentState() 
{
  return currentState;
}

AsyncLedControlLight::ledState AsyncLedControlLight::getNextState() 
{
  return nextState;
}

uint16_t AsyncLedControlLight::getIntervalOn() 
{
  return   (ledOffState == HIGH) ? blinkIntervalOff : blinkIntervalOn;
}

void AsyncLedControlLight::setIntervalOn(uint16_t interval) 
{
  if (ledOffState == HIGH) {
    blinkIntervalOff = interval;    
  } else {
    blinkIntervalOn = interval;    
  }
}

uint16_t AsyncLedControlLight::getIntervalOff() 
{
  return   (ledOffState == HIGH) ? blinkIntervalOn : blinkIntervalOff;
}

void AsyncLedControlLight::setIntervalOff(uint16_t interval) 
{
  if (ledOffState == HIGH) {
    blinkIntervalOn = interval;    
  } else {
    blinkIntervalOff = interval;    
  }
}


uint16_t AsyncLedControlLight::getMaxPWM() 
{
  return maxPWM;
}

void AsyncLedControlLight::setMaxPWM(uint16_t PWM) 
{
  maxPWM = PWM;
}

 void  AsyncLedControlLight::updateService()
{
    unsigned long currentTime = millis();
    
    if ((currentState != nextState) && (nextStateChangeTime <= currentTime)) {
      switch (nextState) {
        case OFF     : digitalWrite(ledPin, ledOffState);
                       currentState = nextState;
                       break;
        case ON      : digitalWrite(ledPin, !ledOffState);
                       currentState = nextState;
                       break;
        case BLINK_DIM :
        case BLINK   : blinkPinState = (currentState == OFF) ? ledOffState : !ledOffState;
                       blinkChangeTime = 0;
                       currentState = nextState;
                       break;
       case ON_DIM   : pwmWrite(ledPin, (ledOffState) ? dimLevelLow : dimLevelHigh);
                       currentState = nextState;
                       break;
       default       : break;
      }
    }
    switch (currentState) {
      case BLINK_DIM: 
      case BLINK    :   if (currentTime > (blinkChangeTime + ((blinkPinState) ? blinkIntervalOn : blinkIntervalOff))) {
                          blinkChangeTime = currentTime;
                          blinkPinState = !blinkPinState;
                          if (currentState == BLINK) {
                            digitalWrite(ledPin, blinkPinState);
                          } else {
                            pwmWrite(ledPin, (blinkPinState) ? dimLevelHigh : dimLevelLow );
                          }
                        }
                        break;
      default         : break;
    }
  
}

 void  AsyncLedControlFade::updateService()
{
    AsyncLedControlLight::updateService();   

    unsigned long currentTime = millis();

    if ((currentState != nextState) && (nextStateChangeTime <= currentTime)) {
      switch (nextState) {
      case FADE_ON_DIM  :
       case FADE_OFF :
       case FADE_ON  :
       case PULSE    : if (currentState != FADE_ON_DIM && currentState != FADE_OFF && currentState != FADE_ON && currentState != PULSE) {
                         blinkChangeTime = 0;
                         if (nextState == FADE_OFF || currentState == ON || currentState == ON_DIM) {
                           pulseDirection = (ledOffState == LOW) ? 1 : -1;
                         }
                         else {
                           pulseDirection = (ledOffState == LOW) ? -1 : 1;                          
                         }
                       }
                       break;
        default      : break;
      }
      currentState = nextState;
    }
    switch (currentState) {
      case FADE_ON_DIM  :
      case FADE_ON  :
      case FADE_OFF :
      case PULSE    : if (currentTime > blinkChangeTime ) {
                        pulseDirection *= -1;
                        blinkChangeTime = currentTime + ((pulseDirection == 1) ? blinkIntervalOn : blinkIntervalOff);
                        
                        if ((currentState == FADE_ON) || (currentState == FADE_ON_DIM)) {
                          if (pulseDirection == ((ledOffState == LOW) ? 1 : -1)) {
                            if (ledOffState == LOW) {
                              if (dimLevelHigh != ((currentState == FADE_ON_DIM) ? dimLevelFadeOn : maxPWM)) {
                                dimLevelHigh = ((currentState == FADE_ON_DIM) ? dimLevelFadeOn : maxPWM);
                              }
                            } else {
                              if (dimLevelLow != ((currentState == FADE_ON_DIM) ? dimLevelFadeOn : 0)) {
                                dimLevelLow = ((currentState == FADE_ON_DIM) ? dimLevelFadeOn : 0);
                              }
                            }                 
                          } else {
                            if (ledOffState == LOW) {
                              if (dimLevelHigh == ((currentState == FADE_ON_DIM) ? dimLevelFadeOn : maxPWM)) {
                                nextState = (currentState == FADE_ON_DIM) ? ON_DIM : ON;
                              }
                            } else {
                              if (dimLevelLow == ((currentState == FADE_ON_DIM) ? dimLevelFadeOn : 0)) {
                                nextState = (currentState == FADE_ON_DIM) ? ON_DIM : ON;
                              }
                            }
                          }
                        }

                        if (currentState == FADE_OFF) {
                          if (pulseDirection == ((ledOffState == LOW) ? -1 : 1)) {
                            if (ledOffState == LOW) {
                              if (dimLevelLow != 0) {
                                dimLevelLow = 0;
                              }
                            } else {
                              if (dimLevelHigh != maxPWM) {
                                dimLevelHigh = maxPWM;
                              }
                            }                 
                          } else {
                            if (ledOffState == LOW) {
                              if (dimLevelLow == 0) {
                                nextState = OFF;
                              }
                            } else {
                              if (dimLevelHigh == maxPWM) {
                                nextState = OFF;
                              }
                            }
                          }
                        }
                        
                      }
                      pwmWrite(ledPin, ((pulseDirection == 1) ? (float)(currentTime - (blinkChangeTime - blinkIntervalOn)) / (float)blinkIntervalOn  * (float)(dimLevelHigh-dimLevelLow) + dimLevelLow
                                                                 : (float)(blinkChangeTime - currentTime)                     / (float)blinkIntervalOff * (float)(dimLevelHigh-dimLevelLow) + dimLevelLow) );
                      break;
      default       : break;
    }
}


/*
      case PULSE    : if (currentTime > blinkChangeTime ) {
                        pulseDirection *= -1;
                        blinkChangeTime = currentTime + ((pulseDirection == 1) ? blinkIntervalOn : blinkIntervalOff);
                        
                        if (currentState == FADE_ON) {
                          if (pulseDirection == ((ledOffState == LOW) ? 1 : -1)) {
                            if (ledOffState == LOW) {
                              if (dimLevelHigh != maxPWM) {
                                dimLevelHigh = maxPWM;
                              }
                            } else {
                              if (dimLevelLow != 0) {
                                dimLevelLow = 0;
                              }
                            }                 
                          } else {
                            if (ledOffState == LOW) {
                              if (dimLevelHigh == maxPWM) {
                                nextState = ON;
                              }
                            } else {
                              if (dimLevelLow == 0) {
                                nextState = ON;
                              }
                            }
                          }
                        }

                        if (currentState == FADE_OFF) {
                          if (pulseDirection == ((ledOffState == LOW) ? -1 : 1)) {
                            if (ledOffState == LOW) {
                              if (dimLevelLow != 0) {
                                dimLevelLow = 0;
                              }
                            } else {
                              if (dimLevelHigh != maxPWM) {
                                dimLevelHigh = maxPWM;
                              }
                            }                 
                          } else {
                            if (ledOffState == LOW) {
                              if (dimLevelLow == 0) {
                                nextState = OFF;
                              }
                            } else {
                              if (dimLevelHigh == maxPWM) {
                                nextState = OFF;
                              }
                            }
                          }
                        }
                        
                      }
*/
 

