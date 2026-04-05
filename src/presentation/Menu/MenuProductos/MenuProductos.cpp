#include "MenuProductos.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <variant>

MenuProductos::MenuProductos(AppRepositories& repository, CliUtils utils)
    : Menu(repository), utils(utils)
{
    Menu::setTitle("Gestion de Productos");
    Menu::setTexToExit("Salir");
    Menu::setNumOptions(5);
}

void MenuProductos::crearProducto()
{
    auto proveedoresHeader = repositories.proveedores.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(proveedoresHeader)) {
        std::cout << "Error: " << std::get<std::string>(proveedoresHeader) << std::endl;
        return;
    }

    if (std::get<HeaderFile>(proveedoresHeader).registrosActivos == 0) {
        std::cout << "No hay proveedores registrados. Debe crear al menos uno "
                     "primero."
                  << std::endl;
        return;
    }

    std::string nombre = readLine("Ingrese el nombre del producto (q para cancelar): ");
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
        const std::string precioText = Menu::readLine("Ingrese el precio del producto: ");
        if (utils.parsePositiveFloat(precioText, precio, false)) {
            break;
        }
        std::cout << "Precio invalido. Debe ser un numero mayor o igual a 0." << std::endl;
    }

    int stock = 0;
    while (true) {
        const std::string stockText = Menu::readLine("Ingrese el stock del producto: ");
        if (utils.parsePositiveInt(stockText, stock)) {
            break;
        }
        std::cout << "Stock invalido. Debe ser un entero mayor o igual a 0." << std::endl;
    }

    int stockMinimo = 0;
    while (true) {
        const std::string stockMinText = readLine("Ingrese el stock minimo del producto: ");
        if (utils.parsePositiveInt(stockMinText, stockMinimo)) {
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

    auto productosHeader = repositories.productos.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(productosHeader)) {
        std::cout << "Error: " << std::get<std::string>(productosHeader) << std::endl;
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

    std::cout << std::format("Se creara el producto: {} ({})", producto.getNombre(),
                             producto.getCodigo())
              << std::endl;
    if (!Menu::confirmAction("Confirma creacion? (s/n): ")) {
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

void MenuProductos::buscarProducto()
{
    const int id = utils.validarId("Ingrese el id del producto a buscar");
    if (id <= 0) {
        std::cout << "Busqueda cancelada." << std::endl;
        return;
    }

    auto result = repositories.productos.leerPorId(id);
    if (std::holds_alternative<std::string>(result)) {
        std::cout << "Error: " << std::get<std::string>(result) << std::endl;
        return;
    }

    const Producto& producto = std::get<Producto>(result);
    std::cout << "Producto encontrado:" << std::endl;
    std::cout << std::format("ID: {}", producto.getId()) << std::endl;
    std::cout << std::format("Nombre: {}", producto.getNombre()) << std::endl;
    std::cout << std::format("Codigo: {}", producto.getCodigo()) << std::endl;
    std::cout << std::format("Descripcion: {}", producto.getDescripcion()) << std::endl;
    std::cout << std::format("Precio: {:.2f}", producto.getPrecio()) << std::endl;
    std::cout << std::format("Stock: {}", producto.getStock()) << std::endl;
}

void MenuProductos::actualizarProducto()
{
    const int id = utils.validarId("Ingrese el id del producto a actualizar");
    if (id <= 0) {
        std::cout << "Actualizacion cancelada." << std::endl;
        return;
    }

    auto result = repositories.productos.leerPorId(id);
    if (std::holds_alternative<std::string>(result)) {
        std::cout << "Error: " << std::get<std::string>(result) << std::endl;
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
        if (!parsePositiveFloat(nuevoPrecioText, nuevoPrecio, false)) {
            std::cout << "Precio invalido." << std::endl;
            return;
        }
        producto.setPrecio(nuevoPrecio);
    }

    const std::string nuevoStockText = readLine("Nuevo stock (enter para mantener actual): ");
    int nuevoStock = 0;
    if (!nuevoStockText.empty()) {
        if (!parsePositiveInt(nuevoStockText, nuevoStock)) {
            std::cout << "Stock invalido." << std::endl;
            return;
        }
        producto.setStock(nuevoStock);
    }

    const std::string nuevoStockMinimoText =
        readLine("Nuevo stock minimo (enter para mantener actual): ");
    int nuevoStockMinimo = 0;
    if (!nuevoStockMinimoText.empty()) {
        if (!parsePositiveInt(nuevoStockMinimoText, nuevoStockMinimo)) {
            std::cout << "Stock minimo invalido." << std::endl;
            return;
        }
        producto.setStockMinimo(nuevoStockMinimo);
    }

    const std::string nuevoProveedorText =
        readLine("Nuevo id de proveedor (enter para mantener actual): ");
    int nuevoIdProveedor = 0;
    if (!nuevoProveedorText.empty()) {
        if (!parsePositiveInt(nuevoProveedorText, nuevoIdProveedor)) {
            std::cout << "ID de proveedor invalido." << std::endl;
            return;
        }

        auto proveedorResult = repositories.proveedores.leerPorId(nuevoIdProveedor);
        if (std::holds_alternative<std::string>(proveedorResult)) {
            std::cout << "Proveedor invalido: " << std::get<std::string>(proveedorResult)
                      << std::endl;
            return;
        }

        producto.setIdProveedor(nuevoIdProveedor);
    }

    producto.setFechaUltimaModificacion(std::chrono::system_clock::now());

    if (!confirmAction("Confirma actualizacion? (s/n): ")) {
        std::cout << "Actualizacion cancelada." << std::endl;
        return;
    }

    auto updateResult = repositories.productos.actualizar(id, producto);
    if (std::holds_alternative<std::string>(updateResult)) {
        std::cout << "Error al actualizar: " << std::get<std::string>(updateResult) << std::endl;
        return;
    }

    std::cout << "Producto actualizado con exito." << std::endl;
}

void MenuProductos::listarProductos()
{
    auto statsResult = repositories.productos.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(statsResult)) {
        std::cout << "Error: " << std::get<std::string>(statsResult) << std::endl;
        return;
    }

    HeaderFile stats = std::get<HeaderFile>(statsResult);
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

void MenuProductos::eliminarProducto()
{
    const int id = utils.validarId("Ingrese el id del producto a eliminar");
    if (id <= 0) {
        std::cout << "Eliminacion cancelada." << std::endl;
        return;
    }

    auto result = repositories.productos.leerPorId(id);
    if (std::holds_alternative<std::string>(result)) {
        std::cout << "Error: " << std::get<std::string>(result) << std::endl;
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

    std::cout << "Producto eliminado con exito." << std::endl;
}

// [this]() { crearProducto(); } lambda function (como pasar callback)
void MenuProductos::showMenu()
{
    setOption(0, "Crear Producto", [this]() { crearProducto(); });
    setOption(1, "Buscar Producto", [this]() { buscarProducto(); });
    setOption(2, "Actualizar Producto", [this]() { actualizarProducto(); });
    setOption(3, "Listar Productos", [this]() { listarProductos(); });
    setOption(4, "Eliminar Producto", [this]() { eliminarProducto(); });

    drawMenu();
}
