#include "presentation/MenuClientes.hpp"

// MenuClientes::MenuClientes() : Menu() {
// }

void MenuClientes::showMenu() {
    OpcionMenu options[] = {{"Crear Cliente", crearCliente},
                            {"Buscar Cliente", buscarCliente},
                            {"Actualizar Cliente", actualizarCliente},
                            {"Listar Clientes", listarClientes},
                            {"Eliminar Cliente", eliminarCliente}};
    Menu::drawMenu("Gestión de Clientes", options, 5);
}

MenuClientes::~MenuClientes() {
}
