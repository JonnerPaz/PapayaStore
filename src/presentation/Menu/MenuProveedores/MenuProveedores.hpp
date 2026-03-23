#pragma once
#include "presentation/Menu/Menu.hpp"

class MenuProveedores : public Menu {
  public:
    explicit MenuProveedores(AppRepositories& repositories) : Menu(repositories) {
        this->setTitle("Gestión de Proveedores");
        this->setTexToExit("Salir");
        this->setNumOptions(5);
    }

    void crearProveedor();
    void buscarProveedor();
    void actualizarProveedor();
    void listarProveedores();
    void eliminarProveedor();

    void showMenu() override;
    ~MenuProveedores();
};
