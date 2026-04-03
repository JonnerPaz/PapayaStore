#include "Cliente.entity.hpp"

#include <sstream>

using namespace std::chrono;

Cliente::Cliente()
    : EntidadBase(0, "", false, std::chrono::system_clock::now(), std::chrono::system_clock::now()),
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
    EntidadBase::copiarCadenaSeguro(this->m_cedula, sizeof(this->m_cedula), cedula);
    EntidadBase::copiarCadenaSeguro(this->m_fechaRegistro, sizeof(this->m_fechaRegistro),
                                    fechaRegistro);

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
    year_month_day ymd{year{y}, month{(unsigned) m}, day{(unsigned) d}};
    return ymd.ok();
}
