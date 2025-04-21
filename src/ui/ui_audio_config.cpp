#include "ui_audio_config.h"
#include <lvgl.h>

void ui_audio_config_show(lv_obj_t* parent) {
    lv_obj_clean(parent);
    // TODO : remplacer par votre UI audio (sélection BT/jack, volume, effets…)
    lv_obj_t* lbl = lv_label_create(parent);
    lv_label_set_text(lbl, "Audio (à venir)");
    lv_obj_center(lbl);
}
