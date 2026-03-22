#include "presentation/CLI.hpp"

#include <iostream>

#include "domain/constants.hpp"

namespace Colors = Constants::ASCII_CODES;

void CLI::showSearchCriteriaMenu() {
    std::cout << Colors::COLOR_CYAN << "Seleccione el criterio de busqueda: " << Colors::COLOR_RESET
              << std::endl;
    std::cout << Colors::COLOR_YELLOW << "1." << Colors::COLOR_RESET << " Id" << std::endl
              << Colors::COLOR_YELLOW << "2." << Colors::COLOR_RESET << " Nombre" << std::endl
              << Colors::COLOR_YELLOW << "3." << Colors::COLOR_RESET << " Listar todos" << std::endl
              << Colors::COLOR_RED << "0." << Colors::COLOR_RESET << " Cancelar" << std::endl;
    std::cout << "Seleccione una opción: ";
}
