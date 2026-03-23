#include "domain/entities/producto/producto.entity.hpp"
#include <cstring>
#include <iostream>

#include "domain/constants.hpp"

using namespace std;
using namespace Constants::ASCII_CODES;
using namespace Constants::PATHS;

Producto::Producto(int id, const char* nombre, const std::string& codigo, const char* descripcion,
                   bool eliminado, time_point<system_clock> fechaCreacion,
                   time_point<system_clock> fechaUltimaModificacion)
    : Entidad(id, nombre, eliminado, fechaCreacion, fechaUltimaModificacion) {
    this->setId(id);
    this->setNombre(nombre);
    strncpy(this->codigo, codigo.c_str(), 20);
    strncpy(this->descripcion, descripcion, 200);
    this->fechaRegistro[0] = '\0';
    this->precio = 0;
    this->stock = 0;
    this->idProveedor = 0;
    this->stockMinimo = 0;
    this->totalVendido = 0;
    this->eliminado = false;
}
