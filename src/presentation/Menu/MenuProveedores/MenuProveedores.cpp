#include "MenuProveedores.hpp"

#include <iostream>

void MenuProveedores::crearProveedor() {
    std::cout << "Crear proveedor (En desarrollo)" << std::endl;
}

void MenuProveedores::buscarProveedor() {
    std::cout << "Buscar proveedor (En desarrollo)" << std::endl;
}

void MenuProveedores::actualizarProveedor() {
    std::cout << "Actualizar proveedor (En desarrollo)" << std::endl;
}

void MenuProveedores::listarProveedores() {
    std::cout << "Listar proveedores (En desarrollo)" << std::endl;
}

void MenuProveedores::eliminarProveedor() {
    std::cout << "Eliminar proveedor (En desarrollo)" << std::endl;
}

void MenuProveedores::showMenu() {
    setOption(0, "Crear Proveedor", [this]() { crearProveedor(); });
    setOption(1, "Buscar Proveedor", [this]() { buscarProveedor(); });
    setOption(2, "Actualizar Proveedor", [this]() { actualizarProveedor(); });
    setOption(3, "Listar Proveedores", [this]() { listarProveedores(); });
    setOption(4, "Eliminar Proveedor", [this]() { eliminarProveedor(); });
    drawMenu();
}
