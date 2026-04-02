#pragma once

#include "domain/entities/entidad.entity.hpp"

enum TipoDeTransaccion { COMPRA, VENTA };

class Transaccion : public EntidadBase
{
   private:
    TipoDeTransaccion tipo;  // COMPRA o VENTA
    int idRelacionado;       // ID del proveedor (compra) o cliente (venta)
    float total;             // cantidad * precioUnitario
    char descripcion[200];   // Notas adicionales (opcional)

    // Hasta 100 productos por transacción
    int productosIds[100];
    int cantidades[100];           // Cantidades de cada producto
    float preciosUnitarios[100];   // Precios unitarios de cada producto
    int cantidadTiposDeProductos;  // Cuantos elementos de los arrays anteriores
                                   // se están usando
   public:
    Transaccion(int id, char* nombre, bool eliminado, time_point<system_clock> fechaCreacion,
                time_point<system_clock> fechaUltimaModificacion, TipoDeTransaccion tipo,
                int idRelacionado, float total, char* descripcion, int productosIds[],
                int cantidades[], float preciosUnitarios[], int cantidadTiposDeProductos);

    auto getTipoTransaccion() const { return this->tipo; }

    bool setTipoTransaccion(TipoDeTransaccion nuevoTipo);

    auto getIdRelacionado() const { return this->idRelacionado; }

    bool setIdRelacionado(int nuevoIdRelacionado);

    auto getTotal() const { return this->total; }

    bool setTotal(float nuevoTotal);

    auto getDescripcion() const { return this->descripcion; }

    bool setDescripcion(char* nuevaDescripcion);

    auto getProductosIds() const { return this->productosIds; }

    bool setProductosIds(int productosIds[], int cantidades[], float preciosUnitarios[],
                         int cantidadTiposDeProductos);

    auto getCantidades() const { return this->cantidades; }

    auto getPreciosUnitarios() const { return this->preciosUnitarios; }

    auto getCantidadTiposDeProductos() const { return this->cantidadTiposDeProductos; }

    bool setCantidadTiposDeProductos(int nuevaCantidadTiposDeProductos)
    {
        this->cantidadTiposDeProductos = nuevaCantidadTiposDeProductos;
        return true;
    }
};
