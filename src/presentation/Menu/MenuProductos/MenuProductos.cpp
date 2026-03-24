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
    auto proveedoresStatsResult = repositories.proveedores.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(proveedoresStatsResult)) {
        std::cout << "Error: " << std::get<std::string>(proveedoresStatsResult) << std::endl;
        return;
    }

    if (std::get<ArchivoStats>(proveedoresStatsResult).registrosActivos == 0) {
        std::cout << "No hay proveedores registrados. Debe crear al menos uno "
                     "primero."
                  << std::endl;
        return;
    }

    const std::string nombre = readLine("Ingrese el nombre del producto (q para cancelar): ");
    if (nombre == "q" || nombre == "Q" || nombre.empty()) {
        std::cout << "Creacion cancelada." << std::endl;
        return;
    }

    const std::string codigo = readLine("Ingrese el codigo del producto (q para cancelar): ");
    if (codigo == "q" || codigo == "Q" || codigo.empty()) {
        std::cout << "Creacion cancelada." << std::endl;
        return;
    }

    const std::string descripcion =
        readLine("Ingrese la descripcion del producto (q para cancelar): ");
    if (descripcion == "q" || descripcion == "Q" || descripcion.empty()) {
        std::cout << "Creacion cancelada." << std::endl;
        return;
    }

    float precio = 0.0f;
    while (true) {
        const std::string precioText = readLine("Ingrese el precio del producto: ");
        if (parseNonNegativeFloat(precioText, precio)) {
            break;
        }
        std::cout << "Precio invalido. Debe ser un numero mayor o igual a 0." << std::endl;
    }

    int stock = 0;
    while (true) {
        const std::string stockText = readLine("Ingrese el stock del producto: ");
        if (parseNonNegativeInt(stockText, stock)) {
            break;
        }
        std::cout << "Stock invalido. Debe ser un entero mayor o igual a 0." << std::endl;
    }

    int stockMinimo = 0;
    while (true) {
        const std::string stockMinText = readLine("Ingrese el stock minimo del producto: ");
        if (parseNonNegativeInt(stockMinText, stockMinimo)) {
            break;
        }
        std::cout << "Stock minimo invalido. Debe ser un entero mayor o igual "
                     "a 0."
                  << std::endl;
    }

    listarProductos();
    const int idProveedor = utils.validarId("Ingrese el id del proveedor");
    if (idProveedor <= 0) {
        std::cout << "Creacion cancelada." << std::endl;
        return;
    }

    auto proveedorResult = repositories.proveedores.leerPorId(idProveedor);
    if (std::holds_alternative<std::string>(proveedorResult)) {
        std::cout << "Proveedor invalido: " << std::get<std::string>(proveedorResult) << std::endl;
        return;
    }

    auto productoStatsResult = repositories.productos.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(productoStatsResult)) {
        std::cout << "Error: " << std::get<std::string>(productoStatsResult) << std::endl;
        return;
    }

    const ArchivoStats productoStats = std::get<ArchivoStats>(productoStatsResult);
    const int nuevoId = productoStats.proximoID;

    const auto now = std::chrono::system_clock::now();
    Producto producto(nuevoId, nombre.c_str(), codigo, descripcion.c_str(), false, now, now);
    producto.setPrecio(precio);
    producto.setStock(stock);
    producto.setIdProveedor(idProveedor);
    producto.setStockMinimo(stockMinimo);
    producto.setTotalVendido(0);

    char fechaActual[11];
    producto.obtenerFechaActual(fechaActual);
    producto.setFechaRegistro(fechaActual);

    std::cout << std::format("Se creara el producto: {} ({})", producto.getNombre(),
                             producto.getCodigo())
              << std::endl;
    if (!confirmAction("Confirma creacion? (s/n): ")) {
        std::cout << "Creacion cancelada." << std::endl;
        return;
    }

    auto saveResult = repositories.productos.guardar(producto);
    if (std::holds_alternative<std::string>(saveResult)) {
        std::cout << "Error al guardar: " << std::get<std::string>(saveResult) << std::endl;
        return;
    }

    std::cout << "Producto creado con exito." << std::endl;
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
