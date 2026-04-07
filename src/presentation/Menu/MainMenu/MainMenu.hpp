#pragma once

#include "presentation/CliUtils.hpp"
#include "presentation/Menu/Menu.hpp"
#include "presentation/Menu/MenuClientes/MenuClientes.hpp"
#include "presentation/Menu/MenuProductos/MenuProductos.hpp"
#include "presentation/Menu/MenuProveedores/MenuProveedores.hpp"
#include "presentation/Menu/MenuReportes/MenuReportes.hpp"
#include "presentation/Menu/MenuTienda/MenuTienda.hpp"
#include "presentation/Menu/MenuTransacciones/MenuTransacciones.hpp"

class MainMenu : public Menu
{
   private:
    CliUtils cliUtils;
    MenuProductos menuProductos;
    MenuProveedores menuProveedores;
    MenuClientes menuClientes;
    MenuTransacciones menuTransacciones;
    MenuReportes menuReportes;
    MenuTienda menuTienda;

   public:
    explicit MainMenu(AppRepositories& repositories);
    void showMenu() override;
};
