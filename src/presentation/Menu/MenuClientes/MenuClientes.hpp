#pragma once
#include "presentation/CliUtils.hpp"
#include "presentation/Menu/Menu.hpp"

class MenuClientes : public Menu
{
   private:
    bool readValidText(const char* prompt, std::string& outValue);
    bool readValidEmail(const char* prompt, std::string& outValue);
    bool cedulaDuplicada(const std::string& cedula, int ignoredId = -1);

   public:
    CliUtils utils;
    explicit MenuClientes(AppRepositories& repositorios, CliUtils utils = CliUtils());

    void crearCliente();
    void buscarCliente();
    void actualizarCliente();
    void listarClientes();
    void eliminarCliente();

    void showMenu() override;
    ~MenuClientes() override = default;
};
