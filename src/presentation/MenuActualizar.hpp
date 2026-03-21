#pragma once
#include "Menu.hpp"

class MenuActualizar : Menu
{
   public:
    void mostrarMenu() override;
    void drawMenu(const char* title, OpcionMenu options[], int numOptions,
                  const char* texToExit = "Volver al menú principal") override;
};
