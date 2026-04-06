#include "Bootstrapper.hpp"

#include "domain/constants.hpp"

using namespace Constants::PATHS;

namespace {

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

}  // namespace

Bootstrapper::Bootstrapper()
    : admin(productos, clientes, proveedores, transacciones),
      repositories{productos, clientes, proveedores, transacciones, admin},
      menuProductos(repositories, cliUtils),
      menuProveedores(repositories, cliUtils),
      menuClientes(repositories, cliUtils),
      menuTransacciones(repositories, cliUtils),
      menuReportes("Gestion de Reportes y seguridad", "Salir", 5, repositories),
      menuTienda(repositories)
{
}

Bootstrapper& Bootstrapper::bootstrapContext()
{
    static Bootstrapper context;
    return context;
}

bool Bootstrapper::ensureFileWithHeader(const fs::path& path)
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

MainOption Bootstrapper::readOption()
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

bool Bootstrapper::bootstrapStorage()
{
    const std::array<fs::path, 5> paths = {
        PRODUCTOS_PATH, PROVEEDORES_PATH, CLIENTES_PATH, TRANSACCIONES_PATH, TIENDA_PATH,
    };

    bool ok = true;
    for (const fs::path& path : paths) {
        ok = this->ensureFileWithHeader(path) && ok;
    }
    return ok;
}

void Bootstrapper::dispatchOption(MainOption option)
{
    switch (option) {
        case MainOption::Productos:
            Bootstrapper().menuProductos.showMenu();
            break;
        case MainOption::Proveedores:
            std::cout << "Abriendo modulo Proveedores\n";
            Bootstrapper().menuProveedores.showMenu();
            break;
        case MainOption::Clientes:
            std::cout << "Abriendo modulo Clientes\n";
            Bootstrapper().menuClientes.showMenu();
            break;
        case MainOption::Transacciones:
            std::cout << "Abriendo modulo Transacciones\n";
            Bootstrapper().menuTransacciones.showMenu();
            break;
        case MainOption::Reportes:
            std::cout << "Abriendo modulo Reportes\n";
            Bootstrapper().menuReportes.showMenu();
            break;
        case MainOption::Tienda:
            std::cout << "Abriendo modulo Tienda\n";
            Bootstrapper().menuTienda.showMenu();
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

void Bootstrapper::runMainLoop()
{
    if (!this->bootstrapStorage()) {
        std::cout << "Error inicializando almacenamiento base\n";
        return;
    }

    MainOption option = MainOption::Invalida;
    do {
        printMainMenu();
        option = this->readOption();
        this->dispatchOption(option);
    } while (option != MainOption::Salir);
}
