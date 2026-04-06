#pragma once
#include <functional>
#include <string>
#include <type_traits>
#include <variant>

#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"
#include "domain/entities/cliente/Cliente.entity.hpp"
#include "domain/entities/producto/producto.entity.hpp"
#include "domain/entities/proveedor/Proveedor.entity.hpp"
#include "domain/entities/transaccion/transaccion.entity.hpp"
#include "domain/repositories/AppRepositories.hpp"

using namespace Constants::ASCII_CODES;

/**
 * @brief Clase base para los menus de la aplicacion
 * @param descripcion - Descripcion de la opción
 * @param accion - Accion a realizar (function)
 */
struct OpcionMenu {
    const char* descripcion;
    std::function<void()> accion;
};

class Menu
{
   private:
    std::string title{};
    int numOptions{};
    std::string texToExit{};
    OpcionMenu options[5];

    std::variant<HeaderFile, std::string> leerHeader(const fs::path& path) const;

   protected:
    AppRepositories& repositories;

    explicit Menu(AppRepositories& repositories);

    virtual void showMenu() = 0;
    void drawMenu();

    bool confirmAction(const char* prompt);
    std::string readLine(const char* prompt);
    void printSuccess(const std::string& message) const;
    void printError(const std::string& error) const;

    template <typename T>
    std::variant<HeaderFile, std::string> obtenerEntidadHeader() const
    {
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

    std::string getTitle() { return title; }

    bool setTitle(std::string title);

    std::string getTexToExit() { return texToExit; }

    bool setTexToExit(std::string texToExit);

    int getNumOptions() { return numOptions; }

    bool setNumOptions(int numOptions);

    bool setOption(int index, const char* desc, std::function<void()> act);

   public:
    virtual ~Menu() = default;
};
