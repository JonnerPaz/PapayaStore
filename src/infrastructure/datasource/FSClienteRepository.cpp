#include "FSClienteRepository.hpp"

#include "domain/constants.hpp"

FSClienteRepository::FSClienteRepository() : m_baseRepository(Constants::PATHS::CLIENTES_PATH) {}

std::variant<Cliente, std::string> FSClienteRepository::leerPorId(int id)
{
    return m_baseRepository.leerTemplate(id);
}

std::variant<bool, std::string> FSClienteRepository::guardar(const Cliente& entidad)
{
    return m_baseRepository.guardarTemplate(entidad);
}

std::variant<bool, std::string> FSClienteRepository::actualizar(int id, const Cliente& entidad)
{
    return m_baseRepository.actualizarTemplate(id, entidad);
}

std::variant<bool, std::string> FSClienteRepository::eliminarLogicamente(int id)
{
    return m_baseRepository.eliminarLogicamenteTemplate(id);
}

std::variant<HeaderFile, std::string> FSClienteRepository::obtenerEstadisticas()
{
    return m_baseRepository.obtenerEstadisticasTemplate();
}
