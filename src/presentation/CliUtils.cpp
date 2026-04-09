#include "CliUtils.hpp"

#include <iostream>
#include <string>

#include "domain/constants.hpp"

using std::string;

std::string CliUtils::readLine(const char* prompt)
{
    std::cout << COLOR_YELLOW << prompt << COLOR_RESET;
    std::string value;
    std::getline(std::cin >> std::ws, value);
    return value;
}

bool CliUtils::readValidText(const char* prompt, std::string& outValue)
{
    outValue = CliUtils::readLine(prompt);
    if (outValue == "q" || outValue == "Q" || outValue.empty()) {
        return false;
    }

    return true;
}

void CliUtils::toLowerCase(char* cadena)
{
    if (cadena == nullptr) return;

    for (int i = 0; cadena[i] != '\0'; ++i) {
        if (cadena[i] >= 'A' && cadena[i] <= 'Z') {
            cadena[i] = cadena[i] + ('a' - 'A');
        }
    }
}

int CliUtils::readValidId(const char* msg)
{
    while (true) {
        std::cout << COLOR_YELLOW << msg << " (q para salir): " << COLOR_RESET;
        string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return -1;
        }

        try {
            int id = stoi(input);
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (id <= 0) {
                std::cout << CLEAR_SCREEN << COLOR_RED
                          << "El id debe ser numérico y mayor a 0. Intente "
                             "nuevamente."
                          << COLOR_RESET << std::endl;
                continue;
            }

            return id;
        } catch (const std::invalid_argument& e) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << CLEAR_SCREEN << COLOR_RED << "El id debe ser numérico." << COLOR_RESET
                      << std::endl;
            std::cout << COLOR_YELLOW << "¿Habrás querido salir? Presiona 'q' para salir."
                      << COLOR_RESET << std::endl;
            continue;
        }
    }
}
