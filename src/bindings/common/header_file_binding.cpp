#include "bindings/bindings.hpp"
#include "domain/HeaderFile.hpp"

void bind_header_file(py::module_& m)
{
    py::class_<HeaderFile>(m, "HeaderFile")
        .def_readwrite("proximo_id", &HeaderFile::proximoID)
        .def_readwrite("cantidad_registros", &HeaderFile::cantidadRegistros)
        .def_readwrite("registros_activos", &HeaderFile::registrosActivos)
        .def_readwrite("version", &HeaderFile::version);
}
