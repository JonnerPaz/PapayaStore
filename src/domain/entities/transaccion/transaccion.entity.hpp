#pragma once

#include "domain/entities/entidad.entity.hpp"

enum TipoDeTransaccion { COMPRA, VENTA };

struct TransaccionDTO {
    int productoId;  // ID del producto
    int cantidad;    // Cantidad de productos
    int precio;      // Precio unitario
};

class Transaccion : public EntidadBase
{
   private:
    TipoDeTransaccion m_tipo{};       // COMPRA o VENTA
    int m_idRelacionado{0};           // ID del proveedor (compra) o cliente (venta)
    float m_total{0};                 // cantidad * precioUnitario
    char m_descripcion[200]{};        // Notas adicionales (opcional)
    TransaccionDTO m_productos[100]{};  // Productos de la transaccion (hasta 100)
    int m_productosTotales{0};        // cuantos productos existen en la transaccion

   public:
    Transaccion();

    Transaccion(int id, char* nombre, bool eliminado, time_point<system_clock> fechaCreacion,
                time_point<system_clock> fechaUltimaModificacion, TipoDeTransaccion tipo,
                int idRelacionado, float total, char* descripcion, TransaccionDTO& productos);

    auto getTipoTransaccion() const { return this->m_tipo; }

    bool setTipoTransaccion(TipoDeTransaccion nuevoTipo);

    auto getIdRelacionado() const { return this->m_idRelacionado; }

    bool setIdRelacionado(int nuevoIdRelacionado);

    auto getTotal() const { return this->m_total; }

    bool setTotal(float nuevoTotal);

    auto getDescripcion() const { return this->m_descripcion; }

    bool setDescripcion(char* nuevaDescripcion);

    auto getProducto(int id) const
    {
        for (auto product : this->m_productos) {
            if (product.productoId == id) {
                return product;
            }
        }

        throw std::invalid_argument("Producto no encontrado");
    }

    bool setProducto(TransaccionDTO nuevoProducto);
};
