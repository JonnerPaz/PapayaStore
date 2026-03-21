#pragma once
#include "entidad.entity.hpp"

class Usuario : public Entidad {
  private:
    char telefono[20];
    char email[100];
    char direccion[200];
    char fechaRegistro[11];
    int cantidad;          // cantidad de transacciones / productos
    int historialIds[100]; // Identificadores de transacciones / productos
  protected:
    virtual char* getTelefono() {
        return this->telefono;
    }

    virtual int setTelefono(char* telefono);

    virtual char* getEmail() {
        return this->email;
    }

    virtual int setEmail(char* email);

    virtual char* getDireccion() {
        return this->direccion;
    }

    virtual char* setDireccion(char* direccion);

    virtual char* getFechaRegistro() {
        return this->fechaRegistro;
    }

    virtual int getCantidad() {
        return this->cantidad;
    };

    virtual int* getHistorialIds() {
        return this->historialIds;
    }

  public:
    virtual ~Usuario() = default;
};
