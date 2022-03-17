/*
** loop.cpp
**
** 1. Show and control the screens
**
** Version 0.01 / 2010-04-23
** Hans Rauch, hans.rauch@gmx.net
** MIT-License ???
*/

#include <Arduino.h>
#include "screen_control.h"

extern GUI gui;
extern int step;
extern bool do_select;
extern char error_str[60];

void loop()
{
    if (do_select)
    {
        do_select = false;
        if (error_str[0] != '\0')
            show_error();
        else
        {
            switch (step)
            {
            case 1:
                create_step_01();
                break;
            case 2:
                create_step_02();
                break;
            case 3:
                create_step_03();
                break;
            case 4:
                create_step_04();
                break;
            case 5:
                create_step_05();
                break;
            case 6:
                create_step_06();
                break;
            case 7:
                create_step_07();
                break;
            case 100:
                create_step_100();
                break;
            case 999:
                create_settings();
                break;
            default:
                step = 0;
                create_step_00();
                break;
            };
            if (step > 0)
            {
                Button("<<<", btn_event_back, 50, 35, 10, 25, LV_ALIGN_IN_BOTTOM_LEFT);
            }
        }
    }
    gui.update();
}
