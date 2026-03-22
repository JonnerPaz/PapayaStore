#pragma once
#include "domain/entities/transaccion/transaccion.entity.hpp"
#include <string>
#include <variant>

class ITransaccionRepository {
  public:
    virtual std::variant<Transaccion, std::string> leerPorId(int id) = 0;
    virtual std::variant<bool, std::string> guardar(const Transaccion& entidad) = 0;
    virtual std::variant<bool, std::string> actualizar(int id, const Transaccion& entidad) = 0;
    virtual std::variant<bool, std::string> eliminarLogicamente(int id) = 0;
    virtual ~ITransaccionRepository() = default;
};
