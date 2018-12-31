

#ifndef __AsyncLedControl_h__
#define __AsyncLedControl_h__
#include "Arduino.h"
#include <PWM.h>

#define DEFAULT_MAX_PWM     255     // Cat be reset through property
#define DEFAULT_INTERVAL   1000    

class AsyncLedControlLight {

  public:
      AsyncLedControlLight (int pin, bool  offState = LOW );
                                     
      typedef enum {
          OFF,
          ON,
          ON_DIM,
          BLINK,
          BLINK_DIM,
          PULSE,
          FADE_ON,
          FADE_ON_DIM,
          FADE_OFF     
      } ledState;

      void turnOnDim(uint16_t level, uint16_t onDelay = 0);
      void turnOn(uint16_t onDelay = 0);
      void turnOff(uint16_t onDelay = 0);
      void blink(uint16_t intervalOn, uint16_t intervalOff, uint16_t onDelay = 0);
      void blinkDim(uint16_t Low, uint16_t High, uint16_t intervalOn, uint16_t intervalOff, uint16_t onDelay = 0);
      
      ledState getCurrentState();
      ledState getNextState();
      uint16_t getIntervalOn();
      void setIntervalOn(uint16_t interval);
      uint16_t getIntervalOff();
      void setIntervalOff(uint16_t interval);
      uint16_t getMaxPWM();
      void setMaxPWM(uint16_t PWM);

      void updateService();

      
  protected:
      int ledPin;
      bool ledOffState;
      uint16_t maxPWM = DEFAULT_MAX_PWM;
      unsigned long nextStateChangeTime;
      uint16_t blinkIntervalOn;
      uint16_t blinkIntervalOff;
      uint16_t dimLevelHigh;
      uint16_t dimLevelLow;
      volatile ledState currentState;
      volatile ledState nextState;
      bool blinkPinState;
      unsigned long blinkChangeTime;
};

class AsyncLedControlFade : public AsyncLedControlLight
{
  public:
      AsyncLedControlFade (int pin, bool  offState = LOW );
      void updateService();
      void fadeOn(uint16_t interval = 1000, uint16_t onDelay = 0);
      void fadeOnDim(uint16_t level, uint16_t interval = 1000, uint16_t onDelay = 0);
      void fadeOff(uint16_t interval = 1000, uint16_t onDelay = 0);
      void pulse(uint16_t intervalOn, uint16_t intervalOff, uint16_t low, uint16_t high, uint16_t onDelay = 0);

  private:
      int8_t pulseDirection;
      uint16_t dimLevelFadeOn;
};


#endif
