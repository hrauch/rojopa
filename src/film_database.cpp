/*
** film_database.cpp
**
** Extracts informations from film database
**
** Version 0.01 / 2010-04-23
** Hans Rauch, hans.rauch@gmx.net
** MIT-License ???
*/

#include "film_database.h"

extern sqlite3 *db_film_data;
extern char error_str[60];
extern char **str_list;
extern int str_list_size;
extern process_type process;
char *zErrMsg = 0;

int beginFileOps()
{
    // Set all chip selects high to avoid bus contention during initialisation of each peripheral
    digitalWrite(22, HIGH); // Touch controller chip select (if used)
    digitalWrite(15, HIGH); // TFT screen chip select
    digitalWrite(5, HIGH);  // SD card chips select, must use GPIO 5 (ESP32 SS)

    SPI.begin();
    if (!SD.begin())
    {
        strcpy(error_str, "SD-Slot/SD-Karte defekt oder fehlt");
        return false;
    }
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        strcpy(error_str, "Falsche Formatierung der SD-Karte");
        return false;
    }
    if (sqlite3_initialize() != SQLITE_OK)
    {
        strcpy(error_str, "Sqlite konnte nicht initialisiert werden");
        return false;
    }
    return true;
}

int openDb(const char *filename, sqlite3 **db)
{
    int rc = sqlite3_open(filename, db);
    if (rc) {
        sprintf(error_str, "Datenbank ist nicht offen : %s", sqlite3_errmsg(*db));
    }
    return rc;
}

static int callback_string_list(void *data, int argc, char **argv, char **azColName)
{
    str_list = (char **)realloc(str_list, (str_list_size + 1) * sizeof(char *));
    str_list[str_list_size] = strdup(argv[0]);
    str_list_size++;
    return 0;
}

static int callback_rec(void *data, int argc, char **argv, char **azColName)
{
   for ( int i = 0; i<argc; i++ ){
       switch (i) {
       case 0:
           process.company = strdup(argv[i]);
           break;
       case 1:
           process.film = strdup(argv[i]);
           break;
       case 2:
           process.developer = strdup(argv[i]);
           break;
       case 3:
           process.dilution = argv[i];
           break;
       case 4:
           process.temperature = atoi(argv[i]);
           break;
       case 5:
           process.dev_time = strdup(argv[i]);
           break;
       case 6:
           process.stop_time = strdup(argv[i]);
           break;
       case 7:
           process.fix_time = strdup(argv[i]);
           break;
       }
   }
   return 0;
}

int db_exec_string_list(sqlite3 *db, const char *sql)
{
    str_list_size = 0;
    int rc = sqlite3_exec(db, sql, callback_string_list, NULL, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        sprintf(error_str, "SQL-Fehler : %s", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return rc;
};

int db_exec_rec(sqlite3 *db, const char *sql)
{
    Serial.println(sql);
    int rc = sqlite3_exec(db, sql, callback_rec, NULL, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        sprintf(error_str, "SQL-Fehler : %s", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return rc;
};

void free_str_list()
{
    for (int i = 0; i < str_list_size; i++)
        free(str_list[i]);
    free(str_list);
    str_list = NULL;
    str_list_size = 0;
}

int init_str_list(char *sql_command, bool free_memory)
{
    if (free_memory)
        free_str_list();
    int rc = db_exec_string_list(db_film_data, sql_command);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db_film_data);
    }
    return rc;
}

int init_companies()
{
    char sql_command[120];
    sprintf(sql_command, "Select distinct company from processes");
    return init_str_list(sql_command, false);
}

int init_films(char *company_str)
{
    char sql_command[120];
    sprintf(sql_command, "Select distinct film from processes where company='%s'", company_str);
    return init_str_list(sql_command);
}

int init_developers(char *company_str, char * film_str)
{
    char sql_command[120];
    sprintf(sql_command, "Select distinct developer from processes where company='%s' and film='%s'", company_str, film_str);
    return init_str_list(sql_command);
}
