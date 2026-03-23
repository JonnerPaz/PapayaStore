#include "MenuClientes.hpp"

#include <format>
#include <iostream>
#include <variant>

MenuClientes::MenuClientes(AppRepositories& repositorios) : Menu(repositorios) {
    setTitle("Gestion de Clientes");
    setTexToExit("Salir");
    setNumOptions(5);
}

void MenuClientes::crearCliente() {
    std::cout << "Crear cliente (En desarrollo)" << std::endl;
}

void MenuClientes::buscarCliente() {
    std::cout << "Buscar cliente (En desarrollo)" << std::endl;
}

void MenuClientes::actualizarCliente() {
    std::cout << "Actualizar cliente (En desarrollo)" << std::endl;
}

void MenuClientes::listarClientes() {
    auto statsResult = repositories.clientes.obtenerEstadisticas();

    if (std::holds_alternative<std::string>(statsResult)) {
        std::cout << "Error: " << std::get<std::string>(statsResult) << std::endl;
        return;
    }

    ArchivoStats stats = std::get<ArchivoStats>(statsResult);

    if (stats.registrosActivos == 0) {
        std::cout << "No hay clientes registrados." << std::endl;
        return;
    }

    std::cout << std::format("--- Lista de Clientes ({}) ---", stats.registrosActivos)
              << std::endl;
    std::cout << std::format("{:<5} | {:<20}", "ID", "Nombre") << std::endl;
    std::cout << "------------------------------" << std::endl;

    for (int id = 1; id < stats.proximoID; ++id) {
        auto result = repositories.clientes.leerPorId(id);
        if (!std::holds_alternative<Cliente>(result)) {
            continue;
        }

        const Cliente& cliente = std::get<Cliente>(result);
        std::cout << std::format("{:<5} | {:<20}", cliente.getId(), cliente.getNombre())
                  << std::endl;
    }
}

void MenuClientes::eliminarCliente() {
    std::cout << "Eliminar cliente (En desarrollo)" << std::endl;
}

void MenuClientes::showMenu() {
    setOption(0, "Crear Cliente", [this]() { crearCliente(); });
    setOption(1, "Buscar Cliente", [this]() { buscarCliente(); });
    setOption(2, "Actualizar Cliente", [this]() { actualizarCliente(); });
    setOption(3, "Listar Clientes", [this]() { listarClientes(); });
    setOption(4, "Eliminar Cliente", [this]() { eliminarCliente(); });

    drawMenu();
}
