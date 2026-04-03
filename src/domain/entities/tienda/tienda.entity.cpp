#include "domain/entities/tienda/tienda.entity.hpp"

Tienda::Tienda() : EntidadBase(0, "Papaya Store", false, system_clock::now(), system_clock::now())
{
}

Tienda::Tienda(int id, const char* nombre, const char* rif, int totalProductosActivos,
               int totalProveedoresActivos, bool eliminado, int totalClientesActivos,
               time_point<system_clock> fechaCreacion, int montoTotalVentas, int montoTotalCompras,
               int totalTransaccionesActivas, time_point<system_clock> fechaUltimaModificacion)
    : EntidadBase(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion),
      m_totalProductosActivos(totalProductosActivos),
      m_totalProveedoresActivos(totalProveedoresActivos),
      m_totalClientesActivos(totalClientesActivos),
      m_totalTransaccionesActivas(totalTransaccionesActivas),
      m_montoTotalVentas(montoTotalVentas),
      m_montoTotalCompras(montoTotalCompras)
{
    EntidadBase::copiarCadenaSeguro(this->m_rif, sizeof(this->m_rif), rif);
}

bool Tienda::setRif(const char* rif)
{
    EntidadBase::copiarCadenaSeguro(this->m_rif, sizeof(this->m_rif), rif);
    return true;
}

bool Tienda::setTotalProductosActivos(int totalProductosActivos)
{
    this->m_totalProductosActivos = totalProductosActivos;
    return true;
}

bool Tienda::setTotalProveedoresActivos(int totalProveedoresActivos)
{
    this->m_totalProveedoresActivos = totalProveedoresActivos;
    return true;
}

bool Tienda::setTotalClientesActivos(int totalClientesActivos)
{
    this->m_totalClientesActivos = totalClientesActivos;
    return true;
}

bool Tienda::setTotalTransaccionesActivas(int totalTransaccionesActivas)
{
    this->m_totalTransaccionesActivas = totalTransaccionesActivas;
    return true;
}

bool Tienda::setMontoTotalVentas(float montoTotalVentas)
{
    this->m_montoTotalVentas = montoTotalVentas;
    return true;
}

bool Tienda::setMontoTotalCompras(float montoTotalCompras)
{
    this->m_montoTotalCompras = montoTotalCompras;
    return true;
}
