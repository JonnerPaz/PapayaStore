#include "domain/entities/producto/producto.entity.hpp"

Producto::Producto(int id, const char* nombre, const std::string& codigo, const char* descripcion,
                   bool eliminado, time_point<system_clock> fechaCreacion,
                   time_point<system_clock> fechaUltimaModificacion)
    : EntidadBase(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion), precio(0.0f),
      stock(0), idProveedor(0), stockMinimo(0), totalVendido(0) {
    setCodigo(codigo.c_str());
    setDescripcion(descripcion);
}
