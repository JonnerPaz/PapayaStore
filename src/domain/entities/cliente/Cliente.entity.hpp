#pragma once
#include <cstring>

#include "domain/entities/entidad.entity.hpp"

class Cliente : public EntidadBase
{
   private:
    char m_telefono[20];
    char m_email[100];
    char m_direccion[200];
    char m_fechaRegistro[11];
    int m_cantidad;           // cantidad de transacciones / productos
    int m_historialIds[100];  // Identificadores de transacciones / productos
    char m_cedula[20];        // Cédula o RIF
    float m_totalCompras;
    int m_transaccionesIds[100];
    int m_cantidadTransacciones;

    bool validarEmail(const char* email);
    bool validarFecha(const char* fecha);

   public:
    Cliente();

    Cliente(int id, const char* nombre, const char* cedula, const char* telefono, const char* email,
            const char* direccion, bool eliminado, const char* fechaRegistro,
            time_point<system_clock> fechaCreacion,
            time_point<system_clock> fechaUltimaModificacion);

    char* getTelefono() { return this->m_telefono; }

    int setTelefono(const char* telefono)
    {
        strcpy(this->m_telefono, telefono);
        return 0;
    }

    char* getEmail() { return this->m_email; }

    int setEmail(const char* email)
    {
        if (!validarEmail(email)) {
            return 1;
        }

        strcpy(this->m_email, email);
        return 0;
    }

    char* getDireccion() { return this->m_direccion; }

    char* setDireccion(const char* direccion)
    {
        strcpy(this->m_direccion, direccion);
        return this->m_direccion;
    }

    char* getFechaRegistro() { return this->m_fechaRegistro; }

    int getCantidad() { return this->m_cantidad; };

    int* getHistorialIds() { return this->m_historialIds; }
};
