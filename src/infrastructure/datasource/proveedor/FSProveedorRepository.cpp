#include "FSProveedorRepository.hpp"

#include "domain/constants.hpp"

FSProveedorRepository::FSProveedorRepository() : baseRepository(Constants::PATHS::PROVEEDORES_PATH)
{
}

std::variant<Proveedor, std::string> FSProveedorRepository::leerPorId(int id)
{
    return baseRepository.leerTemplate(id);
}

std::variant<Proveedor, std::string> FSProveedorRepository::leerPorNombre(const std::string& nombre)
{
    return baseRepository.leerPorNombreTemplate(nombre);
}

std::variant<bool, std::string> FSProveedorRepository::guardar(const Proveedor& entidad)
{
    return baseRepository.guardarTemplate(entidad);
}

std::variant<bool, std::string> FSProveedorRepository::actualizar(int id, const Proveedor& entidad)
{
    return baseRepository.actualizarTemplate(id, entidad);
}

std::variant<bool, std::string> FSProveedorRepository::eliminarLogicamente(int id)
{
    return baseRepository.eliminarLogicamenteTemplate(id);
}

std::variant<HeaderFile, std::string> FSProveedorRepository::obtenerEstadisticas()
{
    return baseRepository.obtenerEstadisticasTemplate();
}
