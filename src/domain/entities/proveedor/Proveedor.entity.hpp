#pragma once

#include "domain/entities/entidad.entity.hpp"

class Proveedor : public EntidadBase
{
   private:
    char m_rif[20]{};  // RIF o identificación fiscal
    int m_productosIds[100]{0};
    int m_cantidadProductos{0};
    char m_telefono[20]{0};
    char m_email[100]{};
    char m_direccion[200]{};
    int m_cantidad{0};           // cantidad de transacciones / productos
    int m_historialIds[100]{0};  // Identificadores de transacciones / productos

    bool validarEmail(const char* email);
    bool validarFecha(const char* fecha);

   public:
    Proveedor();

    Proveedor(int id, const char* nombre, bool eliminado, time_point<system_clock> fechaCreacion,
              time_point<system_clock> fechaUltimaModificacion, const char* rif, int productosIds[],
              int cantidadProductos, const char* telefono, const char* email, const char* direccion,
              int cantidad, int historialIds[]);

    char* getRif() { return this->m_rif; }

    bool setRif(const char* rif);

    char* getTelefono() { return this->m_telefono; }

    bool setTelefono(const char* telefono);

    char* getEmail() { return this->m_email; }

    bool setEmail(const char* email);

    char* getDireccion() { return this->m_direccion; }

    bool setDireccion(const char* direccion);

    int getCantidad() { return this->m_cantidad; };

    int* getHistorialIds() { return this->m_historialIds; }
};
