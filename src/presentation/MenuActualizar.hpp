#pragma once
#include "presentation/Menu/Menu.hpp"

class MenuActualizar : public Menu
{
   public:
    void mostrarMenu();
    void drawMenu(const char* title, OpcionMenu options[], int numOptions,
                  const char* texToExit = "Volver al menú principal");
};
