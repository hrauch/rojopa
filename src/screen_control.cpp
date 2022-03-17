/*
** screen_control.cpp
**
** Shows screens to get informations for the developing process
**
** Version 0.01 / 2010-04-23
** Hans Rauch, hans.rauch@gmx.net
** MIT-License ???
*/

#include "screen_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Preferences.h>
#include <sqlite3.h>
#include "gui.h"
#include "dev_time.h"
#include "dev_motor.h"
#include "film_database.h"

extern GUI gui;
extern char error_str[60];
extern Preferences prefs;
extern lv_obj_t *slider_label_rotation;
extern lv_obj_t *slider_label_dev_time;
extern lv_obj_t *slider_label_temperature;
extern int duty_cycle;
extern int step;
extern bool do_select;
extern char company_str[20];
extern char film_str[20];
extern char developer_str[20];
extern int dev_time_no;
extern char dev_time_str[20];
extern int temperature_no;
extern char temperature_str[20];

extern sqlite3 *db_film_data;
extern char **str_list;
extern int str_list_size;
extern process_type process;

void btn_event_back(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        step -= 1;
        do_select = true;
    }
}

void slider_event_rotation(lv_obj_t *slider, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        static char buf[4];
        snprintf(buf, 4, "%u", lv_slider_get_value(slider));
        lv_label_set_text(slider_label_rotation, buf);
    }
}
void btn_event_rotation(lv_obj_t *btn, lv_event_t event)
{
    if ((event == LV_EVENT_CLICKED) || (event == LV_EVENT_SHORT_CLICKED))
    {
        prefs.begin("ropa", false);
        duty_cycle = atoi(lv_list_get_btn_text(btn));
        prefs.putInt("cycle", duty_cycle);
        prefs.end();
        step = 0;
        do_select = true;
    }
}

void btn_event_settings(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        step = 999;
        do_select = true;
    }
}
void btn_event_step_00(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        step = 1;
        do_select = true;
    }
}
void btn_event_step_100(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        step = 100;
        do_select = true;
    }
}
void btn_event_step_06(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        step = 7;
        do_select = true;
    }
}

void btn_event_company(lv_obj_t *btn, lv_event_t event)
{
    if ((event == LV_EVENT_CLICKED) || (event == LV_EVENT_SHORT_CLICKED))
    {
        strcpy(company_str, lv_list_get_btn_text(btn));
        step = 2;
        do_select = true;
    }
}
void btn_event_film(lv_obj_t *btn, lv_event_t event)
{
    if ((event == LV_EVENT_CLICKED) || (event == LV_EVENT_SHORT_CLICKED))
    {
        strcpy(film_str, lv_list_get_btn_text(btn));
        step = 3;
        do_select = true;
    }
}
void btn_event_developer(lv_obj_t *btn, lv_event_t event)
{
    if ((event == LV_EVENT_CLICKED) || (event == LV_EVENT_SHORT_CLICKED))
    {
        strcpy(developer_str, lv_list_get_btn_text(btn));
        step = 4;
        do_select = true;
    }
}

void slider_event_dev_time(lv_obj_t *slider, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        int push_pull = 0;
        int sec;
        int sec_new;
        char s[10];
        //char dev_time[10];

        push_pull = lv_slider_get_value(slider);
        sec = time_str_to_sec(process.dev_time);
        sec_new = sec;
        while (push_pull > 0)
        {
            sec_new *= 1.11;
            push_pull -= 1;
        }
        while (push_pull < 0)
        {
            sec_new *= 0.86;
            push_pull += 1;
        }
        lv_label_set_text(slider_label_dev_time, sec_to_time_str(sec_new, s));
    }
}
void btn_event_dev_time(lv_obj_t *btn, lv_event_t event)
{
    if ((event == LV_EVENT_CLICKED) || (event == LV_EVENT_SHORT_CLICKED))
    {
        strcpy(dev_time_str, lv_list_get_btn_text(btn));
        step = 5;
        do_select = true;
    }
}

