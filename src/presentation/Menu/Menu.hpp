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

    std::string getTitle() { return title; }

    std::string setTitle(std::string title)
    {
        this->title = title;
        return title;
    }

    std::string getTexToExit() { return texToExit; }

    void setTexToExit(std::string texToExit) { this->texToExit = texToExit; }

    int getNumOptions() { return numOptions; }

    void setNumOptions(int numOptions) { this->numOptions = numOptions; }

    void setOption(int index, const char* desc, std::function<void()> act);

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

   public:
    virtual ~Menu() = default;
};
