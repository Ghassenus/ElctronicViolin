// lvgl_helpers.h
#pragma once
#include <lvgl.h>

void lvgl_setup();
// Lit le touchpad pour LVGL
void my_touchpad_read(lv_indev_drv_t* drv, lv_indev_data_t* data);