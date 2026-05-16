#pragma once

struct HeaderFile {
    int cantidadRegistros; // Total histórico de registros
    int proximoID;         // Siguiente ID a asignar (Autoincremental)
    int registrosActivos;  // Registros que no están marcados como eliminados
    int version;
};
