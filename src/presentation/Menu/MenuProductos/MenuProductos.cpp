#include "MenuProductos.hpp"

#include <format>
#include <iostream>
#include <variant>

MenuProductos::MenuProductos(AppRepositories& repository, CliUtils utils)
    : Menu(repository), utils(utils) {
    setTitle("Gestion de Productos");
    setTexToExit("Salir");
    setNumOptions(5);
}

void MenuProductos::crearProducto() {
    std::cout << "Crear producto (En desarrollo)" << std::endl;
}

void MenuProductos::buscarProducto() {
    std::cout << "Buscar producto (En desarrollo)" << std::endl;
}

void MenuProductos::actualizarProducto() {
    std::cout << "Actualizar producto (En desarrollo)" << std::endl;
}

void MenuProductos::listarProductos() {
    auto statsResult = repositories.productos.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(statsResult)) {
        std::cout << "Error: " << std::get<std::string>(statsResult) << std::endl;
        return;
    }

    ArchivoStats stats = std::get<ArchivoStats>(statsResult);
    if (stats.registrosActivos == 0) {
        std::cout << "No hay productos registrados." << std::endl;
        return;
    }

    std::cout << std::format("--- Lista de Productos ({}) ---", stats.registrosActivos)
              << std::endl;
    std::cout << std::format("{:<5} | {:<20} | {:<15} | {:<10} | {:<5}", "ID", "Nombre",
                             "Codigo", "Precio", "Stock")
              << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;

    for (int id = 1; id < stats.proximoID; ++id) {
        auto result = repositories.productos.leerPorId(id);
        if (!std::holds_alternative<Producto>(result)) {
            continue;
        }

        const Producto& producto = std::get<Producto>(result);
        std::cout << std::format("{:<5} | {:<20} | {:<15} | {:<10.2f} | {:<5}", producto.getId(),
                                 producto.getNombre(), producto.getCodigo(),
                                 producto.getPrecio(), producto.getStock())
                  << std::endl;
    }
}

void MenuProductos::eliminarProducto() {
    std::cout << "Eliminar producto (En desarrollo)" << std::endl;
}

void MenuProductos::showMenu() {
    setOption(0, "Crear Producto", [this]() { crearProducto(); });
    setOption(1, "Buscar Producto", [this]() { buscarProducto(); });
    setOption(2, "Actualizar Producto", [this]() { actualizarProducto(); });
    setOption(3, "Listar Productos", [this]() { listarProductos(); });
    setOption(4, "Eliminar Producto", [this]() { eliminarProducto(); });

    drawMenu();
}
