#include <stdexcept>
#include <string>

#include "bindings/bindings.hpp"
#include "domain/entities/transaccion/transaccion.entity.hpp"

void bind_transaccion(py::module_& m)
{
    py::enum_<TipoDeTransaccion>(m, "TipoDeTransaccion")
        .value("COMPRA", TipoDeTransaccion::COMPRA)
        .value("VENTA", TipoDeTransaccion::VENTA)
        .export_values();

    py::class_<TransaccionDTO>(m, "TransaccionDTO")
        .def(py::init<>())
        .def_readwrite("producto_id", &TransaccionDTO::productoId)
        .def_readwrite("cantidad", &TransaccionDTO::cantidad)
        .def_readwrite("precio", &TransaccionDTO::precio);

    py::class_<Transaccion, EntidadBase>(m, "Transaccion")
        .def(py::init<>())
        .def("get_tipo_transaccion", &Transaccion::getTipoTransaccion)
        .def("set_tipo_transaccion", &Transaccion::setTipoTransaccion)
        .def("get_id_relacionado", &Transaccion::getIdRelacionado)
        .def("set_id_relacionado", &Transaccion::setIdRelacionado)
        .def("get_total", &Transaccion::getTotal)
        .def("set_total", &Transaccion::setTotal)
        .def("get_descripcion",
             [](const Transaccion& t) { return std::string(t.getDescripcion()); })
        .def("set_descripcion",
             [](Transaccion& t, const std::string& descripcion) {
                 return t.setDescripcion(descripcion.c_str());
             })
        .def("get_productos_totales", &Transaccion::getProductosTotales)
        .def("set_productos_totales", &Transaccion::setProductosTotales)
        .def("set_producto", &Transaccion::setProducto)
        .def("set_producto_en_indice", &Transaccion::setProductoEnIndice)
        .def("get_producto_en_indice", [](const Transaccion& t, int index) {
            TransaccionDTO dto{};
            if (!t.getProductoEnIndice(index, dto)) {
                throw std::out_of_range("indice de producto fuera de rango");
            }
            return dto;
        })
        .def("get_productos", [](const Transaccion& t) {
            py::list out;
            const int count = t.getProductosTotales();
            for (int i = 0; i < count; ++i) {
                TransaccionDTO dto{};
                if (t.getProductoEnIndice(i, dto)) {
                    out.append(dto);
                }
            }
            return out;
        });
}
