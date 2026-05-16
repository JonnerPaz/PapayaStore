#pragma once
#include <string>
#include <variant>

#include "domain/HeaderFile.hpp"
#include "domain/entities/proveedor/Proveedor.entity.hpp"

class IProveedorRepository
{
   public:
    virtual std::variant<Proveedor, std::string> leerPorId(int id) = 0;
    virtual std::variant<Proveedor, std::string> leerPorNombre(const std::string& nombre) = 0;
    virtual std::variant<bool, std::string> guardar(const Proveedor& entidad) = 0;
    virtual std::variant<bool, std::string> actualizar(int id, const Proveedor& entidad) = 0;
    virtual std::variant<bool, std::string> eliminarLogicamente(int id) = 0;
    virtual std::variant<HeaderFile, std::string> obtenerEstadisticas() = 0;
    virtual ~IProveedorRepository() = default;
};
