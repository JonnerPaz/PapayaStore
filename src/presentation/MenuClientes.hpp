#pragma once
#include "Menu.hpp"

class MenuClientes : public Menu {
  public:
    MenuClientes() : Menu() {
        this->setTitle("Gestión de Clientes");
        this->setTexToExit("Salir");
        this->setNumOptions(5);
    }

    void crearCliente();
    void buscarCliente();
    void actualizarCliente();
    void listarClientes();
    void eliminarCliente();

    void showMenu() override;
    ~MenuClientes();
};
