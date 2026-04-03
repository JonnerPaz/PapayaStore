#pragma once
#include <chrono>

#include "domain/entities/entidad.entity.hpp"

using namespace std::chrono;
using std::chrono::system_clock;
using std::chrono::time_point;

class Producto : public EntidadBase
{
   private:
    char m_codigo[20]{0};
    char m_descripcion[200]{};  // Descripción del producto
    float m_precio{0.0f};       // Precio unitario
    int m_stock{0};             // Cantidad en inventario
    // llaves foraneas
    int m_idProveedor{0};
    // estadisticas
    int m_stockMinimo{0};
    int m_totalVendido{0};

   public:
    Producto();

    Producto(int id, const char* nombre, bool eliminado, time_point<system_clock> fechaCreacion,
             time_point<system_clock> fechaUltimaModificacion, const char* codigo,
             const char* descripcion, float precio, int stock, int idProveedor, int stockMinimo,
             int totalVendido);

    float getPrecio() const { return m_precio; }

    bool setPrecio(float nuevoPrecio);

    int getStock() const { return m_stock; }

    bool setStock(int nuevoStock);

    int getIdProveedor() const { return m_idProveedor; }

    bool setIdProveedor(int nuevoIdProveedor);

    int getStockMinimo() const { return m_stockMinimo; }

    bool setStockMinimo(int nuevoStockMinimo);

    int getTotalVendido() const { return m_totalVendido; }

    bool setTotalVendido(int nuevoTotalVendido);

    const char* getCodigo() const { return m_codigo; }

    bool setCodigo(const char* nuevoCodigo);

    const char* getDescripcion() const { return m_descripcion; }

    bool setDescripcion(const char* nuevaDescripcion);

    ~Producto() = default;
};
