#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <SoftwareSerial.h>
#include <PubSubClient.h>
#include "Camera_Model_Config.h"
#include "Camera_config.h

const char *ssid = "SSID"; //Wifi SSID
const char *password = "PASSWORD"; //Wifi Password

float T = 0;

const char *ID = "esp32"; // Name of our device, must be unique
const char *TOPIC = "camera/image" //Topic to subscribe to 
//const char *serverIP = "192.168.1.25"; 

IPAddress broker(192.168.1.25); //IP Address of MQTT Broker/Server - Mosquitto
WiFiClient Wclient;

PubSubClient client(Wclient);

//SoftwareSerial Serial1(10,11);

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Serial1.begin(115200);

  //Connect to WiFi
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(" ");
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());

  client.setServer(broker, 1883);
  client.setCallback(callback);
  
  Serial.println("Ready");

  // init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID)) {
      Serial.println("connected");
      Serial.print("Publishing to: ");
      Serial.println(TOPIC);
      Serial.println('\n');

    } else {
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop () {
  while (Serial1.available()>0) 
  {
    float val = Serial1.parseFloat();
    if(Serial1.read() == '\n')
    {
      T == val;
      Serial.print("T : ",T);
    }
  }
  if (T == 1) {
    sendPhoto();
    T = 0;
  }  
}

void sendPhoto () {
  camera_fb_t *fb = esp_camera_fb_get();
    // int tranCount = (fb->len * 2 + (1000 - 1)) / 1000;
    if (fb)
    {
        Serial.printf("width: %d, height: %d, buf: 0x%x, len: %d\n", fb->width, fb->height, fb->buf, fb->len);
        char data[4104];
        for (int i = 0; i < fb->len; i++)
        {
            sprintf(data, "%02X", *((fb->buf + i)));
            msg += data;
            if (msg.length() == 4096)
            {
                timeCount += 1;
                client.beginPublish("img", msg.length(), 0);
                client.print(msg);
                client.endPublish();
                msg = "";
                // Serial.println(timeCount);
            }
        }
        if (msg.length() > 0)
        {
            client.beginPublish("img", msg.length(), 0);
            client.print(msg);
            client.endPublish();
            msg = "";
        }
        client.publish("img", "1");
        timeCount = 0;
        esp_camera_fb_return(fb);
    }
}
void callback(char *topic, byte *payload, unsigned int length)
{
    // Serial.print("Message arrived [");
         // Serial.print (topic); // print information topics
    // Serial.print("] ");
    // for (int i = 0; i < length; i++)
    // {
         // Serial.print ((char) payload [i]); // Print the subject matter
    // }
    // Serial.println();
    if ((char)payload[0] == '1')
    {
        shotflag = true;
    }
    if ((char)payload[0] == '0')
    {
        shotflag = false;
    }
}
void reconnect()
{
    if (WiFiDisconnect)
    {
        WiFi.reconnect();
    }
    while (!client.connected())
    {
        client.connect("EspClient");
        client.subscribe("CAMcontrol");
    }
}
void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case SYSTEM_EVENT_STA_DISCONNECTED:
        WiFiDisconnect = true;
    case SYSTEM_EVENT_STA_CONNECTED:
        WiFiDisconnect = false;
    }
}
