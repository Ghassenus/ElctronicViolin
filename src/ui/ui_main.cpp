// src/ui/ui_main.cpp

#include "ui_main.h"
#include <lvgl.h>
#include <WiFi.h>
#include "ui_wifi_config.h"
#include "ui_header.h"
#include "ui_parameters.h" 
#include <violin_colors.h>
#include <core/battery.h>



// — Screens —
static lv_obj_t* scr_main;
static lv_obj_t* scr_wifi;
static lv_obj_t* scr_netinfo;
static lv_obj_t* scr_audio;
static lv_obj_t* scr_display;
static lv_obj_t* scr_params;     // <-- écran paramètres

const char* main_title ="Accueil";

// — Prototypes —
static void show_screen(lv_obj_t* scr,
                        const char* title,
                        void (*build)(lv_obj_t*));
static void build_main(lv_obj_t* scr);  // <-- callback main
static void cb_show_wifi(lv_event_t* e); // <-- callback ecrna wifi
static void cb_show_netinfo(lv_event_t* e); // <-- callback ecran reseau
static void cb_show_stub(lv_event_t* e); // <-- callback ecran à venir
static void cb_show_params(lv_event_t* e);  // <-- callback paramètres
static void add_back_button(lv_obj_t* scr); // <-- callback bouton retour

