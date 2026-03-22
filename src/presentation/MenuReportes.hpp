#pragma once
#include "Menu.hpp"
#include "domain/datasource/DB.datasource.hpp"
#include <string>

class MenuReportes : public Menu {
  private:
    DB& backupService;

  public:
    MenuReportes(std::string title, std::string texToExit, int numOptions, DB& dbService);
    void verificarIntegridadReferencial();
    void crearBackup();
    void reporteStockCritico();
    void reporteHistorialCliente();
    void mostrarResumenTienda();

    void showMenu() override;
};
