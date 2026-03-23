#include "Cliente.entity.hpp"
#include "domain/entities/entidad.entity.hpp"
#include <chrono>

using std::chrono::system_clock;
using std::chrono::time_point;

Cliente::Cliente(int id, const char* nombre, const char* cedula, const char* telefono,
                 const char* email, char* direccion, bool eliminado, char* fechaRegistro,
                 time_point<system_clock> fechaCreacion,
                 time_point<system_clock> fechaUltimaModificacion)
    : Entidad(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion) {
}
