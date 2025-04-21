#pragma once
#include <Arduino.h>

// Démarre un scan asynchrone (ne bloque pas)
void wifi_manager_scan_start();

// Retourne true tant que le scan est en cours
bool wifi_manager_scan_in_progress();

// Nombre de réseaux trouvés (après scan terminé)
int  wifi_manager_get_scan_count();

// SSID i‑ème trouvé (après scan terminé)
const char* wifi_manager_get_ssid(int index);

// Gestion de la connexion enregistrée
void wifi_manager_connect();
void wifi_manager_save(const char* ssid, const char* pass);
void wifi_manager_clear();
void wifi_manager_scan_handle();
