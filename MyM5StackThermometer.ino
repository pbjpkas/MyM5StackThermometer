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

#define ENABLE_DBG 1

#define STRLEN  32

//On-Screen Display Layout
#define OSD_X    0
#define OSD_Y   60
#define OSD_W  320
#define OSD_H  120

#define OSD_BG_COLOR TFT_WHITE
#define OSD_FG_COLOR TFT_GREEN

//message strings
char msg_01[] = "under -10.0";
char msg_02[] = "over 40.0";
char msg_11[] = "cold";
char msg_12[] = "comfortable";
char msg_13[] = "hot";

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
    strcpy(str, msg_01);
    font = 4;
  }
  else if(40.0 < temperature)
  {
    strcpy(str, msg_02);
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
    strcpy(str, msg_11);
  }
  else if(25.0 < temperature)
  {
    strcpy(str, msg_13);
  }
  else
  {
    strcpy(str, msg_12);
  }
  M5.Lcd.drawCentreString(str, 160, 144, 4);
}

char *genMessageStr(float temperature)
{
  if(temperature < 20.0)
  {
    return msg_11;
  }
  else if(25.0 < temperature)
  {
    return msg_13;
  }
  else
  {
    return msg_12;
  }
}

char *genTemperatureStr(float temperature)
{
  static char str[STRLEN] = {'\0'};

  if(temperature < -10.0)
  {
    return msg_01;
  }
  else if(40.0 < temperature)
  {
    return msg_02;
  }
  else
  {
    sprintf(str, "%.1f", temperature);
    return str;
  }
}

void drawStr(char *temperatureStr, char *messageStr)
{
  int font = 4;
  
  if( !strcmp( msg_01, temperatureStr) )
  {
    font = 4;
  }
  else if( !strcmp( msg_02, temperatureStr) )
  {
    font = 4;
  }
  else
  {
    font = 7;
  }
  M5.Lcd.fillRect(OSD_X, OSD_Y, OSD_W, OSD_H, OSD_BG_COLOR);
  M5.Lcd.drawCentreString(temperatureStr, 160, 80, font);
  M5.Lcd.drawCentreString(messageStr, 160, 144, 4);
}

void myM5StackThermometer()
{
  float fTemperature;
  char sTemperature[STRLEN] = {'\0'};
  char sMessage[STRLEN]     = {'\0'};

  fTemperature = dht12.readTemperature();
#if ENABLE_DBG
  Serial.println(fTemperature);
#endif

  strcpy(sTemperature, genTemperatureStr(fTemperature));
  strcpy(sMessage,     genMessageStr(fTemperature));

  drawStr(sTemperature, sMessage);
}

void initDisplay()
{
  M5.Lcd.fillRect(0, 0, 320, 240, OSD_BG_COLOR);
  M5.Lcd.setTextColor(TFT_BLUE);
  M5.Lcd.drawCentreString("MyM5Stack Thermometer by ka's", 160, 220, 2);
  M5.Lcd.setTextColor(TFT_GREEN);
}

void setup() {
  M5.begin();
  Wire.begin();
  Serial.begin(115200);
  
  initDisplay();
  
#if ENABLE_DBG
  Serial.print("Enter 'd' to Debug Menu\r\n");
#endif

#if ENABLE_UT
  aunit::TestRunner::setTimeout(0); // A timeout value of 0 means an infinite timeout.
#endif
}

void loop() {
#if ENABLE_UT
  aunit::TestRunner::run();
#else
  //monolithic_implementation();
  myM5StackThermometer();

  #if ENABLE_DBG
  if(Serial.available())
  {
    char buf;
    buf = Serial.read();
    if(buf=='d')//Debug
    {
      debugMenu();
    }
  }
  #endif

  delay(2000);

#endif //ENABLE_UT
}


/*
 * Debug Menu
 */
#if ENABLE_DBG
#define ERR_OK       0
#define ERR_INVALID -1
int getStr(char *buf)
{
  int i;

  i=0;
  while(1)
  {
    if(Serial.available())
    {
      buf[i] = Serial.read();
      Serial.print(buf[i]); //echo-back

      if ( (buf[i] == 0x08) or (buf[i] == 0x7f) ) //BackSpace, Delete
      {
        buf[i] = '\0';
        if(i) i--;
      }
      else if( (buf[i] == '\r') or (buf[i] == '\n') )
      {
        buf[i] = '\0';
        return strlen(buf);
      }
      else
      {
        i++;
        if(i>=STRLEN)
        {
          Serial.print("### BUFFER FULL, CLEAR. ###\r\n");
          for(i=0; i<STRLEN; i++) buf[i] = '\0';
          i=0;
          return ERR_INVALID;
        }
      }
    }
  }// while
}

