#pragma once

#include "domain/entities/entidad.entity.hpp"

class Cliente : public EntidadBase
{
   private:
    char m_telefono[20];
    char m_email[100];
    char m_direccion[200];
    int m_cantidad;           // cantidad de transacciones / productos
    int m_historialIds[100];  // Identificadores de transacciones / productos
    char m_cedula[20];        // Cédula o RIF
    float m_totalCompras;
    int m_transaccionesIds[100];
    int m_cantidadTransacciones;

    bool validarEmail(const char* email);

   public:
    Cliente();

    Cliente(int id, const char* nombre, const char* cedula, const char* telefono, const char* email,
            const char* direccion, bool eliminado, time_point<system_clock> fechaCreacion,
            time_point<system_clock> fechaUltimaModificacion);

    const char* getCedula() const { return this->m_cedula; }

    bool setCedula(const char* cedula);

    const char* getTelefono() const { return this->m_telefono; }

    bool setTelefono(const char* telefono);

    const char* getEmail() const { return this->m_email; }

    bool setEmail(const char* email);

    const char* getDireccion() const { return this->m_direccion; }

    bool setDireccion(const char* direccion);

    float getTotalCompras() const { return this->m_totalCompras; }

    bool setTotalCompras(float totalCompras);

    int getCantidadTransacciones() const { return this->m_cantidadTransacciones; }

    const int* getTransaccionesIds() const { return this->m_transaccionesIds; }

    bool agregarTransaccionId(int idTransaccion);

    bool removerTransaccionId(int idTransaccion);

    int getCantidad() const { return this->m_cantidad; };

    bool setCantidad(int cantidad);

    const int* getHistorialIds() const { return this->m_historialIds; }

    int* getHistorialIds() { return this->m_historialIds; }

    bool setHistorialIdEnIndice(int index, int historialId);

    bool setTransaccionIdEnIndice(int index, int idTransaccion);

    bool setCantidadTransacciones(int cantidadTransacciones);
};
