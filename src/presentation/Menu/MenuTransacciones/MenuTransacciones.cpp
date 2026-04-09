#include "MenuTransacciones.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "presentation/CliUtils.hpp"

MenuTransacciones::MenuTransacciones(AppRepositories& repositories, CliUtils utils)
    : Menu(repositories)
{
    this->setTitle("Gestion de Transacciones");
    this->setTexToExit("Salir");
    this->setNumOptions(5);
}

bool MenuTransacciones::readValidCantidad(const char* prompt, int& outCantidad)
{
    while (true) {
        const std::string input = readLine(prompt);
        if (input == "q" || input == "Q") {
            return false;
        }

        if (CliUtils::parsePositiveNumber(input, outCantidad, true)) {
            return true;
        }

        Menu::printError("Cantidad inválida. Debe ser un entero mayor a 0.");
    }
}

bool MenuTransacciones::getItemsFromTransaccion(const Transaccion& transaccion,
                                                std::vector<TransaccionDTO>& outItems,
                                                std::string& outError)
{
    outItems.clear();
    const int itemsCount = transaccion.getProductosTotales();
    if (itemsCount <= 0) {
        outError = "La transacción no tiene productos asociados.";
        return false;
    }

    for (int i = 0; i < itemsCount; ++i) {
        TransaccionDTO item{};
        if (!transaccion.getProductoEnIndice(i, item)) {
            outError = "No se pudo leer el item de transacción.";
            return false;
        }

        outItems.push_back(item);
    }

    return true;
}

bool MenuTransacciones::aplicarCambiosStock(const std::vector<TransaccionDTO>& items,
                                            bool incrementarStock, bool ajustarTotalVendido,
                                            std::vector<Producto>& productosOriginales,
                                            std::string& outError)
{
    productosOriginales.clear();

    for (const auto& item : items) {
        auto productoResult = repositories.productos.leerPorId(item.productoId);
        if (std::holds_alternative<std::string>(productoResult)) {
            outError = "Producto no encontrado para actualizar stock. ID: " +
                       std::to_string(item.productoId);
            return false;
        }

        Producto producto = std::get<Producto>(productoResult);
        const int stockActual = producto.getStock();
        const int totalVendidoActual = producto.getTotalVendido();

        int nuevoStock = stockActual;
        if (incrementarStock) {
            nuevoStock += item.cantidad;
        } else {
            if (stockActual < item.cantidad) {
                outError = "Stock insuficiente para producto ID " +
                           std::to_string(item.productoId) +
                           ". Stock actual: " + std::to_string(stockActual);
                return false;
            }
            nuevoStock -= item.cantidad;
        }

        int nuevoTotalVendido = totalVendidoActual;
        if (ajustarTotalVendido) {
            if (incrementarStock) {
                nuevoTotalVendido = (totalVendidoActual >= item.cantidad)
                                        ? (totalVendidoActual - item.cantidad)
                                        : 0;
            } else {
                nuevoTotalVendido += item.cantidad;
            }
        }

        Producto productoActualizado = producto;
        if (!productoActualizado.setStock(nuevoStock)) {
            outError =
                "No se pudo actualizar stock del producto ID " + std::to_string(item.productoId);
            return false;
        }

        if (ajustarTotalVendido && !productoActualizado.setTotalVendido(nuevoTotalVendido)) {
            outError = "No se pudo actualizar total vendido del producto ID " +
                       std::to_string(item.productoId);
            return false;
        }

        productoActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());

        auto updateResult = repositories.productos.actualizar(item.productoId, productoActualizado);
        if (std::holds_alternative<std::string>(updateResult)) {
            outError = "Error al guardar cambios de stock. Producto ID " +
                       std::to_string(item.productoId) + ": " + std::get<std::string>(updateResult);
            return false;
        }

        productosOriginales.push_back(producto);
    }

    return true;
}

bool MenuTransacciones::rollbackStock(const std::vector<Producto>& productosOriginales)
{
    for (const auto& productoOriginal : productosOriginales) {
        auto rollbackResult =
            repositories.productos.actualizar(productoOriginal.getId(), productoOriginal);
        if (std::holds_alternative<std::string>(rollbackResult)) {
            return false;
        }
    }

    return true;
}

