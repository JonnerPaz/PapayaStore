#pragma once

#include "presentation/cli/CliUtils.hpp"
#include "presentation/cli/Menu/Menu.hpp"
#include "presentation/cli/Menu/MenuClientes/MenuClientes.hpp"
#include "presentation/cli/Menu/MenuProductos/MenuProductos.hpp"
#include "presentation/cli/Menu/MenuProveedores/MenuProveedores.hpp"
#include "presentation/cli/Menu/MenuReportes/MenuReportes.hpp"
#include "presentation/cli/Menu/MenuTransacciones/MenuTransacciones.hpp"

class MainMenu : public Menu
{
   private:
    CliUtils cliUtils;
    MenuProductos menuProductos;
    MenuProveedores menuProveedores;
    MenuClientes menuClientes;
    MenuTransacciones menuTransacciones;
    MenuReportes menuReportes;

   public:
    explicit MainMenu(AppRepositories& repositories);
    void showMenu() override;
};
