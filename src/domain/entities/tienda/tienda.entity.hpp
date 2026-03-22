#include "domain/entities/entidad.entity.hpp"
#include <chrono>

using std::chrono::system_clock;
using std::chrono::time_point;

class Tienda : Entidad {
  private:
    char rif[20];
    int totalProductosActivos;
    int totalProveedoresActivos;
    int totalClientesActivos;
    int totalTransaccionesActivas;
    float montoTotalVentas;
    float montoTotalCompras;

  public:
    Tienda(int id, const char* nombre, const char* rif, int totalProductosActivos,
           int totalProveedoresActivos, bool eliminado, int totalClientesActivos,
           time_point<system_clock> fechaCreacion, int montoTotalVentas, int montoTotalCompras,
           int totalTransaccionesActivas, time_point<system_clock> fechaUltimaModificacion);

    ~Tienda() = default;
};
