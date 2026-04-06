#pragma once
#include "domain/HeaderFile.hpp"
#include "domain/repositories/IProductoRepository.hpp"
#include "infrastructure/datasource/FSBaseRepository.hpp"

class FSProductoRepository : public IProductoRepository
{
   private:
    FSBaseRepository<Producto> baseRepository;

   public:
    FSProductoRepository();

    std::variant<Producto, std::string> leerPorId(int id) override;
    std::variant<bool, std::string> guardar(const Producto& entidad) override;
    std::variant<bool, std::string> actualizar(int id, const Producto& entidad) override;
    std::variant<bool, std::string> eliminarLogicamente(int id) override;
    std::variant<HeaderFile, std::string> obtenerEstadisticas() override;
};
