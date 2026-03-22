#include "MenuReportes.hpp"

using std::string;

MenuReportes::MenuReportes(string title, string texToExit, int numOptions,
                           AppRepositories& r)
    : Menu(), repos(r)
{
    this->setTitle("Reportes");
    this->setTexToExit("Salir");
    this->setNumOptions(3);
}

void MenuReportes::verificarIntegridadReferencial()
{
    this->repos.admin.verificarIntegridadReferencial();
}

void MenuReportes::crearBackup()
{
    this->repos.admin.crearBackup();
}

void MenuReportes::reporteStockCritico()
{
    this->repos.admin.reporteStockCritico();
}

void MenuReportes::reporteHistorialCliente()
{
    this->repos.admin.reporteHistorialCliente();
}

void MenuReportes::mostrarResumenTienda()
{
    // this->repos.admin.mostrarResumenTienda();
}

void MenuReportes::showMenu()
{
    setNumOptions(5);
    setOption(0, "Integridad Referencial",
              [this]() { this->verificarIntegridadReferencial(); });
    setOption(1, "Crear Backup", [this]() { this->crearBackup(); });
    setOption(2, "Productos con stock crítico",
              [this]() { this->reporteStockCritico(); });
    setOption(3, "Historial de Cliente",
              [this]() { this->reporteHistorialCliente(); });
    setOption(4, "Resumen de Tienda",
              [this]() { this->mostrarResumenTienda(); });
    drawMenu();
}
