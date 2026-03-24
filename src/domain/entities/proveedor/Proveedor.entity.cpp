#include "Proveedor.entity.hpp"

using std::chrono::system_clock;
using std::chrono::time_point;

Proveedor::Proveedor(const char* telefono, const char* email, const char* rif,
                     const char* direccion, const char* fechaRegistro, int id, const char* nombre,
                     bool eliminado, time_point<system_clock> fechaCreacion,
                     time_point<system_clock> fechaUltimaModificacion)
    : EntidadBase(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion) {
    this->setTelefono(telefono);
    this->setEmail(email);
    std::strncpy(this->rif, rif, sizeof(this->rif) - 1);
    this->rif[sizeof(this->rif) - 1] = '\0';
    this->setDireccion(direccion);
    std::strncpy(this->fechaRegistro, fechaRegistro, sizeof(this->fechaRegistro) - 1);
    this->fechaRegistro[sizeof(this->fechaRegistro) - 1] = '\0';
}
