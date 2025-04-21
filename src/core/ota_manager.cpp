// src/core/ota_manager.cpp

#include "ota_manager.h"
#include <ArduinoOTA.h>
#include "ui/ui_header.h"  // <-- pour notifier le header

// drapeau interne indiquant si une OTA est en cours
static bool _ota_running = false;

void ota_manager_init() {
  // Nom du device sur le réseau
  ArduinoOTA.setHostname("esp32-violin");
  
  ArduinoOTA
    .onStart([]() {
      Serial.println("[OTA] Démarrage");
      _ota_running = true;
      // Affiche l’icône OTA pendant la mise à jour
      ui_header_notify_ota(true);
    })
    .onEnd([]() {
      Serial.println(F("\n[OTA] Terminé"));
      // Cache l’icône OTA quand c’est fini
      ui_header_notify_ota(false);
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("[OTA] Progress: %u%%\r", (progress * 100) / total);
    })
    .onError([](ota_error_t err) {
      Serial.printf("[OTA] Erreur [%u]: ", err);
      if      (err == OTA_AUTH_ERROR)    Serial.println("Auth Failed");
      else if (err == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
      else if (err == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (err == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (err == OTA_END_ERROR)     Serial.println("End Failed");
      _ota_running = false;
      // On masque aussi l’icône en cas d’erreur
      ui_header_notify_ota(false);
    });

  ArduinoOTA.begin();
  Serial.println(F("[OTA] Prêt"));
}

void ota_manager_handle() {
  // Doit être appelé régulièrement dans loop()
  ArduinoOTA.handle();
}

bool ota_manager_is_running() {
  return _ota_running;
}
