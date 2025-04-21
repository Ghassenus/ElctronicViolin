// src/core/parameters_manager.h
#pragma once

#include <ctime>

/// Initialise la logique (format heure, header…)
void parameters_manager_init();

/// Définit l’heure (heure seulement)
void parameters_manager_set_hour(int hour);

/// Définit les minutes
void parameters_manager_set_minute(int minute);

/// Définit la date (YYYY, MM[1-12], DD[1-31])
void parameters_manager_set_date(int year, int month, int day);

/// Définit le format 12h/24h
void parameters_manager_set_time_format(bool is24h);

/// Renvoie true si on est en format 24h
bool parameters_manager_is_24h_format();
///Get hour  
/// get minute
// get date
int  parameters_manager_get_hour();
int  parameters_manager_get_minute();
void parameters_manager_get_date(int &year, int &month, int &day);
