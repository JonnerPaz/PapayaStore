#pragma once
#include <chrono>

class Cliente {
  public:
    int id;                 // Identificador único (autoincremental)
    char nombre[100];       // Nombre completo del cliente
    char cedula[20];        // Cédula o RIF
    char telefono[20];      // Teléfono de contacto
    char email[100];        // Correo electrónico
    char direccion[200];    // Dirección física
    char fechaRegistro[11]; // Formato: YYYY-MM-DD

    float totalCompras;

    int transaccionesIds[100];
    int cantidadTransacciones;
    // metadata
    bool eliminado;
    std::chrono::time_point<std::chrono::system_clock> fechaCreacion;
    std::chrono::time_point<std::chrono::system_clock> fechaUltimaModificacion;
};
