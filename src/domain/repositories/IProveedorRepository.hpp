#pragma once
#include "domain/entities/ArchivoStats.hpp"
#include "domain/entities/proveedor/Proveedor.entity.hpp"
#include <string>
#include <variant>

class IProveedorRepository {
  public:
    virtual std::variant<Proveedor, std::string> leerPorId(int id) = 0;
    virtual std::variant<bool, std::string> guardar(const Proveedor& entidad) = 0;
    virtual std::variant<bool, std::string> actualizar(int id, const Proveedor& entidad) = 0;
    virtual std::variant<bool, std::string> eliminarLogicamente(int id) = 0;
    virtual std::variant<ArchivoStats, std::string> obtenerEstadisticas() = 0;
    virtual ~IProveedorRepository() = default;
};
