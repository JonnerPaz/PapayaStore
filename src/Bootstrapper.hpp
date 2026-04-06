#pragma once
#include <filesystem>

#include "infrastructure/datasource/admin/FSDatabaseAdmin.hpp"
#include "infrastructure/datasource/cliente/FSClienteRepository.hpp"
#include "infrastructure/datasource/producto/FSProductoRepository.hpp"
#include "infrastructure/datasource/proveedor/FSProveedorRepository.hpp"
#include "infrastructure/datasource/transaccion/FSTransaccionRepository.hpp"
#include "presentation/Menu/MenuClientes/MenuClientes.hpp"
#include "presentation/Menu/MenuProductos/MenuProductos.hpp"
#include "presentation/Menu/MenuProveedores/MenuProveedores.hpp"
#include "presentation/Menu/MenuReportes/MenuReportes.hpp"
#include "presentation/Menu/MenuTienda/MenuTienda.hpp"
#include "presentation/Menu/MenuTransacciones/MenuTransacciones.hpp"

namespace fs = std::filesystem;

enum class MainOption {
    Productos = 1,
    Proveedores = 2,
    Clientes = 3,
    Transacciones = 4,
    Reportes = 5,
    Tienda = 6,
    Salir = 0,
    Invalida = -1,
};

class Bootstrapper
{
   private:
    FSProductoRepository productos;
    FSClienteRepository clientes;
    FSProveedorRepository proveedores;
    FSTransaccionRepository transacciones;
    FSDatabaseAdmin admin;
    AppRepositories repositories;
    CliUtils cliUtils;
    MenuProductos menuProductos;
    MenuProveedores menuProveedores;
    MenuClientes menuClientes;
    MenuTransacciones menuTransacciones;
    MenuReportes menuReportes;
    MenuTienda menuTienda;

    void dispatchOption(MainOption option);

   public:
    Bootstrapper();

    void runMainLoop();
    Bootstrapper& bootstrapContext();
    bool bootstrapStorage();

    MainOption readOption();
    bool ensureFileWithHeader(const fs::path& path);
};
