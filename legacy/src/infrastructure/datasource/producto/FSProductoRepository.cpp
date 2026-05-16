#include "FSProductoRepository.hpp"

#include "domain/constants.hpp"

FSProductoRepository::FSProductoRepository() : baseRepository(Constants::PATHS::PRODUCTOS_PATH) {}

std::variant<Producto, std::string> FSProductoRepository::leerPorId(int id)
{
    return baseRepository.leerTemplate(id);
}

std::variant<Producto, std::string> FSProductoRepository::leerPorNombre(const std::string& nombre)
{
    return baseRepository.leerPorNombreTemplate(nombre);
}

std::variant<bool, std::string> FSProductoRepository::guardar(const Producto& entidad)
{
    return baseRepository.guardarTemplate(entidad);
}

std::variant<bool, std::string> FSProductoRepository::actualizar(int id, const Producto& entidad)
{
    return baseRepository.actualizarTemplate(id, entidad);
}

std::variant<bool, std::string> FSProductoRepository::eliminarLogicamente(int id)
{
    return baseRepository.eliminarLogicamenteTemplate(id);
}

std::variant<HeaderFile, std::string> FSProductoRepository::obtenerEstadisticas()
{
    return baseRepository.obtenerEstadisticasTemplate();
}