// — Init UI —
void ui_init() {
    // 1) Créer tous les écrans
    scr_main    = lv_obj_create(nullptr);    // ecran accueil
    scr_wifi    = lv_obj_create(nullptr);   // ecran wifi
    scr_netinfo = lv_obj_create(nullptr);   // ecran reseau
    scr_audio   = lv_obj_create(nullptr);  // ecran audio
    scr_display = lv_obj_create(nullptr);  // ecran display
    scr_params  = lv_obj_create(nullptr);  // <--  écran param

    // 2) Mettre un fond noir sur chaque écran
    for (auto scr : { scr_main, scr_wifi, scr_netinfo, scr_audio, scr_display, scr_params }) {
        lv_obj_set_style_bg_color(scr, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_bg_opa  (scr, LV_OPA_COVER,   LV_PART_MAIN);
    }

    // 3) Construire et afficher l’écran principal
    show_screen(scr_main, main_title, build_main);
}

// — Construction de l’écran principal —
static void build_main(lv_obj_t* scr) {
    // 3a) Mettre à jour le header
    ui_header_set_title  (main_title);
    ui_header_set_battery(battery_get_percent());
    ui_header_set_wifi   (WiFi.isConnected());
    ui_header_notify_ota(false);

    // 3b) Contenu (90% de la hauteur en bas)
    lv_obj_t* cont = lv_obj_create(scr);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(90));
    lv_obj_align  (cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_layout    (cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow (cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all   (cont, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa    (cont, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(cont, LV_OPA_TRANSP, LV_PART_MAIN);

    // 3c) Grille 4×2 de tuiles
    struct BtnDef { const char* txt; const char* sym; void(*cb)(lv_event_t*); lv_obj_t* target; };
    static const BtnDef defs[8] = {
        {"Wi-Fi",   LV_SYMBOL_WIFI,     cb_show_wifi,    scr_wifi},
        {"Network", LV_SYMBOL_USB,      cb_show_netinfo, scr_netinfo},
        {"Audio",   LV_SYMBOL_AUDIO,    cb_show_stub,    scr_audio},
        {"Display", LV_SYMBOL_IMAGE,    cb_show_stub,    scr_display},
        {"Config", LV_SYMBOL_SETTINGS,  cb_show_params,  scr_params},
        {"Fonct.6", LV_SYMBOL_SETTINGS, cb_show_stub,    scr_display},
        {"Fonct.7", LV_SYMBOL_SETTINGS, cb_show_stub,    scr_display},
        {"Fonct.8", LV_SYMBOL_SETTINGS, cb_show_stub,    scr_display},
    };
    const lv_color_t bg_cols[8] = {
        lv_color_hex(0x9E9E9E), lv_color_hex(0x2196F3),
        lv_color_hex(0x4CAF50), lv_color_hex(0xFFEB3B),
        lv_color_hex(0xF44336), lv_color_hex(0x9C27B0),
        lv_color_hex(0xFFFFFF), lv_color_hex(0xE91E63),
    };
    const lv_color_t ic_cols[8] = {
        lv_color_hex(0x000000), lv_color_hex(0xFFFFFF),
        lv_color_hex(0xFFFFFF), lv_color_hex(0x000000),
        lv_color_hex(0xFFFFFF), lv_color_hex(0xFFFFFF),
        lv_color_hex(0xFFEB3B), lv_color_hex(0xFFFFFF),
    };

    for(int i = 0; i < 8; ++i) {
        auto &d = defs[i];
        // tuile 23%×49%
        lv_obj_t* tile = lv_obj_create(cont);
        lv_obj_set_size(tile, lv_pct(23), lv_pct(49));
        lv_obj_set_layout    (tile, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow (tile, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(tile,
            LV_FLEX_ALIGN_SPACE_BETWEEN,
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all   (tile, 0, LV_PART_MAIN);
        lv_obj_set_style_bg_opa    (tile, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_opa(tile, LV_OPA_TRANSP, LV_PART_MAIN);

        // bouton 100%×80%
        lv_obj_t* btn = lv_btn_create(tile);
        lv_obj_set_size(btn, lv_pct(100), lv_pct(80));
        lv_obj_add_event_cb(btn, d.cb, LV_EVENT_CLICKED, d.target);
        lv_obj_set_style_bg_color(btn, bg_cols[i], LV_PART_MAIN);
        lv_obj_set_style_bg_opa  (btn, LV_OPA_COVER,   LV_PART_MAIN);
        lv_obj_set_style_border_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_radius  (btn, 8, LV_PART_MAIN);
        lv_obj_set_style_pad_all (btn, 0, LV_PART_MAIN);

        // icône 48 dp
        lv_obj_t* ico = lv_label_create(btn);
        lv_label_set_text(ico, d.sym);
        lv_obj_set_style_text_color(ico, ic_cols[i], LV_PART_MAIN);
        lv_obj_set_style_text_font (ico, &lv_font_montserrat_48, LV_PART_MAIN);
        lv_obj_center(ico);

        // légende
        lv_obj_t* lab = lv_label_create(tile);
        lv_label_set_text(lab, d.txt);
        lv_obj_set_style_text_color(lab, lv_color_white(),  LV_PART_MAIN);
        lv_obj_set_style_text_font (lab, &lv_font_montserrat_14, LV_PART_MAIN);
    }
}

// — Affichage générique d’un écran —
static void show_screen(lv_obj_t* scr,
                        const char* title,
                        void (*build)(lv_obj_t*)) {
    lv_obj_clean(scr);
    ui_header_create(scr);
    ui_header_set_title  (title);
    ui_header_set_battery(battery_get_percent());
    ui_header_set_wifi   (WiFi.isConnected());
    ui_header_notify_ota(false);
    build(scr);
    if(scr != scr_main) add_back_button(scr);
    lv_scr_load(scr);
}

// — Callbacks —

static void cb_show_wifi(lv_event_t* e) {
    show_screen((lv_obj_t*)lv_event_get_user_data(e),
                "Config Wi-Fi",
                [](lv_obj_t* scr){
                  ui_wifi_config_show(scr);
                });
}


static void cb_show_netinfo(lv_event_t* e) {
    show_screen((lv_obj_t*)lv_event_get_user_data(e),
                "Infos Reseau",
                [](lv_obj_t* scr){
                  lv_obj_t* lbl = lv_label_create(scr);
                  String info = WiFi.isConnected()
                    ? "SSID: " + WiFi.SSID() + "\nIP: " + WiFi.localIP().toString()
                    : "Mode AP\nIP: " + WiFi.softAPIP().toString();
                  lv_label_set_text(lbl, info.c_str());
                  lv_obj_set_style_text_color(lbl, lv_color_white(), LV_PART_MAIN);
                  lv_obj_center(lbl);
                  
                });
                
}

static void cb_show_stub(lv_event_t* e) {
    show_screen((lv_obj_t*)lv_event_get_user_data(e),
                "A venir…",
                [](lv_obj_t* scr){
                  lv_obj_t* lbl = lv_label_create(scr);
                  lv_label_set_text(lbl, "Fonctionnalité\n a venir");
                  lv_obj_set_style_text_color(lbl, col_yellow, LV_PART_MAIN);
                  lv_obj_center(lbl);
                });
}

//  callback « Config parameters» —
static void cb_show_params(lv_event_t* e) {
    show_screen((lv_obj_t*)lv_event_get_user_data(e),
                "Paramètres",
                [](lv_obj_t* scr){
                  ui_parameters_show(scr);
                });
   
}

// — Bouton « ← » —  
static void add_back_button(lv_obj_t* scr) {
    lv_obj_t* btn = lv_btn_create(scr);
    lv_obj_set_size(btn, 40, 40);
    lv_obj_align   (btn, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_set_style_bg_color(btn, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa  (btn, LV_OPA_COVER,   LV_PART_MAIN);
    lv_obj_set_style_border_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN);

    lv_obj_t* lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_color(lbl, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_center(lbl);

    // callback *sans* capture, qui repose sur la variable globale scr_main
    lv_obj_add_event_cb(btn,
      [](lv_event_t*){
        show_screen(scr_main, main_title, build_main);
      },
      LV_EVENT_CLICKED, nullptr);

    // toujours devant
    lv_obj_move_foreground(btn);
}
