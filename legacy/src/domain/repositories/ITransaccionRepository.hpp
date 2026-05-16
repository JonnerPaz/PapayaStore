#pragma once
#include <string>
#include <variant>

#include "domain/HeaderFile.hpp"
#include "domain/entities/transaccion/transaccion.entity.hpp"

class ITransaccionRepository
{
   public:
    virtual std::variant<Transaccion, std::string> leerPorId(int id) = 0;
    virtual std::variant<Transaccion, std::string> leerPorNombre(const std::string& nombre) = 0;
    virtual std::variant<bool, std::string> guardar(const Transaccion& entidad) = 0;
    virtual std::variant<bool, std::string> actualizar(int id, const Transaccion& entidad) = 0;
    virtual std::variant<bool, std::string> eliminarLogicamente(int id) = 0;
    virtual std::variant<HeaderFile, std::string> obtenerEstadisticas() = 0;
    virtual ~ITransaccionRepository() = default;
};
