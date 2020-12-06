#include <Arduino.h>

//WiFi libraries
#include <WiFi.h>
#include "esp_wifi.h"

//MQTT
#include <PubSubClient.h>

//Camera related
#include "CamConfig.h"
#include "soc/soc.h" 
#include "soc/rtc_cntl_reg.h"
#include <esp_camera.h>

// Camera buffer, URL and picture name
camera_fb_t *fb = NULL;

String msg;

const char *ssid = "******"; //Name of Wifi Network
const char *pass = "*****"; //Password of Wifi Network

const char *ID = "esp32";
const char *PUB_TOPIC = "image";
const char *SUB_TOPIC = "detected";

const char *broker = "192.168.__.__";
WiFiClient wclient;

PubSubClient client(wclient);

void connect_wifi() {
  Serial.print("\nConnecting to : ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(ID)) {
      client.subscribe(SUB_TOPIC);
      Serial.println("connected");
      Serial.print("Publishing to : ");
      Serial.println(PUB_TOPIC);
      Serial.println('\n');
      Serial.print("Subscribed to : ");
      Serial.println(SUB_TOPIC);
      Serial.println('\n');
    }
    else {
      Serial.println("try again in 2 seconds");
      delay(2000);
    }
  }
}

void getimg()
{

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
                client.beginPublish("img", msg.length(), 0);
                client.print(msg);
                client.endPublish();
                msg = "";
                Serial.println("Image Published 1");
            }
        }
        if (msg.length() > 0)
        {
            client.beginPublish("img", msg.length(), 0);
            client.print(msg);
            client.endPublish();
            msg = "";
            Serial.println("Published 2");
        }
        client.publish("img", "1");
        Serial.println("Published 3");
        
        esp_camera_fb_return(fb);
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String response;
  for (int i = 0; i < length; i) {
    response = (char)payload[i];
  }
  delay(2000);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] :");
  Serial.println(response);
  /*if (response == "on")
  {
    client.publish(TOPIC,"on");
    Serial.println("on published to topic 2");
    delay(5000);
  }
  else if (response == "off")
  {
    client.publish(TOPIC2,"off");
    Serial.println("off published to topic 2");
    delay(5000);
  }*/
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  camera_init();
  connect_wifi();
  client.setServer(broker,1883);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
   reconnect();
  }

  getimg();
  delay(5000);
  client.loop();
  delay(5000);
}

bool camera_init()
{
  // IF USING A DIFFERENT BOARD, NEED DIFFERENT PINs
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = 5;
  config.pin_d1       = 18;
  config.pin_d2       = 19;
  config.pin_d3       = 21;
  config.pin_d4       = 36;
  config.pin_d5       = 39;
  config.pin_d6       = 34;
  config.pin_d7       = 35;
  config.pin_xclk     = 0;
  config.pin_pclk     = 22;
  config.pin_vsync    = 25;
  config.pin_href     = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn     = 32;
  config.pin_reset    = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  //init with high specs to pre-allocate larger buffers
  config.frame_size   = FRAMESIZE_QQVGA; // set picture size, FRAMESIZE_QQVGA = 160x120
  config.jpeg_quality = 10;            // quality of JPEG output. 0-63 lower means higher quality
  config.fb_count     = 2;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.print("Camera init failed with error 0x%x, ");
    Serial.println(err);
    return false;
    ESP.restart();
  }

  // Change extra settings if required
  //sensor_t * s = esp_camera_sensor_get();
  //s->set_vflip(s, 0);       //flip it back
  //s->set_brightness(s, 1);  //up the blightness just a bit
  //s->set_saturation(s, -2); //lower the saturation

  else
  {
    return true;
  }
  
}
