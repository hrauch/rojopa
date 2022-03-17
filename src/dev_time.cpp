/*
** dev_time.cpp
**
** Converts strings in seconds and vice versa
**
** Version 0.01 / 2010-04-23
** Hans Rauch, hans.rauch@gmx.net
** MIT-License ???
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dev_time.h>

// Converts time in seconds
int time_str_to_sec(char *t)
{
    int mins;
    int secs;
    char my_time[10];
    char *ptr;

    strcpy(my_time, t);
    ptr = strtok(my_time, ":");
    mins = atoi(ptr);
    ptr = strtok(NULL, ":");
    secs = atoi(ptr);
    return 60 * mins + secs;
}

// Converts seconds in minutes:seconds
char *sec_to_time_str(int s, char retval[10])
{
    int mins;
    int secs;
    mins = s / 60;
    secs = s % 60;
    snprintf(retval, 10, "%02d:%02d", mins, secs);
    return retval;
}

