#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>

#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"
#include "domain/repositories/AppRepositories.hpp"
#include "infrastructure/datasource/FSClienteRepository.hpp"
#include "infrastructure/datasource/FSDatabaseAdmin.hpp"
#include "infrastructure/datasource/FSProductoRepository.hpp"
#include "infrastructure/datasource/FSProveedorRepository.hpp"
#include "infrastructure/datasource/FSTransaccionRepository.hpp"
#include "presentation/CliUtils.hpp"
#include "presentation/Menu/MenuClientes/MenuClientes.hpp"
#include "presentation/Menu/MenuProductos/MenuProductos.hpp"
#include "presentation/Menu/MenuProveedores/MenuProveedores.hpp"
#include "presentation/Menu/MenuReportes/MenuReportes.hpp"
#include "presentation/Menu/MenuTienda/MenuTienda.hpp"
#include "presentation/Menu/MenuTransacciones/MenuTransacciones.hpp"

using namespace Constants::ASCII_CODES;
using namespace Constants::PATHS;

namespace fs = std::filesystem;

namespace Bootstrap {

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

namespace {

struct OopContext {
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

    OopContext()
        : repositories{productos, clientes, proveedores, transacciones, admin},
          menuProductos(repositories, cliUtils),
          menuProveedores(repositories),
          menuClientes(repositories),
          menuTransacciones(repositories),
          menuReportes("Gestion de Reportes y seguridad", "Salir", 5, repositories),
          menuTienda(repositories)
    {
    }
};

OopContext& getOopContext()
{
    static OopContext context;
    return context;
}

bool ensureFileWithHeader(const fs::path& path)
{
    if (path.has_parent_path()) {
        std::error_code ec;
        fs::create_directories(path.parent_path(), ec);
        if (ec) {
            return false;
        }
    }

    if (fs::exists(path)) {
        return true;
    }

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    const HeaderFile header{0, 1, 0, 1};
    file.write(reinterpret_cast<const char*>(&header), sizeof(HeaderFile));
    // si la escritura es correcta, la variable file es true
    return static_cast<bool>(file);
}

MainOption readOption()
{
    int option = -1;
    if (!(std::cin >> option)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return MainOption::Invalida;
    }

    if (option < 0 || option > 6) {
        return MainOption::Invalida;
    }

    return static_cast<MainOption>(option);
}

}  // namespace

bool bootstrapStorage()
{
    const std::array<fs::path, 5> paths = {
        PRODUCTOS_PATH, PROVEEDORES_PATH, CLIENTES_PATH, TRANSACCIONES_PATH, TIENDA_PATH,
    };

    bool ok = true;
    for (const fs::path& path : paths) {
        ok = ensureFileWithHeader(path) && ok;
    }
    return ok;
}

void printMigrationStatus()
{
    std::cout << "Nuevo entrypoint creado: src/main_oop.cpp\n";
    std::cout << "src/main.cpp queda como legacy temporal\n";
    std::cout << "Bootstrap de storage ya disponible en OOP\n";
}

void printMainMenu()
{
    std::cout << "\n=== PAPAYA STORE - Menú Principal (Migración) ===\n";
    std::cout << "1. Gestion de Productos\n";
    std::cout << "2. Gestion de Proveedores\n";
    std::cout << "3. Gestion de Clientes\n";
    std::cout << "4. Gestion de Transacciones\n";
    std::cout << "5. Gestion de Reportes y seguridad\n";
    std::cout << "6. Gestion de Tienda\n";
    std::cout << "0. Salir\n";
    std::cout << "Seleccione una opcion: ";
}

void dispatchOption(MainOption option)
{
    switch (option) {
        case MainOption::Productos:
            getOopContext().menuProductos.showMenu();
            break;
        case MainOption::Proveedores:
            std::cout << "Abriendo modulo Proveedores\n";
            getOopContext().menuProveedores.showMenu();
            break;
        case MainOption::Clientes:
            std::cout << "Abriendo modulo Clientes\n";
            getOopContext().menuClientes.showMenu();
            break;
        case MainOption::Transacciones:
            std::cout << "Abriendo modulo Transacciones\n";
            getOopContext().menuTransacciones.showMenu();
            break;
        case MainOption::Reportes:
            std::cout << "Abriendo modulo Reportes\n";
            getOopContext().menuReportes.showMenu();
            break;
        case MainOption::Tienda:
            std::cout << "Abriendo modulo Tienda\n";
            getOopContext().menuTienda.showMenu();
            break;
        case MainOption::Salir:
            std::cout << "Saliendo del programa...\n";
            break;
        case MainOption::Invalida:
        default:
            std::cout << "Opcion invalida\n";
            break;
    }
}

void runMainLoop()
{
    if (!bootstrapStorage()) {
        std::cout << "Error inicializando almacenamiento base\n";
        return;
    }

    MainOption option = MainOption::Invalida;
    do {
        printMainMenu();
        option = readOption();
        dispatchOption(option);
    } while (option != MainOption::Salir);
}

}  // namespace Bootstrap

int main()
{
    Bootstrap::runMainLoop();
    return 0;
}
