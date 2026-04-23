#pragma once

#include "bindings/database/init_database.hpp"
#include "domain/repositories/IDatabaseAdmin.hpp"
#include "infrastructure/datasource/admin/FSDatabaseAdmin.hpp"
#include "infrastructure/datasource/cliente/FSClienteRepository.hpp"
#include "infrastructure/datasource/producto/FSProductoRepository.hpp"
#include "infrastructure/datasource/proveedor/FSProveedorRepository.hpp"
#include "infrastructure/datasource/transaccion/FSTransaccionRepository.hpp"

class PapayaContext
{
   private:
    FSProductoRepository m_productos;
    FSProveedorRepository m_proveedores;
    FSClienteRepository m_clientes;
    FSTransaccionRepository m_transacciones;
    FSDatabaseAdmin m_admin;

   public:
    PapayaContext();

    bool init_database() const;

    FSProductoRepository& productos();
    FSProveedorRepository& proveedores();
    FSClienteRepository& clientes();
    FSTransaccionRepository& transacciones();
    IDatabaseAdmin& admin();
};
