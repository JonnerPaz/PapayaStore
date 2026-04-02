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

void EntidadBase::copiarCadenaSeguro(char* destino, size_t capacidad, const char* fuente)
{
    if (capacidad == 0) return;

    if (fuente == nullptr) {
        destino[0] = '\0';
        return;
    }

    std::strncpy(destino, fuente, capacidad - 1);
    destino[capacidad - 1] = '\0';
}

bool EntidadBase::setId(int id)
{
    if (id < 0) {
        return false;
    }

    this->m_id = id;
    return true;
}

bool EntidadBase::setNombre(const char* nombre)
{
    if (nombre == nullptr) {
        // TODO: probar si esto funciona
        return false;
    }
    this->copiarCadenaSeguro(this->m_nombre, 100, nombre);
    return true;
}

bool EntidadBase::setEliminado(bool eliminado)
{
    this->m_eliminado = eliminado;
    return true;
}

bool EntidadBase::setFechaUltimaModificacion(time_point<system_clock> fechaUltimaModificacion)
{
    this->m_fechaUltimaModificacion = fechaUltimaModificacion;
    return true;
}

EntidadBase::~EntidadBase() {}
