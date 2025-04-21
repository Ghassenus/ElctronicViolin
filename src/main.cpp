#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <lvgl.h>

#include "core/lvgl_helpers.h"
#include "core/wifi_manager.h"
#include "core/ota_manager.h"
#include "core/api_server.h"     // votre API + routes
#include "ui/ui_main.h"
#include "ui/ui_header.h"

#include <ESPAsyncWebServer.h>

TFT_eSPI tft;
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // LVGL + écran
  lv_init();
  tft.begin();
  tft.setRotation(1);

  // 1) Wi‑Fi
  wifi_manager_scan_start(); 
  wifi_manager_connect();

  // 2) OTA
  ota_manager_init();

  // 3) LVGL
  lvgl_setup();
  ui_init();

  // 4) API & Web
  api_server_init(server);

  // Enfin on démarre **UNE** fois le serveur HTTP/WS
  server.begin();
}

void loop() {
  wifi_manager_scan_handle();     // vérifie la fin du scan asynchrone
  ota_manager_handle();
  lv_timer_handler();
  delay(5);
}
