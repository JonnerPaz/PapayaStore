#pragma once
#include "domain/entities/usuario.entity.hpp"
#include <chrono>

class Proveedor : public Usuario {
  private:
    char rif[20];    // RIF o identificación fiscal
    char email[100]; // Correo electrónico
    int productosIds[100];
    int cantidadProductos;

  public:
    Proveedor(int id, const char* nombre, const char* rif, const char* telefono, const char* email);

    std::chrono::time_point<std::chrono::system_clock> fechaCreacion;
    std::chrono::time_point<std::chrono::system_clock> fechaUltimaModificacion;
};
