/*
 * https://raw.githubusercontent.com/stepansnigirev/tiny_lvgl_gui/master/src/gui.cpp
 */
 
#include "gui.h"
#include <lvgl.h>
#include <Ticker.h>
#include <TFT_eSPI.h>

extern GUI gui;
TFT_eSPI tft;
Ticker tick; /* timer for interrupt handler */
extern int LVGL_TICK_PERIOD;
//extern void lv_tick_handler(void);

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

/* timer to count time in a loop and update the lvgl */
static volatile int t = 0;
//Ticker ms_tick;
/*
static void onMillisecondTicker(void){
  t++;
}
*/

LV_FONT_DECLARE(square1);
LV_FONT_DECLARE(square2);
LV_FONT_DECLARE(square3);
LV_FONT_DECLARE(square4);
LV_FONT_DECLARE(square5);
LV_FONT_DECLARE(square6);
LV_FONT_DECLARE(square7);
LV_FONT_DECLARE(square8);


/* Interrupt driven periodic handler */
static void lv_tick_handler(void)
{
  lv_tick_inc(LVGL_TICK_PERIOD);
}


/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint16_t c;

  tft.startWrite(); /* Start new TFT transaction */
  tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      tft.writeColor(c, 1);
      color_p++;
    }
  }
  tft.endWrite(); /* terminate TFT transaction */
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    uint16_t touchX, touchY;

    bool touched = tft.getTouch(&touchX, &touchY, 600);
    if(!touched)
    {
      return false;
    }
    if(touchX>gui.width() || touchY > gui.height())
    {
      Serial.println("Y or y outside of expected parameters..");
      Serial.print("y:");
      Serial.print(touchX);
      Serial.print(" x:");
      Serial.print(touchY);
    }
    else
    {
      data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 
      /*Save the state and save the pressed coordinate*/
      //if(data->state == LV_INDEV_STATE_PR) touchpad_get_xy(&last_x, &last_y);
      /*Set the coordinates (if released use the last pressed coordinates)*/
      data->point.x = touchX;
      data->point.y = touchY;
      Serial.print("Data x");
      Serial.println(touchX);
      Serial.print("Data y");
      Serial.println(touchY);
    }
    return false; /*Return `false` because we are not buffering and no more data to read*/
}

/********* GUI class *********/

