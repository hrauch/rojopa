/*
** setup.cpp
**
** 1. Opens the film database
**
** Version 0.01 / 2010-04-23
** Hans Rauch, hans.rauch@gmx.net
** MIT-License ???
*/


#include <Arduino.h>
#include <sqlite3.h>
#include <Preferences.h>
#include "gui.h"
#include "film_database.h"
#include "dev_motor.h"


extern GUI gui;
extern sqlite3 *db_film_data;
extern Preferences prefs;
extern int duty_cycle;
hw_timer_t *timer = NULL;
hw_timer_t *timer_beep = NULL;
extern int t_rest;
extern int t_beeper;

int channel = 10;
int pin_buzzer = 16;

// Beeper to signal the end of current subproess
void IRAM_ATTR do_beep()
{
    ledcAttachPin(pin_buzzer, channel);
    ledcWriteTone(channel, 250);
}

// Stopp beeper
void IRAM_ATTR dont_beep()
{
    if (t_beeper > 0)
    {
        t_beeper -= 1;
        if (t_beeper == 0)
        {
            ledcDetachPin(pin_buzzer);
        }
    }
}

// This interrupt routine will be called every second and calls the beeper
//void IRAM_ATTR
void IRAM_ATTR check_time()
{
    if (t_rest > 0)
    {
        if (t_rest % 5 == 0)
        {
            show_rest_time();
        }
        t_rest -= 1;
        if (t_rest < 10)
        {
            t_beeper = (10 - t_rest) * 2;
            do_beep();
        }
    }
}

void setup()
{
    Serial.begin(9600);
    gui.init();
    if (! beginFileOps()) {
        return;
    }
    int rc;
    rc = openDb("/sd/film_data.db", &db_film_data);
    if (rc != SQLITE_OK) 
    { 
        return; 
    }

    // http://www.iotsharing.com/2017/06/how-to-use-interrupt-timer-in-arduino-esp32.html
    // 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up
    timer = timerBegin(0, 80, true);
    timer_beep = timerBegin(1, 80, true);
    timerAttachInterrupt(timer, &check_time, true);
    timerAttachInterrupt(timer_beep, &dont_beep, true);
    // Set alarm to call onTimer function every second 1 tick is 1us => 1 second is 1000000us
    timerAlarmWrite(timer, 1000000, true);
    timerAlarmWrite(timer_beep, 10000, true);
    timerAlarmEnable(timer);
    timerAlarmEnable(timer_beep);
    ledcSetup(channel, 2000, 8);

    prefs.begin("ropa", false);
    duty_cycle = prefs.getInt("cycle", 200);
    if (prefs.getBytesLength("cycle") == 0)
    {
        prefs.putInt("cycle", duty_cycle);
    }
    prefs.end();
}
