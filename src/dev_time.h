/*
** dev_time.h
**
** Converts strings in seconds and vice versa
**
** Version 0.01 / 2010-04-23
** Hans Rauch, hans.rauch@gmx.net
** MIT-License ???
*/

#ifndef DEV_TIME_H
#define DEV_TIME_H

int time_str_to_sec(char *t);
char *sec_to_time_str(int s, char retval[10]);

#endif
