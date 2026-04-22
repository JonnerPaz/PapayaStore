#include "Menu.hpp"

#include <fstream>
#include <iostream>
#include <limits>

#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"

Menu::Menu(AppRepositories& repositories)
    : title("Menu Principal"),
      numOptions(0),
      texToExit("Volver al menú principal"),
      repositories(repositories)

{
}

bool Menu::confirmAction(const char* prompt)
{
    std::cout << COLOR_YELLOW << prompt << COLOR_RESET;
    char confirm = 'n';
    std::cin >> confirm;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return confirm == 's' || confirm == 'S';
}

std::string Menu::readLine(const char* prompt)
{
    std::cout << COLOR_YELLOW << prompt << COLOR_RESET;
    std::string value;
    std::getline(std::cin >> std::ws, value);
    return value;
}

void Menu::printSuccess(const std::string& message) const
{
    std::cout << COLOR_GREEN << message << COLOR_RESET << std::endl;
}

void Menu::printError(const std::string& error) const
{
    std::cout << COLOR_RED << error << COLOR_RESET << std::endl;
}

std::variant<HeaderFile, std::string> Menu::leerHeader(const fs::path& path) const
{
    try {
        HeaderFile header = {};
        std::ifstream archivo(path, std::ios::binary | std::ios::in);
        if (!archivo.is_open()) {
            return "Error al abrir el archivo: " + path.string();
        }

        archivo.seekg(0);
        archivo.read(reinterpret_cast<char*>(&header), sizeof(HeaderFile));
        if (!archivo) {
            return "Error al leer el header del archivo: " + path.string();
        }

        return header;
    } catch (const std::exception& error) {
        return "Error al leer el archivo " + path.string() + ": " + error.what();
    }
}

void Menu::drawMenu()
{
    int option = -1;
    do {
        std::cout << COLOR_CYAN << "\n=== " << this->title << " ===" << COLOR_RESET << std::endl;
        for (int i = 0; i < numOptions; ++i) {
            std::cout << COLOR_YELLOW << i + 1 << "." << COLOR_RESET << " "
                      << this->options[i].descripcion << std::endl;
        }
        std::cout << COLOR_RED << "0." << COLOR_RESET << " " << this->texToExit << std::endl;
        std::cout << COLOR_YELLOW << "Seleccione una opción: ";

        if (!(std::cin >> option) || std::cin.peek() != '\n') {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << COLOR_RED << "Opción inválida. Debe ser numérica." << COLOR_RESET
                      << std::endl;
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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

bool Menu::setTitle(std::string title)
{
    this->title = title;
    return true;
}

bool Menu::setTexToExit(std::string texToExit)
{
    this->texToExit = texToExit;
    return true;
}

bool Menu::setNumOptions(int numOptions)
{
    this->numOptions = numOptions;
    return true;
}

bool Menu::setOption(int index, const char* desc, std::function<void()> act)
{
    if (index >= 0 && index < Menu::MAX_OPTIONS) {
        this->options[index].descripcion = desc;
        this->options[index].accion = act;
        return true;
    }
    return false;
}
