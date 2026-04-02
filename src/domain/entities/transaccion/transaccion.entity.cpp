#include "transaccion.entity.hpp"

Transaccion::Transaccion(int id, char* nombre, bool eliminado,
                         time_point<system_clock> fechaCreacion,
                         time_point<system_clock> fechaUltimaModificacion, TipoDeTransaccion tipo,
                         int idRelacionado, float total, char* descripcion, int productosIds[],
                         int cantidades[], float preciosUnitarios[], int cantidadTiposDeProductos)
    : EntidadBase(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion)
{
    this->tipo = tipo;
    this->idRelacionado = idRelacionado;
    this->total = total;
    this->cantidadTiposDeProductos = cantidadTiposDeProductos;
    this->descripcion[0] = '\0';
    this->copiarCadenaSeguro(this->descripcion, 200, descripcion);
    for (int i = 0; i < 100; ++i) {
        this->productosIds[i] = productosIds[i];
        this->cantidades[i] = cantidades[i];
        this->preciosUnitarios[i] = preciosUnitarios[i];
    }
}

bool Transaccion::setTipoTransaccion(TipoDeTransaccion nuevoTipo)
{
    this->tipo = nuevoTipo;
    return true;
}

bool Transaccion::setIdRelacionado(int nuevoIdRelacionado)
{
    this->idRelacionado = nuevoIdRelacionado;
    return true;
}

bool Transaccion::setTotal(float nuevoTotal)
{
    this->total = nuevoTotal;
    return true;
}

bool Transaccion::setDescripcion(char* nuevaDescripcion)
{
    if (!EntidadBase::copiarCadenaSeguro(this->descripcion, 200, nuevaDescripcion)) {
        return false;
    }
    return true;
}

bool Transaccion::setProductosIds(int productosIds[], int cantidades[], float preciosUnitarios[],
                                  int cantidadTiposDeProductos)
{
    this->cantidadTiposDeProductos = cantidadTiposDeProductos;
    for (int i = 0; i < 100; ++i) {
        this->productosIds[i] = productosIds[i];
        this->cantidades[i] = cantidades[i];
        this->preciosUnitarios[i] = preciosUnitarios[i];
    }
    return true;
}

bool Transaccion::setCantidades(int cantidades[], float preciosUnitarios[],
                                int cantidadTiposDeProductos)
{
    this->cantidadTiposDeProductos = cantidadTiposDeProductos;
    for (int i = 0; i < 100; ++i) {
        this->cantidades[i] = cantidades[i];
        this->preciosUnitarios[i] = preciosUnitarios[i];
    }
    return true;
}

bool Transaccion::setPreciosUnitarios(float preciosUnitarios[], int cantidadTiposDeProductos)
{
    this->cantidadTiposDeProductos = cantidadTiposDeProductos;
    for (int i = 0; i < 100; ++i) {
        this->preciosUnitarios[i] = preciosUnitarios[i];
    }
    return true;
}
