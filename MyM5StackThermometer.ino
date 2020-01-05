/*
  This code is based on
  https://github.com/m5stack/M5Stack/tree/master/examples/Modules/DHT12
 */
/*
For configuration library:
DHT12 dht12("Scale temperature","ID device for I2C");
On "Scale temperature" you can select the preset scale:
CELSIUS, FAHRENHEIT or KELVIN.
And on "ID device", you can put ID sensor, on DHT12
normally is 0x5c.
Examples:
  DHT12 dht12;
The preset scale is CELSIUS and ID is 0x5c.
  DHT12 dht12(KELVIN);
the preset scale is KELVIN and ID is 0x5c.
  DHT12 dht12(FAHRENHEIT,0x53);
The preset scale is FAHRENHEIT and ID is 0x53.
*/
#include <stdio.h>  //sprintf
#include <string.h> //strcpy, strcmp
#include <M5Stack.h>
#include "DHT12.h"
#include <Wire.h>     //The DHT12 uses I2C comunication.
DHT12 dht12;          //Preset scale CELSIUS and ID 0x5c.

#define ENABLE_UT 0
#if ENABLE_UT
  #line 30 "MyM5StackThermometer.ino"
  #include <AUnit.h>
#endif

#define STRLEN  32

//On-Screen Display Layout
#define OSD_X    0
#define OSD_Y   60
#define OSD_W  320
#define OSD_H  120

#define OSD_BG_COLOR TFT_WHITE
#define OSD_FG_COLOR TFT_GREEN

//message strings
#define MSG_01 "under -10.0"
#define MSG_02 "over 40.0"
#define MSG_11 "cold"
#define MSG_12 "comfortable"
#define MSG_13 "hot"

void monolithic_implementation()
{
  char str[STRLEN] = {'\0'};

  // M5.Lcd.drawCentreString(const char *string, int dX, int poY, int font);
  // Only font numbers 2,4,6,7 are valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : . - a p m
  // Font 7 is a 7 segment font and only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : .
  // see: https://github.com/m5stack/M5Stack/blob/master/examples/Advanced/Display/TFT_Clock/TFT_Clock.ino
  int font = 4;

  float temperature = dht12.readTemperature();

  Serial.print("Temperature: ");
  Serial.println(temperature);
  
  M5.Lcd.fillRect(OSD_X, OSD_Y, OSD_W, OSD_H, OSD_BG_COLOR);
  M5.Lcd.setTextColor(OSD_FG_COLOR);
  
  if(temperature < -10.0)
  {
    strcpy(str, MSG_01);
    font = 4;
  }
  else if(40.0 < temperature)
  {
    strcpy(str, MSG_02);
    font = 4;
  }
  else
  {
    sprintf(str, "%.1f", temperature);
    font = 7;
  }
  M5.Lcd.drawCentreString(str, 160, 80, font);
  
  if(temperature < 20.0)
  {
    strcpy(str, MSG_11);
  }
  else if(25.0 < temperature)
  {
    strcpy(str, MSG_13);
  }
  else
  {
    strcpy(str, MSG_12);
  }
  M5.Lcd.drawCentreString(str, 160, 144, 4);
}

char *genMessageStr(float temperature)
{
  if(temperature < 20.0)
  {
    return MSG_11;
  }
  else if(25.0 < temperature)
  {
    return MSG_13;
  }
  else
  {
    return MSG_12;
  }
}

char *genTemperatureStr(float temperature)
{
  static char str[STRLEN] = {'\0'};

  if(temperature < -10.0)
  {
    return MSG_01;
  }
  else if(40.0 < temperature)
  {
    return MSG_02;
  }
  else
  {
    sprintf(str, "%.1f", temperature);
    return str;
  }
}

void setup() {
  M5.begin();
  Wire.begin();
  Serial.begin(115200);
#if ENABLE_UT
  aunit::TestRunner::setTimeout(0); // A timeout value of 0 means an infinite timeout.
#endif
}

void loop() {
#if ENABLE_UT
  aunit::TestRunner::run();
#else
  monolithic_implementation();
  delay(2000);
#endif
}

#if ENABLE_UT
// UT:genMessageStr
test(genMessageStr_150)
{
  assertEqual(0, strcmp(genMessageStr(15.0f), MSG_11));
}

test(genMessageStr_199)
{
  assertEqual(0, strcmp(genMessageStr(19.9f), MSG_11));
}

test(genMessageStr_200)
{
  assertEqual(0, strcmp(genMessageStr(20.0f), MSG_12));
}

test(genMessageStr_225)
{
  assertEqual(0, strcmp(genMessageStr(22.5f), MSG_12));
}

test(genMessageStr_250)
{
  assertEqual(0, strcmp(genMessageStr(25.0f), MSG_12));
}

test(genMessageStr_251)
{
  assertEqual(0, strcmp(genMessageStr(25.1f), MSG_13));
}

test(genMessageStr_300)
{
  assertEqual(0, strcmp(genMessageStr(30.0f), MSG_13));
}

// UT:genTemperatureStr
test(genTemperatureStr_m200)
{
  assertEqual(0, strcmp(genTemperatureStr(-20.0f), MSG_01));
}

test(genTemperatureStr_m150)
{
  assertEqual(0, strcmp(genTemperatureStr(-15.0f), MSG_01));
}

test(genTemperatureStr_m101)
{
  assertEqual(0, strcmp(genTemperatureStr(-10.1f), MSG_01));
}

test(genTemperatureStr_m100)
{
  assertEqual(0, strcmp(genTemperatureStr(-10.0f), "-10.0"));
}

test(genTemperatureStr_m050)
{
  assertEqual(0, strcmp(genTemperatureStr(-5.0f), "-5.0"));
}

test(genTemperatureStr_m001)
{
  assertEqual(0, strcmp(genTemperatureStr(-0.1f), "-0.1"));
}

test(genTemperatureStr_000)
{
  assertEqual(0, strcmp(genTemperatureStr(0.0f), "0.0"));
}

test(genTemperatureStr_225)
{
  assertEqual(0, strcmp(genTemperatureStr(22.5f), "22.5"));
}

test(genTemperatureStr_400)
{
  assertEqual(0, strcmp(genTemperatureStr(40.0f), "40.0"));
}

test(genTemperatureStr_401)
{
  assertEqual(0, strcmp(genTemperatureStr(40.1f), MSG_02));
}

test(genTemperatureStr_500)
{
  assertEqual(0, strcmp(genTemperatureStr(50.0f), MSG_02));
}

test(genTemperatureStr_600)
{
  assertEqual(0, strcmp(genTemperatureStr(60.0f), MSG_02));
}

#endif //ENABLE_UT
