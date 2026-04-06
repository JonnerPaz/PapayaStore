#pragma once
#include "domain/repositories/AppRepositories.hpp"
#include "presentation/CliUtils.hpp"
#include "presentation/Menu/Menu.hpp"

class MenuProductos : public Menu
{
   private:
    bool readValidText(const char* prompt, std::string& outValue);
    void readValidFloat(const char* prompt, float& outValue, const char* errorMsg,
                        bool zeroInclusive = true);
    void readValidInt(const char* prompt, int& outValue, const char* errorMsg,
                      bool zeroInclusive = true);

   public:
    CliUtils utils;
    explicit MenuProductos(AppRepositories& repository, CliUtils utils);

    void crearProducto();
    void buscarProducto();
    void actualizarProducto();
    void listarProductos();
    void eliminarProducto();

    void showMenu() override;
    ~MenuProductos() override = default;
};
