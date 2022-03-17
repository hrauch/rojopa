/*
** RoPa_esp32.cpp
**
** This program controls the rotation of a Paterson development-tank.
** The machine is 3D-printed and uses Lego elements.
**
** Version 0.01 / 2010-04-23
** Hans Rauch, hans.rauch@gmx.net
** MIT-License ???
*/

#include <lvgl.h>
#include <sqlite3.h>
#include <Preferences.h>
#include "gui.h"
#include "film_database.h"

/**********************
 *  STATIC VARIABLES
 **********************/

GUI gui;
sqlite3 *db_film_data;
Preferences prefs;

// The neatest way to access variables stored in EEPROM is using a structure
int cycle = 230;

int t_total = -1;
int t_rest = -1;
int t_beeper = -1;
int duty_cycle = 210;
int step;
bool do_select = true;
char company_str[20];
char film_str[20];
char developer_str[20];
int dev_time_no;
char dev_time_str[20];
int temperature_no;
char temperature_str[20];
char error_str[60];

char **str_list = NULL;
int str_list_size = 0;
process_type process;
lv_obj_t *slider_label_rotation;
lv_obj_t *slider_label_dev_time;
lv_obj_t *slider_label_temperature;

int LVGL_TICK_PERIOD = 60;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

