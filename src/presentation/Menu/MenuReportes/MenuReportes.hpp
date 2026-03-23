#pragma once
#include "domain/repositories/AppRepositories.hpp"
#include "presentation/Menu/Menu.hpp"
#include <string>

class MenuReportes : public Menu {
  public:
    MenuReportes(std::string title, std::string texToExit, int numOptions, AppRepositories& repos);
    void verificarIntegridadReferencial();
    void crearBackup();
    void reporteStockCritico();
    void reporteHistorialCliente();
    void mostrarResumenTienda();

    void showMenu() override;
};
