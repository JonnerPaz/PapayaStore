#include "MenuReportes.hpp"

using std::string;

MenuReportes::MenuReportes(string title, string texToExit, int numOptions, DB& dbService)
    : Menu(), backupService(dbService) {
    this->setTitle("Reportes");
    this->setTexToExit("Salir");
    this->setNumOptions(3);
}

void MenuReportes::verificarIntegridadReferencial() {
    this->backupService.verificarIntegridadReferencial();
}

void MenuReportes::crearBackup() {
    this->backupService.crearBackup();
}

void MenuReportes::reporteStockCritico() {
    this->backupService.reporteStockCritico();
}

void MenuReportes::reporteHistorialCliente() {
    this->backupService.reporteHistorialCliente();
}

void MenuReportes::mostrarResumenTienda() {
    // this->backupService.mostrarResumenTienda();
}

void MenuReportes::showMenu() {
    OpcionMenu options[] = {{"Integridad Referencial", verificarIntegridadReferencial},
                            {"Crear Backup", crearBackup},
                            {"Productos con stock crítico", reporteStockCritico},
                            {"Historial de Cliente", reporteHistorialCliente},
                            {"Resumen de Tienda", mostrarResumenTienda}};
    drawMenu();
}
