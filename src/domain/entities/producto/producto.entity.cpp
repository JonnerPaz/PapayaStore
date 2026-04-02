#include "domain/entities/producto/producto.entity.hpp"

Producto::Producto(int id, const char* nombre, bool eliminado,
                   time_point<system_clock> fechaCreacion,
                   time_point<system_clock> fechaUltimaModificacion, const char* codigo,
                   const char* descripcion, float precio, int stock, int idProveedor,
                   int stockMinimo, int totalVendido)
    : EntidadBase{id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion}
{
    if (id <= 0) throw std::invalid_argument("ID invalido");
    if (nombre == nullptr || nombre[0] == '\0') throw std::invalid_argument("Nombre requerido");
    if (codigo == nullptr || codigo[0] == '\0') throw std::invalid_argument("Codigo requerido");
    if (descripcion == nullptr || descripcion[0] == '\0')
        throw std::invalid_argument("Descripcion requerida");
    if (precio < 0) throw std::invalid_argument("Precio invalido");
    if (stock < 0) throw std::invalid_argument("Stock invalido");
    if (stockMinimo < 0) throw std::invalid_argument("Stock minimo invalido");
    if (totalVendido < 0) throw std::invalid_argument("Total vendido invalido");
    if (idProveedor <= 0) throw std::invalid_argument("Proveedor requerido");

    setNombre(nombre);
    setCodigo(codigo);
    setDescripcion(descripcion);
    setPrecio(precio);
    setStock(stock);
    setIdProveedor(idProveedor);
    setStockMinimo(stockMinimo);
    setTotalVendido(totalVendido);
}

bool Producto::setPrecio(float nuevoPrecio)
{
    if (nuevoPrecio < 0) {
        return false;
    }
    m_precio = nuevoPrecio;
    return true;
}

bool Producto::setStock(int nuevoStock)
{
    if (nuevoStock < 0) {
        return false;
    }
    m_stock = nuevoStock;
    return true;
}

bool Producto::setIdProveedor(int nuevoIdProveedor)
{
    if (nuevoIdProveedor < 0) {
        return false;
    }
    m_idProveedor = nuevoIdProveedor;
    return true;
}

bool Producto::setStockMinimo(int nuevoStockMinimo)
{
    if (nuevoStockMinimo < 0) {
        return false;
    }
    m_stockMinimo = nuevoStockMinimo;
    return true;
}

bool Producto::setTotalVendido(int nuevoTotalVendido)
{
    if (nuevoTotalVendido < 0) {
        return false;
    }
    m_totalVendido = nuevoTotalVendido;
    return true;
}

bool Producto::setCodigo(const char* nuevoCodigo)
{
    if (!EntidadBase::copiarCadenaSeguro(m_codigo, sizeof(m_codigo), nuevoCodigo)) {
        return false;
    }
    return true;
}

bool Producto::setDescripcion(const char* nuevaDescripcion)
{
    if (!EntidadBase::copiarCadenaSeguro(m_descripcion, sizeof(m_descripcion), nuevaDescripcion)) {
        return false;
    }
    return true;
}
