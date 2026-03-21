#pragma once
#include <ctime>
#include <string>

class Producto {
  private:
    int id;                 // Identificador único (autoincremental)
    char nombre[100];       // Nombre del producto
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
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;

  public:
    Producto(int id, const char* nombre, const std::string& codigo, const char* descripcion);
    // Metodos
    Producto crearProducto();
    Producto buscarProducto(int id);
    Producto actualizarProducto(int id);
    Producto actualizarStockProducto(int id);
    Producto listarProductos();
    Producto eliminarProducto(int id);

    ~Producto();
};
