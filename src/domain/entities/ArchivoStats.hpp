#pragma once

/**
 * @brief - Estructura que contiene estadísticas de las entidades guardadas en
 * su respectiva base de datos. En el caso de los archivos, es el DTO de HeaderFile
 */
struct ArchivoStats {
    int cantidadRegistros; // Total histórico de registros
    int proximoID;         // Siguiente ID a asignar (Autoincremental)
    int registrosActivos;  // Registros que no están marcados como eliminados
    int version;
};
