#include "entidad.entity.hpp"

// Clase abstracta
EntidadBase::EntidadBase(int id, const char* nombre, bool eliminado,
                         time_point<system_clock> fechaCreacion,
                         time_point<system_clock> fechaUltimaModificacion) {
    this->id = id;
    strcpy(this->nombre, nombre);
    this->eliminado = eliminado;
    this->fechaCreacion = fechaCreacion;
    this->fechaUltimaModificacion = fechaUltimaModificacion;
}

EntidadBase::~EntidadBase() {
}
