#include <SPI.h>
#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

double T;

Adafruit_AMG88xx amg;

void setup() 
{
  Serial.begin(115200);
  Serial.println("Temperature Test");
  
  //display1.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  if(!display1.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  bool status;

  //default settings
  status = amg.begin(0x69);
  if (!status)
  {
    Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Thermistor Test --");
  Serial.println();
  delay(100);

  display1.setTextSize(2);
  display1.clearDisplay();
  display1.setCursor(0,0);
  display1.setTextColor(WHITE);
  display1.println("");
  display1.display();
}

void loop() {

    T = amg.readThermistor();
    Serial.print("Thermistor Temperature = ");
    Serial.print(T);
    Serial.println(" *C");

    display1.setTextSize(1);
    display1.setCursor(0,15);
    display1.setTextColor(WHITE);
    display1.println("Current Temperature: ");
    display1.write(T);
    display1.println(" *C");
    display1.println();
    delay(100);
    display1.display();

    if (T >= 29)
    {
      display1.setTextSize(1);
      display1.setTextColor(WHITE);
      display1.setCursor(0,20);
      display1.println("ALERT !! You are near a person with temperature.");

      Serial.println("\nALERT !! You are near a person with temperature.");
    }
    
    delay(1000);
}
