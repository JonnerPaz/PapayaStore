#include "MenuProductos.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <variant>

#include "domain/constants.hpp"

MenuProductos::MenuProductos(AppRepositories& repository, CliUtils utils)
    : Menu(repository), utils(utils)
{
    Menu::setTitle("Gestion de Productos");
    Menu::setTexToExit("Salir");
    Menu::setNumOptions(5);
}

bool MenuProductos::readValidText(const char* prompt, std::string& outValue)
{
    outValue = Menu::readLine(prompt);
    if (outValue == "q" || outValue == "Q" || outValue.empty()) {
        Menu::printError("Creacion cancelada.");
        return false;
    }

    return true;
}

void MenuProductos::readValidFloat(const char* prompt, float& outValue, const char* errorMsg,
                                   bool zeroInclusive)
{
    while (true) {
        const std::string input = Menu::readLine(prompt);
        if (utils.parsePositiveFloat(input, outValue, zeroInclusive)) {
            return;
        }

        Menu::printError(errorMsg);
    }
}

void MenuProductos::readValidInt(const char* prompt, int& outValue, const char* errorMsg,
                                 bool zeroInclusive)
{
    while (true) {
        const std::string input = Menu::readLine(prompt);
        if (utils.parsePositiveInt(input, outValue, zeroInclusive)) {
            return;
        }

        Menu::printError(errorMsg);
    }
}

void MenuProductos::crearProducto()
{
    auto proveedoresHeader = repositories.proveedores.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(proveedoresHeader)) {
        Menu::printError("Error: " + std::get<std::string>(proveedoresHeader));
        return;
    }

    if (std::get<HeaderFile>(proveedoresHeader).registrosActivos == 0) {
        Menu::printError("No hay proveedores registrados. Debe crear al menos uno primero.");
        return;
    }

    std::string nombre;
    if (!readValidText("Ingrese el nombre del producto (q para cancelar): ", nombre)) {
        return;
    }

    std::string codigo;
    if (!readValidText("Ingrese el codigo del producto (q para cancelar): ", codigo)) {
        return;
    }

    std::string descripcion;
    if (!readValidText("Ingrese la descripcion del producto (q para cancelar): ", descripcion)) {
        return;
    }

    float precio = 0.0f;
    readValidFloat("Ingrese el precio del producto: ", precio,
                   "Precio inválido. Debe ser un numero mayor o igual a 0.", false);

    int stock = 0;
    readValidInt("Ingrese el stock del producto: ", stock,
                 "Stock invalido. Debe ser un numero entero mayor o igual a 0.");

    int stockMinimo = 0;
    readValidInt("Ingrese el stock minimo del producto: ", stockMinimo,
                 "Stock minimo inválido. Debe ser un entero mayor o igual a 0.");

    listarProductos();
    const int idProveedor = utils.validarId("Ingrese el id del proveedor");
    if (idProveedor <= 0) {
        Menu::printError("El Id ingresado no existe. Debe ser mayor a 0.");
        return;
    }

    auto proveedorHeader = repositories.proveedores.leerPorId(idProveedor);
    if (std::holds_alternative<std::string>(proveedorHeader)) {
        Menu::printError("Error: " + std::get<std::string>(proveedorHeader));
        return;
    }

    auto productosHeader = repositories.productos.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(productosHeader)) {
        Menu::printError("Error: " + std::get<std::string>(productosHeader));
        return;
    }

    const HeaderFile productoStats = std::get<HeaderFile>(productosHeader);
    const int nuevoId = productoStats.proximoID;

    // se aprovechan las propiedades por defecto:
    // eliminado, fechaCreacion, fechaUltimaModificacion, totalVendido
    Producto producto{};
    producto.setId(nuevoId);
    producto.setNombre(nombre.c_str());
    producto.setCodigo(codigo.c_str());
    producto.setDescripcion(descripcion.c_str());
    producto.setPrecio(precio);
    producto.setStock(stock);
    producto.setIdProveedor(idProveedor);
    producto.setStockMinimo(stockMinimo);

    std::cout << COLOR_YELLOW
              << std::format("Se creara el producto: {}{} {}({}{}{})", COLOR_GREEN,
                             producto.getNombre(), COLOR_YELLOW, COLOR_GREEN, producto.getCodigo(),
                             COLOR_YELLOW)
              << COLOR_RESET << std::endl;
    if (!Menu::confirmAction("¿Estás seguro? (s/n): ")) {
        Menu::printError("Creacion cancelada.");
        return;
    }

    auto saveResult = repositories.productos.guardar(producto);
    if (std::holds_alternative<std::string>(saveResult)) {
        Menu::printError("Error al guardar: " + std::get<std::string>(saveResult));
        return;
    }

    Menu::printSuccess("Producto creado con exito.");
}

