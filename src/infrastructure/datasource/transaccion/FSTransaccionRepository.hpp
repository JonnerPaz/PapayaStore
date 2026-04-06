#pragma once
#include "domain/HeaderFile.hpp"
#include "domain/repositories/ITransaccionRepository.hpp"
#include "infrastructure/datasource/FSBaseRepository.hpp"

class FSTransaccionRepository : public ITransaccionRepository
{
   private:
    FSBaseRepository<Transaccion> baseRepository;

   public:
    FSTransaccionRepository();

    std::variant<Transaccion, std::string> leerPorId(int id) override;
    std::variant<bool, std::string> guardar(const Transaccion& entidad) override;
    std::variant<bool, std::string> actualizar(int id, const Transaccion& entidad) override;
    std::variant<bool, std::string> eliminarLogicamente(int id) override;
    std::variant<HeaderFile, std::string> obtenerEstadisticas() override;
};
