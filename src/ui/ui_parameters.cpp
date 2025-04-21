#include "ui_parameters.h"
#include "ui_date_time.h"
#include "violin_colors.h"
#include <cstdio>

// Conteneurs globaux
static lv_obj_t* s_side_menu;
static lv_obj_t* s_body;

// Callbacks menu
static void _show_page_date(lv_event_t* e) {
    ui_date_time_build(s_body);
    lv_obj_set_style_text_color(s_body, lv_color_white(), LV_PART_MAIN);
}
static void _show_page_stub(lv_event_t* e) {
    lv_obj_clean(s_body);
    lv_obj_set_style_text_color(s_body, lv_color_white(), LV_PART_MAIN);
    lv_obj_t* lbl = lv_label_create(s_body);
    lv_label_set_text(lbl, "À venir");
    lv_obj_set_style_text_color(lbl, col_yellow, LV_PART_MAIN);
    lv_obj_center(lbl);
}

void ui_parameters_show(lv_obj_t* scr) {
    // On efface le contenu actuel
    //lv_obj_clean(scr);

    // 1) Container général (90% sous le header)
    lv_obj_t* cont = lv_obj_create(scr);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(85));
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa   (cont, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(cont, LV_OPA_TRANSP, LV_PART_MAIN);

    // 2) Side‑menu (30% largeur)
    s_side_menu = lv_list_create(cont);
    lv_obj_set_size(s_side_menu, lv_pct(30), lv_pct(100));
    lv_obj_align(s_side_menu, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(s_side_menu, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_text_color(s_side_menu, lv_color_white(), LV_PART_MAIN);

    // 3) Zone de contenu (70% largeur)
    s_body = lv_obj_create(cont);
    lv_obj_set_size(s_body, lv_pct(70), lv_pct(100));
    lv_obj_align_to(s_body, s_side_menu, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_opa   (s_body, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(s_body, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_body, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_text_color(s_body, lv_color_white(), LV_PART_MAIN);

    // 4) Éléments du menu
    lv_obj_t* it_date = lv_list_add_btn(s_side_menu, NULL, "Date & Heure");
    lv_obj_add_event_cb(it_date, _show_page_date, LV_EVENT_CLICKED, NULL);

    for(int i = 1; i <= 4; ++i) {
        char buf[16];
        snprintf(buf, sizeof(buf), "Param%d", i);
        lv_obj_t* it = lv_list_add_btn(s_side_menu, NULL, buf);
        lv_obj_add_event_cb(it, _show_page_stub, LV_EVENT_CLICKED, NULL);
    }

    // 5) Page par défaut
    lv_event_send(it_date, LV_EVENT_CLICKED, NULL);
}