// color definition: https://github.com/m5stack/M5Stack/blob/master/src/utility/In_eSPI.h
void selectTextColor()
{
  char buf;
  unsigned int textColor = TFT_BLACK;

  while(Serial.available()){ Serial.read(); } // clear buffer

  Serial.println("-- COLOR ---------------------");
  Serial.println("a : BLACK");
  Serial.println("b : BLUE");
  Serial.println("c : GREEN");
  Serial.println("d : CYAN");
  Serial.println("e : RED");
  Serial.println("f : MAGENTA");
  Serial.println("g : YELLOW");
  Serial.println("h : WHITE");
  Serial.println("i : ORANGE");
  Serial.println("j : GREENYELLOW");
  Serial.println("k : PINK");
  Serial.print("Select : ");

  while(!Serial.available()){};
  buf = Serial.read();
  Serial.println(buf);
  
  switch(buf)
  {
    case 'a':
      textColor = TFT_BLACK;
      break;
    case 'b':
      textColor = TFT_BLUE;
      break;
    case 'c':
      textColor = TFT_GREEN;
      break;
    case 'd':
      textColor = TFT_CYAN;
      break;
    case 'e':
      textColor = TFT_RED;
      break;
    case 'f':
      textColor = TFT_MAGENTA;
      break;
    case 'g':
      textColor = TFT_YELLOW;
      break;
    case 'h':
      textColor = TFT_WHITE;
      break;
    case 'i':
      textColor = TFT_ORANGE;
      break;
    case 'j':
      textColor = TFT_GREENYELLOW;
      break;
    case 'k':
      textColor = TFT_PINK;
      break;
    default:
      Serial.println("?");
      break;
  }
  M5.Lcd.setTextColor(textColor);
  
  while(Serial.available()){ Serial.read(); } // clear buffer
}

int debugDrawStr()
{
  char sTemperature[STRLEN] = {'\0'};
  char sMessage[STRLEN]     = {'\0'};
  int err = ERR_OK;

  while(Serial.available()){ Serial.read(); } // clear buffer
  
  Serial.println("-- Debug drawStr()------------");
  
  Serial.print("Enter Temperature : ");
  err = getStr(&sTemperature[0]);
  if(err<0) return err;
  Serial.print("\n");
  
  Serial.print("Enter Message     : ");
  err = getStr(&sMessage[0]);
  if(err<0) return err;
  Serial.print("\n");
  
  drawStr(sTemperature, sMessage);
  
  while(Serial.available()){ Serial.read(); } // clear buffer
  return ERR_OK;
}

void printHelp()
{
    Serial.println("-- HELP ----------------------");
    Serial.print( F("This is ") );
    Serial.print( F(__FILE__) );
    Serial.print( F(" ") );
    Serial.print( F("Build at ") );
    Serial.print( F(__DATE__) );
    Serial.print( F(" ") );
    Serial.print( F(__TIME__) );
    Serial.print( F("\r\n") );
}

void debugMenu()
{
  char buf;

  while(1)
  {
    while(Serial.available()){ Serial.read(); } // clear buffer

    Serial.println("-- DEBUG MENU ----------------");
    Serial.println("1 : selectTextColor()");
    Serial.println("2 : debugDrawStr()");
    Serial.println("3 : printHelp()"); 
    Serial.println("q : Quit Debug Menu"); 
    Serial.print("Select : ");

    while(!Serial.available()){};
    buf = Serial.read();
    Serial.println(buf);
    
    switch(buf)
    {
      case '1':
        selectTextColor();
        break;
      case '2':
        debugDrawStr();
        break;
      case '3':
        printHelp();
        break;
      case 'q':
        Serial.println("-- QUIT ----------------------");
        while(Serial.available()){ Serial.read(); } // clear buffer
        return;
      default:
        Serial.println("?");
        break;
    }
  }
}
#endif // ENABLE_DBG


/*
 * Unit Test
 */
#if ENABLE_UT
// UT:genMessageStr
test(genMessageStr_150)
{
  assertEqual(0, strcmp(genMessageStr(15.0f), msg_11));
}

test(genMessageStr_199)
{
  assertEqual(0, strcmp(genMessageStr(19.9f), msg_11));
}

test(genMessageStr_200)
{
  assertEqual(0, strcmp(genMessageStr(20.0f), msg_12));
}

test(genMessageStr_225)
{
  assertEqual(0, strcmp(genMessageStr(22.5f), msg_12));
}

test(genMessageStr_250)
{
  assertEqual(0, strcmp(genMessageStr(25.0f), msg_12));
}

test(genMessageStr_251)
{
  assertEqual(0, strcmp(genMessageStr(25.1f), msg_13));
}

test(genMessageStr_300)
{
  assertEqual(0, strcmp(genMessageStr(30.0f), msg_13));
}

// UT:genTemperatureStr
test(genTemperatureStr_m200)
{
  assertEqual(0, strcmp(genTemperatureStr(-20.0f), msg_01));
}

test(genTemperatureStr_m150)
{
  assertEqual(0, strcmp(genTemperatureStr(-15.0f), msg_01));
}

test(genTemperatureStr_m101)
{
  assertEqual(0, strcmp(genTemperatureStr(-10.1f), msg_01));
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
  assertEqual(0, strcmp(genTemperatureStr(40.1f), msg_02));
}

test(genTemperatureStr_500)
{
  assertEqual(0, strcmp(genTemperatureStr(50.0f), msg_02));
}

test(genTemperatureStr_600)
{
  assertEqual(0, strcmp(genTemperatureStr(60.0f), msg_02));
}

#endif //ENABLE_UT