void slider_event_temperature(lv_obj_t *slider, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        static char buf[4];
        snprintf(buf, 4, "%u", lv_slider_get_value(slider));
        lv_label_set_text(slider_label_temperature, buf);
    }
}
void btn_event_temperature(lv_obj_t *btn, lv_event_t event)
{
    if ((event == LV_EVENT_CLICKED) || (event == LV_EVENT_SHORT_CLICKED))
    {
        strcpy(temperature_str, lv_list_get_btn_text(btn));
        step = 6;
        do_select = true;
    }
}

void build_buttons(char *header, char *steps, char **headers, int headers_size, lv_event_cb_t btn_event)
{
    gui.clear();
    Header(header, false, 10, 5, LV_ALIGN_IN_TOP_LEFT);
    Header(steps, false, -5, 5, LV_ALIGN_IN_TOP_RIGHT);
    if (headers_size <= 6)
    {
        for (int i = 0; i < headers_size; i++)
        {
            Button(headers[i], btn_event, 90, 40, 10 + ((i % 3) * 105), 70 + (i / 3) * 50, (int)LV_ALIGN_IN_TOP_LEFT);
        }
    }
    else
    {
        List list = List(lv_disp_get_scr_act(NULL), btn_event);
        for (int i = 0; i < headers_size; i++)
        {
            list.add_btn(LV_SYMBOL_DIRECTORY, headers[i]);
        }
    }
}

void show_error(void)
{
    char text[180];
    char header[60] = "RoPa - Filmentwicklung";
    gui.clear();
    Header(header, false, 10, 5, LV_ALIGN_IN_TOP_LEFT);
    sprintf(text, "Es trat ein Fehler auf:\n\n%s", error_str);
    Text(text, gui.width() - 20, 10, -20);
}

void create_settings(void)
{
    char s[10];
    char text[180];
    char header[60] = "RoPa - Filmentwicklung";
    gui.clear();
    Header(header, false, 10, 5, LV_ALIGN_IN_TOP_LEFT);
    strcpy(text, "Bitte passen Sie die Rotations-\ngeschwindigkeit Ihre vorhandene \nSpannungsquelle an:");
    Text(text, gui.width() - 20, 10, -50);
    Slider(lv_scr_act(), slider_event_rotation, 155, 255, duty_cycle,
           gui.width() - 50, 30, 0, 10, LV_ALIGN_CENTER);
    strcpy(s, itoa(duty_cycle, s, 10));
    Button btn = Button(s, btn_event_rotation, 90, 35, 0, -20, LV_ALIGN_IN_BOTTOM_MID);
    slider_label_rotation = btn.label;
}

void create_step_00(void)
{
    char text[180];
    char header[60] = "RoPa - Filmentwicklung";
    strcpy(film_str, "");
    strcpy(developer_str, "");
    gui.clear();
    Header(header, false, 10, 5, LV_ALIGN_IN_TOP_LEFT);
    strcpy(text, "Diese Steuersoftware rotiert Paterson-\nFilmentwicklungsdosen abwechselnd nach links und rechts -\n\nmit zufallsgesteuerten Zeiten.");
    Text(text, gui.width() - 20, 10, -20);
    Button("30min Rotation", btn_event_step_100, 130, 35, 5, 25, LV_ALIGN_IN_BOTTOM_LEFT);
    Button btn = Button("X", btn_event_settings, 40, 35, 30, 25, LV_ALIGN_IN_BOTTOM_MID);
    Button("E-Prozess", btn_event_step_00, 130, 35, -34, 25, LV_ALIGN_IN_BOTTOM_RIGHT);
    lv_label_set_text(btn.label, LV_SYMBOL_SETTINGS);
}

void create_step_01(void)
{
    char header[60] = "RoPa: Welcher Hersteller?";
    char steps[5] = "1/5";
    init_companies();

    build_buttons(header, steps, str_list, str_list_size, btn_event_company);
}

