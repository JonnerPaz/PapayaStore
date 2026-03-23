#include "domain/entities/tienda/tienda.entity.hpp"

using std::chrono::system_clock;
using std::chrono::time_point;

Tienda::Tienda(int id, const char* nombre, const char* rif, int totalProductosActivos,
               int totalProveedoresActivos, bool eliminado, int totalClientesActivos,
               time_point<system_clock> fechaCreacion, int montoTotalVentas, int montoTotalCompras,
               int totalTransaccionesActivas, time_point<system_clock> fechaUltimaModificacion)
    : Entidad(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion) {
    strncpy(this->rif, rif, 20);

    this->totalProductosActivos = totalProductosActivos;
    this->totalProveedoresActivos = totalProveedoresActivos;
    this->totalClientesActivos = totalClientesActivos;
    this->totalTransaccionesActivas = totalTransaccionesActivas;
    this->montoTotalVentas = montoTotalVentas;
    this->montoTotalCompras = montoTotalCompras;

    this->setId(id);
    this->setNombre(nombre);
    this->setNombre(nombre);
    this->setEliminado(eliminado);
    this->setFechaCreacion(fechaCreacion);
    this->setFechaUltimaModificacion(fechaUltimaModificacion);
}
