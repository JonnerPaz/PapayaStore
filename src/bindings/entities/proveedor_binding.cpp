#include <string>

#include "bindings/bindings.hpp"
#include "domain/entities/proveedor/Proveedor.entity.hpp"

void bind_proveedor(py::module_& m)
{
    py::class_<Proveedor, EntidadBase>(m, "Proveedor")
        .def(py::init<>())
        .def("get_rif", [](const Proveedor& p) { return std::string(p.getRif()); })
        .def("set_rif", [](Proveedor& p, const std::string& rif) { return p.setRif(rif.c_str()); })
        .def("get_telefono", [](const Proveedor& p) { return std::string(p.getTelefono()); })
        .def("set_telefono",
             [](Proveedor& p, const std::string& telefono) {
                 return p.setTelefono(telefono.c_str());
             })
        .def("get_email", [](const Proveedor& p) { return std::string(p.getEmail()); })
        .def("set_email",
             [](Proveedor& p, const std::string& email) { return p.setEmail(email.c_str()); })
        .def("get_direccion", [](const Proveedor& p) { return std::string(p.getDireccion()); })
        .def("set_direccion",
             [](Proveedor& p, const std::string& direccion) {
                 return p.setDireccion(direccion.c_str());
             })
        .def("get_cantidad", &Proveedor::getCantidad)
        .def("set_cantidad", &Proveedor::setCantidad)
        .def("get_cantidad_productos", &Proveedor::getCantidadProductos)
        .def("set_cantidad_productos", &Proveedor::setCantidadProductos)
        .def("set_producto_id_en_indice", &Proveedor::setProductoIdEnIndice)
        .def("set_historial_id_en_indice", &Proveedor::setHistorialIdEnIndice)
        .def("get_productos_ids", [](const Proveedor& p) {
            const int count = p.getCantidadProductos();
            py::list out;
            const int* ids = p.getProductosIds();
            for (int i = 0; i < count; ++i) {
                out.append(ids[i]);
            }
            return out;
        })
        .def("get_historial_ids", [](const Proveedor& p) {
            const int count = p.getCantidad();
            py::list out;
            const int* ids = p.getHistorialIds();
            for (int i = 0; i < count; ++i) {
                out.append(ids[i]);
            }
            return out;
        });
}
