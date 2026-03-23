#include "MenuClientes.hpp"
#include "presentation/Menu/Menu.hpp"
#include <format>
#include <iostream>
#include <variant>

MenuClientes::MenuClientes(string title, string texToExit, int numOptions,
                           AppRepositories& repositorios)
    : Menu(repositorios) {
    this->setTitle(title);
    this->setTexToExit(texToExit);
    this->setNumOptions(numOptions);
}

void MenuClientes::crearCliente() {
    // Implementación pendiente
    std::cout << "Crear cliente (En desarrollo)\n";
}

void MenuClientes::buscarCliente() {
    // Implementación pendiente
    std::cout << "Buscar cliente (En desarrollo)\n";
}

void MenuClientes::actualizarCliente() {
    // Implementación pendiente
    std::cout << "Actualizar cliente (En desarrollo)\n";
}

void MenuClientes::listarClientes() {
    auto statsResult = repositories.clientes.obtenerEstadisticas();

    if (std::holds_alternative<std::string>(statsResult)) {
        std::cout << "Error: " << std::get<std::string>(statsResult) << "\n";
        return;
    }

    ArchivoStats stats = std::get<ArchivoStats>(statsResult);

    if (stats.registrosActivos == 0) {
        std::cout << "No hay clientes registrados.\n";
        return;
    }

    std::cout << "--- Lista de Clientes (" << stats.registrosActivos << ") ---\n";
    std::cout << std::format("{:<5} | {:<20} | {:<15} | {:<15} | {:<25}\n", "ID", "Nombre",
                             "Cedula", "Telefono", "Email");
    std::cout << "---------------------------------------------------------------------------------"
                 "-----\n";

    for (int id = 1; id < stats.proximoID; ++id) {
        auto result = repositories.clientes.leerPorId(id);
        if (std::holds_alternative<Cliente>(result)) {
            Cliente cliente = std::get<Cliente>(result);
            if (!cliente.isEliminado()) {
                std::cout << std::format("{:<5} | {:<20} | {:<15} | {:<15} | {:<25}\n", cliente.id,
                                         cliente.nombre, cliente.cedula, cliente.telefono,
                                         cliente.email);
            }
        }
    }

    std::cout << "---------------------------------------------------------------------------------"
                 "-----\n";
}

void MenuClientes::eliminarCliente() {
    // Implementación pendiente
    std::cout << "Eliminar cliente (En desarrollo)\n";
}

void MenuClientes::showMenu() {
    // [this]() {} es una lambda
    this->setOption(0, "Crear Cliente", [this]() { crearCliente(); });
    this->setOption(1, "Buscar Cliente", [this]() { buscarCliente(); });
    this->setOption(2, "Actualizar Cliente", [this]() { actualizarCliente(); });
    this->setOption(3, "Listar Clientes", [this]() { listarClientes(); });
    this->setOption(4, "Eliminar Cliente", [this]() { eliminarCliente(); });
    Menu::drawMenu();
}

MenuClientes::~MenuClientes() {
}
