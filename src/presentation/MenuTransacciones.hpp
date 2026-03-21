#pragma once
#include "Menu.hpp"

class MenuTransacciones : public Menu {
  public:
    MenuTransacciones() : Menu() {
        this->setTitle("Gestión de Transacciones");
        this->setTexToExit("Salir");
        this->setNumOptions(5);
    };

    void registrarCompra();
    void registrarVenta();
    void buscarTransacciones();
    void listarTransacciones();
    void cancelarTransaccion();

    void showMenu() override;
    ~MenuTransacciones();
};
