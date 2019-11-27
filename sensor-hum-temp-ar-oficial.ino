#include <Wire.h>
#include <Adafruit_AM2315.h>
//#include "SSD1306.h"

//#define pin_relay 13//5
//#define pin_solo 15//36
//#define scl 5//22
//#define sda 18//21

//SSD1306 display(0x3c, sda, scl);
Adafruit_AM2315 am2315;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  Serial.println("AM2315 Test!");

  if (! am2315.begin()) {
     Serial.println("Sensor not found, check wiring & pullups!");
     while (1);
  }
  //display.init();
}

void loop() {
  float temperature, humidity;// umidade_solo_porcentagem;
  
  if (! am2315.readTemperatureAndHumidity(&temperature, &humidity)) {
    Serial.println("Failed to read data from AM2315");
    return;
  }
  Serial.print("Temp *C: "); Serial.println(temperature);
  Serial.print("Hum %: "); Serial.println(humidity);

  /*delay(1000);
  display.clear();
  display.drawString(0,0,"Cheguei 1\n");
  display.display();
  delay(1000);
  display.clear();
  display.drawString(0,0,"Cheguei 2\n");
  display.display();
  delay(1000);*/
}
