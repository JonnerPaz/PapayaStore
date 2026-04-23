#include <string>

#include "bindings/bindings.hpp"
#include "domain/entities/tienda/tienda.entity.hpp"

void bind_tienda(py::module_& m)
{
    py::class_<Tienda, EntidadBase>(m, "Tienda")
        .def(py::init<>())
        .def("get_rif", [](const Tienda& t) { return std::string(t.getRif()); })
        .def("set_rif", [](Tienda& t, const std::string& rif) { return t.setRif(rif.c_str()); })
        .def("get_total_productos_activos", &Tienda::getTotalProductosActivos)
        .def("set_total_productos_activos", &Tienda::setTotalProductosActivos)
        .def("get_total_proveedores_activos", &Tienda::getTotalProveedoresActivos)
        .def("set_total_proveedores_activos", &Tienda::setTotalProveedoresActivos)
        .def("get_total_clientes_activos", &Tienda::getTotalClientesActivos)
        .def("set_total_clientes_activos", &Tienda::setTotalClientesActivos)
        .def("get_total_transacciones_activas", &Tienda::getTotalTransaccionesActivas)
        .def("set_total_transacciones_activas", &Tienda::setTotalTransaccionesActivas)
        .def("get_monto_total_ventas", &Tienda::getMontoTotalVentas)
        .def("set_monto_total_ventas", &Tienda::setMontoTotalVentas)
        .def("get_monto_total_compras", &Tienda::getMontoTotalCompras)
        .def("set_monto_total_compras", &Tienda::setMontoTotalCompras);
}
