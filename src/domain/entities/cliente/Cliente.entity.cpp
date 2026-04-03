#include "Cliente.entity.hpp"

#include <sstream>

using namespace std::chrono;

Cliente::Cliente()
    : EntidadBase(0, "", false, std::chrono::system_clock::now(), std::chrono::system_clock::now()),
      cantidad(0),
      totalCompras(0.0f),
      cantidadTransacciones(0)
{
    telefono[0] = '\0';
    email[0] = '\0';
    direccion[0] = '\0';
    fechaRegistro[0] = '\0';
    cedula[0] = '\0';
    for (int i = 0; i < 100; ++i) {
        historialIds[i] = 0;
        transaccionesIds[i] = 0;
    }
}

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
