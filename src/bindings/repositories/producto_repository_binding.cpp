#include "bindings/bindings.hpp"
#include "infrastructure/datasource/producto/FSProductoRepository.hpp"

void bind_producto_repository(py::module_& m)
{
    py::class_<FSProductoRepository>(m, "ProductoRepository")
        .def(py::init<>())
        .def("leer_por_id", &FSProductoRepository::leerPorId)
        .def("leer_por_nombre", &FSProductoRepository::leerPorNombre)
        .def("guardar", &FSProductoRepository::guardar)
        .def("actualizar", &FSProductoRepository::actualizar)
        .def("eliminar_logicamente", &FSProductoRepository::eliminarLogicamente)
        .def("obtener_estadisticas", &FSProductoRepository::obtenerEstadisticas);
}
