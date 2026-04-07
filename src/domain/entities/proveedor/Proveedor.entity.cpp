#include "Proveedor.entity.hpp"

#include <sstream>

Proveedor::Proveedor()
    : EntidadBase(0, "", false, std::chrono::system_clock::now(), std::chrono::system_clock::now())
{
}

Proveedor::Proveedor(int id, const char* nombre, bool eliminado,
                     time_point<system_clock> fechaCreacion,
                     time_point<system_clock> fechaUltimaModificacion, const char* rif,
                     int productosIds[], int cantidadProductos, const char* telefono,
                     const char* email, const char* direccion, int cantidad, int historialIds[])
    : EntidadBase(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion)
{
    this->setCantidadProductos(cantidadProductos);
    for (int i = 0; i < cantidadProductos && i < 100; ++i) {
        this->setProductoIdEnIndice(i, productosIds[i]);
    }

    this->setTelefono(telefono);
    this->setEmail(email);
    this->setRif(rif);
    this->setDireccion(direccion);

    this->setCantidad(cantidad);
    for (int i = 0; i < cantidad && i < 100; ++i) {
        this->setHistorialIdEnIndice(i, historialIds[i]);
    }
}

bool Proveedor::validarEmail(const char* email)
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

bool Proveedor::validarFecha(const char* fecha)
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
    std::chrono::year_month_day ymd{std::chrono::year{y}, std::chrono::month{(unsigned) m},
                                    std::chrono::day{(unsigned) d}};
    return ymd.ok();
}

bool Proveedor::setTelefono(const char* telefono)
{
    if (telefono == nullptr) {
        return false;
    }

    if (!EntidadBase::copiarCadenaSeguro(this->m_telefono, sizeof(this->m_telefono), telefono)) {
        return false;
    }

    return true;
}

bool Proveedor::setEmail(const char* email)
{
    if (email == nullptr) {
        return false;
    }

    if (!validarEmail(email)) {
        return false;
    }

    if (!EntidadBase::copiarCadenaSeguro(this->m_email, sizeof(this->m_email), email)) {
        return false;
    }

    return true;
}

bool Proveedor::setRif(const char* rif)
{
    if (rif == nullptr) {
        return false;
    }

    return EntidadBase::copiarCadenaSeguro(this->m_rif, sizeof(this->m_rif), rif);
}

bool Proveedor::setDireccion(const char* direccion)
{
    if (direccion == nullptr) {
        return false;
    }

    if (!EntidadBase::copiarCadenaSeguro(this->m_direccion, sizeof(this->m_direccion), direccion)) {
        return false;
    }
    return true;
}

bool Proveedor::setProductoIdEnIndice(int index, int productoId)
{
    if (index < 0 || index >= 100 || productoId < 0) {
        return false;
    }

    this->m_productosIds[index] = productoId;
    return true;
}

bool Proveedor::setCantidadProductos(int cantidadProductos)
{
    if (cantidadProductos < 0 || cantidadProductos > 100) {
        return false;
    }

    this->m_cantidadProductos = cantidadProductos;
    return true;
}

bool Proveedor::setCantidad(int cantidad)
{
    if (cantidad < 0 || cantidad > 100) {
        return false;
    }

    this->m_cantidad = cantidad;
    return true;
}

bool Proveedor::setHistorialIdEnIndice(int index, int historialId)
{
    if (index < 0 || index >= 100 || historialId < 0) {
        return false;
    }

    this->m_historialIds[index] = historialId;
    return true;
}
