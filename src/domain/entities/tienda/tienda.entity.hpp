#pragma once
#include <chrono>
#include <cstring>

#include "domain/entities/entidad.entity.hpp"

using std::chrono::system_clock;
using std::chrono::time_point;

class Tienda : public EntidadBase
{
   private:
    char rif[20];
    int totalProductosActivos;
    int totalProveedoresActivos;
    int totalClientesActivos;
    int totalTransaccionesActivas;
    float montoTotalVentas;
    float montoTotalCompras;

   public:
    Tienda();
    Tienda(int id, const char* nombre, const char* rif, int totalProductosActivos,
           int totalProveedoresActivos, bool eliminado, int totalClientesActivos,
           time_point<system_clock> fechaCreacion, int montoTotalVentas, int montoTotalCompras,
           int totalTransaccionesActivas, time_point<system_clock> fechaUltimaModificacion);

    const char* getRif() const { return rif; }

    bool setRif(const char* rif);

    int getTotalProductosActivos() const { return totalProductosActivos; }

    void setTotalProductosActivos(int totalProductosActivos)
    {
        this->totalProductosActivos = totalProductosActivos;
    }

    int getTotalProveedoresActivos() const { return totalProveedoresActivos; }

    void setTotalProveedoresActivos(int totalProveedoresActivos)
    {
        this->totalProveedoresActivos = totalProveedoresActivos;
    }

    int getTotalClientesActivos() const { return totalClientesActivos; }

    void setTotalClientesActivos(int totalClientesActivos)
    {
        this->totalClientesActivos = totalClientesActivos;
    }

    int getTotalTransaccionesActivas() const { return totalTransaccionesActivas; }

    void setTotalTransaccionesActivas(int totalTransaccionesActivas)
    {
        this->totalTransaccionesActivas = totalTransaccionesActivas;
    }

    float getMontoTotalVentas() const { return montoTotalVentas; }

    void setMontoTotalVentas(float montoTotalVentas) { this->montoTotalVentas = montoTotalVentas; }

    float getMontoTotalCompras() const { return montoTotalCompras; }

    void setMontoTotalCompras(float montoTotalCompras)
    {
        this->montoTotalCompras = montoTotalCompras;
    }

    ~Tienda() = default;
};
