#pragma once

#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bind_database_init(py::module_& m);
void bind_header_file(py::module_& m);
void bind_entidad_base(py::module_& m);

void bind_producto(py::module_& m);
void bind_proveedor(py::module_& m);
void bind_cliente(py::module_& m);
void bind_transaccion(py::module_& m);
void bind_tienda(py::module_& m);

void bind_producto_repository(py::module_& m);
void bind_proveedor_repository(py::module_& m);
void bind_cliente_repository(py::module_& m);
void bind_transaccion_repository(py::module_& m);
