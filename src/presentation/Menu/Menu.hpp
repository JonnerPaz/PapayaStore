#pragma once
#include "domain/constants.hpp"
#include "domain/entities/ArchivoStats.hpp"
#include "domain/entities/cliente/Cliente.entity.hpp"
#include "domain/entities/producto/producto.entity.hpp"
#include "domain/entities/proveedor/Proveedor.entity.hpp"
#include "domain/entities/transaccion/transaccion.entity.hpp"
#include "domain/repositories/AppRepositories.hpp"
#include "presentation/CliUtils.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>

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

    std::variant<ArchivoStats, std::string> leerHeader(const fs::path& path) const {
        try {
            ArchivoStats header = {};
            std::ifstream archivo(path, std::ios::binary | std::ios::in);
            if (!archivo.is_open()) {
                return "Error al abrir el archivo: " + path.string();
            }

            archivo.seekg(0);
            archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoStats));
            if (!archivo) {
                return "Error al leer el header del archivo: " + path.string();
            }

            return header;
        } catch (const std::exception& error) {
            return "Error al leer el archivo " + path.string() + ": " + error.what();
        }
    }

  protected:
    AppRepositories& repositories;
    explicit Menu(AppRepositories& repositories) : numOptions(0), repositories(repositories) {
    }

    virtual void showMenu() = 0;

    template <typename T> std::variant<ArchivoStats, std::string> obtenerEntidadHeader() const {
        if constexpr (std::is_same_v<T, Producto>) {
            return leerHeader(Constants::PATHS::PRODUCTOS_PATH);
        } else if constexpr (std::is_same_v<T, Proveedor>) {
            return leerHeader(Constants::PATHS::PROVEEDORES_PATH);
        } else if constexpr (std::is_same_v<T, Cliente>) {
            return leerHeader(Constants::PATHS::CLIENTES_PATH);
        } else if constexpr (std::is_same_v<T, Transaccion>) {
            return leerHeader(Constants::PATHS::TRANSACCIONES_PATH);
        }

        return "Entidad no soportada para obtener header.";
    }

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
    virtual ~Menu() = default;
};
