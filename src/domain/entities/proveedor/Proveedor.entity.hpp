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
    Proveedor(char* telefono, char* email, char* direccion, char* fechaRegistro, int id,
              char* nombre, bool eliminado, time_point<system_clock> fechaCreacion,
              time_point<system_clock> fechaUltimaModificacion);

    std::chrono::time_point<std::chrono::system_clock> fechaCreacion;
    std::chrono::time_point<std::chrono::system_clock> fechaUltimaModificacion;
};
