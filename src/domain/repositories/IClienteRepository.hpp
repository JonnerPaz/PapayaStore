#pragma once
#include "domain/entities/ArchivoStats.hpp"
#include "domain/entities/cliente/Cliente.entity.hpp"
#include <string>
#include <variant>

class IClienteRepository {
  public:
    virtual std::variant<Cliente, std::string> leerPorId(int id) = 0;
    virtual std::variant<bool, std::string> guardar(const Cliente& entidad) = 0;
    virtual std::variant<bool, std::string> actualizar(int id, const Cliente& entidad) = 0;
    virtual std::variant<bool, std::string> eliminarLogicamente(int id) = 0;
    virtual std::variant<ArchivoStats, std::string> obtenerEstadisticas() = 0;
    virtual ~IClienteRepository() = default;
};
