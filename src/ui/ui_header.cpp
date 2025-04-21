#include "ui_header.h"
#include <time.h>
#include <WiFi.h>
#include "core/ota_manager.h"
#include <lvgl.h>
#include "violin_colors.h"
#include <core/battery.h>

static lv_obj_t* hdr;
static lv_obj_t* lbl_time;
static lv_obj_t* lbl_title;
static lv_obj_t* icon_wifi;
static lv_obj_t* icon_batt;
static lv_obj_t* icon_ota;

// Timer LvGL
static void _header_timer_cb(lv_timer_t*) {
    ui_header_update_time();
    ui_header_set_battery(battery_get_percent());
    ui_header_set_wifi(WiFi.status() == WL_CONNECTED);
    ui_header_notify_ota(ota_manager_is_running());
}

lv_obj_t* ui_header_create(lv_obj_t* parent) {
    // --- HEADER ---
    hdr = lv_obj_create(parent);
    lv_obj_set_size (hdr, lv_pct(100), lv_pct(10));
    lv_obj_align    (hdr, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_opa    (hdr, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(hdr, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all   (hdr, 0, LV_PART_MAIN);

    // OTA (droite)
    icon_ota = lv_label_create(hdr);
    lv_label_set_text(icon_ota, "↻");
    lv_obj_add_flag(icon_ota, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_color(icon_ota, col_red, LV_PART_MAIN);
    lv_obj_align(icon_ota, LV_ALIGN_RIGHT_MID, -5, 0);

    // Batterie ← OTA
    icon_batt = lv_label_create(hdr);
    lv_label_set_text(icon_batt, LV_SYMBOL_BATTERY_FULL);
    lv_obj_set_style_text_color(icon_batt, lv_color_white(), LV_PART_MAIN);
    lv_obj_align_to(icon_batt, icon_ota, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    // Wi‑Fi ← Batterie
    icon_wifi = lv_label_create(hdr);
    lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(icon_wifi, col_green, LV_PART_MAIN);
    lv_obj_align_to(icon_wifi, icon_batt, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    // Titre centré
    lbl_title = lv_label_create(hdr);
    lv_label_set_text(lbl_title, "");
    lv_obj_set_style_text_color(lbl_title, col_yellow, LV_PART_MAIN);
    lv_obj_set_style_text_font (lbl_title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(lbl_title, LV_ALIGN_CENTER, 0, 0);

    // Heure ← gauche
    lbl_time = lv_label_create(hdr);
    lv_label_set_text(lbl_time, "00:00");
    lv_obj_set_style_text_color(lbl_time, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font (lbl_time, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(lbl_time, LV_ALIGN_LEFT_MID, 5, 0);

    // Monte le header en premier plan par rapport aux autres enfants de 'parent'
    lv_obj_move_foreground(hdr);

    // --- CONTENU (90% bas) ---
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size (cont, lv_pct(100), lv_pct(90));
    lv_obj_align    (cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa    (cont, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(cont, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all   (cont, 0, LV_PART_MAIN);

    // Timer de mise à jour header (1 s pour tester, passez à 60000 pour 1 min)
    lv_timer_create(_header_timer_cb, 1000, nullptr);

    return cont;
}

void ui_header_update_time() {
    time_t now = time(nullptr);
    struct tm tm;
    localtime_r(&now, &tm);
    char buf[6];
    snprintf(buf, sizeof(buf), "%02d:%02d", tm.tm_hour, tm.tm_min);
    lv_label_set_text(lbl_time, buf);
}

void ui_header_set_title(const char* txt) {
    lv_label_set_text(lbl_title, txt);
}

void ui_header_set_battery(int level_percent) {
    const char* sym = LV_SYMBOL_BATTERY_EMPTY;
    lv_color_t   c   = col_red;
    if      (level_percent > 80) { sym = LV_SYMBOL_BATTERY_FULL; c = col_green; }
    else if (level_percent > 50) { sym = LV_SYMBOL_BATTERY_3;    c = col_light_green; }
    else if (level_percent > 20) { sym = LV_SYMBOL_BATTERY_2;    c = col_yellow; }
    lv_label_set_text(icon_batt, sym);
    lv_obj_set_style_text_color(icon_batt, c, LV_PART_MAIN);
}

void ui_header_set_wifi(bool connected) {
    lv_label_set_text(icon_wifi, connected ? LV_SYMBOL_WIFI : LV_SYMBOL_CLOSE);
}

void ui_header_notify_ota(bool in_progress) {
    if(in_progress) lv_obj_clear_flag(icon_ota, LV_OBJ_FLAG_HIDDEN);
    else            lv_obj_add_flag   (icon_ota, LV_OBJ_FLAG_HIDDEN);
}
