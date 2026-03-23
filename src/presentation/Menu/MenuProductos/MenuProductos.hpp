#pragma once
#include "domain/repositories/AppRepositories.hpp"
#include "presentation/CliUtils.hpp"
#include "presentation/Menu/Menu.hpp"

class MenuProductos : public Menu {
  public:
    CliUtils utils;
    explicit MenuProductos(AppRepositories& repository, CliUtils utils);

    void crearProducto();
    void buscarProducto();
    void actualizarProducto();
    void listarProductos();
    void eliminarProducto();

    void showMenu() override;
    ~MenuProductos() override = default;
};
