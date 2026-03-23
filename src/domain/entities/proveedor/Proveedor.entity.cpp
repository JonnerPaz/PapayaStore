#include "Proveedor.entity.hpp"
#include "domain/entities/entidad.entity.hpp"
#include <chrono>

using std::chrono::system_clock;
using std::chrono::time_point;

Proveedor::Proveedor(char* telefono, char* email, char* direccion, char* fechaRegistro, int id,
                     char* nombre, bool eliminado, time_point<system_clock> fechaCreacion,
                     time_point<system_clock> fechaUltimaModificacion)
    : EntidadBase(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion) {

    this->setEliminado(eliminado);
    this->setId(id);
    this->setNombre(nombre);
    this->setFechaCreacion(fechaCreacion);
    this->setTelefono(telefono);
    this->setEmail(email);
    strcpy(this->rif, rif);
}
