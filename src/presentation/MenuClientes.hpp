#pragma once
#include "Menu.hpp"
#include <string>

using std::string;

class MenuClientes : public Menu {
  public:
    MenuClientes(string title, string texToExit, int numOptions);

    void crearCliente();
    void buscarCliente();
    void actualizarCliente();
    void listarClientes();
    void eliminarCliente();

    void showMenu() override;
    ~MenuClientes();
};
