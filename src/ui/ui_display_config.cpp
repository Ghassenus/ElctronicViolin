#include "ui_display_config.h"
#include <lvgl.h>
#include "ui_header.h"
#include <WiFi.h>

void ui_display_config_show(lv_obj_t* parent) {
    lv_obj_clean(parent);
    // TODO : remplacer par votre UI éclairage (luminosité, contraste…)
    lv_obj_t* lbl = lv_label_create(parent);
    lv_label_set_text(lbl, "Éclairage (à venir)");
    lv_obj_center(lbl);

    ui_header_set_title("Eclairage");
    ui_header_set_battery(50);
    ui_header_set_wifi(WiFi.isConnected());
    ui_header_notify_ota(false);
}