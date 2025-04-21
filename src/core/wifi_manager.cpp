// src/core/wifi_manager.cpp

#include "wifi_manager.h"
#include <WiFi.h>
#include <Preferences.h>

static const int MAX_AP = 16;
static char ssid_list[MAX_AP][33];
static int  ap_count  = 0;
static bool scanning  = false;

void wifi_manager_scan_start() {
  scanning = true;
  // Passe en mode AP+STA pour conserver l’AP actif tout en scannant
  WiFi.mode(WIFI_AP_STA);

  // Lance un scan asynchrone
  WiFi.scanNetworks(true /*async*/, true /*showHidden*/);
}

bool wifi_manager_scan_in_progress() {
  return scanning;
}

void wifi_manager_scan_handle() {
  if(!scanning) return;

  int n = WiFi.scanComplete();  // -1 tant que le scan n'est pas fini
  if(n >= 0) {
    ap_count = (n > MAX_AP) ? MAX_AP : n;
    for(int i = 0; i < ap_count; ++i) {
      WiFi.SSID(i).toCharArray(ssid_list[i], sizeof(ssid_list[i]));
    }
    WiFi.scanDelete();
    scanning = false;
  }
}

int wifi_manager_get_scan_count() {
  return ap_count;
}

const char* wifi_manager_get_ssid(int index) {
  if(index < 0 || index >= ap_count) return "";
  return ssid_list[index];
}

void wifi_manager_save(const char* ssid, const char* pass) {
  Preferences p; p.begin("wifi", false);
  p.putString("ssid", ssid);
  p.putString("pass", pass);
  p.end();
}

void wifi_manager_clear() {
  Preferences p; p.begin("wifi", false);
  p.clear();
  p.end();
}

void wifi_manager_connect() {
  Preferences p; p.begin("wifi", true);
  String ssid = p.getString("ssid", "");
  String pass = p.getString("pass", "");
  p.end();

  if(ssid.length()) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    unsigned long t0 = millis();
    while(WiFi.status() != WL_CONNECTED && millis() - t0 < 10000) {
      delay(200);
    }
  }
  if(WiFi.status() != WL_CONNECTED) {
    // Passe en AP seul si échec
    WiFi.softAP("VIOLIN-Setup", "Violin1234");
  }
}
