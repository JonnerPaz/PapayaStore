#pragma once
#include <string>
#include <variant>

#include "domain/HeaderFile.hpp"
#include "domain/entities/cliente/Cliente.entity.hpp"

class IClienteRepository
{
   public:
    virtual std::variant<Cliente, std::string> leerPorId(int id) = 0;
    virtual std::variant<bool, std::string> guardar(const Cliente& entidad) = 0;
    virtual std::variant<bool, std::string> actualizar(int id, const Cliente& entidad) = 0;
    virtual std::variant<bool, std::string> eliminarLogicamente(int id) = 0;
    virtual std::variant<HeaderFile, std::string> obtenerEstadisticas() = 0;
    virtual ~IClienteRepository() = default;
};
