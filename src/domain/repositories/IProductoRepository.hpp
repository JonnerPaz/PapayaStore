#pragma once
#include <string>
#include <variant>

#include "domain/HeaderFile.hpp"
#include "domain/entities/producto/producto.entity.hpp"

class IProductoRepository
{
   public:
    virtual std::variant<Producto, std::string> leerPorId(int id) = 0;
    virtual std::variant<Producto, std::string> leerPorNombre(const std::string& nombre) = 0;
    virtual std::variant<bool, std::string> guardar(const Producto& entidad) = 0;
    virtual std::variant<bool, std::string> actualizar(int id, const Producto& entidad) = 0;
    virtual std::variant<bool, std::string> eliminarLogicamente(int id) = 0;
    virtual std::variant<HeaderFile, std::string> obtenerEstadisticas() = 0;
    virtual ~IProductoRepository() = default;
};
