#pragma once
#include "Menu.hpp"

class MenuProductos : public Menu {
  public:
    MenuProductos() : Menu() {
        this->setTitle("Gestión de Productos");
        this->setTexToExit("Salir");
        this->setNumOptions(5);
    }

    void crearProducto();
    void buscarProducto();
    void actualizarProducto();
    void listarProductos();
    void eliminarProducto();

    void showMenu() override;
    ~MenuProductos();
};
