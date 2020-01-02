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
#include <string.h> //strcpy
#include <M5Stack.h>
#include "DHT12.h"
#include <Wire.h>     //The DHT12 uses I2C comunication.
DHT12 dht12;          //Preset scale CELSIUS and ID 0x5c.
//#line 27 "MyM5StackThermometer.ino"
//#include <AUnit.h>

#define STRLEN  32

//On-Screen Display Layout
#define OSD_X    0
#define OSD_Y   60
#define OSD_W  320
#define OSD_H  120

#define OSD_BG_COLOR TFT_WHITE
#define OSD_FG_COLOR TFT_GREEN

void monolithic_implementation()
{
  char str[STRLEN] = {'\0'};
  char *msg01 = "under -10.0";
  char *msg02 = "over 40.0";
  char *msg11 = "cold";
  char *msg12 = "comfortable";
  char *msg13 = "hot";
  int font_number = 4;
  float temperature = dht12.readTemperature();
  
  Serial.print("Temperature: ");
  Serial.println(temperature);
  
  M5.Lcd.fillRect(OSD_X, OSD_Y, OSD_W, OSD_H, OSD_BG_COLOR);
  M5.Lcd.setTextColor(OSD_FG_COLOR);
  
  if(temperature < -10.0)
  {
    strcpy(str, msg01);
    font_number = 4;
  }
  else if(40.0 < temperature)
  {
    strcpy(str, msg02);
    font_number = 4;
  }
  else
  {
    sprintf(str, "%.1f", temperature);
    font_number = 7; // 7-seg LED
  }
  M5.Lcd.drawCentreString(str, 160, 80, font_number); // string, x position, y position, font number
  
  if(temperature < 20.0)
  {
    strcpy(str, msg11);
  }
  else if(25.0 < temperature)
  {
    strcpy(str, msg13);
  }
  else
  {
    strcpy(str, msg12);
  }
  M5.Lcd.drawCentreString(str, 160, 144, 4); // string, x position, y position, font number
}

void setup() {
  M5.begin();
  Wire.begin();
  Serial.begin(115200);
}

void loop() {
  monolithic_implementation();
  delay(2000);
}
