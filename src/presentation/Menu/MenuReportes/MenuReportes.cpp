#include "MenuReportes.hpp"

#include <format>
#include <iostream>

using std::string;

MenuReportes::MenuReportes(string title, string texToExit, int numOptions, AppRepositories& r)
    : Menu(r), menuTienda(r) {
    this->setTitle(title);
    this->setTexToExit(texToExit);
    this->setNumOptions(numOptions);
}

void MenuReportes::verificarIntegridadReferencial() {
    auto [erroresProductosProveedor, erroresTransaccionRelacionado, erroresTransaccionProducto,
          erroresTipoTransaccion] = this->repositories.admin.verificarIntegridadReferencial();

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

void MenuReportes::crearBackup() {
    this->repositories.admin.crearBackup();
    std::cout << COLOR_GREEN << "Backup creado correctamente." << COLOR_RESET << std::endl;
}

void MenuReportes::reporteStockCritico() {
    const int totalCriticos = this->repositories.admin.reporteStockCritico();
    std::cout << std::format("{}Productos con stock critico: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             totalCriticos)
              << COLOR_RESET << std::endl;
}

void MenuReportes::reporteHistorialCliente() {
    this->repositories.admin.reporteHistorialCliente();
}

void MenuReportes::mostrarResumenTienda() {
    this->menuTienda.mostrarResumenTienda();
}

void MenuReportes::showMenu() {
    this->setNumOptions(5);
    setOption(0, "Integridad Referencial", [this]() { this->verificarIntegridadReferencial(); });
    setOption(1, "Crear Backup", [this]() { this->crearBackup(); });
    setOption(2, "Productos con stock crítico", [this]() { this->reporteStockCritico(); });
    setOption(3, "Historial de Cliente", [this]() { this->reporteHistorialCliente(); });
    setOption(4, "Resumen de Tienda", [this]() { this->mostrarResumenTienda(); });
    drawMenu();
}