void MenuProductos::buscarProducto()
{
    const int id = utils.validarId("Ingrese el id del producto a buscar");
    if (id <= 0) {
        Menu::printError("El Id ingresado no existe. Búsqueda cancelada.");
        return;
    }

    auto result = repositories.productos.leerPorId(id);
    if (std::holds_alternative<std::string>(result)) {
        Menu::printError("Error: " + std::get<std::string>(result));
        return;
    }

    const Producto& producto = std::get<Producto>(result);
    Menu::printSuccess("Producto encontrado:");
    std::cout << std::format("{}ID: {}{}", COLOR_YELLOW, COLOR_GREEN, producto.getId())
              << std::endl;
    std::cout << std::format("{}Nombre: {}{}", COLOR_YELLOW, COLOR_GREEN, producto.getNombre())
              << std::endl;
    std::cout << std::format("{}Codigo: {}{}", COLOR_YELLOW, COLOR_GREEN, producto.getCodigo())
              << std::endl;
    std::cout << std::format("{}Descripcion: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             producto.getDescripcion())
              << std::endl;
    std::cout << std::format("{}Precio: {}{:.2f}", COLOR_YELLOW, COLOR_GREEN, producto.getPrecio())
              << std::endl;
    std::cout << std::format("{}Stock: {}{}", COLOR_YELLOW, COLOR_GREEN, producto.getStock())
              << std::endl;
}

void MenuProductos::actualizarProducto()
{
    const int id = utils.validarId("Ingrese el id del producto a actualizar");
    if (id <= 0) {
        Menu::printError("El Id ingresado no existe. Actualizacion cancelada.");
        return;
    }

    auto result = repositories.productos.leerPorId(id);
    if (std::holds_alternative<std::string>(result)) {
        Menu::printError("Error: " + std::get<std::string>(result));
        return;
    }

    Producto producto = std::get<Producto>(result);

    const std::string nuevoNombre = readLine("Nuevo nombre (enter para mantener actual): ");
    if (!nuevoNombre.empty()) {
        producto.setNombre(nuevoNombre.c_str());
    }

    const std::string nuevoCodigo = readLine("Nuevo codigo (enter para mantener actual): ");
    if (!nuevoCodigo.empty()) {
        producto.setCodigo(nuevoCodigo.c_str());
    }

    const std::string nuevaDescripcion =
        readLine("Nueva descripcion (enter para mantener actual): ");
    if (!nuevaDescripcion.empty()) {
        producto.setDescripcion(nuevaDescripcion.c_str());
    }

    const std::string nuevoPrecioText = readLine("Nuevo precio (enter para mantener actual): ");
    float nuevoPrecio = 0.0f;
    if (!nuevoPrecioText.empty()) {
        if (!utils.parsePositiveFloat(nuevoPrecioText, nuevoPrecio, false)) {
            Menu::printError("Precio invalido.");
            return;
        }
        producto.setPrecio(nuevoPrecio);
    }

    const std::string nuevoStockText = readLine("Nuevo stock (enter para mantener actual): ");
    int nuevoStock = 0;
    if (!nuevoStockText.empty()) {
        if (!utils.parsePositiveInt(nuevoStockText, nuevoStock)) {
            Menu::printError("Stock invalido.");
            return;
        }
        producto.setStock(nuevoStock);
    }

    const std::string nuevoStockMinimoText =
        readLine("Nuevo stock minimo (enter para mantener actual): ");
    int nuevoStockMinimo = 0;
    if (!nuevoStockMinimoText.empty()) {
        if (!utils.parsePositiveInt(nuevoStockMinimoText, nuevoStockMinimo)) {
            std::cout << "Stock minimo invalido." << std::endl;
            return;
        }
        producto.setStockMinimo(nuevoStockMinimo);
    }

    const std::string nuevoProveedorText =
        readLine("Nuevo id de proveedor (enter para mantener actual): ");
    int nuevoIdProveedor = 0;
    if (!nuevoProveedorText.empty()) {
        if (!utils.parsePositiveInt(nuevoProveedorText, nuevoIdProveedor)) {
            Menu::printError("ID de Proveedor invalido.");
            return;
        }

        auto proveedorResult = repositories.proveedores.leerPorId(nuevoIdProveedor);
        if (std::holds_alternative<std::string>(proveedorResult)) {
            Menu::printError("Proveedor inválido: " + std::get<std::string>(proveedorResult));
            return;
        }

        producto.setIdProveedor(nuevoIdProveedor);
    }

    producto.setFechaUltimaModificacion(std::chrono::system_clock::now());

    if (!confirmAction("Confirma actualizacion? (s/n): ")) {
        Menu::printError("Actualizacion cancelada.");
        return;
    }

    auto updateResult = repositories.productos.actualizar(id, producto);
    if (std::holds_alternative<std::string>(updateResult)) {
        Menu::printError("Error al actualizar: " + std::get<std::string>(updateResult));
        return;
    }

    Menu::printSuccess("Producto actualizado con éxito.");
}

void MenuProductos::listarProductos()
{
    auto productosHeader = repositories.productos.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(productosHeader)) {
        Menu::printError("Error: " + std::get<std::string>(productosHeader));
        return;
    }

    HeaderFile stats = std::get<HeaderFile>(productosHeader);
    if (stats.registrosActivos == 0) {
        Menu::printError("No hay productos registrados.");
        return;
    }

    std::cout << std::format("{}--- Lista de Productos ({}{}{}) ---", COLOR_CYAN, COLOR_GREEN,
                             COLOR_YELLOW, stats.registrosActivos)
              << std::endl;
    std::cout << std::format("{}{:<5} | {:<20} | {:<15} | {:<10} | {:<5}", COLOR_YELLOW, "ID",
                             "Nombre", "Codigo", "Precio", "Stock")
              << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;

    for (int id = 1; id < stats.proximoID; ++id) {
        auto result = repositories.productos.leerPorId(id);
        if (!std::holds_alternative<Producto>(result)) {
            continue;
        }

        const Producto& producto = std::get<Producto>(result);
        std::cout << std::format("{}{:<5} | {:<20} | {:<15} | {:<10.2f} | {:<5}", COLOR_GREEN,
                                 producto.getId(), producto.getNombre(), producto.getCodigo(),
                                 producto.getPrecio(), producto.getStock())
                  << std::endl;
    }
}

