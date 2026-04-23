#include "bindings/context/papaya_context.hpp"

PapayaContext::PapayaContext()
    : m_productos(),
      m_proveedores(),
      m_clientes(),
      m_transacciones(),
      m_admin(m_productos, m_clientes, m_proveedores, m_transacciones)
{
}

bool PapayaContext::init_database() const
{
    return init_database_impl();
}

FSProductoRepository& PapayaContext::productos()
{
    return m_productos;
}

FSProveedorRepository& PapayaContext::proveedores()
{
    return m_proveedores;
}

FSClienteRepository& PapayaContext::clientes()
{
    return m_clientes;
}

FSTransaccionRepository& PapayaContext::transacciones()
{
    return m_transacciones;
}

IDatabaseAdmin& PapayaContext::admin()
{
    return m_admin;
}
