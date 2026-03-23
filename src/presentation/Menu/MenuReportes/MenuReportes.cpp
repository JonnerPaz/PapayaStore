#include "MenuReportes.hpp"

using std::string;

MenuReportes::MenuReportes(string title, string texToExit, int numOptions, AppRepositories& r)
    : Menu(r) {
    this->setTitle(title);
    this->setTexToExit(texToExit);
    this->setNumOptions(numOptions);
}

void MenuReportes::verificarIntegridadReferencial() {
    this->repositories.admin.verificarIntegridadReferencial();
}

void MenuReportes::crearBackup() {
    this->repositories.admin.crearBackup();
}

void MenuReportes::reporteStockCritico() {
    this->repositories.admin.reporteStockCritico();
}

void MenuReportes::reporteHistorialCliente() {
    this->repositories.admin.reporteHistorialCliente();
}

void MenuReportes::mostrarResumenTienda() {
    // this->repos.admin.mostrarResumenTienda();
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
