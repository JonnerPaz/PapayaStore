#pragma once
#include "Menu.h"

class MenuTienda : Menu {
  public:
    void mostrarMenu() override;
    void drawMenu(const char* title, OpcionMenu options[], int numOptions,
                  const char* texToExit = "Volver al menú principal") override;
};
