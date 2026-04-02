#include "entidad.entity.hpp"

// Clase abstracta
EntidadBase::EntidadBase(int id, const char* nombre, bool eliminado,
                         time_point<system_clock> fechaCreacion,
                         time_point<system_clock> fechaUltimaModificacion)
{
    this->m_id = id;
    strcpy(this->m_nombre, nombre);
    this->m_eliminado = eliminado;
    this->m_fechaCreacion = fechaCreacion;
    this->m_fechaUltimaModificacion = fechaUltimaModificacion;
}

EntidadBase::~EntidadBase() {}
