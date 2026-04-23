#include "bindings/bindings.hpp"
#include "infrastructure/datasource/cliente/FSClienteRepository.hpp"

void bind_cliente_repository(py::module_& m)
{
    py::class_<FSClienteRepository>(m, "ClienteRepository")
        .def(py::init<>())
        .def("leer_por_id", &FSClienteRepository::leerPorId)
        .def("leer_por_nombre", &FSClienteRepository::leerPorNombre)
        .def("guardar", &FSClienteRepository::guardar)
        .def("actualizar", &FSClienteRepository::actualizar)
        .def("eliminar_logicamente", &FSClienteRepository::eliminarLogicamente)
        .def("obtener_estadisticas", &FSClienteRepository::obtenerEstadisticas);
}
