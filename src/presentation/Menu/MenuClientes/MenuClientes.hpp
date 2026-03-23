#pragma once
#include "domain/repositories/AppRepositories.hpp"
#include "presentation/Menu/Menu.hpp"
#include <string>

using std::string;

class MenuClientes : public Menu {
  public:
    explicit MenuClientes(string title, string texToExit, int numOptions,
                          AppRepositories& repositorios);

    void crearCliente();
    void buscarCliente();
    void actualizarCliente();
    void listarClientes();
    void eliminarCliente();

    void showMenu() override;
    ~MenuClientes();
};