void MenuProductos::eliminarProducto()
{
    const int id = utils.validarId("Ingrese el id del producto a eliminar");
    if (id <= 0) {
        Menu::printError("Eliminación cancelada.");
        return;
    }

    auto result = repositories.productos.leerPorId(id);
    if (std::holds_alternative<std::string>(result)) {
        Menu::printError("Error: " + std::get<std::string>(result));
        return;
    }

    const Producto& producto = std::get<Producto>(result);
    std::cout << std::format("Producto a eliminar: {} ({})", producto.getNombre(),
                             producto.getCodigo())
              << std::endl;

    if (!confirmAction("Confirma eliminacion? (s/n): ")) {
        std::cout << "Eliminacion cancelada." << std::endl;
        return;
    }

    auto deleteResult = repositories.productos.eliminarLogicamente(id);
    if (std::holds_alternative<std::string>(deleteResult)) {
        std::cout << "Error al eliminar: " << std::get<std::string>(deleteResult) << std::endl;
        return;
    }

    Menu::printSuccess("Producto eliminado con éxito.");
}

// [this]() { crearProducto(); } lambda function (como pasar callback)
void MenuProductos::showMenu()
{
    setOption(0, "Crear Producto", [this]() { crearProducto(); });
    setOption(1, "Buscar Producto", [this]() { buscarProducto(); });
    setOption(2, "Actualizar Producto", [this]() { actualizarProducto(); });
    setOption(3, "Listar Productos", [this]() { listarProductos(); });
    setOption(4, "Eliminar Producto", [this]() { eliminarProducto(); });

    Menu::drawMenu();
}
