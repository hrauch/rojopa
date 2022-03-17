/*
** screen_control.h
**
** Shows screens to get informations for the developing process
**
** Version 0.01 / 2010-04-23
** Hans Rauch, hans.rauch@gmx.net
** MIT-License ???
*/

#ifndef SCREEN_CONTROL_H
#define SCREEN_CONTROL_H

#include "gui.h"

void btn_event_back(lv_obj_t *btn, lv_event_t event);
void btn_event_step_00(lv_obj_t *btn, lv_event_t event);
void btn_event_step_100(lv_obj_t *btn, lv_event_t event);
void btn_event_step_06(lv_obj_t *btn, lv_event_t event);

void btn_event_company(lv_obj_t *btn, lv_event_t event);
void btn_event_film(lv_obj_t *btn, lv_event_t event);
void btn_event_developer(lv_obj_t *btn, lv_event_t event);
void slider_event_dev_time(lv_obj_t *slider, lv_event_t event);
void btn_event_dev_time(lv_obj_t *btn, lv_event_t event);
void slider_event_temperature(lv_obj_t *slider, lv_event_t event);
void btn_event_temperature(lv_obj_t *btn, lv_event_t event);

void build_buttons(char *header, char *steps, char **headers, int headers_size, lv_event_cb_t btn_event);

void show_error(void);
void create_settings(void);
void create_step_00(void);
void create_step_01(void);
void create_step_02(void);
void create_step_03(void);
void create_step_04(void);
void create_step_05(void);
void create_step_06(void);
void create_step_07(void);
void create_step_100(void);

#endif // SCREEN_CONTROL_H