float MenuTransacciones::calcularTotalTransaccion(const std::vector<TransaccionDTO>& items,
                                                  std::string& outError)
{
    float total = 0.0f;
    for (const auto& item : items) {
        if (item.cantidad < 0 || item.precio < 0) {
            outError = "Items de transaccion invalidos para calcular total.";
            return -1.0f;
        }

        total += static_cast<float>(item.cantidad) * item.precio;
    }

    outError.clear();
    return total;
}

void MenuTransacciones::imprimirDetalleTransaccion(const Transaccion& transaccion)
{
    const char* tipoStr = (transaccion.getTipoTransaccion() == COMPRA) ? "COMPRA" : "VENTA";
    Menu::printSuccess("Transaccion encontrada:");
    std::cout << std::format("{}ID: {}{}", COLOR_YELLOW, COLOR_GREEN, transaccion.getId())
              << std::endl;
    std::cout << std::format("{}Tipo: {}{}", COLOR_YELLOW, COLOR_GREEN, tipoStr) << std::endl;
    std::cout << std::format("{}ID Relacionado: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             transaccion.getIdRelacionado())
              << std::endl;
    std::cout << std::format("{}Total: {}${:.2f}", COLOR_YELLOW, COLOR_GREEN,
                             transaccion.getTotal())
              << std::endl;
    std::cout << std::format("{}Descripcion: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             transaccion.getDescripcion())
              << std::endl;

    std::vector<TransaccionDTO> items;
    std::string itemsError;
    if (!getItemsFromTransaccion(transaccion, items, itemsError)) {
        Menu::printError(itemsError);
        return;
    }

    std::cout << std::format("{}Items: {}{}", COLOR_YELLOW, COLOR_GREEN, items.size()) << std::endl;
    for (size_t i = 0; i < items.size(); ++i) {
        const auto& item = items[i];
        std::cout << std::format("{}[{}] Producto ID: {}{}", COLOR_YELLOW, i + 1, COLOR_GREEN,
                                 item.productoId)
                  << std::endl;
        std::cout << std::format("{}    Cantidad: {}{}", COLOR_YELLOW, COLOR_GREEN, item.cantidad)
                  << std::endl;
        std::cout << std::format("{}    Precio: {}${:.2f}", COLOR_YELLOW, COLOR_GREEN, item.precio)
                  << std::endl;
    }
}

void MenuTransacciones::registrarCompra()
{
    const int idProveedor = CliUtils::readValidId("Ingrese el id del proveedor para la compra");
    if (idProveedor <= 0) {
        Menu::printError("Operacion cancelada.");
        return;
    }

    auto proveedorResult = repositories.proveedores.leerPorId(idProveedor);
    if (std::holds_alternative<std::string>(proveedorResult)) {
        Menu::printError("Proveedor invalido: " + std::get<std::string>(proveedorResult));
        return;
    }

    Transaccion transaccion;
    transaccion.setTipoTransaccion(COMPRA);
    transaccion.setIdRelacionado(idProveedor);
    transaccion.setDescripcion("Compra registrada desde menu");

    while (true) {
        const int idProducto = CliUtils::readValidId("Ingrese id de producto (q para finalizar)");
        if (idProducto <= 0) {
            if (transaccion.getProductosTotales() == 0) {
                Menu::printError("Operacion cancelada.");
                return;
            }
            break;
        }

        auto productoResult = repositories.productos.leerPorId(idProducto);
        if (std::holds_alternative<std::string>(productoResult)) {
            Menu::printError("Producto invalido: " + std::get<std::string>(productoResult));
            continue;
        }

        Producto producto = std::get<Producto>(productoResult);
        if (producto.getIdProveedor() != idProveedor) {
            Menu::printError("El producto no pertenece al proveedor seleccionado.");
            continue;
        }

        int cantidad = 0;
        if (!readValidCantidad("Cantidad comprada (q para cancelar item): ", cantidad)) {
            Menu::printError("Item cancelado.");
            continue;
        }

        TransaccionDTO item{
            .productoId = idProducto, .cantidad = cantidad, .precio = producto.getPrecio()};

        if (!transaccion.setProducto(item)) {
            Menu::printError("No se pudo agregar el producto a la transaccion.");
            return;
        }

        Menu::printSuccess("Producto agregado a la compra.");
        if (!confirmAction("Desea agregar otro producto? (s/n): ")) {
            break;
        }
    }

    std::vector<TransaccionDTO> items;
    std::string itemsError;
    if (!getItemsFromTransaccion(transaccion, items, itemsError)) {
        Menu::printError(itemsError);
        return;
    }

    std::string totalError;
    const float total = calcularTotalTransaccion(items, totalError);
    if (total < 0.0f) {
        Menu::printError(totalError);
        return;
    }
    transaccion.setTotal(total);

    auto transHeaderResult = repositories.transacciones.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(transHeaderResult)) {
        Menu::printError("Error: " + std::get<std::string>(transHeaderResult));
        return;
    }

    const int nuevoId = std::get<HeaderFile>(transHeaderResult).proximoID;
    transaccion.setId(nuevoId);
    transaccion.setEliminado(false);
    transaccion.setFechaUltimaModificacion(std::chrono::system_clock::now());

    std::cout << std::format("Se registrara la compra ID {} por un total de ${:.2f}", nuevoId,
                             total)
              << std::endl;
    if (!confirmAction("Confirmar compra? (s/n): ")) {
        Menu::printError("Operacion cancelada.");
        return;
    }

    auto saveResult = repositories.transacciones.guardar(transaccion);
    if (std::holds_alternative<std::string>(saveResult)) {
        Menu::printError("Error al guardar transaccion: " + std::get<std::string>(saveResult));
        return;
    }

    std::vector<Producto> productosOriginales;
    std::string stockError;
    if (!aplicarCambiosStock(items, true, false, productosOriginales, stockError)) {
        repositories.transacciones.eliminarLogicamente(nuevoId);
        rollbackStock(productosOriginales);
        Menu::printError("Error al actualizar stock de compra: " + stockError);
        return;
    }

    repositories.admin.sincronizarContadoresTienda();
    Menu::printSuccess("Compra registrada con exito.");
}

void MenuTransacciones::registrarVenta()
{
    const int idCliente = CliUtils::readValidId("Ingrese el id del cliente para la venta");
    if (idCliente <= 0) {
        Menu::printError("Operacion cancelada.");
        return;
    }

    auto clienteResult = repositories.clientes.leerPorId(idCliente);
    if (std::holds_alternative<std::string>(clienteResult)) {
        Menu::printError("Cliente invalido: " + std::get<std::string>(clienteResult));
        return;
    }

    Transaccion transaccion;
    transaccion.setTipoTransaccion(VENTA);
    transaccion.setIdRelacionado(idCliente);
    transaccion.setDescripcion("Venta registrada desde menu");

    while (true) {
        const int idProducto = CliUtils::readValidId("Ingrese id de producto (q para finalizar)");
        if (idProducto <= 0) {
            if (transaccion.getProductosTotales() == 0) {
                Menu::printError("Operacion cancelada.");
                return;
            }
            break;
        }

        auto productoResult = repositories.productos.leerPorId(idProducto);
        if (std::holds_alternative<std::string>(productoResult)) {
            Menu::printError("Producto invalido: " + std::get<std::string>(productoResult));
            continue;
        }

        Producto producto = std::get<Producto>(productoResult);
        int cantidad = 0;
        if (!readValidCantidad("Cantidad vendida (q para cancelar item): ", cantidad)) {
            Menu::printError("Item cancelado.");
            continue;
        }

        if (producto.getStock() < cantidad) {
            Menu::printError("Stock insuficiente para ese producto.");
            continue;
        }

        TransaccionDTO item{
            .productoId = idProducto, .cantidad = cantidad, .precio = producto.getPrecio()};
        if (!transaccion.setProducto(item)) {
            Menu::printError("No se pudo agregar el producto a la transaccion.");
            return;
        }

        Menu::printSuccess("Producto agregado a la venta.");
        if (!confirmAction("Desea agregar otro producto? (s/n): ")) {
            break;
        }
    }

    std::vector<TransaccionDTO> items;
    std::string itemsError;
    if (!getItemsFromTransaccion(transaccion, items, itemsError)) {
        Menu::printError(itemsError);
        return;
    }

    for (const auto& item : items) {
        auto productoResult = repositories.productos.leerPorId(item.productoId);
        if (std::holds_alternative<std::string>(productoResult)) {
            Menu::printError("Producto invalido durante validacion final. ID: " +
                             std::to_string(item.productoId));
            return;
        }

        const Producto& producto = std::get<Producto>(productoResult);
        if (producto.getStock() < item.cantidad) {
            Menu::printError("Stock insuficiente en validacion final para producto ID " +
                             std::to_string(item.productoId));
            return;
        }
    }

    std::string totalError;
    const float total = calcularTotalTransaccion(items, totalError);
    if (total < 0.0f) {
        Menu::printError(totalError);
        return;
    }
    transaccion.setTotal(total);

    auto transHeaderResult = repositories.transacciones.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(transHeaderResult)) {
        Menu::printError("Error: " + std::get<std::string>(transHeaderResult));
        return;
    }

    const int nuevoId = std::get<HeaderFile>(transHeaderResult).proximoID;
    transaccion.setId(nuevoId);
    transaccion.setEliminado(false);
    transaccion.setFechaUltimaModificacion(std::chrono::system_clock::now());

    std::cout << std::format("Se registrara la venta ID {} por un total de ${:.2f}", nuevoId, total)
              << std::endl;
    if (!confirmAction("Confirmar venta? (s/n): ")) {
        Menu::printError("Operacion cancelada.");
        return;
    }

    auto saveResult = repositories.transacciones.guardar(transaccion);
    if (std::holds_alternative<std::string>(saveResult)) {
        Menu::printError("Error al guardar transaccion: " + std::get<std::string>(saveResult));
        return;
    }

    std::vector<Producto> productosOriginales;
    std::string stockError;
    if (!aplicarCambiosStock(items, false, true, productosOriginales, stockError)) {
        const bool stockRollback = rollbackStock(productosOriginales);
        auto deleteResult = repositories.transacciones.eliminarLogicamente(nuevoId);

        std::string error = "Error al actualizar stock de venta: " + stockError;
        if (!stockRollback) {
            error += " No se pudo revertir el stock aplicado.";
        }
        if (std::holds_alternative<std::string>(deleteResult)) {
            error += " No se pudo revertir la transacción: " + std::get<std::string>(deleteResult);
        }

        Menu::printError(error);
        return;
    }

    auto clienteResultActual = repositories.clientes.leerPorId(idCliente);
    if (std::holds_alternative<std::string>(clienteResultActual)) {
        const bool stockRollback = rollbackStock(productosOriginales);
        auto deleteResult = repositories.transacciones.eliminarLogicamente(nuevoId);

        std::string error = "No se pudo actualizar métricas del cliente: " +
                            std::get<std::string>(clienteResultActual);
        if (!stockRollback) {
            error += " No se pudo revertir el stock aplicado.";
        }
        if (std::holds_alternative<std::string>(deleteResult)) {
            error += " No se pudo revertir la transacción: " + std::get<std::string>(deleteResult);
        }

        Menu::printError(error);
        return;
    }

    Cliente cliente = std::get<Cliente>(clienteResultActual);
    Cliente clienteOriginal = cliente;

    if (!cliente.setTotalCompras(cliente.getTotalCompras() + total)) {
        const bool stockRollback = rollbackStock(productosOriginales);
        auto deleteResult = repositories.transacciones.eliminarLogicamente(nuevoId);

        std::string error = "No se pudo actualizar total de compras del cliente.";
        if (!stockRollback) {
            error += " No se pudo revertir el stock aplicado.";
        }
        if (std::holds_alternative<std::string>(deleteResult)) {
            error += " No se pudo revertir la transacción: " + std::get<std::string>(deleteResult);
        }

        Menu::printError(error);
        return;
    }

    if (!cliente.agregarTransaccionId(nuevoId)) {
        const bool stockRollback = rollbackStock(productosOriginales);
        auto deleteResult = repositories.transacciones.eliminarLogicamente(nuevoId);

        std::string error =
            "No se pudo actualizar historial del cliente (capacidad máxima alcanzada).";
        if (!stockRollback) {
            error += " No se pudo revertir el stock aplicado.";
        }
        if (std::holds_alternative<std::string>(deleteResult)) {
            error += " No se pudo revertir la transacción: " + std::get<std::string>(deleteResult);
        }

        Menu::printError(error);
        return;
    }

    cliente.setFechaUltimaModificacion(std::chrono::system_clock::now());
    auto clientePersistResult = repositories.clientes.actualizar(idCliente, cliente);
    if (std::holds_alternative<std::string>(clientePersistResult)) {
        const bool stockRollback = rollbackStock(productosOriginales);
        auto deleteResult = repositories.transacciones.eliminarLogicamente(nuevoId);
        auto clienteRollback = repositories.clientes.actualizar(idCliente, clienteOriginal);

        std::string error = "No se pudo guardar métricas del cliente: " +
                            std::get<std::string>(clientePersistResult);
        if (!stockRollback) {
            error += " No se pudo revertir el stock aplicado.";
        }
        if (std::holds_alternative<std::string>(deleteResult)) {
            error += " No se pudo revertir la transacción: " + std::get<std::string>(deleteResult);
        }
        if (std::holds_alternative<std::string>(clienteRollback)) {
            error += " No se pudo revertir el cliente: " + std::get<std::string>(clienteRollback);
        }

        Menu::printError(error);
        return;
    }

    repositories.admin.sincronizarContadoresTienda();
    Menu::printSuccess("Venta registrada con exito.");
}

