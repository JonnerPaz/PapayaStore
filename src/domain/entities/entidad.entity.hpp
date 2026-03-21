#pragma once
#include <chrono>
#include <cstring>

class Entidad {
  private:
    int id;           // Identificador único (autoincremental)
    char nombre[100]; // Nombre de la entidad
    bool eliminado;   // Indica si la entidad ha sido eliminada
    std::chrono::time_point<std::chrono::system_clock> fechaCreacion;
    std::chrono::time_point<std::chrono::system_clock> fechaUltimaModificacion;

  protected:
    Entidad(int id, const char* nombre) {
        this->id = id;
        strcpy(this->nombre, nombre);
        this->eliminado = false;
        this->fechaCreacion = std::chrono::system_clock::now();
        this->fechaUltimaModificacion = std::chrono::system_clock::now();
    }

    virtual int getId() {
        return this->id;
    }

    virtual char* getNombre() {
        return this->nombre;
    }

    virtual char* setNombre(const char* nombre) {
        strcpy(this->nombre, nombre);
        return this->nombre;
    }

    virtual bool setEliminado(bool eliminado) {
        this->eliminado = eliminado;
        return true;
    }

  public:
    virtual ~Entidad() = default;
};
