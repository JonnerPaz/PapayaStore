#pragma once
#include "domain/HeaderFile.hpp"
#include "domain/repositories/IClienteRepository.hpp"
#include "infrastructure/datasource/FSBaseRepository.hpp"

class FSClienteRepository : public IClienteRepository
{
   private:
    FSBaseRepository<Cliente> m_baseRepository;

   public:
    FSClienteRepository();

    std::variant<Cliente, std::string> leerPorId(int id) override;
    std::variant<Cliente, std::string> leerPorNombre(const std::string& nombre) override;

    std::variant<bool, std::string> guardar(const Cliente& entidad) override;

    std::variant<bool, std::string> actualizar(int id, const Cliente& entidad) override;

    std::variant<bool, std::string> eliminarLogicamente(int id) override;

    std::variant<HeaderFile, std::string> obtenerEstadisticas() override;
};