void MenuTransacciones::buscarTransacciones()
{
    const int id = CliUtils::readValidId("Ingrese el id de la transaccion a buscar");
    if (id <= 0) {
        Menu::printError("Busqueda cancelada.");
        return;
    }

    auto result = repositories.transacciones.leerPorId(id);
    if (std::holds_alternative<std::string>(result)) {
        Menu::printError("Error: " + std::get<std::string>(result));
        return;
    }

    imprimirDetalleTransaccion(std::get<Transaccion>(result));
}

void MenuTransacciones::listarTransacciones()
{
    auto headerResult = repositories.transacciones.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(headerResult)) {
        Menu::printError("Error: " + std::get<std::string>(headerResult));
        return;
    }

    const HeaderFile header = std::get<HeaderFile>(headerResult);
    if (header.registrosActivos == 0) {
        Menu::printError("No hay transacciones registradas.");
        return;
    }

    std::cout << std::format("{}--- Lista de Transacciones ({}{}{}) ---", COLOR_CYAN, COLOR_GREEN,
                             COLOR_YELLOW, header.registrosActivos)
              << std::endl;
    std::cout << std::format("{}{:<5} | {:<8} | {:<8} | {:<10} | {:<10}", COLOR_YELLOW, "ID",
                             "Tipo", "Items", "Total", "Relacionado")
              << std::endl;
    std::cout << "---------------------------------------------------------------" << std::endl;

    for (int id = 1; id < header.proximoID; ++id) {
        auto result = repositories.transacciones.leerPorId(id);
        if (!std::holds_alternative<Transaccion>(result)) {
            continue;
        }

        const Transaccion& transaccion = std::get<Transaccion>(result);
        const char* tipoStr = (transaccion.getTipoTransaccion() == COMPRA) ? "COMPRA" : "VENTA";

        std::cout << std::format("{}{:<5} | {:<8} | {:<8} | ${:<9.2f} | {:<10}", COLOR_GREEN,
                                 transaccion.getId(), tipoStr, transaccion.getProductosTotales(),
                                 transaccion.getTotal(), transaccion.getIdRelacionado())
                  << std::endl;
    }
}

