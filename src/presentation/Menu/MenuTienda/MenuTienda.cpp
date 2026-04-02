#include "MenuTienda.hpp"

#include <iostream>

MenuTienda::MenuTienda(AppRepositories& repos) : Menu(repos)
{
    setTitle("Gestion de Tienda");
    setTexToExit("Salir");
    setNumOptions(1);
}

void MenuTienda::mostrarResumenTienda()
{
    std::cout << "Resumen de tienda (En desarrollo)" << std::endl;
}

void MenuTienda::showMenu()
{
    setOption(0, "Mostrar resumen de tienda", [this]() { mostrarResumenTienda(); });
    drawMenu();
}