void GUI::init(int w, int h){
    
    screenWidth = w;
    screenHeight = h;
    
    /* init hardware */
    lv_init();
    tft.begin();

    /* calibrate touchpad - depends on orientation */
    tft.setRotation(1);
    uint16_t calData[5] = { 222, 3435, 287, 3522, 1 };
    tft.setTouch(calData);

    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

    /*Initialize the display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
    lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

    tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);
  
    //Set the theme..
    // ..night ..alien ..material ..zen ..mono ..default
    lv_theme_t * th = lv_theme_material_init(210, NULL);
    lv_theme_set_current(th);

    lv_obj_t * scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);
}
void GUI::update(){
    lv_task_handler();
    delay(50);
}
uint16_t GUI::width(){ return screenWidth; }
uint16_t GUI::height(){ return screenHeight; }

void GUI::clear(){
  lv_obj_clean(lv_scr_act());
}

/********* GUIObject *********/

void GUIObject::size(uint16_t width, uint16_t height){
  if(obj == NULL){ return; }
  lv_obj_set_size(obj, width, height);
}
void GUIObject::position(uint16_t x, uint16_t y){
  if(obj == NULL){ return; }
  lv_obj_set_pos(obj, x, y);
}
void GUIObject::lv_object(lv_obj_t * o){
  obj = o;
}
lv_obj_t * GUIObject::lv_object(){
  return obj;
}
void GUIObject::align(int align){
  lv_obj_align(obj, NULL, align, 0, lv_obj_get_y(obj));
}
void GUIObject::align(int align, int x_mod, int y_mod, lv_obj_t *refObj){
    if(obj == NULL){ return; }
    lv_obj_align(obj, refObj, align, x_mod, y_mod);
}

/********* Label class *********/

Label::Label(const char * txt, bool is_long_text, int w){
    obj = lv_label_create(lv_scr_act(), NULL);
    if (is_long_text) { lv_label_set_long_mode(obj, LV_LABEL_LONG_BREAK); lv_obj_set_width(obj, w); }
    lv_label_set_text(obj, txt);
    lv_label_set_body_draw(obj, true);
}
Label::Label(std::string txt, bool is_long_text, int w){
    obj = lv_label_create(lv_scr_act(), NULL);
    if (is_long_text) { lv_label_set_long_mode(obj, LV_LABEL_LONG_BREAK); lv_obj_set_width(obj, w); }
    lv_label_set_text(obj, txt.c_str());
    lv_label_set_body_draw(obj, true);
}
Label::Label(lv_obj_t * lbl, bool is_long_text, int w){
    obj = lbl;
}
void Label::text(const char * txt){
    if(obj == NULL){ return; }
    lv_label_set_text(obj, txt);
}
void Label::text(std::string txt){
    text(txt.c_str());
}
void Label::setFont(lv_font_t *font)
{
    if(obj == NULL){ return; }
    static lv_style_t my_style;
    lv_style_copy(&my_style, lv_label_get_style(obj, LV_LABEL_STYLE_MAIN));
    my_style.text.font = font;
    lv_label_set_style(obj, LV_LABEL_STYLE_MAIN, &my_style);
    
}

/********* Header class *********/

void Header::changeFont(bool is_big, int x_mod, int y_mod, int align_mode){
    if (is_big) {
        setFont(&lv_font_roboto_28);
    }
    else {
        setFont(&lv_font_roboto_22);
    }
    align(align_mode, x_mod, y_mod);
}

Header::Header(const char * txt, bool is_big, int x_mod, int y_mod, int align_mode): Label(txt) {
    changeFont(is_big, x_mod, y_mod, align_mode);
}
Header::Header(std::string txt, bool is_big, int x_mod, int y_mod, int align_mode): Label(txt) {
    changeFont(is_big, x_mod, y_mod, align_mode);
}
Header::Header(lv_obj_t * lbl, bool is_big, int x_mod, int y_mod, int align_mode): Label(lbl) {
    changeFont(is_big, x_mod, y_mod, align_mode);
}

/********* Text class *********/

Text::Text(const char * txt, int w, int x_mod, int y_mod, int align_mode): Label(txt, true, w) {
    align(align_mode, x_mod, y_mod);
}
Text::Text(std::string txt, int w, int x_mod, int y_mod, int align_mode): Label(txt, true, w) {
    align(align_mode, x_mod, y_mod);
}
Text::Text(lv_obj_t * lbl, int w, int x_mod, int y_mod, int align_mode): Label(lbl, true, w) {
    align(align_mode, x_mod, y_mod);
}

/********* Button class *********/

Button::Button(const char * txt, lv_event_cb_t event_cb, int w, int h, int x_mod, int y_mod, int align_mode){
    obj = lv_btn_create(lv_scr_act(), NULL); //Add a button the current screen
    lv_obj_align(obj, NULL, align_mode, x_mod, y_mod);
    lv_obj_set_size(obj, w, h); //Set its size
    lv_obj_set_event_cb(obj, event_cb); //Assign a callback to the button
    label = lv_label_create(obj, NULL); //Add a label to the button
    lv_label_set_text(label, txt); //Set the labels text
}
Button::Button(std::string txt, lv_event_cb_t event_cb, int w, int h, int x_mod, int y_mod, int align_mode){
}
Button::Button(lv_obj_t * lbl, lv_event_cb_t event_cb, int w, int h, int x_mod, int y_mod, int align_mode){
    obj = lbl;
}

/********* List of button class *********/

List::List(lv_obj_t *parent, lv_event_cb_t event_cb, int w, int h, int x_mod, int y_mod, int align_mode)
{
  event = event_cb;
  list = lv_list_create(parent, NULL);
  lv_obj_set_height(list, h);
  lv_obj_set_width(list, w);
  lv_obj_align(list, NULL, align_mode, x_mod, y_mod);
  lv_list_clean(list);
}

void List::add_btn(const void *img_src, const char *txt, bool is_selected)
{
    lv_obj_t *list_btn;
    list_btn = lv_list_add_btn(list, img_src, txt);
    lv_obj_set_event_cb(list_btn, event);
    if (is_selected) {
        lv_list_set_btn_selected(list, list_btn);
    }
}

Slider::Slider(lv_obj_t *parent, lv_event_cb_t event_cb, int min, int max, int value,
     int w, int h, int x_mod, int y_mod, int align_mode)
{
  lv_obj_t *slider = lv_slider_create(parent, NULL);
  lv_obj_set_width(slider, w);
  lv_obj_set_height(slider, h);
  lv_obj_align(slider, NULL, align_mode, x_mod, y_mod);
  lv_slider_set_range(slider, min, max);
  lv_slider_set_value(slider, value, LV_ANIM_ON);
  lv_obj_set_event_cb(slider, event_cb);
};