void create_step_02(void)
{
    char header[60] = "RoPa: Welcher Film?";
    char steps[5] = "2/5";
    init_films(company_str);

    build_buttons(header, steps, str_list, str_list_size, btn_event_film);
    strcpy(header, company_str);
    Text(header, 300, 10, -70);
}

void create_step_03(void)
{
    char header[60] = "RoPa: Welcher Entwickler?";
    char steps[5] = "3/5";
    init_developers(company_str, film_str);

    build_buttons(header, steps, str_list, str_list_size, btn_event_developer);
    sprintf(header, "%s / %s", company_str, film_str);
    Text(header, 300, 10, -70);
}

void create_step_04(void)
{
    char header[60] = "RoPa: Pull/Push?";
    char steps[5] = "4/5";
    char sql_command[120];

    gui.clear();
    Header(header, false, 10, 5, LV_ALIGN_IN_TOP_LEFT);
    Header(steps, false, -5, 5, LV_ALIGN_IN_TOP_RIGHT);
    sprintf(sql_command, "Select * from processes where company='%s' and film='%s' and developer='%s'", company_str, film_str, developer_str);

    free_str_list();
    db_exec_rec(db_film_data, sql_command);
    /*
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db_film_data);
    }
    */
    sprintf(header, "%s - %s / %s: %s %iG", process.company, process.film, process.developer, process.dev_time, process.temperature);
    Text(header, 300, 10, -70);

    Slider(lv_scr_act(), slider_event_dev_time, -2, 2, 0,
                           gui.width() - 50, 30, 0, 0, LV_ALIGN_CENTER);
    Button btn = Button(process.dev_time, btn_event_dev_time, 90, 35, 0, -20, LV_ALIGN_IN_BOTTOM_MID);
    slider_label_dev_time = btn.label;
}

void create_step_05(void)
{
    char header[60] = "RoPa: Welche Temperatur?";
    char steps[5] = "5/5";
    char s[5];

    gui.clear();
    Header(header, false, 10, 5, LV_ALIGN_IN_TOP_LEFT);
    Header(steps, false, -5, 5, LV_ALIGN_IN_TOP_RIGHT);
    sprintf(header, "%s - %s / %s: %s %iG", process.company, process.film, process.developer, dev_time_str, process.temperature);
    Text(header, 300, 10, -70);

    Slider(lv_scr_act(), slider_event_temperature, process.temperature - 4, process.temperature + 4, process.temperature,
                           gui.width() - 50, 30, 0, 0, LV_ALIGN_CENTER);
    __itoa(process.temperature, s, 10);
    Button btn = Button(s, btn_event_temperature, 90, 35, 0, -20, LV_ALIGN_IN_BOTTOM_MID);
    slider_label_temperature = btn.label;
}

void create_step_06(void)
{
    char header[60] = "RoPa: Entwicklung starten";
    int sec = time_str_to_sec(dev_time_str);
    int extra_temp = atoi(temperature_str) - process.temperature;
    while (extra_temp > 0)
    {
        sec *= 0.9;
        extra_temp -= 1;
    }
    while (extra_temp < 0)
    {
        sec *= 1.11;
        extra_temp += 1;
    }
    strcpy(dev_time_str, sec_to_time_str(sec, (char *)dev_time_str));
    gui.clear();
    Header(header, false, 10, 5, LV_ALIGN_IN_TOP_LEFT);
    sprintf(header, "%s - %s / %s: %s %sG", process.company, process.film, process.developer, dev_time_str, temperature_str);
    Text(header, 300, 10, -70);

    Button("Starten ...", btn_event_step_06, 100, 35, 0, 30, LV_ALIGN_CENTER);
}

void create_step_07(void)
{
    do_develop(time_str_to_sec(dev_time_str), 60, time_str_to_sec(process.fix_time));
    do_select = true;
    step = 0;
}

void create_step_100(void)
{
    do_rotate(30*60);
    do_select = true;
    step = 0;
}
