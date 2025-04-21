//src/ui/ui_header.h
#pragma once
#include <lvgl.h>

// Crée un header (10 % haut de l’écran) + un conteneur contenu (90 % bas).
// Retourne le conteneur dans lequel vous dessinerez votre UI.
lv_obj_t* ui_header_create(lv_obj_t* parent);

// Met à jour l’heure affichée (format HH:MM)
// À appeler périodiquement, p.ex. via un timer LVGL ou dans loop()
void ui_header_update_time();

// Change le texte central
void ui_header_set_title(const char* txt);

// Met à jour l’icône batterie (0..100%)
void ui_header_set_battery(int level_percent);

// Met à jour l’icône Wi‑Fi (true = connecté)
void ui_header_set_wifi(bool connected);

// Montre/caché l’icône OTA en cours
void ui_header_notify_ota(bool in_progress);
