/*
** dev_motor.h
**
** Controls motor for rotating
**
** Version 0.01 / 2010-04-26
** Hans Rauch, hans.rauch@gmx.net
** MIT-License ???
*/

#ifndef DEV_MOTOR_H
#define DEV_MOTOR_H

void motor(int direction, int milli_secs);
void do_turn(int rest);
void do_change_chemicals();
void do_develop(int dev_secs, int stop_secs, int fix_secs);
void do_rotate(int secs);

void show_rest_time();

#endif
