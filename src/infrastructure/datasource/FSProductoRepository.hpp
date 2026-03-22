#pragma once
#include "domain/constants.hpp"
#include "domain/repositories/IProductoRepository.hpp"
#include "infrastructure/datasource/FSBaseRepository.hpp"

class FSProductoRepository : public IProductoRepository, private FSBaseRepository<Producto> {
  public:
    FSProductoRepository() : FSBaseRepository<Producto>(Constants::PATHS::PRODUCTOS_PATH) {
    }

    std::variant<Producto, std::string> leerPorId(int id) override {
        return leerTemplate(id);
    }

    std::variant<bool, std::string> guardar(const Producto& entidad) override {
        return guardarTemplate(entidad);
    }

    std::variant<bool, std::string> actualizar(int id, const Producto& entidad) override {
        return actualizarTemplate(id, entidad);
    }

    std::variant<bool, std::string> eliminarLogicamente(int id) override {
        return eliminarLogicamenteTemplate(id);
    }
};
