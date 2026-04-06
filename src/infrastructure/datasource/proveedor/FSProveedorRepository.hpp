#pragma once
#include "domain/HeaderFile.hpp"
#include "domain/repositories/IProveedorRepository.hpp"
#include "infrastructure/datasource/FSBaseRepository.hpp"

class FSProveedorRepository : public IProveedorRepository
{
   private:
    FSBaseRepository<Proveedor> baseRepository;

   public:
    FSProveedorRepository();

    std::variant<Proveedor, std::string> leerPorId(int id) override;

    std::variant<bool, std::string> guardar(const Proveedor& entidad) override;

    std::variant<bool, std::string> actualizar(int id, const Proveedor& entidad) override;

    std::variant<bool, std::string> eliminarLogicamente(int id) override;

    std::variant<HeaderFile, std::string> obtenerEstadisticas() override;
};
