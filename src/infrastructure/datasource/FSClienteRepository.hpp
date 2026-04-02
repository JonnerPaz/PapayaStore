#pragma once
#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"
#include "domain/repositories/IClienteRepository.hpp"
#include "infrastructure/datasource/FSBaseRepository.hpp"

class FSClienteRepository : public IClienteRepository, private FSBaseRepository<Cliente>
{
   public:
    FSClienteRepository() : FSBaseRepository<Cliente>(Constants::PATHS::CLIENTES_PATH) {}

    std::variant<Cliente, std::string> leerPorId(int id) override { return leerTemplate(id); }

    std::variant<bool, std::string> guardar(const Cliente& entidad) override
    {
        return guardarTemplate(entidad);
    }

    std::variant<bool, std::string> actualizar(int id, const Cliente& entidad) override
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
