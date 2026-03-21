#pragma once
#include <chrono>

enum TipoDeTransaccion { COMPRA, VENTA };

class Transaccion {
  private:
    int id;                 // Identificador único (autoincremental)
    TipoDeTransaccion tipo; // COMPRA o VENTA
    int idRelacionado;      // ID del proveedor (compra) o cliente (venta)
    float total;            // cantidad * precioUnitario
    char fecha[11];         // Formato: YYYY-MM-DD
    char descripcion[200];  // Notas adicionales (opcional)

    // Hasta 100 productos por transacción
    int productosIds[100];
    int cantidades[100];          // Cantidades de cada producto
    float preciosUnitarios[100];  // Precios unitarios de cada producto
    int cantidadTiposDeProductos; // Cuantos elementos de los arrays anteriores
                                  // se están usando

    // metadata
    bool eliminado;
    std::chrono::time_point<std::chrono::system_clock> fechaCreacion;
    std::chrono::time_point<std::chrono::system_clock> fechaUltimaModificacion;
};
