#pragma once
#include "presentation/Menu/Menu.hpp"

class MenuClientes : public Menu {
  public:
    explicit MenuClientes(AppRepositories& repositorios);

    void crearCliente();
    void buscarCliente();
    void actualizarCliente();
    void listarClientes();
    void eliminarCliente();

    void showMenu() override;
    ~MenuClientes() override = default;
};
