#include "entidad.entity.hpp"

#include <string>

#include "domain/utils/utils.hpp"

// Clase abstracta
EntidadBase::EntidadBase(int id, const char* nombre, bool eliminado,
                         time_point<system_clock> fechaCreacion,
                         time_point<system_clock> fechaUltimaModificacion)
{
    this->m_id = id;
    if (!this->setNombre(nombre)) {
        this->m_nombre[0] = '\0';
    }
    this->m_eliminado = eliminado;
    this->m_fechaCreacion = fechaCreacion;
    this->m_fechaUltimaModificacion = fechaUltimaModificacion;
}

bool EntidadBase::copiarCadenaSeguro(char* destino, size_t capacidad, const char* fuente)
{
    if (capacidad == 0) return false;

    if (fuente == nullptr) {
        destino[0] = '\0';
        return false;
    }

    std::strncpy(destino, fuente, capacidad - 1);
    destino[capacidad - 1] = '\0';
    return true;
}

bool EntidadBase::setId(int id)
{
    if (id <= 0) {
        return false;
    }

    this->m_id = id;
    return true;
}

bool EntidadBase::setNombre(const char* nombre)
{
    if (nombre == nullptr) {
        return false;
    }

    const std::string sanitizedName = DomainUtils::sanitizeName(nombre);
    if (sanitizedName.empty()) {
        return false;
    }

    this->copiarCadenaSeguro(this->m_nombre, sizeof(this->m_nombre), sanitizedName.c_str());
    return true;
}

bool EntidadBase::setEliminado(bool eliminado)
{
    this->m_eliminado = eliminado;
    return true;
}

bool EntidadBase::setFechaCreacion(time_point<system_clock> fechaCreacion)
{
    this->m_fechaCreacion = fechaCreacion;
    return true;
}

bool EntidadBase::setFechaUltimaModificacion(time_point<system_clock> fechaUltimaModificacion)
{
    this->m_fechaUltimaModificacion = fechaUltimaModificacion;
    return true;
}

EntidadBase::~EntidadBase() {}
