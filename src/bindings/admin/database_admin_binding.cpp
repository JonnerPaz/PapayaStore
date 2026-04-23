#include "bindings/bindings.hpp"
#include "domain/repositories/IDatabaseAdmin.hpp"

void bind_database_admin(py::module_& m)
{
    py::class_<IDatabaseAdmin>(m, "DatabaseAdmin")
        .def("crear_backup", &IDatabaseAdmin::crearBackup)
        .def("verificar_integridad_referencial", &IDatabaseAdmin::verificarIntegridadReferencial)
        .def("reporte_stock_critico", &IDatabaseAdmin::reporteStockCritico)
        .def("reporte_historial_cliente", &IDatabaseAdmin::reporteHistorialCliente)
        .def("sincronizar_contadores_tienda", &IDatabaseAdmin::sincronizarContadoresTienda);
}
