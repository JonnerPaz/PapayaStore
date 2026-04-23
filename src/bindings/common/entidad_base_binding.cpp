#include <string>

#include "bindings/bindings.hpp"
#include "domain/entities/entidad.entity.hpp"

void bind_entidad_base(py::module_& m)
{
    py::class_<EntidadBase>(m, "EntidadBase")
        .def("get_id", &EntidadBase::getId)
        .def("set_id", &EntidadBase::setId)
        .def("get_nombre", [](const EntidadBase& e) { return std::string(e.getNombre()); })
        .def("set_nombre",
             [](EntidadBase& e, const std::string& nombre) { e.setNombre(nombre.c_str()); })
        .def("get_eliminado", &EntidadBase::getEliminado)
        .def("set_eliminado", &EntidadBase::setEliminado)
        .def("get_fecha_creacion", &EntidadBase::getFechaCreacion)
        .def("set_fecha_creacion", &EntidadBase::setFechaCreacion)
        .def("get_fecha_ultima_modificacion", &EntidadBase::getFechaUltimaModificacion)
        .def("set_fecha_ultima_modificacion", &EntidadBase::setFechaUltimaModificacion);
}
