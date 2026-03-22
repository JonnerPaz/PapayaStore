#pragma once
#include "domain/entities/entidad.entity.hpp"
#include <cstring>
#include <ctime>
#include <string>
#include <variant>

class Producto : public Entidad {
  private:
    char codigo[20];        // Código del producto (ej: "PROD-001")
    char descripcion[200];  // Descripción del producto
    char fechaRegistro[11]; // Formato: YYYY-MM-DD
    float precio;           // Precio unitario
    int stock;              // Cantidad en inventario
    // llaves foraneas
    int idProveedor;
    // estadisticas
    int stockMinimo;
    int totalVendido;
    // metadata
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;

    static void copiarCadenaSeguro(char* destino, size_t capacidad, const char* fuente) {
        if (capacidad == 0)
            return;

        if (fuente == nullptr) {
            destino[0] = '\0';
            return;
        }

        std::strncpy(destino, fuente, capacidad - 1);
        destino[capacidad - 1] = '\0';
    }

  public:
    Producto(int id, const char* nombre, const std::string& codigo, const char* descripcion);
    // Metodos
    Producto crearProducto();
    Producto buscarProducto(int id);
    Producto actualizarProducto(int id);
    Producto actualizarStockProducto(int id);
    Producto listarProductos();
    Producto eliminarProducto(int id);

    float getPrecio() const {
        return precio;
    }

    int getStock() const {
        return stock;
    }

    int getIdProveedor() const {
        return idProveedor;
    }

    int getStockMinimo() const {
        return stockMinimo;
    }

    int getTotalVendido() const {
        return totalVendido;
    }

    const char* getCodigo() const {
        return codigo;
    }

    const char* getDescripcion() const {
        return descripcion;
    }

    const char* getFechaRegistro() const {
        return fechaRegistro;
    }

    void setPrecio(float nuevoPrecio) {
        precio = nuevoPrecio;
    }

    void setStock(int nuevoStock) {
        stock = nuevoStock;
    }

    void setIdProveedor(int nuevoIdProveedor) {
        idProveedor = nuevoIdProveedor;
    }

    void setStockMinimo(int nuevoStockMinimo) {
        stockMinimo = nuevoStockMinimo;
    }

    void setTotalVendido(int nuevoTotalVendido) {
        totalVendido = nuevoTotalVendido;
    }

    void setCodigo(const char* nuevoCodigo) {
        copiarCadenaSeguro(codigo, sizeof(codigo), nuevoCodigo);
    }

    void setDescripcion(const char* nuevaDescripcion) {
        copiarCadenaSeguro(descripcion, sizeof(descripcion), nuevaDescripcion);
    }

    void setFechaRegistro(const char* nuevaFechaRegistro) {
        copiarCadenaSeguro(fechaRegistro, sizeof(fechaRegistro), nuevaFechaRegistro);
    }

    ~Producto() = default;
};

class ProductoBuilder {
  private:
    int id = 0;
    const char* nombre = "";
    const char* codigo = "";
    const char* descripcion = "";
    float precio = 0.0f;
    int stock = 0;
    int idProveedor = 0;
    int stockMinimo = 0;
    int totalVendido = 0;
    const char* fechaRegistro = "";

  public:
    ProductoBuilder& conId(int nuevoId) {
        id = nuevoId;
        return *this;
    }

    ProductoBuilder& conNombre(const char* nuevoNombre) {
        nombre = nuevoNombre;
        return *this;
    }

    ProductoBuilder& conCodigo(const char* nuevoCodigo) {
        codigo = nuevoCodigo;
        return *this;
    }

    ProductoBuilder& conDescripcion(const char* nuevaDescripcion) {
        descripcion = nuevaDescripcion;
        return *this;
    }

    ProductoBuilder& conPrecio(float nuevoPrecio) {
        precio = nuevoPrecio;
        return *this;
    }

    ProductoBuilder& conStock(int nuevoStock) {
        stock = nuevoStock;
        return *this;
    }

    ProductoBuilder& conIdProveedor(int nuevoIdProveedor) {
        idProveedor = nuevoIdProveedor;
        return *this;
    }

    ProductoBuilder& conStockMinimo(int nuevoStockMinimo) {
        stockMinimo = nuevoStockMinimo;
        return *this;
    }

    ProductoBuilder& conTotalVendido(int nuevoTotalVendido) {
        totalVendido = nuevoTotalVendido;
        return *this;
    }

    ProductoBuilder& conFechaRegistro(const char* nuevaFechaRegistro) {
        fechaRegistro = nuevaFechaRegistro;
        return *this;
    }

    std::variant<Producto, std::string> build() const {
        if (id <= 0)
            return "ID inválido: debe ser mayor a 0.";
        if (nombre == nullptr || nombre[0] == '\0')
            return "Nombre inválido: no puede estar vacío.";
        if (codigo == nullptr || codigo[0] == '\0')
            return "Código inválido: no puede estar vacío.";
        if (descripcion == nullptr || descripcion[0] == '\0')
            return "Descripción inválida: no puede estar vacía.";
        if (precio < 0)
            return "Precio inválido: debe ser mayor o igual a 0.";
        if (stock < 0)
            return "Stock inválido: debe ser mayor o igual a 0.";
        if (stockMinimo < 0)
            return "Stock mínimo inválido: debe ser mayor o igual a 0.";
        if (totalVendido < 0)
            return "Total vendido inválido: debe ser mayor o igual a 0.";
        if (idProveedor <= 0)
            return "ID de proveedor inválido: debe ser mayor a 0.";

        Producto producto(id, nombre, codigo, descripcion);
        producto.setPrecio(precio);
        producto.setStock(stock);
        producto.setIdProveedor(idProveedor);
        producto.setStockMinimo(stockMinimo);
        producto.setTotalVendido(totalVendido);
        if (fechaRegistro != nullptr && fechaRegistro[0] != '\0') {
            producto.setFechaRegistro(fechaRegistro);
        }

        return producto;
    }
};
