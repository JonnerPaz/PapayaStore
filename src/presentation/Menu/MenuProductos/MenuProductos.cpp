#include "MenuProductos.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <limits>
#include <string>
#include <variant>

namespace {

// Get user input
// Lo que hacía antes asignarPropiedad* pero no me terminó de servir pa un coño
std::string readLine(const char* prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin >> std::ws, value);
    return value;
}

bool confirmAction(const char* prompt) {
    std::cout << prompt;
    char confirm = 'n';
    std::cin >> confirm;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return confirm == 's' || confirm == 'S';
}

bool parsePositiveInt(const std::string& input, int& outValue) {
    if (input.empty()) {
        return false;
    }

    try {
        const int parsed = std::stoi(input);
        if (parsed <= 0) {
            return false;
        }
        outValue = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

bool parseNonNegativeInt(const std::string& input, int& outValue) {
    if (input.empty()) {
        return false;
    }

    try {
        const int parsed = std::stoi(input);
        if (parsed < 0) {
            return false;
        }
        outValue = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

bool parseNonNegativeFloat(const std::string& input, float& outValue) {
    if (input.empty()) {
        return false;
    }

    try {
        const float parsed = std::stof(input);
        if (parsed < 0) {
            return false;
        }
        outValue = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace

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
    std::cout << std::format("{:<5} | {:<20} | {:<15} | {:<10} | {:<5}", "ID", "Nombre", "Codigo",
                             "Precio", "Stock")
              << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;

    for (int id = 1; id < stats.proximoID; ++id) {
        auto result = repositories.productos.leerPorId(id);
        if (!std::holds_alternative<Producto>(result)) {
            continue;
        }

        const Producto& producto = std::get<Producto>(result);
        std::cout << std::format("{:<5} | {:<20} | {:<15} | {:<10.2f} | {:<5}", producto.getId(),
                                 producto.getNombre(), producto.getCodigo(), producto.getPrecio(),
                                 producto.getStock())
                  << std::endl;
    }
}

void MenuProductos::eliminarProducto() {
}

// [this]() { crearProducto(); } lambda function (como pasar callback)
void MenuProductos::showMenu() {
    setOption(0, "Crear Producto", [this]() { crearProducto(); });
    setOption(1, "Buscar Producto", [this]() { buscarProducto(); });
    setOption(2, "Actualizar Producto", [this]() { actualizarProducto(); });
    setOption(3, "Listar Productos", [this]() { listarProductos(); });
    setOption(4, "Eliminar Producto", [this]() { eliminarProducto(); });

    drawMenu();
}
