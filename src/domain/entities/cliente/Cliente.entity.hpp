#pragma once
#include "domain/entities/usuario.entity.hpp"
#include <chrono>

class Cliente : Usuario {
  public:
    Cliente(int id, const char* nombre, const char* cedula, const char* telefono,
            const char* email);
    char cedula[20]; // Cédula o RIF
    float totalCompras;
    int transaccionesIds[100];
    int cantidadTransacciones;
    std::chrono::time_point<std::chrono::system_clock> fechaCreacion;
    std::chrono::time_point<std::chrono::system_clock> fechaUltimaModificacion;
};
