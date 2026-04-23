#include "bindings/bindings.hpp"
#include "infrastructure/datasource/transaccion/FSTransaccionRepository.hpp"

void bind_transaccion_repository(py::module_& m)
{
    py::class_<FSTransaccionRepository>(m, "TransaccionRepository")
        .def(py::init<>())
        .def("leer_por_id", &FSTransaccionRepository::leerPorId)
        .def("leer_por_nombre", &FSTransaccionRepository::leerPorNombre)
        .def("guardar", &FSTransaccionRepository::guardar)
        .def("actualizar", &FSTransaccionRepository::actualizar)
        .def("eliminar_logicamente", &FSTransaccionRepository::eliminarLogicamente)
        .def("obtener_estadisticas", &FSTransaccionRepository::obtenerEstadisticas);
}
