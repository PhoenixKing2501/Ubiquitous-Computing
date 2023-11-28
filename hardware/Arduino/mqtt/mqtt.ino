/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a nonblocking
 reconnect function built into the loop()-function. 

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

//#include <ESP8266WiFi.h>


#define CAMERA_MODEL_AI_THINKER  // Has PSRAM
#include "camera_pins.h"

#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems

#include "esp_camera.h"
#include <ESPPubSubClientWrapper.h>

#include "Arduino.h"
#include "FS.h"      // SD Card ESP32
#include "SD_MMC.h"  // SD Card ESP32

// Update these with values suitable for your network.

#define BUILTIN_LED 4

// const char* ssid = "MSI Lappy";
const char* ssid = "Utsav Hoppy";
// const char* password = "M$1ut$@v";
const char* password = "Utsav_Hoppy@1234";
// const char* mqtt_server = "utsav-msi-lappy";
uint8_t ip[4] = { 192, 168, 233, 206 };

ESPPubSubClientWrapper client(ip);
// long lastMsg = 0;
// char msg[50];
// int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void config_camera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // if (psramFound()) {
  config.frame_size = FRAMESIZE_VGA;  // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
  config.jpeg_quality = 10;
  config.fb_count = 2;
  config.grab_mode = CAMERA_GRAB_LATEST;
  // } else {
  //   config.frame_size = FRAMESIZE_SVGA;
  //   config.jpeg_quality = 12;
  //   config.fb_count = 1;
  // }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t* s = esp_camera_sensor_get();
  s->set_whitebal(s, 1);
  s->set_awb_gain(s, 1);
  s->set_raw_gma(s, 1);
  s->set_lenc(s, 1);
  s->set_dcw(s, 0);
  s->set_bpc(s, 1);
  s->set_wpc(s, 1);
}

void connectSuccess(uint16_t count) {
  Serial.println("Connected to MQTT-Broker!\nThis is connection nb: ");
  Serial.println(count);
  // lastMsg = millis();
  client.publish("outTopic", "hello world");
}

void capture(char* topic, uint8_t* payload, unsigned int payloadLen) {
  // Take Picture with Camera
  // digitalWrite(BUILTIN_LED, HIGH);
  // delay(200);
  Serial.println("Capturing...");
  camera_fb_t* fb = esp_camera_fb_get();
  delay(200);
  // digitalWrite(BUILTIN_LED, LOW);

  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // auto ret = client.publish("outTopic", fb->buf, fb->len);
  bool ret = true;

  Serial.println("Sending...");
  ret = ret && client.beginPublish("image", fb->len, false);
  int bytes = client.write(fb->buf, fb->len);
  ret = ret && client.endPublish();

  if (!ret || bytes < fb->len) {
    Serial.println();
    Serial.println("Could not send Image");
    Serial.println("PSRAM found: " + String(psramFound()));
    Serial.print("Total heap: ");
    Serial.println(ESP.getHeapSize());
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("Total PSRAM: ");
    Serial.println(ESP.getPsramSize());
    Serial.print("Free PSRAM: ");
    Serial.println(ESP.getFreePsram());
    Serial.print("Image Length: ");
    Serial.println(fb->len);
  }

  Serial.println("Sent");

  esp_camera_fb_return(fb);
}

void picture(char* topic, char* payload) {
  if (!payload)
    return;

  String path = "";

  if (strcmp(payload, "good") == 0)
    path = "/good.jpg";
  else if (strcmp(payload, "regular") == 0)
    path = "/regular.jpg";
  else if (strcmp(payload, "bad") == 0)
    path = "/bad.jpg";
  else
    return;

  Serial.printf("Sending %s...", path.c_str());

  // fs::FS& fs = SD_MMC;
  File file = SD_MMC.open(path.c_str());

  if (!file) {
    Serial.println("Failed to open file!");
    return;
  }

  size_t len = file.size();
  uint8_t buf[512];

  client.beginPublish("image", len, false);
  while (len > 0) {
    size_t toRead = len;

    if (toRead > 512) {
      toRead = 512;
    }

    file.read(buf, toRead);
    len -= toRead;

    client.write(buf, toRead);
  }
  client.endPublish();

  Serial.println("Sent");

  // file.close();
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  //disable brownout detector
  Serial.begin(115200);

  if (!SD_MMC.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD_MMC card attached");
    return;
  }

  Serial.print("SD_MMC Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

  config_camera();
  setup_wifi();

  // client.setCallback(callback);
  client.onConnect(connectSuccess);
  // client.on("lightOff", [](char* topic, byte* payload, unsigned int length) {digitalWrite(BUILTIN_LED, LOW);});
  // client.on("lightOn", [](char* topic, byte* payload, unsigned int length) {digitalWrite(BUILTIN_LED, HIGH);});
  // client.on("disconnect", [](char* topic, byte* payload, unsigned int length) {client.disconnect();});
  client.on("picture", picture);
  client.on("capture", capture);
  // client.subscribe("inTopic");
}

void loop() {

  client.loop();

  // long now = millis();
  // if (client.connected())
  //   if (now - lastMsg > 2000) {
  //     lastMsg = now;
  //     ++value;
  //     snprintf (msg, 50, "hello world #%ld", value);
  //     Serial.print("Publish message: ");
  //     Serial.println(msg);
  //     client.publish("outTopic", msg);
  //   }
}
