#pragma once
#include "presentation/cli/CliUtils.hpp"
#include "presentation/cli/Menu/Menu.hpp"

class MenuProveedores : public Menu
{
   private:
    bool readValidEmail(const char* prompt, std::string& outValue);
    bool readValidPhone(const char* prompt, std::string& outValue);
    bool nombreDuplicado(const std::string& nombre, int ignoredId = -1);
    bool rifDuplicado(const std::string& rif, int ignoredId = -1);

   public:
    explicit MenuProveedores(AppRepositories& repositories, CliUtils utils = CliUtils{});

    void crearProveedor();
    void buscarProveedor();
    void actualizarProveedor();
    void listarProveedores();
    void eliminarProveedor();

    void showMenu() override;
    ~MenuProveedores() override = default;
};
