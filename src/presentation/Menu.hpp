#pragma once
#include "domain/constants.hpp"
#include <functional>
#include <iostream>
#include <string>

using namespace Constants::ASCII_CODES;

struct OpcionMenu {
    const char* descripcion;
    std::function<void()> accion;
};

class Menu {
  private:
    int numOptions;
    std::string title;
    std::string texToExit = "Volver al menú principal";
    OpcionMenu options[5];

  protected:
    virtual void showMenu() = 0;

    void drawMenu() {
        int option;
        do {
            std::cout << COLOR_CYAN << "\n=== " << title << " ===" << COLOR_RESET << std::endl;
            for (int i = 0; i < numOptions; ++i) {
                std::cout << COLOR_YELLOW << i + 1 << "." << COLOR_RESET << " "
                          << this->options[i].descripcion << std::endl;
            }
            std::cout << COLOR_RED << "0." << COLOR_RESET << " " << this->texToExit << std::endl;
            std::cout << "Seleccione una opción: ";

            if (!(std::cin >> option))
                break;

            if (option < 0 || option > numOptions) {
                std::cout << COLOR_RED << "Opción inválida" << COLOR_RESET << std::endl;
                continue;
            }

            int index = option - 1;
            if (index >= 0 && index < numOptions && options[index].accion != nullptr) {
                options[index].accion();
            }

            // Si el indice es -1 ((index = 0) -1) se sale del menu
            if (index == -1) {
                std::cout << COLOR_GREEN
                          << (tolower(texToExit[0]) == 's' ? "Saliendo..." : "Volviendo...")
                          << COLOR_RESET << std::endl;
                break;
            }
        } while (option != 0);
    }

    std::string getTitle() {
        return title;
    }

    std::string setTitle(std::string title) {
        this->title = title;
        return title;
    }

    std::string getTexToExit() {
        return texToExit;
    }

    void setTexToExit(std::string texToExit) {
        this->texToExit = texToExit;
    }

    int getNumOptions() {
        return numOptions;
    }

    void setNumOptions(int numOptions) {
        this->numOptions = numOptions;
    }

    void setOption(int index, const char* desc, std::function<void()> act) {
        if (index >= 0 && index < 5) {
            this->options[index].descripcion = desc;
            this->options[index].accion = act;
        }
    }

  public:
    virtual ~Menu();
};
