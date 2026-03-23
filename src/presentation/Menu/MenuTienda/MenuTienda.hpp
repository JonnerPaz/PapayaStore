#pragma once
#include "presentation/Menu/Menu.hpp"

class MenuTienda : Menu {
  public:
    void showMenu() override;
    void drawMenu();
    void mostrarMenuTienda();

    void initLoop();
};
