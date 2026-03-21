#include <ctime>
#include <string.h>

class Tienda {
  private:
    int id;
    char nombre[100];
    char rif[20];
    int totalProductosActivos;
    int totalProveedoresActivos;
    int totalClientesActivos;
    int totalTransaccionesActivas;
    float montoTotalVentas;
    float montoTotalCompras;
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;

  public:
    Tienda(int id, const char* nombre, const char* rif, int totalProductosActivos,
           int totalProveedoresActivos, bool eliminado = false, int totalClientesActivos = 0,
           time_t fechaCreacion = time(nullptr), int montoTotalVentas = 0,
           int montoTotalCompras = 0, int totalTransaccionesActivas = 0,
           time_t fechaUltimaModificacion = time(nullptr)) {
        this->id = id;
        strncpy(this->nombre, nombre, 100);
        strncpy(this->rif, rif, 20);
        strncpy(this->rif, rif, 20);
        this->totalProductosActivos = totalProductosActivos;
        this->totalProveedoresActivos = totalProveedoresActivos;
        this->totalClientesActivos = totalClientesActivos;
        this->totalTransaccionesActivas = totalTransaccionesActivas;
        this->montoTotalVentas = montoTotalVentas;
        this->montoTotalCompras = montoTotalCompras;
        this->eliminado = eliminado;
        this->fechaCreacion = fechaCreacion;
        this->fechaUltimaModificacion = fechaUltimaModificacion;
    };

    void initTienda();
    virtual void mostrarResumenTienda() = 0;
};
