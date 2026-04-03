#include "transaccion.entity.hpp"

Transaccion::Transaccion()
    : EntidadBase(0, "", false, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now())
{
}

Transaccion::Transaccion(int id, char* nombre, bool eliminado,
                         time_point<system_clock> fechaCreacion,
                         time_point<system_clock> fechaUltimaModificacion, TipoDeTransaccion tipo,
                         int idRelacionado, float total, char* descripcion,
                         TransaccionDTO& productos)
    : EntidadBase(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion),
      m_tipo(tipo),
      m_idRelacionado(idRelacionado),
      m_total(total),
      m_productos(productos)
{
    EntidadBase::copiarCadenaSeguro(this->m_descripcion, sizeof(this->m_descripcion), descripcion);
}

bool Transaccion::setTipoTransaccion(TipoDeTransaccion nuevoTipo)
{
    this->m_tipo = nuevoTipo;
    return true;
}

bool Transaccion::setIdRelacionado(int nuevoIdRelacionado)
{
    this->m_idRelacionado = nuevoIdRelacionado;
    return true;
}

bool Transaccion::setTotal(float nuevoTotal)
{
    this->m_total = nuevoTotal;
    return true;
}

bool Transaccion::setDescripcion(char* nuevaDescripcion)
{
    bool copiedString = EntidadBase::copiarCadenaSeguro(
        this->m_descripcion, sizeof(this->m_descripcion), nuevaDescripcion);

    if (!copiedString) return false;

    return true;
}

bool Transaccion::setProducto(TransaccionDTO nuevoProducto)
{
    auto isDTOValid =
        nuevoProducto.cantidad >= 0 && nuevoProducto.precio >= 0 && nuevoProducto.productoId >= 0;

    if (!isDTOValid) return false;

    // if product already exists, sum its quantities
    for (auto product : this->m_productos) {
        if (product.productoId == nuevoProducto.productoId) {
            product.cantidad += nuevoProducto.cantidad;
            return true;
        }
    }

    // if product doesn't exists, append it
    if (this->m_productosTotales >= 100) return false;

    this->m_productos[this->m_productosTotales] = nuevoProducto;
    this->m_productosTotales += 1;

    return true;
}