void MenuTransacciones::cancelarTransaccion()
{
    const int id = CliUtils::readValidId("Ingrese el id de la transaccion a cancelar");
    if (id <= 0) {
        Menu::printError("Operacion cancelada.");
        return;
    }

    auto transResult = repositories.transacciones.leerPorId(id);
    if (std::holds_alternative<std::string>(transResult)) {
        Menu::printError("Error: " + std::get<std::string>(transResult));
        return;
    }

    Transaccion transaccion = std::get<Transaccion>(transResult);

    std::vector<TransaccionDTO> items;
    std::string itemsError;
    if (!getItemsFromTransaccion(transaccion, items, itemsError)) {
        Menu::printError(itemsError);
        return;
    }

    const char* tipoStr = (transaccion.getTipoTransaccion() == COMPRA) ? "COMPRA" : "VENTA";
    std::cout << std::format("Se cancelara la transaccion {} de tipo {} por total ${:.2f}",
                             transaccion.getId(), tipoStr, transaccion.getTotal())
              << std::endl;
    if (!confirmAction("Confirmar cancelacion? (s/n): ")) {
        Menu::printError("Operacion cancelada.");
        return;
    }

    if (transaccion.getTipoTransaccion() == COMPRA) {
        for (const auto& item : items) {
            auto productoResult = repositories.productos.leerPorId(item.productoId);
            if (std::holds_alternative<std::string>(productoResult)) {
                Menu::printError("No se pudo validar stock para cancelar compra. Producto ID: " +
                                 std::to_string(item.productoId));
                return;
            }

            const Producto& producto = std::get<Producto>(productoResult);
            if (producto.getStock() < item.cantidad) {
                Menu::printError(
                    "No se puede cancelar la compra: el stock actual es menor al ajustado.");
                return;
            }
        }
    }

    std::vector<Producto> productosOriginales;
    std::string stockError;
    const bool incrementarStock = transaccion.getTipoTransaccion() == VENTA;
    const bool ajustarTotalVendido = transaccion.getTipoTransaccion() == VENTA;
    if (!aplicarCambiosStock(items, incrementarStock, ajustarTotalVendido, productosOriginales,
                             stockError)) {
        rollbackStock(productosOriginales);
        Menu::printError("Error al revertir stock: " + stockError);
        return;
    }

    Cliente clienteOriginal;
    bool clienteActualizado = false;
    if (transaccion.getTipoTransaccion() == VENTA) {
        auto clienteResult = repositories.clientes.leerPorId(transaccion.getIdRelacionado());
        if (std::holds_alternative<std::string>(clienteResult)) {
            rollbackStock(productosOriginales);
            Menu::printError("No se pudo actualizar métricas del cliente al cancelar: " +
                             std::get<std::string>(clienteResult));
            return;
        }

        Cliente cliente = std::get<Cliente>(clienteResult);
        clienteOriginal = cliente;

        float totalComprasActualizado = cliente.getTotalCompras() - transaccion.getTotal();
        if (totalComprasActualizado < 0.0f) {
            totalComprasActualizado = 0.0f;
        }

        if (!cliente.setTotalCompras(totalComprasActualizado)) {
            rollbackStock(productosOriginales);
            Menu::printError("No se pudo ajustar total de compras del cliente.");
            return;
        }

        if (!cliente.removerTransaccionId(transaccion.getId())) {
            rollbackStock(productosOriginales);
            Menu::printError("No se pudo ajustar historial del cliente.");
            return;
        }

        cliente.setFechaUltimaModificacion(std::chrono::system_clock::now());
        auto updateClienteResult = repositories.clientes.actualizar(cliente.getId(), cliente);
        if (std::holds_alternative<std::string>(updateClienteResult)) {
            rollbackStock(productosOriginales);
            Menu::printError("No se pudo guardar cambios del cliente: " +
                             std::get<std::string>(updateClienteResult));
            return;
        }

        clienteActualizado = true;
    }

    auto deleteResult = repositories.transacciones.eliminarLogicamente(id);
    if (std::holds_alternative<std::string>(deleteResult)) {
        const bool stockRollback = rollbackStock(productosOriginales);
        std::string error = "Error al cancelar transaccion: " + std::get<std::string>(deleteResult);

        if (!stockRollback) {
            error += " No se pudo revertir el stock ajustado.";
        }

        if (clienteActualizado) {
            auto clienteRollback =
                repositories.clientes.actualizar(clienteOriginal.getId(), clienteOriginal);
            if (std::holds_alternative<std::string>(clienteRollback)) {
                error += " No se pudo revertir estado del cliente: " +
                         std::get<std::string>(clienteRollback);
            }
        }

        Menu::printError(error);
        return;
    }

    repositories.admin.sincronizarContadoresTienda();
    Menu::printSuccess("Transaccion cancelada con exito.");
}

void MenuTransacciones::showMenu()
{
    setOption(0, "Registrar Compra", [this]() { registrarCompra(); });
    setOption(1, "Registrar Venta", [this]() { registrarVenta(); });
    setOption(2, "Buscar Transacciones", [this]() { buscarTransacciones(); });
    setOption(3, "Listar Transacciones", [this]() { listarTransacciones(); });
    setOption(4, "Cancelar Transaccion", [this]() { cancelarTransaccion(); });
    drawMenu();
}
