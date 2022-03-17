/*
** dev_motor.h
**
** Controls motor for rotating
**
** Version 0.01 / 2010-04-26
** Hans Rauch, hans.rauch@gmx.net
** MIT-License ???
*/

#include <Arduino.h>
#include "gui.h"
#include "dev_motor.h"
#include "dev_time.h"
#include "film_database.h"

extern GUI gui;
extern int t_total;
extern int t_rest;
extern process_type process;
extern char dev_time_str[20];
extern char temperature_str[20];
extern int duty_cycle;

const int LEFT = 0;
const int STOP = 1;
const int RIGHT = 2;

// Motor 1
/*
int GSM1 = 10;
int in1 = 9;
int in2 = 8;
*/
// Motor 2

int GSM1 = 17;
int in3 = 32;
int in4 = 33;
const int channel = 0;

// Rotate motor for <milli_secs>
void motor(int direction, int milli_secs)
{
    ledcAttachPin(GSM1, channel);
    ledcWrite(channel, duty_cycle);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    switch (direction)
    {
    case LEFT:
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
        break;
    case STOP:
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        break;
    case RIGHT:
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
        break;
    }
    delay(milli_secs);
}

void show_rest_time()
{
    char time_str[10];
    char rest_str[20];

    sprintf(rest_str, "Rest %s", sec_to_time_str(t_rest, time_str));
    Header(rest_str, false, 0, 0, LV_ALIGN_CENTER);
    gui.update();
}

// Turn motor on for 10 seconds: left, stop, turn right, stop
void do_turn(int rest)
{
    int diff = 0;
    t_total = rest;
    t_rest = rest;
    ledcSetup(channel, 2000, 8);
    while (t_rest > 0)
    {
        show_rest_time();
        if (t_rest < 10)
        {
            motor(LEFT, (1000 * t_rest) / 2);
            motor(STOP, 500);
            motor(RIGHT, (1000 * t_rest) / 2);
            motor(STOP, 500);
        }
        else
        {
            diff = rand() % 3000;
            if (diff < 5)
            {
                diff = rest;
            }
            motor(LEFT, 4500 - diff);
            motor(STOP, 500);
            motor(RIGHT, 4500 + diff);
            motor(STOP, 500);
        }
    }
}

void show_head_of_process(char * header)
{
    gui.clear();
    Header("RoPa - Filmentwicklung", false, 10, 5, LV_ALIGN_IN_TOP_LEFT);
    Header(header, false, 10, -80, LV_ALIGN_IN_LEFT_MID);
}

// Motors stop for changing the chemicals
void do_change_chemicals()
{
    char time_str[20];
    int time_rest = 10;
    Header("[Leerung]", false, 200, -80, LV_ALIGN_IN_LEFT_MID);
    t_rest = -1;
    while (time_rest > 0)
    {
        //itoa(time_rest, time_str, 10);
        sprintf(time_str, "        %i        ", time_rest);
        Header(time_str, false, 0, 0, LV_ALIGN_CENTER);
        time_rest -= 1;
        gui.update();
        delay(1000);
    }
}

// This routine controls the complete process
void do_develop(int dev_secs, int stop_secs, int fix_secs)
{
    char info[60];
    int dev_rest;
    int stop_rest;
    int fix_rest;

    dev_rest = dev_secs;
    stop_rest = stop_secs;
    fix_rest = fix_secs;
    show_head_of_process((char *)"Entwicklung");
    sprintf(info, "%s - %s / %s: %s %sG", process.company, process.film, process.developer, dev_time_str, temperature_str);
    Text(info, 300, 10, -50);
    // Time to init screen
    do_turn(dev_rest);
    do_change_chemicals();
    show_head_of_process((char *)"Stoppbad");
    do_turn(stop_rest);
    do_change_chemicals();
    show_head_of_process((char *)"Fixierbad");
    do_turn(fix_rest);
    do_change_chemicals();
    show_head_of_process((char *)"1. Waesserung");
    do_turn(30);
    do_change_chemicals();
    show_head_of_process((char *)"2. Waesserung");
    do_turn(60);
    do_change_chemicals();
    show_head_of_process((char *)"3. Waesserung");
    do_turn(120);
    do_change_chemicals();
}

// This routine rotates the tank (for 30m)
void do_rotate(int secs)
{
    show_head_of_process((char *)"Dauerrotation");
    // Time to init screen
    do_turn(secs);
}
