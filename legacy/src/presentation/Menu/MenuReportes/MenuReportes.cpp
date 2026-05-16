#include "MenuReportes.hpp"

#include <cctype>
#include <format>
#include <fstream>
#include <iostream>

#include "domain/constants.hpp"
#include "domain/entities/tienda/tienda.entity.hpp"
#include "infrastructure/datasource/EntityTraits.hpp"

using std::string;

MenuReportes::MenuReportes(string title, string texToExit, int numOptions, AppRepositories& r)
    : Menu(r)
{
    this->setTitle(title);
    this->setTexToExit(texToExit);
    this->setNumOptions(numOptions);
}

void MenuReportes::verificarIntegridadReferencial()
{
    int erroresProductosProveedor = 0;
    int erroresTransaccionRelacionado = 0;
    int erroresTransaccionProducto = 0;
    int erroresTipoTransaccion = 0;

    try {
        std::tie(erroresProductosProveedor, erroresTransaccionRelacionado,
                 erroresTransaccionProducto, erroresTipoTransaccion) =
            this->repositories.admin.verificarIntegridadReferencial();
    } catch (const std::exception& e) {
        Menu::printError("Error al verificar integridad referencial: " + std::string(e.what()));
        return;
    }

    std::cout << COLOR_CYAN << "Resultado de integridad referencial" << COLOR_RESET << std::endl;
    std::cout << std::format("{}Productos sin proveedor valido: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             erroresProductosProveedor)
              << COLOR_RESET << std::endl;
    std::cout << std::format("{}Transacciones sin relacionado valido: {}{}", COLOR_YELLOW,
                             COLOR_GREEN, erroresTransaccionRelacionado)
              << COLOR_RESET << std::endl;
    std::cout << std::format("{}Transacciones con productos invalidos: {}{}", COLOR_YELLOW,
                             COLOR_GREEN, erroresTransaccionProducto)
              << COLOR_RESET << std::endl;
    std::cout << std::format("{}Transacciones con tipo invalido: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             erroresTipoTransaccion)
              << COLOR_RESET << std::endl;
}

void MenuReportes::crearBackup()
{
    try {
        this->repositories.admin.crearBackup();
    } catch (const std::exception& e) {
        Menu::printError("Error al crear backup: " + std::string(e.what()));
        return;
    }

    std::cout << COLOR_GREEN << "Backup creado correctamente." << COLOR_RESET << std::endl;
}

void MenuReportes::reporteStockCritico()
{
    int totalCriticos = 0;
    try {
        totalCriticos = this->repositories.admin.reporteStockCritico();
    } catch (const std::exception& e) {
        Menu::printError("Error al generar reporte de stock critico: " + std::string(e.what()));
        return;
    }

    std::cout << std::format("{}Productos con stock critico: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             totalCriticos)
              << COLOR_RESET << std::endl;
}

void MenuReportes::reporteHistorialCliente()
{
    const std::string idText = readLine("Ingrese el id del cliente (q para cancelar): ");
    if (idText == "q" || idText == "Q" || idText.empty()) {
        printError("Operacion cancelada.");
        return;
    }

    int idCliente = 0;
    try {
        std::size_t pos = 0;
        idCliente = std::stoi(idText, &pos, 10);
        for (std::size_t i = pos; i < idText.size(); ++i) {
            if (!std::isspace(static_cast<unsigned char>(idText[i]))) {
                printError("ID invalido. Debe ser numerico.");
                return;
            }
        }
    } catch (...) {
        printError("ID invalido. Debe ser numerico.");
        return;
    }

    if (idCliente <= 0) {
        printError("ID invalido. Debe ser mayor a 0.");
        return;
    }

    try {
        this->repositories.admin.reporteHistorialCliente(idCliente);
    } catch (const std::exception& e) {
        Menu::printError("Error al generar historial del cliente: " + std::string(e.what()));
        return;
    }
}

void MenuReportes::mostrarResumenTienda()
{
    auto productosHeader = repositories.productos.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(productosHeader)) {
        Menu::printError("Error: " + std::get<std::string>(productosHeader));
        return;
    }

    auto proveedoresHeader = repositories.proveedores.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(proveedoresHeader)) {
        Menu::printError("Error: " + std::get<std::string>(proveedoresHeader));
        return;
    }

    auto clientesHeader = repositories.clientes.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(clientesHeader)) {
        Menu::printError("Error: " + std::get<std::string>(clientesHeader));
        return;
    }

    auto transaccionesHeader = repositories.transacciones.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(transaccionesHeader)) {
        Menu::printError("Error: " + std::get<std::string>(transaccionesHeader));
        return;
    }

    Tienda tienda;
    bool tiendaCargada = false;
    std::ifstream tiendaFile(Constants::PATHS::TIENDA_PATH, std::ios::binary);
    if (tiendaFile.is_open()) {
        HeaderFile tiendaHeader = {};
        tiendaFile.read(reinterpret_cast<char*>(&tiendaHeader), sizeof(HeaderFile));
        if (tiendaFile && tiendaHeader.cantidadRegistros > 0) {
            tiendaFile.seekg(sizeof(HeaderFile), std::ios::beg);
            tiendaCargada =
                EntityTraits<Tienda>::readFromStream(tiendaFile, tienda) && !tienda.getEliminado();
        }
    }

    if (!tiendaCargada) {
        tienda.setId(1);
        tienda.setNombre("Papaya Store");
        tienda.setRif("N/A");
    }

    Menu::printSuccess("Resumen de tienda:");
    std::cout << std::format("{}ID: {}{}", COLOR_YELLOW, COLOR_GREEN, tienda.getId()) << std::endl;
    std::cout << std::format("{}Nombre: {}{}", COLOR_YELLOW, COLOR_GREEN, tienda.getNombre())
              << std::endl;
    std::cout << std::format("{}RIF: {}{}", COLOR_YELLOW, COLOR_GREEN, tienda.getRif())
              << std::endl;
    std::cout << std::format("{}Productos activos: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             std::get<HeaderFile>(productosHeader).registrosActivos)
              << std::endl;
    std::cout << std::format("{}Proveedores activos: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             std::get<HeaderFile>(proveedoresHeader).registrosActivos)
              << std::endl;
    std::cout << std::format("{}Clientes activos: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             std::get<HeaderFile>(clientesHeader).registrosActivos)
              << std::endl;
    std::cout << std::format("{}Transacciones activas: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             std::get<HeaderFile>(transaccionesHeader).registrosActivos)
              << std::endl;
    std::cout << std::format("{}Monto total ventas: {}${:.2f}", COLOR_YELLOW, COLOR_GREEN,
                             tienda.getMontoTotalVentas())
              << std::endl;
    std::cout << std::format("{}Monto total compras: {}${:.2f}", COLOR_YELLOW, COLOR_GREEN,
                             tienda.getMontoTotalCompras())
              << std::endl;
}

void MenuReportes::showMenu()
{
    this->setNumOptions(5);
    setOption(0, "Integridad Referencial", [this]() { this->verificarIntegridadReferencial(); });
    setOption(1, "Crear Backup", [this]() { this->crearBackup(); });
    setOption(2, "Productos con stock crítico", [this]() { this->reporteStockCritico(); });
    setOption(3, "Historial de Cliente", [this]() { this->reporteHistorialCliente(); });
    setOption(4, "Resumen de Tienda", [this]() { this->mostrarResumenTienda(); });
    drawMenu();
}
