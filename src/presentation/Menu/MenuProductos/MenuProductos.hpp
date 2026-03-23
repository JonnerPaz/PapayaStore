#pragma once
#include "domain/repositories/AppRepositories.hpp"
#include "presentation/Menu/Menu.hpp"

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
