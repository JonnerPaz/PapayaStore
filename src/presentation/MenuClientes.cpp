#include "presentation/MenuClientes.hpp"
#include "presentation/Menu.hpp"

MenuClientes::MenuClientes(string title, string texToExit, int numOptions) : Menu() {
    this->setTitle(title);
    this->setTexToExit(texToExit);
    this->setNumOptions(numOptions);
}

void MenuClientes::showMenu() {
    OpcionMenu options[] = {{"Crear Cliente", crearCliente},
                            {"Buscar Cliente", buscarCliente},
                            {"Actualizar Cliente", actualizarCliente},
                            {"Listar Clientes", listarClientes},
                            {"Eliminar Cliente", eliminarCliente}};
    Menu::drawMenu();
}

MenuClientes::~MenuClientes() {
}
