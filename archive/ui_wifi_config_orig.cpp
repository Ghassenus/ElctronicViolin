// src/ui/ui_wifi_config.cpp

#include "ui_wifi_config.h"
#include <lvgl.h>
#include "core/wifi_manager.h"
#include <Preferences.h>
#include <WiFi.h>

// — LVGL objects —
static lv_obj_t* lbl_status;
static lv_obj_t* dd_ssid;
static lv_obj_t* ta_pass;
static lv_obj_t* eye_btn;
static lv_obj_t* btn_save;
static lv_obj_t* kb;

// Timer to poll async scan completion
static lv_timer_t* scan_timer = nullptr;

// — Forward declarations —
static void start_scan_and_schedule();
static void scan_timer_cb(lv_timer_t* timer);

// 1) Show keyboard when clicking the password field
static void on_ta_click(lv_event_t* e) {
    lv_keyboard_set_textarea(kb, ta_pass);
    lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
}

// 2) Hide keyboard on “Enter”
static void on_kb_ready(lv_event_t* e) {
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
}

// 3) Toggle password visibility
static void on_eye_toggle(lv_event_t* e) {
    bool pwd = lv_textarea_get_password_mode(ta_pass);
    lv_textarea_set_password_mode(ta_pass, !pwd);
    lv_label_set_text(lv_obj_get_child(eye_btn, 0),
                      pwd ? LV_SYMBOL_EYE_OPEN : LV_SYMBOL_EYE_CLOSE);
}

// 4) Save credentials & attempt connection
static void on_save(lv_event_t* e) {
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_t* parent = (lv_obj_t*)lv_event_get_user_data(e);

    // get selected SSID
    char buf[33];
    lv_dropdown_get_selected_str(dd_ssid, buf, sizeof(buf));
    const char* pwd = lv_textarea_get_text(ta_pass);

    // save prefs
    Preferences p; p.begin("wifi", false);
    p.putString("ssid", buf);
    p.putString("pass", pwd);
    p.end();

    wifi_manager_connect();

    // feedback
    lv_obj_t* msg = lv_label_create(parent);
    lv_label_set_text(msg,
      WiFi.status() == WL_CONNECTED
        ? "Connecté " LV_SYMBOL_OK
        : "Échec "    LV_SYMBOL_CLOSE
    );
    lv_obj_align(msg, LV_ALIGN_BOTTOM_MID, 0, -20);
}

// 5) Manual scan button handler
static void on_scan_btn(lv_event_t* e) {
    start_scan_and_schedule();
}

// Timer callback: poll scan completion and update dropdown
static void scan_timer_cb(lv_timer_t* timer) {
    if (wifi_manager_scan_in_progress()) {
        // still scanning...
        return;
    }
    // scan finished
    wifi_manager_scan_handle(); // collect results
    int cnt = wifi_manager_get_scan_count();
    String list;
    for (int i = 0; i < cnt; ++i) {
        if (i) list += "\n";
        list += wifi_manager_get_ssid(i);
    }
    lv_dropdown_set_options(dd_ssid, list.c_str());
    lv_timer_del(timer);
    scan_timer = nullptr;
}

// Helper to start scan and schedule timer
static void start_scan_and_schedule() {
    // async scan start
    wifi_manager_scan_start();
    // show placeholder
    lv_dropdown_set_options(dd_ssid, "Scanning...");
    // schedule timer every 300ms
    if (scan_timer) lv_timer_del(scan_timer);
    scan_timer = lv_timer_create(scan_timer_cb, 300, NULL);
}

// Entry point: build and show the Wi‑Fi config page
void ui_wifi_config_show(lv_obj_t* parent) {
    lv_obj_clean(parent);

    // Status label at top
    lbl_status = lv_label_create(parent);
    if (WiFi.status() == WL_CONNECTED) {
        String t = "SSID: " + WiFi.SSID();
        lv_label_set_text(lbl_status, t.c_str());
    } else {
        lv_label_set_text(lbl_status, "Mode AP");
    }
    lv_obj_align(lbl_status, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_color(lbl_status, lv_color_white(), LV_PART_MAIN);

    // Dropdown for SSIDs
    dd_ssid = lv_dropdown_create(parent);
    lv_obj_set_width(dd_ssid, 180);
    lv_obj_align(dd_ssid, LV_ALIGN_TOP_MID, -20, 40);

    // Scan button
    lv_obj_t* btn_scan = lv_btn_create(parent);
    lv_obj_set_size(btn_scan, 30, 30);
    lv_obj_align_to(btn_scan, dd_ssid, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_event_cb(btn_scan, on_scan_btn, LV_EVENT_CLICKED, NULL);
    lv_obj_t* scan_lbl = lv_label_create(btn_scan);
    lv_label_set_text(scan_lbl, LV_SYMBOL_REFRESH);
    lv_obj_center(scan_lbl);

    // Start initial scan
    start_scan_and_schedule();

    // Password field
    ta_pass = lv_textarea_create(parent);
    lv_textarea_set_one_line(ta_pass, true);
    lv_textarea_set_password_mode(ta_pass, true);
    lv_textarea_set_placeholder_text(ta_pass, "Mot de passe");
    lv_obj_set_width(ta_pass, 180);
    lv_obj_align(ta_pass, LV_ALIGN_CENTER, -60, 10);
    lv_obj_add_event_cb(ta_pass, on_ta_click, LV_EVENT_CLICKED, NULL);

    // Eye toggle
    eye_btn = lv_btn_create(parent);
    lv_obj_set_size(eye_btn, 30, 30);
    lv_obj_align_to(eye_btn, ta_pass, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_add_event_cb(eye_btn, on_eye_toggle, LV_EVENT_CLICKED, NULL);
    lv_obj_t* eye_lbl = lv_label_create(eye_btn);
    lv_label_set_text(eye_lbl, LV_SYMBOL_EYE_OPEN);
    lv_obj_center(eye_lbl);

    // Save button
    btn_save = lv_btn_create(parent);
    lv_obj_set_size(btn_save, 60, 30);
    lv_obj_align_to(btn_save, eye_btn, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_event_cb(btn_save, on_save, LV_EVENT_CLICKED, parent);
    lv_obj_t* save_lbl = lv_label_create(btn_save);
    lv_label_set_text(save_lbl, "OK " LV_SYMBOL_OK);
    lv_obj_center(save_lbl);

    // Virtual keyboard
    kb = lv_keyboard_create(parent);
    lv_obj_set_size(kb, lv_pct(100), lv_pct(40));
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(kb, on_kb_ready, LV_EVENT_READY, NULL);
}
