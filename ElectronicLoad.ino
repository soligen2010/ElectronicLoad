/*
 Copyright 2018 by Dennis Cabell
 KE8FZX

 Control software for DC Electronic Load

 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
 #include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <ClickEncoder.h>
#include <MsTimer2.h>
#include <PWM.h>
#include "ElectronicLoadCalibration.h"
#include "ElectronicLoad.h"
#include "AsyncLedControl.h"


Adafruit_ADS1115 ads;
float adsMVperStep;
LiquidCrystal lcd(LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);  
ClickEncoder encoder = ClickEncoder( ROT_ENCODER_A,ROT_ENCODER_B , EXTRA_BTN , ROT_ENCODER_STEPS_PER_NOTCH);

float minVolts = MIN_VOLTS;
float voltIn = 0.0;
float ampIn = 0.0;
float setIn = 0.0;
float watts = 0.0;

runMode_t runMode = RUN_MODE_SET; 
runMode_t oldRunMode = RUN_MODE_SET; 
unsigned long lastMinVoltsChange = 0;
unsigned long loadStartTime;

AsyncLedControlFade lcdLed = AsyncLedControlFade(LCD_BACKLIGHT_PIN,LOW);                 
AsyncLedControlFade led = AsyncLedControlFade(LED_PIN,LOW);                 

int fanSpeed = 210;

void timerIsr() {      
  encoder.service();
  lcdLed.updateService();
  led.updateService();
}

void setup() {
  InitTimersSafe();    // use PWM library to control frequencies
  bool success = SetPinFrequencySafe(9,25000); // set fan PWM frequency to 25 kHz
  
  //Serial.begin(115200);
  
  setupADC();
  lcd.begin(16, 2);


  MsTimer2::set(1,timerIsr);
  MsTimer2::start(); 
  encoder.setButtonHeldEnabled(false);   // Turn off held/release and double clicks on encoder
  encoder.setDoubleClickEnabled(false);

  pinMode(PWM_DRIVER_PIN,OUTPUT);  // Start PWM signal to feed negative voltage generator
  analogWrite(PWM_DRIVER_PIN,127);

  setRunMode(RUN_MODE_SET);          // initialise run mode - this sets teh LED pin appropriately
  
  digitalWrite(LOAD_ON_PIN,LOW);     // Start with load off
  pinMode(LOAD_ON_PIN,OUTPUT);     
  
  lcd.clear();
  lcdLed.fadeOn(500);
} 

void getMinVoltSetting() {

  static int32_t last, value;
  value += encoder.getValue();
    
  static int32_t setting = 1;
  
  if (value != last) {
    setting = setting - (last - value);
    last = value;
    if (setting < MIN_VOLTS*VOLT_STEP) {setting = MIN_VOLTS*VOLT_STEP;}
    if (setting > MAX_VOLTS*VOLT_STEP) {setting = MAX_VOLTS*VOLT_STEP;}
    minVolts = (float)setting/VOLT_STEP;
    lastMinVoltsChange = millis();
  }  
}

bool determineRunMode() {
  // normally return false but return true when run mode changes to RUN_MODE_LOAD so special action can be taken
  
  bool buttonPressed = (encoder.getButton() == ClickEncoder::Clicked) ? true : false;

  bool r = false;
  
  if ( ampIn > MAX_AMPS and runMode == RUN_MODE_LOAD ) {
    setRunMode(RUN_MODE_AMP_HIGH_ERROR);
  }  else {
  if ( (runMode == RUN_MODE_SET) and ((setIn > MAX_AMPS) or ((runMode == RUN_MODE_AMP_HIGH_ERROR) and (setIn > (MAX_AMPS*.997)))) ) {  // Add some hysteresis for turning off error mode
    setRunMode(RUN_MODE_AMP_HIGH_ERROR);
  }  else {
  if ( (voltIn > MAX_VOLTS) or ((runMode == RUN_MODE_VOLT_HIGH_ERROR) and (voltIn > (MAX_VOLTS*.997))) ) { // Add some hysteresis for turning off error mode
    setRunMode(RUN_MODE_VOLT_HIGH_ERROR);
  }
  else {
    if (( voltIn < minVolts ) and (runMode == RUN_MODE_LOAD)) {
      turnOffLoad();
      adjustFanSpeed(RUN_MODE_SET,setIn,watts);
      displayEndRunVoltLow();                                  // pulse display and wait for button press
      setRunMode(RUN_MODE_SET);
    }
    else {
      if ( (watts > MAX_WATTS) or ((runMode == RUN_MODE_WATT_ERROR) and (watts > (MAX_WATTS*.997))) ) {   // Add some hysteresis for turning off error mode
        setRunMode(RUN_MODE_WATT_ERROR);    
      }
      else {
        if (runMode != RUN_MODE_LOAD and runMode != RUN_MODE_SET) {
          setRunMode(RUN_MODE_SET);
        }
        if (buttonPressed){
          if (runMode == RUN_MODE_LOAD) {
            setRunMode(RUN_MODE_SET);
          }
          else {     
            if (runMode == RUN_MODE_SET) {
              if (voltIn > minVolts) {
                setRunMode(RUN_MODE_LOAD);
                r = true;
              } else {
                lcd.clear();
                displayError(" Volts Too Low", true );
              }
            }  
          } 
        }
      }
    }
  }  
  }  
  }  
  return r;
}

void loop() {   
  int a;
  int overSampleCount = 16;   // number of over-samples to take before updating display

  bool firstLoopInRunModeLoad = false;
    
  int16_t voltReading; 
  int16_t ampReading; 
  int16_t setReading; 
  
  int32_t voltOverSample = 0; 
  int32_t ampOverSample = 0; 
  int32_t setOverSample = 0; 

  for(a = 0; a < overSampleCount; a = a + 1) { 
    getMinVoltSetting();
    firstLoopInRunModeLoad = determineRunMode();
 
    // take actions such as setting run mode with each sample, but display only over-sampled readings
    // this reduces display jitter
    
    voltReading = ads.readADC_Differential_2_3();
    ampReading = ads.readADC_Differential_1_3();
    setReading = ads.readADC_Differential_0_3();
    
    voltOverSample = voltOverSample + voltReading;
    ampOverSample = ampOverSample + ampReading;
    setOverSample = setOverSample + setReading;
    
    voltIn =  (((float) voltReading) * adsMVperStep  * VOLT_SET_MULTIPLIER) + VOLT_SET_OFFSET;
    ampIn = (((((float) ampReading) * adsMVperStep)-.001) / SHUNT_RESISTOR) + SHUNT_OFFSET ;
    setIn = (((((((float) setReading) * adsMVperStep)-.001) / SHUNT_RESISTOR) + SHUNT_OFFSET) * AMP_SET_ADJUST) + AMP_SET_OFFSET ;
      
    if (ampIn < 0.0) {ampIn = 0.0;}
    if (voltIn < 0.0) {voltIn = 0.0;}
  
    if (runMode == RUN_MODE_LOAD) { 
      watts = voltIn * ampIn;
    }
    else {
      watts = voltIn * setIn;
    }
    adjustFanSpeed(runMode, watts, ampIn);
    if (firstLoopInRunModeLoad) {display(runMode);}    // make sure something is on the display asap when RUN_MODE_LOAD is entered
  }
  voltIn =  (((float) voltOverSample) / (float) a * adsMVperStep  * VOLT_SET_MULTIPLIER) + VOLT_SET_OFFSET;
  ampIn = (((((float) ampOverSample) / (float) a * adsMVperStep)) / SHUNT_RESISTOR) + SHUNT_OFFSET;
  setIn = (((((((float) setOverSample) / (float) a * adsMVperStep))  / SHUNT_RESISTOR) + SHUNT_OFFSET) * AMP_SET_ADJUST) + AMP_SET_OFFSET;

  // Avoid display jitter when there is nothng connected
  if (setIn < 0.0007) {setIn = 0.0;}
  if (ampIn < 0.0007) {ampIn = 0.0;}
  if (voltIn < 0.008) {voltIn = 0.0;}
  
  if (runMode == RUN_MODE_LOAD) { 
    watts = voltIn * ampIn;
  }
  else {
    watts = voltIn * setIn;
  }
  display(runMode);
}


void setRunMode(runMode_t newRunMode) {
    if (newRunMode == RUN_MODE_LOAD) {
      digitalWrite(LOAD_ON_PIN,HIGH);
      led.fadeOn(300);
      if (runMode != RUN_MODE_LOAD) {
        loadStartTime = millis();
      }
    }
    else {
      turnOffLoad();
    }
    if (runMode != newRunMode) {
      lcd.clear();
      oldRunMode = runMode;
      runMode = newRunMode;
    }
}

void turnOffLoad() {
      digitalWrite(LOAD_ON_PIN,LOW);
      led.fadeOff(300);
      lcdLed.fadeOn(300);

}

void displayError(char* message, bool WaitForButtonPress) {
    lcd.setCursor(0, 0);
    lcd.print(message);
    if (WaitForButtonPress) {
      lcdLed.pulse(700,700,50,255);
      while ((encoder.getButton() != ClickEncoder::Clicked)) {}
      setRunMode(RUN_MODE_SET);
    }
}

void displayEndRunVoltLow() {
    lcdLed.pulse(1500,1500,70,255);
    while ((encoder.getButton() != ClickEncoder::Clicked)) {}
}

void display(runMode_t mode) {

  if (mode == RUN_MODE_AMP_HIGH_ERROR) {
    displayError(" Amps Exceeded", ((oldRunMode == RUN_MODE_LOAD) ? true : false) );
  }
  if (mode == RUN_MODE_VOLT_HIGH_ERROR) {
    displayError(" Volts Exceeded", ((oldRunMode == RUN_MODE_LOAD) ? true : false) );
  }
  if (mode == RUN_MODE_WATT_ERROR) {
    displayError(" Watts Exceeded", ((oldRunMode == RUN_MODE_LOAD) ? true : false) );
  }
  if (mode == RUN_MODE_LOAD) {
    lcd.setCursor(0, 0);lcd.print(TimeToString((millis() - loadStartTime)/1000));
    lcd.setCursor(0, 1);lcd.print("I=");
    lcd.setCursor(2, 1);lcd.print(ampIn, 3);
  }
  if (mode == RUN_MODE_SET) {
    lcd.setCursor(0, 0);lcd.print("  Set   ");
    lcd.setCursor(0, 1);lcd.print("I=");
    lcd.setCursor(2, 1);lcd.print(setIn, 3);
  }
  if (mode == RUN_MODE_LOAD or mode == RUN_MODE_SET) {
    lcd.setCursor(7, 1);lcd.print("  W=");
    if (watts > 10.0) {
      lcd.setCursor(11, 1);lcd.print(watts, 2);
    }
    else {
        lcd.setCursor(11, 1);lcd.print(" ");
        lcd.setCursor(12, 1);lcd.print(watts, 2);
    }
    
    lcd.setCursor(8, 0);lcd.print(" V=");
    float voltDisplay = voltIn;
    if (millis() < (lastMinVoltsChange + MIN_VOLTS_DISPLAY_HOLD_DUR)) {voltDisplay = minVolts;}
    if (voltDisplay > 10.0) {
      lcd.setCursor(11, 0);lcd.print(voltDisplay, 2);
    }
    else {
        lcd.setCursor(11, 0);lcd.print(" ");
        lcd.setCursor(12, 0);lcd.print(voltDisplay, 2);
    }
  }
}

// t is time in seconds;
char * TimeToString(unsigned long t)
{
  static char str[12];
  long h = t / 3600;
  t = t % 3600;
  int m = t / 60;
  int s = t % 60;
  if (h > 99) {
    h = 99;
    m = 99;
    s = 99;
  }
  sprintf(str, "%02ld:%02d:%02d", h, m, s);
  return str;
}

void setupADC() {
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                           ADS1015  ADS1115
  //                                                                           -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);             // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);                   // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);                   // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);                  // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);                 // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);               // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV  
  
  ads.begin();
  ads.setGain(GAIN_TWO);
  adsMVperStep = ads.voltsPerBit();
  ads.setSPS(ADS1115_DR_860SPS);      // 860 Samples per second

}

void adjustFanSpeed (runMode_t mode, float watts, float amps) {
  int newFanSpeed;
  #define FAN_IDLE_SPEED  155
  #define FAN_MAX_SPEED   255
   
  if (mode == RUN_MODE_LOAD) {
    // scale fan speed based on how much heat is generated.  The transistor's dsipation is based on watts, but the shunt
    // resistor disipation is based on the amps.  Choose fan speed based on whichever is higher
    newFanSpeed =  max (FAN_IDLE_SPEED + (float ((watts/MAX_WATTS) * (float (FAN_MAX_SPEED - FAN_IDLE_SPEED))))  ,
                        FAN_IDLE_SPEED + (float ((amps/MAX_AMPS)   * (float (FAN_MAX_SPEED - FAN_IDLE_SPEED))))  );
  } else {
    newFanSpeed = FAN_IDLE_SPEED;
  }
  if (newFanSpeed != fanSpeed) {
    fanSpeed = newFanSpeed;
    pwmWrite(9,fanSpeed);  
  }
}
