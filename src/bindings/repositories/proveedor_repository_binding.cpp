#include "bindings/bindings.hpp"
#include "infrastructure/datasource/proveedor/FSProveedorRepository.hpp"

void bind_proveedor_repository(py::module_& m)
{
    py::class_<FSProveedorRepository>(m, "ProveedorRepository")
        .def(py::init<>())
        .def("leer_por_id", &FSProveedorRepository::leerPorId)
        .def("leer_por_nombre", &FSProveedorRepository::leerPorNombre)
        .def("guardar", &FSProveedorRepository::guardar)
        .def("actualizar", &FSProveedorRepository::actualizar)
        .def("eliminar_logicamente", &FSProveedorRepository::eliminarLogicamente)
        .def("obtener_estadisticas", &FSProveedorRepository::obtenerEstadisticas);
}
