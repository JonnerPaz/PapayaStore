#pragma once
#include "domain/entities/usuario.entity.hpp"
#include <chrono>

class Cliente : Usuario {
  public:
    char cedula[20]; // Cédula o RIF
    float totalCompras;
    int transaccionesIds[100];
    int cantidadTransacciones;
    std::chrono::time_point<std::chrono::system_clock> fechaCreacion;
    std::chrono::time_point<std::chrono::system_clock> fechaUltimaModificacion;
};
