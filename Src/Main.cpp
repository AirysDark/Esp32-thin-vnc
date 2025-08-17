#include <WiFi.h>
#include <Arduino_GFX_Library.h>
#include <XPT2046_Touchscreen.h>
#include "VNCClient.h"

// WiFi credentials
const char *ssid = "YourSSID";
const char *password = "YourPassword";

// VNC server details
const char *vnc_host = "192.168.1.100";  // Replace with your VNC server IP
const uint16_t vnc_port = 5901;
const char *vnc_password = "yourVNCPass";  // Replace with your VNC password

// Display pins
#define TFT_CS    5
#define TFT_DC    27
#define TFT_RST   33

// Touch pins
#define TOUCH_CS  4

// Setup Display
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, 18 /*SCK*/, 23 /*MOSI*/, 19 /*MISO*/);
Arduino_GFX *gfx = new Arduino_ILI9488_18bit(bus, TFT_RST, 0 /*rotation*/);

// Setup Touch
XPT2046_Touchscreen ts(TOUCH_CS);

// VNC client instance
VNCClient vncClient;

// Display pixel draw callback
void drawPixel(int16_t x, int16_t y, uint16_t color) {
  gfx->drawPixel(x, y, color);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  gfx->begin();
  gfx->fillScreen(BLACK);

  ts.begin();
  ts.setRotation(1);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  vncClient.setDrawPixelCallback(drawPixel);
  if (vncClient.connect(vnc_host, vnc_port, vnc_password)) {
    Serial.println("VNC Connected!");
  } else {
    Serial.println("VNC Connect Failed.");
  }
}

void loop() {
  vncClient.loop();

  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    vncClient.sendPointerEvent(p.x, p.y, true);  // Touch press
    delay(50);
    vncClient.sendPointerEvent(p.x, p.y, false); // Touch release
  }
}
