#pragma once
#include "domain/constants.hpp"
#include <cstring>
#include <iostream>
#include <limits.h>
#include <string>

using namespace Constants::ASCII_CODES;
using std::string;

template <typename T>
// remove_reference_t remueve las referencias de T y devuelve T
// Ej. si T es int& -> devuelve int
concept AsignarNum = std::is_arithmetic_v<std::remove_reference_t<T>>;

class Utils {
  public:
    static void convertirAMinusculas(char* cadena) {
        if (cadena == nullptr)
            return;
        for (int i = 0; cadena[i] != '\0'; ++i) {
            if (cadena[i] >= 'A' && cadena[i] <= 'Z') {
                cadena[i] = cadena[i] + ('a' - 'A');
            }
        }
    }

    // lee un id y valida que sea un numero y mayor o igual a 0
    // si el usuario ingresa 'q', retorna -1
    int validarId(const char* msg) {
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

    static bool contieneSubstring(const char* cadena, const char* subcadena) {
        char* copyCadena = new char[strlen(cadena) + 1];
        strcpy(copyCadena, cadena);
        convertirAMinusculas(copyCadena);

        char* copySubcadena = new char[strlen(subcadena) + 1];
        strcpy(copySubcadena, subcadena);
        convertirAMinusculas(copySubcadena);

        bool result = (strstr(copyCadena, copySubcadena) != nullptr);

        delete[] copyCadena;
        delete[] copySubcadena;

        return result;
    }

    static void asignarPropiedadNum(const char* msg, AsignarNum auto& prop) {
        std::cout << COLOR_YELLOW << msg << COLOR_RESET;
        while (true) {
            std::cin >> prop;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << COLOR_RED
                          << "El valor debe ser numérico. Intente nuevamente: " << COLOR_RESET
                          << std::endl;
                continue;
            }
            // Clean buffer no matter if std::cin fail or not
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (prop >= 0) {
                break;
            }
            std::cout << CLEAR_SCREEN << COLOR_RED
                      << "El valor debe ser mayor o igual a 0. Intente nuevamente: " << COLOR_RESET
                      << std::endl;
        }
        std::cout << std::endl;
    }

    template <size_t N> static void asignarPropiedadString(const char* msg, char (&prop)[N]) {
        std::cout << COLOR_YELLOW << msg << COLOR_RESET;
        if (std::cin.peek() == '\n') {
            std::cin.ignore();
        }

        std::cin.getline(prop, N);

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cout << std::endl;
    }
};
