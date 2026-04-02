#pragma once
#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"
#include "domain/repositories/IProveedorRepository.hpp"
#include "infrastructure/datasource/FSBaseRepository.hpp"

class FSProveedorRepository : public IProveedorRepository, private FSBaseRepository<Proveedor>
{
   public:
    FSProveedorRepository() : FSBaseRepository<Proveedor>(Constants::PATHS::PROVEEDORES_PATH) {}

    std::variant<Proveedor, std::string> leerPorId(int id) override { return leerTemplate(id); }

    std::variant<bool, std::string> guardar(const Proveedor& entidad) override
    {
        return guardarTemplate(entidad);
    }

    std::variant<bool, std::string> actualizar(int id, const Proveedor& entidad) override
    {
        return actualizarTemplate(id, entidad);
    }

    std::variant<bool, std::string> eliminarLogicamente(int id) override
    {
        return eliminarLogicamenteTemplate(id);
    }

    std::variant<HeaderFile, std::string> obtenerEstadisticas() override
    {
        return obtenerEstadisticasTemplate();
    }
};
