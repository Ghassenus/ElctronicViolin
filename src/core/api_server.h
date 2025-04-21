// src/core/api_server.h
#pragma once

#include <ESPAsyncWebServer.h>

/**
 * @brief Enregistre toutes les routes API sur le serveur HTTP passé en argument.
 * @param server L’instance AsyncWebServer créée en main().
 */
void api_server_init(AsyncWebServer &server);
