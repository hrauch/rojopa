/** @file gui.h
 *  A header file defining classes of the tiny C++ lvgl wrapper.
 *  Only simple Label, QR and Button classes are implemented.
 *  
 *  https://raw.githubusercontent.com/stepansnigirev/tiny_lvgl_gui/master/src/gui.h
 * 
 *
 *  Example:
 *  @code
 *  #include <mbed.h>
 *  #include <gui.h>
 *
 *  GUI gui;     // our GUI instance
 *  Label lbl;   // label in the global scope, we will change text from the button callback
 *  int cnt = 0; // click counter
 *
 *  // button callback, default lvgl type
 *  static lv_res_t callback(lv_obj_t * btn){
 *    // Button button(btn); // can be converted to C++ class
 *    cnt++;
 *    char msg[40];
 *    sprintf(msg, "Button clicked %d times!", cnt);
 *    lbl.text(msg);
 *    return LV_RES_OK;
 *  }
 *
 *  int main() {
 *
 *    gui.init();
 *
 *    // Create a label to log clicks
 *    lbl = Label("Hello display!");
 *    lbl.size(gui.width(), 100); // full width
 *    lbl.position(0, 200);
 *    lbl.alignText(ALIGN_TEXT_CENTER);
 *
 *    // Create a button
 *    Button btn(callback, "Click me!");
 *    btn.size(300, 100);
 *    btn.position(0, 300);
 *    btn.align(ALIGN_CENTER);
 *
 *    while(1) {
 *      gui.update();
 *    }
 *  }
 *  @endcode
 */

#ifndef LVGL_GUI_H
#define LVGL_GUI_H

#include <lvgl.h>
#include <string>

#define ALIGN_CENTER LV_ALIGN_IN_TOP_MID
#define ALIGN_LEFT   LV_ALIGN_IN_TOP_LEFT
#define ALIGN_RIGHT  LV_ALIGN_IN_TOP_RIGHT

#define ALIGN_TEXT_CENTER LV_LABEL_ALIGN_CENTER
#define ALIGN_TEXT_RIGHT  LV_LABEL_ALIGN_RIGHT
#define ALIGN_TEXT_LEFT   LV_LABEL_ALIGN_LEFT

//static void lv_tick_handler(void);

class Label;
class Button;

/** GUI class.
 *  Used to initialize the littlevgl library, the display and the touchscreen.
 */
class GUI{
public:
    /** Initialize and register the drivers. Should be called first in the very beginning */
    void init(int w = 320, int h = 240);
    /** Update the GUI. Call this function in a loop to get button callbacks and UI updates. */
    void update();
    /** Get the width of the screen. */
    uint16_t width();
    /** Get the height of the screen. */
    uint16_t height();
    /** Clear the screen and delete all elements */
    void clear(); // Clear the display. All components will be deleted.
private:
    int screenWidth;
    int screenHeight;
};

class GUIObject{
public:
    lv_obj_t * obj;
    void lv_object(lv_obj_t * o);
    lv_obj_t * lv_object();
    void size(uint16_t width, uint16_t height);
    void position(uint16_t width, uint16_t height);
    void align(int align_mode = ALIGN_CENTER);
    void align(int align_mode=LV_ALIGN_IN_LEFT_MID, int x_mod=0, int y_mod=0, lv_obj_t *refObj=NULL);
};

/** Label class. Creates a simple label on the screen.
 */
class Label : public GUIObject{
public:
    /** Create a label from lvgl object */
    Label(lv_obj_t * lbl, bool is_long_text=false, int w=320);
    /** Create a label with text `txt` */
    Label(const char * txt, bool is_long_text=false, int w=320);
    /** Create a label with text `txt` */
    Label(std::string txt, bool is_long_text=false, int w=320);
    /** Set text of the label to `txt` */
    void text(const char * txt);
    /** Set text of the label to `txt` */
    void text(std::string txt);
    /** Change font and height of font */
    void setFont(lv_font_t *font);
};


/** Header class. Creates a simple header on the screen.
 */
class Header : public Label{
public:
    /** Create a header from lvgl object */
    Header(lv_obj_t * lbl, bool is_big = true, int x_mod=0, int y_mod=0, int align_mode=LV_ALIGN_IN_TOP_MID);
    /** Create a header with text `txt` */
    Header(const char * txt, bool is_big = true, int x_mod=0, int y_mod=0, int align_mode=LV_ALIGN_IN_TOP_MID);
    /** Create a header with text `txt` */
    Header(std::string txt, bool is_big = true, int x_mod=0, int y_mod=0, int align_mode=LV_ALIGN_IN_TOP_MID);
private:
    void changeFont(bool is_big, int x_mod, int y_mod, int align);
};


/** Text class. Creates a simple text on the screen.
 */
class Text : public Label{
public:
    /** Create a header from lvgl object */
    Text(lv_obj_t * lbl, int w=320, int x_mod=0, int y_mod=0, int align_mode=LV_ALIGN_IN_LEFT_MID);
    /** Create a header with text `txt` */
    Text(const char * txt, int w=320, int x_mod=0, int y_mod=0, int align_mode=LV_ALIGN_IN_LEFT_MID);
    /** Create a header with text `txt` */
    Text(std::string txt, int w=320, int x_mod=0, int y_mod=0, int align_mode=LV_ALIGN_IN_LEFT_MID);
};


/** Button class. Creates a simple button on the screen.
 */
class Button : public GUIObject{
public:
    lv_obj_t *label;

    /** Create a button from lvgl object */
    Button(lv_obj_t * lbl, lv_event_cb_t event_cb=NULL, int w=100, int h=40, int x_mod=0, int y_mod=0, int align_mode=LV_ALIGN_IN_BOTTOM_MID);
    /** Create a label with text `txt` */
    Button(const char * txt, lv_event_cb_t event_cb=NULL, int w=100, int h=40, int x_mod=0, int y_mod=0, int align_mode=LV_ALIGN_IN_BOTTOM_MID);
    /** Create a button with text `txt` */
    Button(std::string txt, lv_event_cb_t event_cb=NULL, int w=100, int h=40, int x_mod=0, int y_mod=0, int align_mode=LV_ALIGN_IN_BOTTOM_MID);
};

/** List class. Creates a simple list of button on the screen.
 */
class List : public GUIObject
{
public:
    lv_obj_t *list;
    lv_event_cb_t event;

    /** Create a list button from lvgl object */
    List(lv_obj_t *parent, lv_event_cb_t event_cb, int w = 180, int h = 160, int x_mod = 0, int y_mod = 70, int align_mode = LV_ALIGN_IN_TOP_MID);

    void add_btn(const void *img_src, const char *txt, bool is_selected=false);
};

/** Slider class. Creates a simple slider on the screen.
 */
class Slider : public GUIObject
{
public:
    lv_obj_t *slider;

    /** Create a list button from lvgl object */
    Slider(lv_obj_t *parent, lv_event_cb_t event_cb, int min, int max, int value, 
                    int w = 280, int h = 30, int x_mod = 0, int y_mod = 70, int align_mode = LV_ALIGN_CENTER);
};

#endif // LVGL_GUI_H
