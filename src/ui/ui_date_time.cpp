#include "ui_date_time.h"
#include "core/parameters_manager.h"
#include <cstdio>
#include <time.h>

// UI objects
static lv_obj_t* ta_date;
static lv_obj_t* cal;
static lv_obj_t* sb_hour;
static lv_obj_t* sb_minute;
static lv_obj_t* kb;

// Affiche/masque le calendrier
static void _on_date_focus(lv_event_t* e) {
    lv_obj_clear_flag(cal, LV_OBJ_FLAG_HIDDEN);
}
static void _on_date_defocus(lv_event_t* e) {
    lv_obj_add_flag(cal, LV_OBJ_FLAG_HIDDEN);
}

// Choix d’une date dans le calendrier
static void _on_cal_sel(lv_event_t* e) {
    lv_calendar_date_t d;
    if(lv_calendar_get_pressed_date(cal, &d)) {
        char buf[11];
        snprintf(buf, sizeof(buf), "%02d/%02d/%04d",
                 d.day, d.month + 1, d.year);
        lv_textarea_set_text(ta_date, buf);
        lv_obj_add_flag(cal, LV_OBJ_FLAG_HIDDEN);
    }
}

// Cache le clavier
static void _on_kb_ready(lv_event_t* e) {
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
}

// OK: applique date, heure, minute
static void _on_ok(lv_event_t* e) {
    // Date
    const char* s = lv_textarea_get_text(ta_date);
    int dd, mm, yy;
    if(sscanf(s, "%d/%d/%d", &dd, &mm, &yy) == 3) {
        parameters_manager_set_date(yy, mm, dd);
    }
    // Heure et minute
    int h = lv_spinbox_get_value(sb_hour);
    int m = lv_spinbox_get_value(sb_minute);
    parameters_manager_set_hour(h);
    parameters_manager_set_minute(m);
}

// Construction de la page
void ui_date_time_build(lv_obj_t* parent) {
    lv_obj_clean(parent);

    // Container (plein parent)
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_align(cont, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_opa   (cont, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(cont, LV_OPA_TRANSP, LV_PART_MAIN);

    // 1) Date TextArea
    ta_date = lv_textarea_create(cont);
    lv_obj_set_width(ta_date, lv_pct(80));
    lv_obj_align(ta_date, LV_ALIGN_TOP_MID, 0, 10);
    lv_textarea_set_placeholder_text(ta_date, "JJ/MM/AAAA");
    lv_obj_add_event_cb(ta_date, _on_date_focus,    LV_EVENT_FOCUSED,   NULL);
    lv_obj_add_event_cb(ta_date, _on_date_defocus,  LV_EVENT_DEFOCUSED, NULL);

    // Valeur initiale date
    {
      int yy, mm, dd;
      parameters_manager_get_date(yy, mm, dd);
      char buf[11];
      snprintf(buf, sizeof(buf), "%02d/%02d/%04d", dd, mm, yy);
      lv_textarea_set_text(ta_date, buf);
    }

    // 2) Calendrier caché
    cal = lv_calendar_create(cont);
    lv_calendar_set_today_date(cal, 2025, 1, 1);
    lv_calendar_set_showed_date(cal, 2025, 1);
    lv_obj_align(cal, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_add_event_cb(cal, _on_cal_sel, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_flag(cal, LV_OBJ_FLAG_HIDDEN);

    // 3) Spinners Heure et Minute
    sb_hour = lv_spinbox_create(cont);
    lv_spinbox_set_range(sb_hour, 0, 23);
    lv_spinbox_set_digit_format(sb_hour, 2, 0);
    lv_obj_set_width(sb_hour, 60);
    lv_obj_align(sb_hour, LV_ALIGN_CENTER, -30, 20);

    sb_minute = lv_spinbox_create(cont);
    lv_spinbox_set_range(sb_minute, 0, 59);
    lv_spinbox_set_digit_format(sb_minute, 2, 0);
    lv_obj_set_width(sb_minute, 60);
    lv_obj_align(sb_minute, LV_ALIGN_CENTER, 30, 20);

    // Séparateur “:” entre les spinboxes
    lv_obj_t* lbl = lv_label_create(cont);
    lv_label_set_text(lbl, ":");
    lv_obj_align(lbl, LV_ALIGN_CENTER, 0, 20);

    // Valeur initiale heure/minute
    {
      int h, m;
      h=parameters_manager_get_hour();
      m=parameters_manager_get_minute();

      lv_spinbox_set_value(sb_hour,   h);
      lv_spinbox_set_value(sb_minute, m);
    }

    // 4) Bouton OK
    lv_obj_t* btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 80, 40);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(btn, _on_ok, LV_EVENT_CLICKED, NULL);
    lv_obj_t* l = lv_label_create(btn);
    lv_label_set_text(l, "OK");
    lv_obj_center(l);

    // 5) Clavier virtuel
    kb = lv_keyboard_create(cont);
    lv_obj_set_size(kb, lv_pct(100), lv_pct(40));
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(kb, _on_kb_ready, LV_EVENT_READY, NULL);

    // Lier le clavier à la TextArea date
    lv_keyboard_set_textarea(kb, ta_date);
}