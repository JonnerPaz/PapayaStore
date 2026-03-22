#pragma once
#include <string>

#include "Menu.hpp"
#include "domain/repositories/AppRepositories.hpp"

class MenuReportes : public Menu
{
   public:
    MenuReportes(std::string title, std::string texToExit, int numOptions,
                 AppRepositories& repos);
    void verificarIntegridadReferencial();
    void crearBackup();
    void reporteStockCritico();
    void reporteHistorialCliente();
    void mostrarResumenTienda();

    void showMenu() override;
};
