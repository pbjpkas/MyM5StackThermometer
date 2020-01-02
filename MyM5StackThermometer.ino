/*
  This code is based on
  https://github.com/m5stack/M5Stack/tree/master/examples/Modules/DHT12
 */
#include <M5Stack.h>
#include "DHT12.h"
#include <Wire.h>     //The DHT12 uses I2C comunication.
DHT12 dht12;          //Preset scale CELSIUS and ID 0x5c.

#line 11 "MyM5StackThermometer.ino"
#include <AUnit.h>
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

test(correct) {
  int x = 1;
  assertEqual(x, 1);
}

test(incorrect) {
  int x = 1;
  assertNotEqual(x, 1);
}

void setup() {
  M5.begin();
  Wire.begin();
  Serial.println("Prueba de libreria DHT12:");
  M5.Lcd.println("Prueba de libreria DHT12:");
}

void loop() {
  //Read temperature with preset scale.
  Serial.print("Temperatura: ");
  M5.Lcd.print("Temperatura: ");
  Serial.print(dht12.readTemperature());
  M5.Lcd.print(dht12.readTemperature());

  //Read humidity.
  Serial.print("*C  Humedad: ");
  M5.Lcd.print("*C  Humedad: ");
  Serial.print(dht12.readHumidity());
  M5.Lcd.println(dht12.readHumidity());

  //Read temperature as forced fahrenheit.
  Serial.println("%RH");
  Serial.println("%RH");
  Serial.print("Temperatura: ");
  Serial.print(dht12.readTemperature(FAHRENHEIT));

  //Read termperature as forced kelvin.
  Serial.println("*F");
  Serial.print("Temperatura: ");
  Serial.print(dht12.readTemperature(KELVIN));
  Serial.println("*K");

  while(1){ aunit::TestRunner::run();}

  delay(5000);
}
