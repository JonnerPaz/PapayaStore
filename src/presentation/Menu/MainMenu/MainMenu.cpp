#include "MainMenu.hpp"

#include <variant>

MainMenu::MainMenu(AppRepositories& repositories)
    : Menu(repositories),
      menuProductos(repositories, cliUtils),
      menuProveedores(repositories, cliUtils),
      menuClientes(repositories, cliUtils),
      menuTransacciones(repositories, cliUtils),
      menuReportes("Gestión de Reportes y seguridad", "Salir", 5, repositories),
      menuTienda(repositories)
{
    setTitle("PAPAYA STORE - Menú Principal");
    setTexToExit("Salir");
    setNumOptions(6);
}

void MainMenu::showMenu()
{
    setOption(0, "Gestión de Productos", [this]() {
        // if there are no proveedores, we can't create a producto
        auto proveedoresHeader = repositories.proveedores.obtenerEstadisticas();
        if (std::holds_alternative<std::string>(proveedoresHeader)) {
            Menu::printError("Error: " + std::get<std::string>(proveedoresHeader));
            return;
        }

        if (std::get<HeaderFile>(proveedoresHeader).registrosActivos == 0) {
            Menu::printError("No hay proveedores registrados. Debe crear al menos uno primero.");
            return;
        }

        menuProductos.showMenu();
    });

    setOption(1, "Gestión de Proveedores", [this]() { menuProveedores.showMenu(); });
    setOption(2, "Gestión de Clientes", [this]() { menuClientes.showMenu(); });
    setOption(3, "Gestión de Transacciones", [this]() { menuTransacciones.showMenu(); });
    setOption(4, "Gestión de Reportes y seguridad", [this]() { menuReportes.showMenu(); });
    setOption(5, "Gestión de Tienda", [this]() { menuTienda.showMenu(); });
    Menu::drawMenu();
}
