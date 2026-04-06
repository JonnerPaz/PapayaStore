#pragma once
#include "presentation/CliUtils.hpp"
#include "presentation/Menu/Menu.hpp"

class MenuProveedores : public Menu
{
   private:
    bool readValidText(const char* prompt, std::string& outValue);
    bool readValidEmail(const char* prompt, std::string& outValue);
    bool rifDuplicado(const std::string& rif, int ignoredId = -1);

   public:
    CliUtils utils;
    explicit MenuProveedores(AppRepositories& repositories, CliUtils utils = CliUtils{});

    void crearProveedor();
    void buscarProveedor();
    void actualizarProveedor();
    void listarProveedores();
    void eliminarProveedor();

    void showMenu() override;
    ~MenuProveedores() override = default;
};
