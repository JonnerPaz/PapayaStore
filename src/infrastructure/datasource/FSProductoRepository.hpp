#pragma once
#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"
#include "domain/repositories/IProductoRepository.hpp"
#include "infrastructure/datasource/FSBaseRepository.hpp"

class FSProductoRepository : public IProductoRepository
{
   private:
    FSBaseRepository<Producto> baseRepository;

   public:
    FSProductoRepository() : baseRepository(Constants::PATHS::PRODUCTOS_PATH) {}

    std::variant<Producto, std::string> leerPorId(int id) override
    {
        return baseRepository.leerTemplate(id);
    }

    std::variant<bool, std::string> guardar(const Producto& entidad) override
    {
        return baseRepository.guardarTemplate(entidad);
    }

    std::variant<bool, std::string> actualizar(int id, const Producto& entidad) override
    {
        return baseRepository.actualizarTemplate(id, entidad);
    }

    std::variant<bool, std::string> eliminarLogicamente(int id) override
    {
        return baseRepository.eliminarLogicamenteTemplate(id);
    }

    std::variant<HeaderFile, std::string> obtenerEstadisticas() override
    {
        return baseRepository.obtenerEstadisticasTemplate();
    }
};
