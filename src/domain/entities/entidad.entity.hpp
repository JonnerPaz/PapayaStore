#pragma once
#include <chrono>

class Entidad {
  protected:
    int id;           // Identificador único (autoincremental)
    char nombre[100]; // Nombre de la entidad
    bool eliminado;   // Indica si la entidad ha sido eliminada
    std::chrono::time_point<std::chrono::system_clock> fechaCreacion;
    std::chrono::time_point<std::chrono::system_clock> fechaUltimaModificacion;

  public:
    virtual ~Entidad() = default;
};

class Usuario : public Entidad {
  protected:
    char telefono[20];
    char email[100];
    char direccion[200];
    char fechaRegistro[11];
    int cantidad;          // cantidad de transacciones / productos
    int historialIds[100]; // Identificadores de transacciones / productos

  public:
    ~Usuario() = default;
};
