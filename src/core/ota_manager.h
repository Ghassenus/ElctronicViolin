#pragma once

/// Initialise le service OTA (à appeler dans setup)
void ota_manager_init();

/// A appeler dans loop() pour traiter les requêtes OTA
void ota_manager_handle();

/** Renvoie true si une mise à jour OTA est en cours */
bool ota_manager_is_running();
