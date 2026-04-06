#include "Cliente.entity.hpp"

#include <sstream>

Cliente::Cliente()
    : EntidadBase(0, "", false, system_clock::now(), system_clock::now()),
      m_cantidad(0),
      m_totalCompras(0.0f),
      m_cantidadTransacciones(0)
{
    m_telefono[0] = '\0';
    m_email[0] = '\0';
    m_direccion[0] = '\0';
    m_fechaRegistro[0] = '\0';
    m_cedula[0] = '\0';
    for (int i = 0; i < 100; ++i) {
        m_historialIds[i] = 0;
        m_transaccionesIds[i] = 0;
    }
}

Cliente::Cliente(int id, const char* nombre, const char* cedula, const char* telefono,
                 const char* email, const char* direccion, bool eliminado,
                 const char* fechaRegistro, time_point<system_clock> fechaCreacion,
                 time_point<system_clock> fechaUltimaModificacion)
    : EntidadBase(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion),
      m_cantidad(0),
      m_totalCompras(0.0f),
      m_cantidadTransacciones(0)
{
    this->setTelefono(telefono);
    this->setEmail(email);
    this->setDireccion(direccion);
    if (!EntidadBase::copiarCadenaSeguro(this->m_cedula, sizeof(this->m_cedula), cedula)) {
        throw std::runtime_error("Error al copiar la cedula");
    }

    // init arrays
    for (int i = 0; i < 100; ++i) {
        m_historialIds[i] = 0;
        m_transaccionesIds[i] = 0;
    }
}

bool Cliente::validarEmail(const char* email)
{
    int PosicionAt = -1;             // verificar y darle valor al "@"
    bool TienePuntoDespues = false;  // para comprobar que tenga "." despues del "@"
    int longitud = strlen(email);    // para que el ciclo for sepa hasta donde buscar

    // la funcion "for" para recorrer todo el email
    for (int i = 0; i < longitud; i++) {
        if (email[i] == '@') {
            // verificacion si ya habia un @ y hay dos @
            if (PosicionAt != -1) return false;
            PosicionAt = i;
        }
        // verificacion de si hay un '.' despues de ya tener el '@'
        if (PosicionAt != -1 && email[i] == '.') {
            if (i > PosicionAt + 1) {
                TienePuntoDespues = true;
            }
        }
    }

    // verificacion de los errores
    if (PosicionAt > 0 && PosicionAt < longitud - 1 && TienePuntoDespues) {
        return true;
    }

    return false;
}

bool Cliente::validarFecha(const char* fecha)
{
    if (fecha == nullptr) {
        return false;
    }

    // extraemos los datos de YYYY-MM-DD
    std::stringstream ss(fecha);
    int y, m, d;
    char dash1, dash2;

    // verificar si es correcto el formato
    if (!(ss >> y >> dash1 >> m >> dash2 >> d)) {
        return false;
    }

    // para ver que tenga los guiones donde van
    if (dash1 != '-' || dash2 != '-') {
        return false;
    }
    // year_month_day representa una fecha en el calendario civil
    std::chrono::year_month_day ymd{std::chrono::year{y}, std::chrono::month{(unsigned) m},
                                    std::chrono::day{(unsigned) d}};
    return ymd.ok();
}

bool Cliente::setCedula(const char* cedula)
{
    if (cedula == nullptr) {
        return false;
    }

    return EntidadBase::copiarCadenaSeguro(this->m_cedula, sizeof(this->m_cedula), cedula);
}

bool Cliente::setTelefono(const char* telefono)
{
    if (telefono == nullptr) {
        return false;
    }

    return EntidadBase::copiarCadenaSeguro(this->m_telefono, sizeof(this->m_telefono), telefono);
}

bool Cliente::setEmail(const char* email)
{
    if (email == nullptr) {
        return false;
    }

    if (!validarEmail(email)) {
        return false;
    }

    return EntidadBase::copiarCadenaSeguro(this->m_email, sizeof(this->m_email), email);
}

bool Cliente::setDireccion(const char* direccion)
{
    if (direccion == nullptr) {
        return false;
    }

    return EntidadBase::copiarCadenaSeguro(this->m_direccion, sizeof(this->m_direccion), direccion);
}

bool Cliente::setFechaRegistro(const char* fechaRegistro)
{
    if (fechaRegistro == nullptr || !validarFecha(fechaRegistro)) {
        return false;
    }

    return EntidadBase::copiarCadenaSeguro(this->m_fechaRegistro, sizeof(this->m_fechaRegistro),
                                           fechaRegistro);
}

bool Cliente::setTotalCompras(float totalCompras)
{
    if (totalCompras < 0) {
        return false;
    }

    this->m_totalCompras = totalCompras;
    return true;
}

bool Cliente::agregarTransaccionId(int idTransaccion)
{
    if (idTransaccion <= 0) {
        return false;
    }

    for (int i = 0; i < this->m_cantidadTransacciones; ++i) {
        if (this->m_transaccionesIds[i] == idTransaccion) {
            return true;
        }
    }

    if (this->m_cantidadTransacciones >= 100) {
        return false;
    }

    this->m_transaccionesIds[this->m_cantidadTransacciones] = idTransaccion;
    this->m_cantidadTransacciones += 1;
    return true;
}

bool Cliente::removerTransaccionId(int idTransaccion)
{
    if (idTransaccion <= 0 || this->m_cantidadTransacciones <= 0) {
        return false;
    }

    int index = -1;
    for (int i = 0; i < this->m_cantidadTransacciones; ++i) {
        if (this->m_transaccionesIds[i] == idTransaccion) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return false;
    }

    for (int i = index; i < this->m_cantidadTransacciones - 1; ++i) {
        this->m_transaccionesIds[i] = this->m_transaccionesIds[i + 1];
    }

    this->m_transaccionesIds[this->m_cantidadTransacciones - 1] = 0;
    this->m_cantidadTransacciones -= 1;
    return true;
}
