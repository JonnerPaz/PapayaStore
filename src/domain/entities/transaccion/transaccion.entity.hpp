#pragma once
#include "domain/entities/entidad.entity.hpp"
#include <chrono>

enum TipoDeTransaccion { COMPRA, VENTA };
using std::chrono::system_clock;
using std::chrono::time_point;

class Transaccion : public EntidadBase {
  public:
    TipoDeTransaccion tipo;             // COMPRA o VENTA
    int idRelacionado;                  // ID del proveedor (compra) o cliente (venta)
    float total;                        // cantidad * precioUnitario
    time_point<system_clock> fecha[11]; // Formato: YYYY-MM-DD
    char descripcion[200];              // Notas adicionales (opcional)

    // Hasta 100 productos por transacción
    int productosIds[100];
    int cantidades[100];          // Cantidades de cada producto
    float preciosUnitarios[100];  // Precios unitarios de cada producto
    int cantidadTiposDeProductos; // Cuantos elementos de los arrays anteriores
                                  // se están usando

    Transaccion()
        : EntidadBase(0, "", false, std::chrono::system_clock::now(),
                      std::chrono::system_clock::now()),
          tipo(COMPRA), idRelacionado(0), total(0.0f), cantidadTiposDeProductos(0) {
        descripcion[0] = '\0';

        // init arrays
        for (int i = 0; i < 100; ++i) {
            productosIds[i] = 0;
            cantidades[i] = 0;
            preciosUnitarios[i] = 0.0f;
        }
    }
};
