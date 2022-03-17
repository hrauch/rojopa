/*
** film_database.h
**
** Extracts informations from film database
**
** Version 0.01 / 2010-04-23
** Hans Rauch, hans.rauch@gmx.net
** MIT-License ???
*/

#ifndef FILM_DATABASE_H
#define FILM_DATABASE_H

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include "SD.h"

typedef struct
{
    char *company;
    char *film;
    char *developer;
    char *dilution;
    int temperature;
    char *dev_time;
    char *stop_time;
    char *fix_time;
} process_type;

int beginFileOps();
int openDb(const char *filename, sqlite3 **db);
int db_exec_string_list(sqlite3 *db, const char *sql);
int db_exec_rec(sqlite3 *db, const char *sql);

void free_str_list();
int init_str_list(char *sql_command, bool free_memory = true);

int init_companies();
int init_films(char * company_str);
int init_developers(char *company_str, char *film_str);

#endif // FILM_DATABASE_H
