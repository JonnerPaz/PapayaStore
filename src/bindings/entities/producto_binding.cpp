#include <format>
#include <string>

#include "bindings/bindings.hpp"
#include "domain/entities/producto/producto.entity.hpp"

void bind_producto(py::module_& m)
{
    py::class_<Producto, EntidadBase>(m, "Producto")
        .def(py::init<>())
        .def("get_precio", &Producto::getPrecio)
        .def("set_precio", &Producto::setPrecio)
        .def("get_stock", &Producto::getStock)
        .def("set_stock", &Producto::setStock)
        .def("get_id_proveedor", &Producto::getIdProveedor)
        .def("set_id_proveedor", &Producto::setIdProveedor)
        .def("get_stock_minimo", &Producto::getStockMinimo)
        .def("set_stock_minimo", &Producto::setStockMinimo)
        .def("get_total_vendido", &Producto::getTotalVendido)
        .def("set_total_vendido", &Producto::setTotalVendido)
        .def("get_codigo", [](const Producto& p) { return std::string(p.getCodigo()); })
        .def("set_codigo",
             [](Producto& p, const std::string& codigo) { p.setCodigo(codigo.c_str()); })
        .def("get_descripcion", [](const Producto& p) { return std::string(p.getDescripcion()); })
        .def("set_descripcion",
             [](Producto& p, const std::string& desc) { p.setDescripcion(desc.c_str()); })
        .def("__repr__", [](const Producto& p) {
            return std::format("Producto({}, {})", p.getNombre(), p.getId());
        });
}
