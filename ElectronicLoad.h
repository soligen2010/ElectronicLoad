#include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <ClickEncoder.h>
#include <MsTimer2.h>


#ifndef __ElectronicLoad_h__
#define __ElectronicLoad_h__

#define LED_PIN             6     // Indicates when load is on
#define PWM_DRIVER_PIN      5     // Sends square wave to the -2.5V generator
#define LOAD_ON_PIN        14     // Pin to turn on the load.  HIGH is load on.  


#define ROT_ENCODER_A                        17     // Rotary Encoder pin A
#define ROT_ENCODER_B                        16     // Rotary Encoder pin B
#define ROT_ENCODER_BTN                      15     // Rotary Encoder click button. Momentary. Low is pressed
#define EXTRA_BTN                             2     // For future use

#define ROT_ENCODER_STEPS_PER_NOTCH           4     // Rotary Encoder number of steps that register per notch

#define MIN_VOLTS     0.2F   // Lowest setting for low voltage turn off
#define VOLT_STEP    20.0F   // 20.0 equates to .05 volts per click on the rotary encoder

#define LCD_RS_PIN          3
#define LCD_EN_PIN          4
#define LCD_D4_PIN          7
#define LCD_D5_PIN          8
#define LCD_D6_PIN         11
#define LCD_D7_PIN         12
#define LCD_BACKLIGHT_PIN  10          // For flashing display


#define MIN_VOLTS_DISPLAY_HOLD_DUR  1000ul        // Duration is milliseconds. Unsigned long

typedef enum 
{
  RUN_MODE_SET,                 // set mode.  load is off
  RUN_MODE_LOAD,                // Load on
  RUN_MODE_VOLT_HIGH_ERROR,     // voltage out of range
  RUN_MODE_AMP_HIGH_ERROR,     // Amps out of range
  RUN_MODE_WATT_ERROR,          // watts out of range
} runMode_t;



#endif

