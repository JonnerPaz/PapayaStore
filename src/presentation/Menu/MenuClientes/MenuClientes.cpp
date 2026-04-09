#include "MenuClientes.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <variant>

#include "domain/constants.hpp"
#include "presentation/CliUtils.hpp"

MenuClientes::MenuClientes(AppRepositories& repositorios, CliUtils utils) : Menu(repositorios)
{
    setTitle("Gestion de Clientes");
    setTexToExit("Salir");
    setNumOptions(5);
}

bool MenuClientes::readValidEmail(const char* prompt, std::string& outValue)
{
    while (true) {
        outValue = Menu::readLine(prompt);
        if (outValue == "q" || outValue == "Q" || outValue.empty()) {
            Menu::printError("Creacion cancelada.");
            return false;
        }

        Cliente cliente;
        if (cliente.setEmail(outValue.c_str())) {
            return true;
        }

        Menu::printError("Email invalido. Debe tener un formato valido.");
    }
}

bool MenuClientes::readValidPhone(const char* prompt, std::string& outValue)
{
    while (true) {
        outValue = Menu::readLine(prompt);
        if (outValue == "q" || outValue == "Q" || outValue.empty()) {
            return false;
        }

        Cliente cliente;
        if (cliente.setTelefono(outValue.c_str())) {
            outValue = cliente.getTelefono();
            return true;
        }

        Menu::printError(
            "Telefono invalido. Use entre 7 y 15 digitos; puede incluir + al inicio,"
            " espacios, guiones o parentesis.");
    }
}

bool MenuClientes::nombreDuplicado(const std::string& nombre, int ignoredId)
{
    auto result = repositories.clientes.leerPorNombre(nombre);
    if (std::holds_alternative<std::string>(result)) {
        return false;
    }

    const Cliente& cliente = std::get<Cliente>(result);
    return cliente.getId() != ignoredId;
}

bool MenuClientes::cedulaDuplicada(const std::string& cedula, int ignoredId)
{
    const auto clientesHeader = repositories.clientes.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(clientesHeader)) {
        return false;
    }

    const HeaderFile stats = std::get<HeaderFile>(clientesHeader);
    for (int id = 1; id < stats.proximoID; ++id) {
        auto clienteResult = repositories.clientes.leerPorId(id);
        if (!std::holds_alternative<Cliente>(clienteResult)) {
            continue;
        }

        Cliente cliente = std::get<Cliente>(clienteResult);
        if (cliente.getId() == ignoredId) {
            continue;
        }

        if (cedula == cliente.getCedula()) {
            return true;
        }
    }

    return false;
}

void MenuClientes::crearCliente()
{
    auto clientesHeader = repositories.clientes.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(clientesHeader)) {
        Menu::printError("Error: " + std::get<std::string>(clientesHeader));
        return;
    }

    std::string nombre;
    const char* genericErrMsg = "Creacion cancelada.";
    while (true) {
        if (!CliUtils::readValidText("Ingrese el nombre del cliente (q para cancelar): ", nombre)) {
            Menu::printError(genericErrMsg);
            return;
        }

        if (nombreDuplicado(nombre)) {
            Menu::printError("Ya existe un cliente con el nombre ingresado.");
            continue;
        }

        break;
    }

    std::string cedula;
    while (true) {
        if (!CliUtils::readValidText("Ingrese la cédula del cliente (q para cancelar): ", cedula)) {
            Menu::printError(genericErrMsg);
            return;
        }

        if (cedulaDuplicada(cedula)) {
            Menu::printError("Ya existe un cliente con la cedula ingresada.");
            continue;
        }

        break;
    }

    std::string telefono;
    if (!readValidPhone("Ingrese el telefono del cliente (q para cancelar): ", telefono)) {
        Menu::printError("Creacion cancelada.");
        return;
    }

    std::string email;
    if (!readValidEmail("Ingrese el email del cliente (q para cancelar): ", email)) {
        return;
    }

    std::string direccion;
    if (!CliUtils::readValidText("Ingrese la direccion del cliente (q para cancelar): ",
                                 direccion)) {
        return;
    }

    const HeaderFile clienteStats = std::get<HeaderFile>(clientesHeader);
    const int nuevoId = clienteStats.proximoID;

    Cliente cliente{};
    if (!cliente.setId(nuevoId) || !cliente.setNombre(nombre.c_str()) ||
        !cliente.setCedula(cedula.c_str()) || !cliente.setTelefono(telefono.c_str()) ||
        !cliente.setEmail(email.c_str()) || !cliente.setDireccion(direccion.c_str())) {
        Menu::printError("Error al crear cliente. Datos invalidos.");
        return;
    }

    std::cout << COLOR_YELLOW
              << std::format("Se creara el cliente: {}{} {}({}{}{})", COLOR_GREEN,
                             cliente.getNombre(), COLOR_YELLOW, COLOR_GREEN, cliente.getCedula(),
                             COLOR_YELLOW)
              << COLOR_RESET << std::endl;
    if (!Menu::confirmAction("¿Estás seguro? (s/n): ")) {
        Menu::printError("Creacion cancelada.");
        return;
    }

    auto saveResult = repositories.clientes.guardar(cliente);
    if (std::holds_alternative<std::string>(saveResult)) {
        Menu::printError("Error al guardar: " + std::get<std::string>(saveResult));
        return;
    }

    try {
        repositories.admin.sincronizarContadoresTienda();
    } catch (const std::exception& e) {
        Menu::printError("Advertencia: cliente creado, pero no se pudo sincronizar tienda: " +
                         std::string(e.what()));
    }

    Menu::printSuccess("Cliente creado con exito.");
}

