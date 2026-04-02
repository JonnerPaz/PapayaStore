#include "Cliente.entity.hpp"

using std::chrono::system_clock;
using std::chrono::time_point;

Cliente::Cliente(int id, const char* nombre, const char* cedula, const char* telefono,
                 const char* email, const char* direccion, bool eliminado,
                 const char* fechaRegistro, time_point<system_clock> fechaCreacion,
                 time_point<system_clock> fechaUltimaModificacion)
    : EntidadBase(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion),
      cantidad(0),
      totalCompras(0.0f),
      cantidadTransacciones(0)
{
    setTelefono(telefono);
    setEmail(email);
    setDireccion(direccion);
    std::strncpy(this->cedula, cedula, sizeof(this->cedula) - 1);
    this->cedula[sizeof(this->cedula) - 1] = '\0';
    std::strncpy(this->fechaRegistro, fechaRegistro, sizeof(this->fechaRegistro) - 1);
    this->fechaRegistro[sizeof(this->fechaRegistro) - 1] = '\0';

    // init arrays
    for (int i = 0; i < 100; ++i) {
        historialIds[i] = 0;
        transaccionesIds[i] = 0;
    }
}
