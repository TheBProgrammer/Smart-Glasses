//Code for Arduino Pro Mini

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AMG88xx.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_AMG88xx amg;

//SoftwareSerial mySerial(); // Rx, Tx

int D = 0;

void setup() {
  Serial.begin(115200);

  if (!amg.begin(0x69))
  {
    Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
    while (1);
  }

  if(!display1.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display1.clearDisplay();
}

void loop() {
  
  display1.display();
  display1.setTextSize(1);
  display1.setCursor(27, 0);
  display1.setTextColor(WHITE);
  display1.println("Alert System"); 
  
  Serial.println(amg.readThermistor());

  if (amg.readThermistor() >= 25)
  {
    Serial.write(1);
    delay(10000);
  }
  D = 0;
  D = Serial.read();
  if (D == 2)
  {
    display1.clearDisplay();

    display1.display();
    display1.setTextSize(1);
    display1.setCursor(27, 0);
    display1.setTextColor(WHITE);
    display1.println("Alert !!"); 
    
    display1.setTextSize(1);
    display1.setTextColor(WHITE);
    display1.setCursor(0,15);
    display1.println("You are near a person\nwith temperature.");
    display1.display(); 
    delay(10000); 
    display1.clearDisplay();
    D = 0;
  }
  else 
  {
    display1.display();
    display1.setTextSize(1);
    display1.setCursor(47, 15);
    display1.setTextColor(WHITE);
    display1.println("SAFE"); 
  }
  
    /*display1.clearDisplay();
    display1.display();
    display1.setTextSize(2);
    display1.setTextColor(WHITE);
    display1.setCursor(20, 0);
    display1.println("ALERT !!");

    display1.setTextSize(1);
    display1.setTextColor(WHITE);
    display1.setCursor(0,15);
    display1.startscrollleft(0x00, 0x0F);
    delay(2000);
    display1.println("You are near a person with temperature.");
    display1.display(); */
}