void MenuClientes::buscarCliente()
{
    const int id = CliUtils::readValidId("Ingrese el id del cliente a buscar");
    if (id <= 0) {
        Menu::printError("El Id ingresado no existe. Busqueda cancelada.");
        return;
    }

    auto result = repositories.clientes.leerPorId(id);
    if (std::holds_alternative<std::string>(result)) {
        Menu::printError("Error: " + std::get<std::string>(result));
        return;
    }

    Cliente cliente = std::get<Cliente>(result);
    Menu::printSuccess("Cliente encontrado:");
    std::cout << std::format("{}ID: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getId()) << std::endl;
    std::cout << std::format("{}Nombre: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getNombre())
              << std::endl;
    std::cout << std::format("{}Cedula: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getCedula())
              << std::endl;
    std::cout << std::format("{}Telefono: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getTelefono())
              << std::endl;
    std::cout << std::format("{}Email: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getEmail())
              << std::endl;
    std::cout << std::format("{}Direccion: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getDireccion())
              << std::endl;
}

void MenuClientes::actualizarCliente()
{
    const int id = CliUtils::readValidId("Ingrese el id del cliente a actualizar");
    if (id <= 0) {
        Menu::printError("El Id ingresado no existe. Actualizacion cancelada.");
        return;
    }

    auto result = repositories.clientes.leerPorId(id);
    if (std::holds_alternative<std::string>(result)) {
        Menu::printError("Error: " + std::get<std::string>(result));
        return;
    }

    Cliente cliente = std::get<Cliente>(result);
    while (true) {
        std::cout << COLOR_CYAN << "\n--- Cliente actual ---" << COLOR_RESET << std::endl;
        std::cout << std::format("{}ID: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getId())
                  << std::endl;
        std::cout << std::format("{}Nombre: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getNombre())
                  << std::endl;
        std::cout << std::format("{}Cedula: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getCedula())
                  << std::endl;
        std::cout << std::format("{}Telefono: {}{}", COLOR_YELLOW, COLOR_GREEN,
                                 cliente.getTelefono())
                  << std::endl;
        std::cout << std::format("{}Email: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getEmail())
                  << std::endl;
        std::cout << std::format("{}Direccion: {}{}", COLOR_YELLOW, COLOR_GREEN,
                                 cliente.getDireccion())
                  << std::endl;

        std::cout << COLOR_CYAN << "\nSeleccione una propiedad a actualizar:" << COLOR_RESET
                  << std::endl;
        std::cout << COLOR_YELLOW << "1." << COLOR_RESET << " Nombre" << std::endl;
        std::cout << COLOR_YELLOW << "2." << COLOR_RESET << " Cedula" << std::endl;
        std::cout << COLOR_YELLOW << "3." << COLOR_RESET << " Telefono" << std::endl;
        std::cout << COLOR_YELLOW << "4." << COLOR_RESET << " Email" << std::endl;
        std::cout << COLOR_YELLOW << "5." << COLOR_RESET << " Direccion" << std::endl;
        std::cout << COLOR_RED << "0." << COLOR_RESET << " Salir" << std::endl;

        const std::string opcionText = readLine("Opcion: ");
        int opcion = -1;
        if (!CliUtils::parsePositiveNumber(opcionText, opcion, true) || opcion > 5) {
            Menu::printError("Opcion invalida.");
            continue;
        }

        if (opcion == 0) {
            Menu::printSuccess("Actualizacion finalizada.");
            break;
        }

        switch (opcion) {
            case 1: {
                const std::string nuevoNombre =
                    readLine("Nuevo nombre (q o enter para cancelar): ");
                if (nuevoNombre == "q" || nuevoNombre == "Q" || nuevoNombre.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                if (nombreDuplicado(nuevoNombre, id)) {
                    Menu::printError("Ya existe un cliente con el nombre ingresado.");
                    break;
                }

                std::cout << std::format("Nombre actual: {} | Nuevo nombre: {}",
                                         cliente.getNombre(), nuevoNombre)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de nombre? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Cliente clienteActualizado = cliente;
                if (!clienteActualizado.setNombre(nuevoNombre.c_str())) {
                    Menu::printError("Nombre invalido.");
                    break;
                }

                clienteActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.clientes.actualizar(id, clienteActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar nombre: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                cliente = clienteActualizado;
                Menu::printSuccess("Nombre actualizado con exito.");
                break;
            }
            case 2: {
                const std::string nuevaCedula =
                    readLine("Nueva cedula (q o enter para cancelar): ");
                if (nuevaCedula == "q" || nuevaCedula == "Q" || nuevaCedula.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                if (cedulaDuplicada(nuevaCedula, id)) {
                    Menu::printError("Ya existe un cliente con la cedula ingresada.");
                    break;
                }

                std::cout << std::format("Cedula actual: {} | Nueva cedula: {}",
                                         cliente.getCedula(), nuevaCedula)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de cedula? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Cliente clienteActualizado = cliente;
                if (!clienteActualizado.setCedula(nuevaCedula.c_str())) {
                    Menu::printError("Cedula invalida.");
                    break;
                }

                clienteActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.clientes.actualizar(id, clienteActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar cedula: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                cliente = clienteActualizado;
                Menu::printSuccess("Cedula actualizada con exito.");
                break;
            }
            case 3: {
                std::string nuevoTelefono;
                if (!readValidPhone("Nuevo telefono (q o enter para cancelar): ", nuevoTelefono)) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                std::cout << std::format("Telefono actual: {} | Nuevo telefono: {}",
                                         cliente.getTelefono(), nuevoTelefono)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de telefono? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Cliente clienteActualizado = cliente;
                if (!clienteActualizado.setTelefono(nuevoTelefono.c_str())) {
                    Menu::printError("Telefono invalido.");
                    break;
                }

                clienteActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.clientes.actualizar(id, clienteActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar telefono: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                cliente = clienteActualizado;
                Menu::printSuccess("Telefono actualizado con exito.");
                break;
            }
            case 4: {
                const std::string nuevoEmail = readLine("Nuevo email (q o enter para cancelar): ");
                if (nuevoEmail == "q" || nuevoEmail == "Q" || nuevoEmail.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Cliente validadorEmail;
                if (!validadorEmail.setEmail(nuevoEmail.c_str())) {
                    Menu::printError("Email invalido.");
                    break;
                }

                std::cout << std::format("Email actual: {} | Nuevo email: {}", cliente.getEmail(),
                                         nuevoEmail)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de email? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Cliente clienteActualizado = cliente;
                if (!clienteActualizado.setEmail(nuevoEmail.c_str())) {
                    Menu::printError("Email invalido.");
                    break;
                }

                clienteActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.clientes.actualizar(id, clienteActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar email: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                cliente = clienteActualizado;
                Menu::printSuccess("Email actualizado con exito.");
                break;
            }
            case 5: {
                const std::string nuevaDireccion =
                    readLine("Nueva direccion (q o enter para cancelar): ");
                if (nuevaDireccion == "q" || nuevaDireccion == "Q" || nuevaDireccion.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                std::cout << std::format("Direccion actual: {} | Nueva direccion: {}",
                                         cliente.getDireccion(), nuevaDireccion)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de direccion? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Cliente clienteActualizado = cliente;
                if (!clienteActualizado.setDireccion(nuevaDireccion.c_str())) {
                    Menu::printError("Direccion invalida.");
                    break;
                }

                clienteActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.clientes.actualizar(id, clienteActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar direccion: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                cliente = clienteActualizado;
                Menu::printSuccess("Direccion actualizada con exito.");
                break;
            }
            default:
                Menu::printError("Opcion inválida.");
                break;
        }
    }
}

void MenuClientes::listarClientes()
{
    auto clientesHeader = repositories.clientes.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(clientesHeader)) {
        Menu::printError("Error: " + std::get<std::string>(clientesHeader));
        return;
    }

    HeaderFile stats = std::get<HeaderFile>(clientesHeader);
    if (stats.registrosActivos == 0) {
        Menu::printError("No hay clientes registrados.");
        return;
    }

    std::cout << std::format("{}--- Lista de Clientes ({}{}{}) ---", COLOR_CYAN, COLOR_GREEN,
                             COLOR_YELLOW, stats.registrosActivos)
              << std::endl;
    std::cout << std::format("{}{:<5} | {:<20} | {:<15} | {:<15} | {:<25} | {:<20}", COLOR_YELLOW,
                             "ID", "Nombre", "Cedula", "Telefono", "Email", "Direccion")
              << std::endl;
    std::cout << "---------------------------------------------------------------------------------"
                 "----------------------------"
              << std::endl;

    for (int id = 1; id < stats.proximoID; ++id) {
        auto result = repositories.clientes.leerPorId(id);
        if (!std::holds_alternative<Cliente>(result)) {
            continue;
        }

        Cliente cliente = std::get<Cliente>(result);
        std::cout << std::format("{}{:<5} | {:<20} | {:<15} | {:<15} | {:<25} | {:<20}",
                                 COLOR_GREEN, cliente.getId(), cliente.getNombre(),
                                 cliente.getCedula(), cliente.getTelefono(), cliente.getEmail(),
                                 cliente.getDireccion())
                  << std::endl;
    }
}

void MenuClientes::eliminarCliente()
{
    const int id = CliUtils::readValidId("Ingrese el id del cliente a eliminar");
    if (id <= 0) {
        Menu::printError("Eliminacion cancelada.");
        return;
    }

    auto clienteResult = repositories.clientes.leerPorId(id);
    if (std::holds_alternative<std::string>(clienteResult)) {
        Menu::printError("Error: " + std::get<std::string>(clienteResult));
        return;
    }

    auto transaccionesHeader = repositories.transacciones.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(transaccionesHeader)) {
        Menu::printError("Error: " + std::get<std::string>(transaccionesHeader));
        return;
    }

    bool tieneTransaccionesActivas = false;
    HeaderFile transStats = std::get<HeaderFile>(transaccionesHeader);
    for (int transId = 1; transId < transStats.proximoID; ++transId) {
        auto transResult = repositories.transacciones.leerPorId(transId);
        if (!std::holds_alternative<Transaccion>(transResult)) {
            continue;
        }

        const Transaccion& transaccion = std::get<Transaccion>(transResult);
        if (transaccion.getTipoTransaccion() == VENTA && transaccion.getIdRelacionado() == id) {
            tieneTransaccionesActivas = true;
            break;
        }
    }

    if (tieneTransaccionesActivas) {
        Menu::printError("No se puede eliminar el cliente porque tiene transacciones activas.");
        return;
    }

    Cliente cliente = std::get<Cliente>(clienteResult);
    std::cout << std::format("Cliente a eliminar: {} ({})", cliente.getNombre(),
                             cliente.getCedula())
              << std::endl;

    if (!confirmAction("Confirma eliminacion? (s/n): ")) {
        Menu::printError("Eliminacion cancelada.");
        return;
    }

    auto deleteResult = repositories.clientes.eliminarLogicamente(id);
    if (std::holds_alternative<std::string>(deleteResult)) {
        Menu::printError("Error al eliminar: " + std::get<std::string>(deleteResult));
        return;
    }

    try {
        repositories.admin.sincronizarContadoresTienda();
    } catch (const std::exception& e) {
        Menu::printError("Advertencia: cliente eliminado, pero no se pudo sincronizar tienda: " +
                         std::string(e.what()));
    }

    Menu::printSuccess("Cliente eliminado con exito.");
}

void MenuClientes::showMenu()
{
    setOption(0, "Crear Cliente", [this]() { crearCliente(); });
    setOption(1, "Buscar Cliente", [this]() { buscarCliente(); });
    setOption(2, "Actualizar Cliente", [this]() { actualizarCliente(); });
    setOption(3, "Listar Clientes", [this]() { listarClientes(); });
    setOption(4, "Eliminar Cliente", [this]() { eliminarCliente(); });

    drawMenu();
}
