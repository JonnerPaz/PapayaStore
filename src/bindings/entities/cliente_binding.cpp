#include <string>

#include "bindings/bindings.hpp"
#include "domain/entities/cliente/Cliente.entity.hpp"

void bind_cliente(py::module_& m)
{
    py::class_<Cliente, EntidadBase>(m, "Cliente")
        .def(py::init<>())
        .def("get_cedula", [](const Cliente& c) { return std::string(c.getCedula()); })
        .def("set_cedula",
             [](Cliente& c, const std::string& cedula) { return c.setCedula(cedula.c_str()); })
        .def("get_telefono", [](const Cliente& c) { return std::string(c.getTelefono()); })
        .def("set_telefono",
             [](Cliente& c, const std::string& telefono) {
                 return c.setTelefono(telefono.c_str());
             })
        .def("get_email", [](const Cliente& c) { return std::string(c.getEmail()); })
        .def("set_email",
             [](Cliente& c, const std::string& email) { return c.setEmail(email.c_str()); })
        .def("get_direccion", [](const Cliente& c) { return std::string(c.getDireccion()); })
        .def("set_direccion",
             [](Cliente& c, const std::string& direccion) {
                 return c.setDireccion(direccion.c_str());
             })
        .def("get_total_compras", &Cliente::getTotalCompras)
        .def("set_total_compras", &Cliente::setTotalCompras)
        .def("get_cantidad", &Cliente::getCantidad)
        .def("set_cantidad", &Cliente::setCantidad)
        .def("get_cantidad_transacciones", &Cliente::getCantidadTransacciones)
        .def("set_cantidad_transacciones", &Cliente::setCantidadTransacciones)
        .def("agregar_transaccion_id", &Cliente::agregarTransaccionId)
        .def("remover_transaccion_id", &Cliente::removerTransaccionId)
        .def("set_transaccion_id_en_indice", &Cliente::setTransaccionIdEnIndice)
        .def("set_historial_id_en_indice", &Cliente::setHistorialIdEnIndice)
        .def("get_transacciones_ids", [](const Cliente& c) {
            const int count = c.getCantidadTransacciones();
            py::list out;
            const int* ids = c.getTransaccionesIds();
            for (int i = 0; i < count; ++i) {
                out.append(ids[i]);
            }
            return out;
        })
        .def("get_historial_ids", [](const Cliente& c) {
            const int count = c.getCantidad();
            py::list out;
            const int* ids = c.getHistorialIds();
            for (int i = 0; i < count; ++i) {
                out.append(ids[i]);
            }
            return out;
        });
}
