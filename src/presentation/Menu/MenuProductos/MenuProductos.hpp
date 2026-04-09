#pragma once
#include "domain/repositories/AppRepositories.hpp"
#include "presentation/CliUtils.hpp"
#include "presentation/Menu/Menu.hpp"

class MenuProductos : public Menu
{
   private:
    bool nombreDuplicado(const std::string& nombre, int ignoredId = -1);
    bool codigoDuplicado(const std::string& codigo, int ignoredId = -1);
    void readValidFloat(const char* prompt, float& outValue, const char* errorMsg,
                        bool zeroInclusive = true);
    void readValidInt(const char* prompt, int& outValue, const char* errorMsg,
                      bool zeroInclusive = true);

   public:
    explicit MenuProductos(AppRepositories& repository, CliUtils utils);

    void crearProducto();
    void buscarProducto();
    void actualizarProducto();
    void listarProductos();
    void eliminarProducto();

    void showMenu() override;
    ~MenuProductos() override = default;
};
