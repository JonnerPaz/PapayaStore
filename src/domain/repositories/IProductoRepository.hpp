#pragma once
#include "domain/entities/producto/producto.entity.hpp"
#include <string>
#include <variant>

class IProductoRepository {
  public:
    virtual std::variant<Producto, std::string> leerPorId(int id) = 0;
    virtual std::variant<bool, std::string> guardar(const Producto& entidad) = 0;
    virtual std::variant<bool, std::string> actualizar(int id, const Producto& entidad) = 0;
    virtual std::variant<bool, std::string> eliminarLogicamente(int id) = 0;
    virtual ~IProductoRepository() = default;
};
