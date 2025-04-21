// src/ui/ui_wifi_config.cpp

#include "ui_wifi_config.h"
#include <lvgl.h>
#include "core/wifi_manager.h"
#include <WiFi.h>
#include <cstdio>

static lv_obj_t* lbl_status;
static lv_obj_t* btn_scan;
static lv_timer_t* scan_timer = nullptr;

// handler du bouton Scan
static void scan_btn_cb(lv_event_t* e) {
  // Lance le scan non‑bloquant
  wifi_manager_scan_start();
  // Feedback immédiat
  lv_label_set_text(lbl_status, "Scanning...");
  // Crée un timer pour vérifier l’avancement du scan
  if (scan_timer) lv_timer_del(scan_timer);
  scan_timer = lv_timer_create([](lv_timer_t* t){
    if (!wifi_manager_scan_in_progress()) {
      // Scan terminé, récupère les résultats
      wifi_manager_scan_handle();
      int n = wifi_manager_get_scan_count();
      char buf[32];
      snprintf(buf, sizeof(buf), "Found %d networks", n);
      lv_label_set_text(lbl_status, buf);
      lv_timer_del(t);
      scan_timer = nullptr;
    }
  }, 200, nullptr);
}

void ui_wifi_config_show(lv_obj_t* parent) {
  lv_obj_clean(parent);

  // Label de statut
  lbl_status = lv_label_create(parent);
  lv_label_set_text(lbl_status, "Press Scan");
  lv_obj_center(lbl_status);

  // Bouton Scan
  btn_scan = lv_btn_create(parent);
  lv_obj_align(btn_scan, LV_ALIGN_BOTTOM_MID, 0, -20);
  lv_obj_add_event_cb(btn_scan, scan_btn_cb, LV_EVENT_CLICKED, nullptr);
  lv_obj_t* l = lv_label_create(btn_scan);
  lv_label_set_text(l, "Scan");
  lv_obj_center(l);
}
