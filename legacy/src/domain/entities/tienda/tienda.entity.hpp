#pragma once

#include "domain/entities/entidad.entity.hpp"

class Tienda : public EntidadBase
{
   private:
    char m_rif[20]{""};
    int m_totalProductosActivos{0};
    int m_totalProveedoresActivos{0};
    int m_totalClientesActivos{0};
    int m_totalTransaccionesActivas{0};
    float m_montoTotalVentas{0};
    float m_montoTotalCompras{0};

   public:
    Tienda();
    Tienda(int id, const char* nombre, const char* rif, int totalProductosActivos,
           int totalProveedoresActivos, bool eliminado, int totalClientesActivos,
           time_point<system_clock> fechaCreacion, int montoTotalVentas, int montoTotalCompras,
           int totalTransaccionesActivas, time_point<system_clock> fechaUltimaModificacion);

    const char* getRif() const { return m_rif; }

    bool setRif(const char* rif);

    int getTotalProductosActivos() const { return m_totalProductosActivos; }

    bool setTotalProductosActivos(int totalProductosActivos);

    int getTotalProveedoresActivos() const { return m_totalProveedoresActivos; }

    bool setTotalProveedoresActivos(int totalProveedoresActivos);

    int getTotalClientesActivos() const { return m_totalClientesActivos; }

    bool setTotalClientesActivos(int totalClientesActivos);

    int getTotalTransaccionesActivas() const { return m_totalTransaccionesActivas; }

    bool setTotalTransaccionesActivas(int totalTransaccionesActivas);

    float getMontoTotalVentas() const { return m_montoTotalVentas; }

    bool setMontoTotalVentas(float montoTotalVentas);

    float getMontoTotalCompras() const { return m_montoTotalCompras; }

    bool setMontoTotalCompras(float montoTotalCompras);

    ~Tienda() = default;
};
