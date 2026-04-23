#include "bindings/bindings.hpp"
#include "bindings/context/papaya_context.hpp"

void bind_papaya_context(py::module_& m)
{
    py::class_<PapayaContext>(m, "PapayaContext")
        .def(py::init<>())
        .def("init_database", &PapayaContext::init_database)
        .def_property_readonly(
            "productos", [](PapayaContext& ctx) -> FSProductoRepository& { return ctx.productos(); },
            py::return_value_policy::reference_internal)
        .def_property_readonly(
            "proveedores",
            [](PapayaContext& ctx) -> FSProveedorRepository& { return ctx.proveedores(); },
            py::return_value_policy::reference_internal)
        .def_property_readonly(
            "clientes", [](PapayaContext& ctx) -> FSClienteRepository& { return ctx.clientes(); },
            py::return_value_policy::reference_internal)
        .def_property_readonly(
            "transacciones",
            [](PapayaContext& ctx) -> FSTransaccionRepository& { return ctx.transacciones(); },
            py::return_value_policy::reference_internal)
        .def_property_readonly(
            "admin", [](PapayaContext& ctx) -> IDatabaseAdmin& { return ctx.admin(); },
            py::return_value_policy::reference_internal);
}
