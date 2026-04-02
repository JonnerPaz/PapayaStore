#pragma once

#include "presentation/Menu/Menu.hpp"

class MenuTienda : public Menu
{
   public:
    explicit MenuTienda(AppRepositories& repos);

    void mostrarResumenTienda();
    void showMenu() override;
};
