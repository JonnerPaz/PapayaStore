#pragma once
#include <ctime>
#include <string.h>
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
    Producto(int id, const char* nombre, const std::string& codigo, const char* descripcion) {
        this->id = id;
        strncpy(this->nombre, nombre, 100);
        strncpy(this->codigo, codigo.c_str(), 20);
        strncpy(this->descripcion, descripcion, 200);
        this->fechaRegistro[0] = '\0';
        this->precio = 0;
        this->stock = 0;
        this->idProveedor = 0;
        this->stockMinimo = 0;
        this->totalVendido = 0;
        this->eliminado = false;
        this->fechaCreacion = time(nullptr);
        this->fechaUltimaModificacion = time(nullptr);
    }

    // Metodos
    Producto crearProducto();
    Producto buscarProducto(int id);
    Producto actualizarProducto(int id);
    Producto actualizarStockProducto(int id);
    Producto listarProductos();
    Producto eliminarProducto(int id);
};
