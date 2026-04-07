#include "transaccion.entity.hpp"

Transaccion::Transaccion()
    : EntidadBase(0, "", false, std::chrono::system_clock::now(), std::chrono::system_clock::now())
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
      m_total(total)
{
    EntidadBase::copiarCadenaSeguro(this->m_descripcion, sizeof(this->m_descripcion), descripcion);
    this->setProducto(productos);
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

bool Transaccion::setDescripcion(const char* nuevaDescripcion)
{
    bool copiedString = EntidadBase::copiarCadenaSeguro(
        this->m_descripcion, sizeof(this->m_descripcion), nuevaDescripcion);

    if (!copiedString) return false;

    return true;
}

bool Transaccion::getProductoEnIndice(int index, TransaccionDTO& outProducto) const
{
    if (index < 0 || index >= this->m_productosTotales) {
        return false;
    }

    outProducto = this->m_productos[index];
    return true;
}

bool Transaccion::setProductosTotales(int productosTotales)
{
    if (productosTotales < 0 || productosTotales > 100) {
        return false;
    }

    this->m_productosTotales = productosTotales;
    return true;
}

bool Transaccion::setProductoEnIndice(int index, const TransaccionDTO& producto)
{
    if (index < 0 || index >= 100) {
        return false;
    }

    if (producto.productoId <= 0 || producto.cantidad <= 0 || producto.precio < 0) {
        return false;
    }

    this->m_productos[index] = producto;
    return true;
}

bool Transaccion::setProducto(TransaccionDTO nuevoProducto)
{
    auto isDTOValid =
        nuevoProducto.cantidad > 0 && nuevoProducto.precio >= 0 && nuevoProducto.productoId > 0;

    if (!isDTOValid) return false;

    // if product already exists, sum its quantities
    for (int i = 0; i < this->m_productosTotales; ++i) {
        if (this->m_productos[i].productoId == nuevoProducto.productoId) {
            this->m_productos[i].cantidad += nuevoProducto.cantidad;
            return true;
        }
    }

    // if product doesn't exists, append it
    if (this->m_productosTotales >= 100) return false;

    this->m_productos[this->m_productosTotales] = nuevoProducto;
    this->m_productosTotales += 1;

    return true;
}
