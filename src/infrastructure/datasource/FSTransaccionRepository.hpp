#pragma once
#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"
#include "domain/repositories/ITransaccionRepository.hpp"
#include "infrastructure/datasource/FSBaseRepository.hpp"

class FSTransaccionRepository : public ITransaccionRepository
{
   public:
    FSTransaccionRepository() : baseRepository(Constants::PATHS::TRANSACCIONES_PATH)
    {
    }

    std::variant<Transaccion, std::string> leerPorId(int id) override
    {
        return baseRepository.leerTemplate(id);
    }

    std::variant<bool, std::string> guardar(const Transaccion& entidad) override
    {
        return baseRepository.guardarTemplate(entidad);
    }

    std::variant<bool, std::string> actualizar(int id, const Transaccion& entidad) override
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

   private:
    FSBaseRepository<Transaccion> baseRepository;
};
