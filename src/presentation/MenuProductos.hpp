#pragma once
#include "Menu.hpp"
#include "domain/repositories/AppRepositories.hpp"

class MenuProductos : public Menu {
  public:
    explicit MenuProductos(AppRepositories& repository);

    void crearProducto();
    void buscarProducto();
    void actualizarProducto();
    void listarProductos();
    void eliminarProducto();

    void showMenu() override;
    ~MenuProductos();
};
