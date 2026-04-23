#include <pybind11/pybind11.h>

#include "bindings/bindings.hpp"

namespace py = pybind11;

PYBIND11_MODULE(papaya_backend, m)
{
    m.doc() = "PapayaStore Backend Bindings";

    bind_database_init(m);
    bind_header_file(m);
    bind_entidad_base(m);
    bind_database_admin(m);

    bind_producto(m);
    bind_proveedor(m);
    bind_cliente(m);
    bind_transaccion(m);
    bind_tienda(m);

    bind_producto_repository(m);
    bind_proveedor_repository(m);
    bind_cliente_repository(m);
    bind_transaccion_repository(m);

    bind_papaya_context(m);
}
