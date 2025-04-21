// src/core/parameters_manager.cpp
#include "parameters_manager.h"
#include <time.h>
#include <sys/time.h>
#include <Preferences.h>
#include "ui/ui_header.h"
#include <WiFi.h>

// clefs Preferences
static const char * PREF_NAMESPACE = "params";
static const char * KEY_FORMAT   = "fmt24h";

void parameters_manager_init() {
    // Récupère format 12/24h
    Preferences p; p.begin(PREF_NAMESPACE, true);
    bool is24 = p.getBool(KEY_FORMAT, true);
    p.end();

    // Met à jour header
    ui_header_notify_ota(false); // pas d’OTA ici
    ui_header_set_wifi(WiFi.status()==WL_CONNECTED);

    // Met à jour l’affichage de l’heure selon le format
    ui_header_update_time();
    parameters_manager_set_time_format(is24);
}

static void _apply_new_time(const struct tm & tm_new) {
    // Convertit struct tm en time_t
    time_t t = mktime(const_cast<struct tm*>(&tm_new));
    struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
    settimeofday(&tv, nullptr);

    // Met à jour la RTC matérielle ici si besoin...
    // rtc.write(tm_new);

    // Et rafraîchit immédiatement le header
    ui_header_update_time();
}

void parameters_manager_set_hour(int hour) {
    time_t now = time(nullptr);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    tm_now.tm_hour = hour;
    _apply_new_time(tm_now);
}

void parameters_manager_set_minute(int minute) {
    time_t now = time(nullptr);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    tm_now.tm_min = minute;
    _apply_new_time(tm_now);
}

void parameters_manager_set_date(int year, int month, int day) {
    time_t now = time(nullptr);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    tm_now.tm_year = year - 1900;  // struct tm attend année depuis 1900
    tm_now.tm_mon  = month - 1;    // struct tm attend 0–11
    tm_now.tm_mday = day;
    _apply_new_time(tm_now);
}

void parameters_manager_set_time_format(bool is24h) {
    // Sauvegarde
    Preferences p; p.begin(PREF_NAMESPACE, false);
    p.putBool(KEY_FORMAT, is24h);
    p.end();

    // Met à jour le header (on peut l’étendre pour afficher AM/PM si 12 h)
    ui_header_update_time();
}

bool parameters_manager_is_24h_format() {
    Preferences p; p.begin(PREF_NAMESPACE, true);
    bool is24 = p.getBool(KEY_FORMAT, true);
    p.end();
    return is24;
}

int parameters_manager_get_hour() {
    time_t now = time(nullptr);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    return tm_now.tm_hour;
  }
  
  int parameters_manager_get_minute() {
    time_t now = time(nullptr);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    return tm_now.tm_min;
  }
  
  void parameters_manager_get_date(int &year, int &month, int &day) {
    time_t now = time(nullptr);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    year  = tm_now.tm_year + 1900; // struct tm stocke années depuis 1900
    month = tm_now.tm_mon  + 1;    // struct tm stocke mois 0..11
    day   = tm_now.tm_mday;
  }
