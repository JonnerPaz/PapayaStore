#pragma once
#include <filesystem>

#include "infrastructure/datasource/admin/FSDatabaseAdmin.hpp"
#include "infrastructure/datasource/cliente/FSClienteRepository.hpp"
#include "infrastructure/datasource/producto/FSProductoRepository.hpp"
#include "infrastructure/datasource/proveedor/FSProveedorRepository.hpp"
#include "infrastructure/datasource/transaccion/FSTransaccionRepository.hpp"
#include "presentation/cli/Menu/MainMenu/MainMenu.hpp"

namespace fs = std::filesystem;

class Bootstrapper
{
   private:
    FSProductoRepository productos;
    FSClienteRepository clientes;
    FSProveedorRepository proveedores;
    FSTransaccionRepository transacciones;
    FSDatabaseAdmin admin;
    AppRepositories repositories;
    MainMenu mainMenu;

   public:
    Bootstrapper();

    void runMainLoop();
    Bootstrapper& bootstrapContext();
    bool bootstrapStorage();

    bool ensureFileWithHeader(const fs::path& path);
    bool ensureTiendaRecord();
};
