#pragma once
#include "IClienteRepository.hpp"
#include "IDatabaseAdmin.hpp"
#include "IProductoRepository.hpp"
#include "IProveedorRepository.hpp"
#include "ITransaccionRepository.hpp"

/**
 * @brief - Dependecy injection de repositorios en el sistema
 */
struct AppRepositories {
    IProductoRepository& productos;
    IClienteRepository& clientes;
    IProveedorRepository& proveedores;
    ITransaccionRepository& transacciones;
    IDatabaseAdmin& admin;
};
