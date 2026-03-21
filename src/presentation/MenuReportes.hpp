#pragma once
#include "Menu.hpp"

class MenuReportes : public Menu {
  public:
    MenuReportes() : Menu() {
        this->setTitle("Reportes");
        this->setTexToExit("Salir");
        this->setNumOptions(3);
    }

    void verificarIntegridadReferencial();
    void crearBackup();
    void reporteStockCritico();
    void reporteHistorialCliente();
    void mostrarResumenTienda();

    void showMenu() override;
};
