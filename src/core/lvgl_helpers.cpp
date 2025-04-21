#include "lvgl_helpers.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

#define TOUCH_CS   33
#define TOUCH_IRQ  36
#define TOUCH_SCLK 25
#define TOUCH_MISO 39
#define TOUCH_MOSI 32

extern TFT_eSPI tft;

// Le driver XPT2046 sur votre bus SPI dédié
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

// --- FLUSH DISPLAY ------------------------------------------------
static void my_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t*)color_p, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

// --- LECTURE TOUCHPAD ---------------------------------------------
void my_touchpad_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
  if(!ts.touched()) {
    data->state = LV_INDEV_STATE_REL;
    return;
  }

  TS_Point p = ts.getPoint();
  //Serial.printf("RAW Touch: x=%u y=%u z=%u\n", p.x, p.y, p.z);

  // Vos valeurs de calibration
  const uint16_t raw_x_min =  361;
  const uint16_t raw_x_max = 3685;
  const uint16_t raw_y_min =  482;
  const uint16_t raw_y_max = 3686;

  data->state = LV_INDEV_STATE_PR;
  data->point.x = constrain(
    map(p.x, raw_x_min, raw_x_max, 0, tft.width()  - 1),
    0, tft.width()  - 1
  );
  data->point.y = constrain(
    map(p.y, raw_y_min, raw_y_max, 0, tft.height() - 1),
    0, tft.height() - 1
  );
}

// --- INITIALISATION LVGL ------------------------------------------
void lvgl_setup() {
  // 1) Buffer LVGL
  static lv_disp_draw_buf_t draw_buf;
  static lv_color_t buf[240 * 10];
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, sizeof(buf)/sizeof(lv_color_t));

  // 2) Driver écran
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = tft.width();
  disp_drv.ver_res = tft.height();
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // 3) Bus SPI tactile
  pinMode(TOUCH_IRQ, INPUT_PULLUP);
  SPI.begin(TOUCH_SCLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
  ts.begin();
  ts.setRotation(1);

  // 4) Driver entrée LVGL
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type    = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
}