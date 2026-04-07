#include "MenuTienda.hpp"

#include <format>
#include <fstream>
#include <iostream>
#include <variant>

#include "domain/constants.hpp"
#include "domain/entities/tienda/tienda.entity.hpp"
#include "infrastructure/datasource/EntityTraits.hpp"

MenuTienda::MenuTienda(AppRepositories& repos) : Menu(repos)
{
    setTitle("Gestion de Tienda");
    setTexToExit("Salir");
    setNumOptions(1);
}

void MenuTienda::mostrarResumenTienda()
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

    float montoTotalVentas = 0.0f;
    float montoTotalCompras = 0.0f;
    HeaderFile transStats = std::get<HeaderFile>(transaccionesHeader);
    for (int id = 1; id < transStats.proximoID; ++id) {
        auto transaccionResult = repositories.transacciones.leerPorId(id);
        if (!std::holds_alternative<Transaccion>(transaccionResult)) {
            continue;
        }

        const Transaccion& transaccion = std::get<Transaccion>(transaccionResult);
        if (transaccion.getTipoTransaccion() == VENTA) {
            montoTotalVentas += transaccion.getTotal();
        } else if (transaccion.getTipoTransaccion() == COMPRA) {
            montoTotalCompras += transaccion.getTotal();
        }
    }

    Tienda tienda;
    bool tiendaCargada = false;
    std::ifstream tiendaFile(Constants::PATHS::TIENDA_PATH, std::ios::binary);
    if (tiendaFile.is_open()) {
        HeaderFile tiendaHeader = {};
        tiendaFile.read(reinterpret_cast<char*>(&tiendaHeader), sizeof(HeaderFile));
        if (tiendaFile && tiendaHeader.cantidadRegistros > 0) {
            tiendaFile.seekg(sizeof(HeaderFile), std::ios::beg);
            tiendaCargada = EntityTraits<Tienda>::readFromStream(tiendaFile, tienda) &&
                            !tienda.getEliminado();
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
                             montoTotalVentas)
              << std::endl;
    std::cout << std::format("{}Monto total compras: {}${:.2f}", COLOR_YELLOW, COLOR_GREEN,
                             montoTotalCompras)
              << std::endl;
}

void MenuTienda::showMenu()
{
    setOption(0, "Mostrar resumen de tienda", [this]() { mostrarResumenTienda(); });
    drawMenu();
}
