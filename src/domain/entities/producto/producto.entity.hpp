#pragma once
#include <chrono>
#include <cstring>
#include <ctime>
#include <string>
#include <variant>

#include "domain/entities/entidad.entity.hpp"

using namespace std::chrono;
using std::chrono::system_clock;
using std::chrono::time_point;

class Producto : public EntidadBase
{
   private:
    char codigo[20] = "";        // Código del producto (ej: "PROD-001")
    char descripcion[200] = "";  // Descripción del producto
    char fechaRegistro[11] = "";
    float precio = 0.0f;  // Precio unitario
    int stock = 0;        // Cantidad en inventario
    // llaves foraneas
    int idProveedor = 0;
    // estadisticas
    int stockMinimo = 0;
    int totalVendido = 0;

    void copiarCadenaSeguro(char* destino, size_t capacidad, const char* fuente)
    {
        if (capacidad == 0) return;

        if (fuente == nullptr) {
            destino[0] = '\0';
            return;
        }

        std::strncpy(destino, fuente, capacidad - 1);
        destino[capacidad - 1] = '\0';
    }

   public:
    Producto()
        : EntidadBase(0, "", false, std::chrono::system_clock::now(),
                      std::chrono::system_clock::now()),
          fechaRegistro{},
          precio(0.0f),
          stock(0),
          idProveedor(0),
          stockMinimo(0),
          totalVendido(0)
    {
    }

    Producto(int id, const char* nombre, const std::string& codigo, const char* descripcion,
             bool eliminado, time_point<system_clock> fechaCreacion,
             time_point<system_clock> fechaUltimaModificacion);

    float getPrecio() const { return precio; }

    int getStock() const { return stock; }

    int getIdProveedor() const { return idProveedor; }

    int getStockMinimo() const { return stockMinimo; }

    int getTotalVendido() const { return totalVendido; }

    const char* getCodigo() const { return codigo; }

    const char* getDescripcion() const { return descripcion; }

    const char* getFechaRegistro() const { return fechaRegistro; }

    void setId(int nuevoId)
    {
        if (nuevoId < 0) {
            // TODO: probar si esto funciona
            throw std::invalid_argument("Id inválido: debe ser mayor o igual a 0.");
        }
        m_id = nuevoId;
    }

    void setNombre(const char* nuevoNombre)
    {
        if (nuevoNombre == nullptr) {
            // TODO: probar si esto funciona
            throw std::invalid_argument("Nombre inválido: debe ser una cadena de caracteres.");
        }
        copiarCadenaSeguro(this->m_nombre, 50, nuevoNombre);
    }

    void setPrecio(float nuevoPrecio)
    {
        if (nuevoPrecio < 0) {
            // TODO: probar si esto funciona
            throw std::invalid_argument("Precio inválido: debe ser mayor o igual a 0.");
        }
        precio = nuevoPrecio;
    }

    void setStock(int nuevoStock)
    {
        if (nuevoStock < 0) {
            // TODO: probar si esto funciona
            throw std::invalid_argument("Stock inválido: debe ser mayor o igual a 0.");
        }
        stock = nuevoStock;
    }

    void setIdProveedor(int nuevoIdProveedor)
    {
        if (nuevoIdProveedor < 0) {
            // TODO: probar si esto funciona
            throw std::invalid_argument("IdProveedor inválido: debe ser mayor o igual a 0.");
        }
        idProveedor = nuevoIdProveedor;
    }

    void setStockMinimo(int nuevoStockMinimo)
    {
        if (nuevoStockMinimo < 0) {
            // TODO: probar si esto funciona
            throw std::invalid_argument("StockMinimo inválido: debe ser mayor o igual a 0.");
        }
        stockMinimo = nuevoStockMinimo;
    }

    void setTotalVendido(int nuevoTotalVendido)
    {
        if (nuevoTotalVendido < 0) {
            // TODO: probar si esto funciona
            throw std::invalid_argument("TotalVendido inválido: debe ser mayor o igual a 0.");
        }
        totalVendido = nuevoTotalVendido;
    }

    void setCodigo(const char* nuevoCodigo)
    {
        copiarCadenaSeguro(codigo, sizeof(codigo), nuevoCodigo);
    }

    void setDescripcion(const char* nuevaDescripcion)
    {
        copiarCadenaSeguro(descripcion, sizeof(descripcion), nuevaDescripcion);
    }

    void setFechaRegistro(const char* nuevaFechaRegistro)
    {
        copiarCadenaSeguro(fechaRegistro, sizeof(fechaRegistro), nuevaFechaRegistro);
    }

    ~Producto() = default;
};
