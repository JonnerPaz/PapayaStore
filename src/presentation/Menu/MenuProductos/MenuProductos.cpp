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
                   "Precio inválido. Debe ser un numero mayor o igual a 0.", true);

    int stock = 0;
    readValidInt("Ingrese el stock del producto: ", stock,
                 "Stock invalido. Debe ser un numero entero mayor o igual a 0.");

    int stockMinimo = 0;
    readValidInt("Ingrese el stock minimo del producto: ", stockMinimo,
                 "Stock minimo inválido. Debe ser un entero mayor o igual a 0.");

    std::cout << std::format("{}--- Proveedores disponibles ---{}", COLOR_CYAN, COLOR_RESET)
              << std::endl;
    for (int proveedorId = 1; proveedorId < std::get<HeaderFile>(proveedoresHeader).proximoID;
         ++proveedorId) {
        auto proveedorResult = repositories.proveedores.leerPorId(proveedorId);
        if (!std::holds_alternative<Proveedor>(proveedorResult)) {
            continue;
        }

        const Proveedor& proveedor = std::get<Proveedor>(proveedorResult);
        std::cout << std::format("{}ID: {}{} {}- {}{}", COLOR_YELLOW, COLOR_GREEN,
                                 proveedor.getId(), COLOR_YELLOW, COLOR_GREEN,
                                 proveedor.getNombre())
                  << COLOR_RESET << std::endl;
    }

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

    try {
        repositories.admin.sincronizarContadoresTienda();
    } catch (const std::exception& e) {
        Menu::printError("Advertencia: producto creado, pero no se pudo sincronizar tienda: " +
                         std::string(e.what()));
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
    while (true) {
        std::cout << COLOR_CYAN << "\n--- Producto actual ---" << COLOR_RESET << std::endl;
        std::cout << std::format("{}ID: {}{}", COLOR_YELLOW, COLOR_GREEN, producto.getId())
                  << std::endl;
        std::cout << std::format("{}Nombre: {}{}", COLOR_YELLOW, COLOR_GREEN, producto.getNombre())
                  << std::endl;
        std::cout << std::format("{}Codigo: {}{}", COLOR_YELLOW, COLOR_GREEN, producto.getCodigo())
                  << std::endl;
        std::cout << std::format("{}Descripcion: {}{}", COLOR_YELLOW, COLOR_GREEN,
                                 producto.getDescripcion())
                  << std::endl;
        std::cout << std::format("{}Precio: {}{:.2f}", COLOR_YELLOW, COLOR_GREEN,
                                 producto.getPrecio())
                  << std::endl;
        std::cout << std::format("{}Stock: {}{}", COLOR_YELLOW, COLOR_GREEN, producto.getStock())
                  << std::endl;
        std::cout << std::format("{}Stock minimo: {}{}", COLOR_YELLOW, COLOR_GREEN,
                                 producto.getStockMinimo())
                  << std::endl;
        std::cout << std::format("{}ID Proveedor: {}{}", COLOR_YELLOW, COLOR_GREEN,
                                 producto.getIdProveedor())
                  << std::endl;

        std::cout << COLOR_CYAN << "\nSeleccione una propiedad a actualizar:" << COLOR_RESET
                  << std::endl;
        std::cout << COLOR_YELLOW << "1." << COLOR_RESET << " Nombre" << std::endl;
        std::cout << COLOR_YELLOW << "2." << COLOR_RESET << " Codigo" << std::endl;
        std::cout << COLOR_YELLOW << "3." << COLOR_RESET << " Descripción" << std::endl;
        std::cout << COLOR_YELLOW << "4." << COLOR_RESET << " Precio" << std::endl;
        std::cout << COLOR_YELLOW << "5." << COLOR_RESET << " Stock" << std::endl;
        std::cout << COLOR_YELLOW << "6." << COLOR_RESET << " Stock minimo" << std::endl;
        std::cout << COLOR_YELLOW << "7." << COLOR_RESET << " ID proveedor" << std::endl;
        std::cout << COLOR_RED << "0." << COLOR_RESET << " Salir" << std::endl;

        const std::string opcionText = readLine("Opcion: ");
        int opcion = -1;
        if (!utils.parsePositiveInt(opcionText, opcion, true) || opcion > 7) {
            Menu::printError("Opcion inválida.");
            continue;
        }

        if (opcion == 0) {
            Menu::printSuccess("Actualizacion finalizada.");
            break;
        }

        switch (opcion) {
            case 1: {
                const std::string nuevoNombre =
                    readLine("Nuevo nombre (q o enter para cancelar): ");
                if (nuevoNombre == "q" || nuevoNombre == "Q" || nuevoNombre.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                std::cout << std::format("Nombre actual: {} | Nuevo nombre: {}",
                                         producto.getNombre(), nuevoNombre)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de nombre? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Producto productoActualizado = producto;
                if (!productoActualizado.setNombre(nuevoNombre.c_str())) {
                    Menu::printError("Nombre invalido.");
                    break;
                }

                productoActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.productos.actualizar(id, productoActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar nombre: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                producto = productoActualizado;
                Menu::printSuccess("Nombre actualizado con éxito.");
                break;
            }
            case 2: {
                const std::string nuevoCodigo =
                    readLine("Nuevo codigo (q o enter para cancelar): ");
                if (nuevoCodigo == "q" || nuevoCodigo == "Q" || nuevoCodigo.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                std::cout << std::format("Codigo actual: {} | Nuevo codigo: {}",
                                         producto.getCodigo(), nuevoCodigo)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de codigo? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Producto productoActualizado = producto;
                if (!productoActualizado.setCodigo(nuevoCodigo.c_str())) {
                    Menu::printError("Codigo invalido.");
                    break;
                }

                productoActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.productos.actualizar(id, productoActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar codigo: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                producto = productoActualizado;
                Menu::printSuccess("Codigo actualizado con éxito.");
                break;
            }
            case 3: {
                const std::string nuevaDescripcion =
                    readLine("Nueva descripcion (q o enter para cancelar): ");
                if (nuevaDescripcion == "q" || nuevaDescripcion == "Q" ||
                    nuevaDescripcion.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                std::cout << std::format("Descripcion actual: {} | Nueva descripcion: {}",
                                         producto.getDescripcion(), nuevaDescripcion)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de descripcion? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Producto productoActualizado = producto;
                if (!productoActualizado.setDescripcion(nuevaDescripcion.c_str())) {
                    Menu::printError("Descripcion invalida.");
                    break;
                }

                productoActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.productos.actualizar(id, productoActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar descripcion: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                producto = productoActualizado;
                Menu::printSuccess("Descripcion actualizada con éxito.");
                break;
            }
            case 4: {
                const std::string nuevoPrecioText =
                    readLine("Nuevo precio (q o enter para cancelar): ");
                if (nuevoPrecioText == "q" || nuevoPrecioText == "Q" || nuevoPrecioText.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                float nuevoPrecio = 0.0f;
                if (!utils.parsePositiveFloat(nuevoPrecioText, nuevoPrecio, true)) {
                    Menu::printError("Precio invalido.");
                    break;
                }

                std::cout << std::format("Precio actual: {:.2f} | Nuevo precio: {:.2f}",
                                         producto.getPrecio(), nuevoPrecio)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de precio? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Producto productoActualizado = producto;
                if (!productoActualizado.setPrecio(nuevoPrecio)) {
                    Menu::printError("Precio invalido.");
                    break;
                }

                productoActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.productos.actualizar(id, productoActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar precio: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                producto = productoActualizado;
                Menu::printSuccess("Precio actualizado con éxito.");
                break;
            }
            case 5: {
                const std::string nuevoStockText =
                    readLine("Nuevo stock (q o enter para cancelar): ");
                if (nuevoStockText == "q" || nuevoStockText == "Q" || nuevoStockText.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                int nuevoStock = 0;
                if (!utils.parsePositiveInt(nuevoStockText, nuevoStock)) {
                    Menu::printError("Stock invalido.");
                    break;
                }

                std::cout << std::format("Stock actual: {} | Nuevo stock: {}", producto.getStock(),
                                         nuevoStock)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de stock? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Producto productoActualizado = producto;
                if (!productoActualizado.setStock(nuevoStock)) {
                    Menu::printError("Stock invalido.");
                    break;
                }

                productoActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.productos.actualizar(id, productoActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar stock: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                producto = productoActualizado;
                Menu::printSuccess("Stock actualizado con éxito.");
                break;
            }
            case 6: {
                const std::string nuevoStockMinimoText =
                    readLine("Nuevo stock minimo (q o enter para cancelar): ");
                if (nuevoStockMinimoText == "q" || nuevoStockMinimoText == "Q" ||
                    nuevoStockMinimoText.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                int nuevoStockMinimo = 0;
                if (!utils.parsePositiveInt(nuevoStockMinimoText, nuevoStockMinimo)) {
                    Menu::printError("Stock minimo invalido.");
                    break;
                }

                std::cout << std::format("Stock minimo actual: {} | Nuevo stock minimo: {}",
                                         producto.getStockMinimo(), nuevoStockMinimo)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de stock minimo? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Producto productoActualizado = producto;
                if (!productoActualizado.setStockMinimo(nuevoStockMinimo)) {
                    Menu::printError("Stock minimo invalido.");
                    break;
                }

                productoActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.productos.actualizar(id, productoActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar stock minimo: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                producto = productoActualizado;
                Menu::printSuccess("Stock minimo actualizado con éxito.");
                break;
            }
            case 7: {
                const std::string nuevoProveedorText =
                    readLine("Nuevo id de proveedor (q o enter para cancelar): ");
                if (nuevoProveedorText == "q" || nuevoProveedorText == "Q" ||
                    nuevoProveedorText.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                int nuevoIdProveedor = 0;
                if (!utils.parsePositiveInt(nuevoProveedorText, nuevoIdProveedor)) {
                    Menu::printError("ID de proveedor invalido.");
                    break;
                }

                auto proveedorResult = repositories.proveedores.leerPorId(nuevoIdProveedor);
                if (std::holds_alternative<std::string>(proveedorResult)) {
                    Menu::printError("Proveedor invalido: " +
                                     std::get<std::string>(proveedorResult));
                    break;
                }

                std::cout << std::format("Proveedor actual: {} | Nuevo proveedor: {}",
                                         producto.getIdProveedor(), nuevoIdProveedor)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de proveedor? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Producto productoActualizado = producto;
                if (!productoActualizado.setIdProveedor(nuevoIdProveedor)) {
                    Menu::printError("ID de proveedor invalido.");
                    break;
                }

                productoActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.productos.actualizar(id, productoActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar proveedor: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                producto = productoActualizado;
                Menu::printSuccess("Proveedor actualizado con éxito.");
                break;
            }
            default:
                Menu::printError("Opcion invalida.");
                break;
        }
    }
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

    auto transaccionesHeader = repositories.transacciones.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(transaccionesHeader)) {
        Menu::printError("Error: " + std::get<std::string>(transaccionesHeader));
        return;
    }

    bool productoEnTransaccionesActivas = false;
    HeaderFile transStats = std::get<HeaderFile>(transaccionesHeader);
    for (int transId = 1; transId < transStats.proximoID; ++transId) {
        auto transResult = repositories.transacciones.leerPorId(transId);
        if (!std::holds_alternative<Transaccion>(transResult)) {
            continue;
        }

        const Transaccion& transaccion = std::get<Transaccion>(transResult);
        for (int i = 0; i < transaccion.getProductosTotales(); ++i) {
            TransaccionDTO item = {};
            if (!transaccion.getProductoEnIndice(i, item)) {
                continue;
            }

            if (item.productoId == id) {
                productoEnTransaccionesActivas = true;
                break;
            }
        }

        if (productoEnTransaccionesActivas) {
            break;
        }
    }

    if (productoEnTransaccionesActivas) {
        Menu::printError("No se puede eliminar el producto porque tiene transacciones activas.");
        return;
    }

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

    try {
        repositories.admin.sincronizarContadoresTienda();
    } catch (const std::exception& e) {
        Menu::printError("Advertencia: producto eliminado, pero no se pudo sincronizar tienda: " +
                         std::string(e.what()));
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
