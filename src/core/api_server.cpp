// src/core/api_server.cpp
#include "api_server.h"
#include <ArduinoJson.h>
#include "core/wifi_manager.h"
#include "core/parameters_manager.h"
#include "core/battery.h"
#include <WiFi.h>
#include <time.h>

void api_server_init(AsyncWebServer &server) {
  // 1) GET /api/status
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *req){
    StaticJsonDocument<256> doc;
    doc["wifi"]      = WiFi.isConnected();
    doc["ssid"]      = WiFi.SSID();
    doc["ip"]        = WiFi.localIP().toString();
    doc["signal"]    = WiFi.RSSI();
    doc["mac"]       = WiFi.macAddress();
    doc["battery"]   = battery_get_percent();
    doc["format24h"] = parameters_manager_is_24h_format();

    time_t now = time(nullptr);
    struct tm tm; localtime_r(&now, &tm);
    char buf_time[6];
    snprintf(buf_time, sizeof(buf_time), "%02d:%02d", tm.tm_hour, tm.tm_min);
    doc["time"] = buf_time;

    char buf_date[11];
    snprintf(buf_date, sizeof(buf_date),
             "%02d/%02d/%04d",
             tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    doc["date"] = buf_date;

    String out;
    serializeJson(doc, out);
    req->send(200, "application/json", out);
  });

  // 2) POST /api/wifi/connect
  server.on("/api/wifi/connect", HTTP_POST,
    // onRequest: rien à faire ici
    [](AsyncWebServerRequest *req){},
    // onUpload: pas d’upload
    nullptr,
    // onBody: on lit tout le JSON, parse et agit
    [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total){
      static String body;
      if(index == 0) body = "";
      body += String((char*)data, len);
      if(index + len != total) return; // pas encore tout reçu

      StaticJsonDocument<200> doc;
      auto err = deserializeJson(doc, body);
      if(err || !doc["ssid"].is<const char*>() || !doc["pass"].is<const char*>()) {
        req->send(400, "application/json", "{\"error\":\"ssid & pass required\"}");
        return;
      }
      wifi_manager_save(doc["ssid"], doc["pass"]);
      wifi_manager_connect();
      req->send(200, "application/json", "{\"result\":\"ok\"}");
    }
  );

  // 3) POST /api/params/time
  server.on("/api/params/time", HTTP_POST,
    [](AsyncWebServerRequest *req){},
    nullptr,
    [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total){
      static String body;
      if(index == 0) body = "";
      body += String((char*)data, len);
      if(index + len != total) return;

      StaticJsonDocument<128> doc;
      auto err = deserializeJson(doc, body);
      if(err || !doc["hour"].is<int>() || !doc["minute"].is<int>()) {
        req->send(400, "application/json", "{\"error\":\"hour & minute required\"}");
        return;
      }
      parameters_manager_set_hour(doc["hour"].as<int>());
      parameters_manager_set_minute(doc["minute"].as<int>());
      req->send(200, "application/json", "{\"result\":\"ok\"}");
    }
  );

  // 4) POST /api/params/date
  server.on("/api/params/date", HTTP_POST,
    [](AsyncWebServerRequest *req){},
    nullptr,
    [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total){
      static String body;
      if(index == 0) body = "";
      body += String((char*)data, len);
      if(index + len != total) return;

      StaticJsonDocument<128> doc;
      auto err = deserializeJson(doc, body);
      if(err
         || !doc["year"].is<int>()
         || !doc["month"].is<int>()
         || !doc["day"].is<int>()) {
        req->send(400, "application/json", "{\"error\":\"year, month & day required\"}");
        return;
      }
      parameters_manager_set_date(
        doc["year"].as<int>(),
        doc["month"].as<int>(),
        doc["day"].as<int>()
      );
      req->send(200, "application/json", "{\"result\":\"ok\"}");
    }
  );

  // 5) POST /api/params/format
  server.on("/api/params/format", HTTP_POST,
    [](AsyncWebServerRequest *req){},
    nullptr,
    [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total){
      static String body;
      if(index == 0) body = "";
      body += String((char*)data, len);
      if(index + len != total) return;

      StaticJsonDocument<64> doc;
      auto err = deserializeJson(doc, body);
      if(err || !doc["format24"].is<bool>()) {
        req->send(400, "application/json", "{\"error\":\"format24 required\"}");
        return;
      }
      parameters_manager_set_time_format(doc["format24"].as<bool>());
      req->send(200, "application/json", "{\"result\":\"ok\"}");
    }
  );

  // Note : **NE PAS** appeler `server.begin()` ici !
  // Lancement du serveur unique se fait en main().
}
