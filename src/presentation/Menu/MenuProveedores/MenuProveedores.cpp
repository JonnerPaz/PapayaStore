#include "MenuProveedores.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <variant>

#include "domain/constants.hpp"

MenuProveedores::MenuProveedores(AppRepositories& repositories, CliUtils utils)
    : Menu(repositories), utils(utils)
{
    this->setTitle("Gestion de Proveedores");
    this->setTexToExit("Salir");
    this->setNumOptions(5);
}

bool MenuProveedores::readValidText(const char* prompt, std::string& outValue)
{
    outValue = Menu::readLine(prompt);
    if (outValue == "q" || outValue == "Q" || outValue.empty()) {
        Menu::printError("Creacion cancelada.");
        return false;
    }

    return true;
}

bool MenuProveedores::readValidEmail(const char* prompt, std::string& outValue)
{
    while (true) {
        outValue = Menu::readLine(prompt);
        if (outValue == "q" || outValue == "Q" || outValue.empty()) {
            Menu::printError("Creacion cancelada.");
            return false;
        }

        Proveedor proveedor;
        if (proveedor.setEmail(outValue.c_str())) {
            return true;
        }

        Menu::printError("Email invalido. Debe tener un formato valido.");
    }
}

bool MenuProveedores::rifDuplicado(const std::string& rif, int ignoredId)
{
    const auto proveedoresHeader = repositories.proveedores.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(proveedoresHeader)) {
        return false;
    }

    const HeaderFile stats = std::get<HeaderFile>(proveedoresHeader);
    for (int id = 1; id < stats.proximoID; id++) {
        auto proveedorResult = repositories.proveedores.leerPorId(id);
        if (!std::holds_alternative<Proveedor>(proveedorResult)) {
            continue;
        }

        Proveedor proveedor = std::get<Proveedor>(proveedorResult);
        if (proveedor.getId() == ignoredId) {
            continue;
        }

        if (rif == proveedor.getRif()) {
            return true;
        }
    }

    return false;
}

void MenuProveedores::crearProveedor()
{
    auto proveedoresHeader = repositories.proveedores.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(proveedoresHeader)) {
        Menu::printError("Error: " + std::get<std::string>(proveedoresHeader));
        return;
    }

    std::string nombre;
    if (!readValidText("Ingrese el nombre del proveedor (q para cancelar): ", nombre)) {
        return;
    }

    std::string rif;
    while (true) {
        rif = readLine("Ingrese el RIF del proveedor (q para cancelar): ");
        if (rif == "q" || rif == "Q" || rif.empty()) {
            Menu::printError("Creacion cancelada.");
            return;
        }

        if (rifDuplicado(rif)) {
            Menu::printError("Ya existe un proveedor con el RIF ingresado.");
            continue;
        }

        break;
    }

    std::string telefono;
    if (!readValidText("Ingrese el telefono del proveedor (q para cancelar): ", telefono)) {
        return;
    }

    std::string email;
    if (!readValidEmail("Ingrese el email del proveedor (q para cancelar): ", email)) {
        return;
    }

    std::string direccion;
    if (!readValidText("Ingrese la direccion del proveedor (q para cancelar): ", direccion)) {
        return;
    }

    const HeaderFile proveedorStats = std::get<HeaderFile>(proveedoresHeader);
    const int nuevoId = proveedorStats.proximoID;

    Proveedor proveedor{};
    if (!proveedor.setId(nuevoId) || !proveedor.setNombre(nombre.c_str()) ||
        !proveedor.setRif(rif.c_str()) || !proveedor.setTelefono(telefono.c_str()) ||
        !proveedor.setEmail(email.c_str()) || !proveedor.setDireccion(direccion.c_str())) {
        Menu::printError("Error al crear proveedor. Datos invalidos.");
        return;
    }

    std::cout << COLOR_YELLOW
              << std::format("Se creara el proveedor: {}{} {}({}{}{})", COLOR_GREEN,
                             proveedor.getNombre(), COLOR_YELLOW, COLOR_GREEN, proveedor.getRif(),
                             COLOR_YELLOW)
              << COLOR_RESET << std::endl;
    if (!Menu::confirmAction("¿Estás seguro? (s/n): ")) {
        Menu::printError("Creacion cancelada.");
        return;
    }

    auto saveResult = repositories.proveedores.guardar(proveedor);
    if (std::holds_alternative<std::string>(saveResult)) {
        Menu::printError("Error al guardar: " + std::get<std::string>(saveResult));
        return;
    }

    Menu::printSuccess("Proveedor creado con exito.");
}

void MenuProveedores::buscarProveedor()
{
    const int id = utils.validarId("Ingrese el id del proveedor a buscar");
    if (id <= 0) {
        Menu::printError("El Id ingresado no existe. Busqueda cancelada.");
        return;
    }

    auto result = repositories.proveedores.leerPorId(id);
    if (std::holds_alternative<std::string>(result)) {
        Menu::printError("Error: " + std::get<std::string>(result));
        return;
    }

    Proveedor proveedor = std::get<Proveedor>(result);
    Menu::printSuccess("Proveedor encontrado:");
    std::cout << std::format("{}ID: {}{}", COLOR_YELLOW, COLOR_GREEN, proveedor.getId())
              << std::endl;
    std::cout << std::format("{}Nombre: {}{}", COLOR_YELLOW, COLOR_GREEN, proveedor.getNombre())
              << std::endl;
    std::cout << std::format("{}RIF: {}{}", COLOR_YELLOW, COLOR_GREEN, proveedor.getRif())
              << std::endl;
    std::cout << std::format("{}Telefono: {}{}", COLOR_YELLOW, COLOR_GREEN, proveedor.getTelefono())
              << std::endl;
    std::cout << std::format("{}Email: {}{}", COLOR_YELLOW, COLOR_GREEN, proveedor.getEmail())
              << std::endl;
    std::cout << std::format("{}Direccion: {}{}", COLOR_YELLOW, COLOR_GREEN,
                             proveedor.getDireccion())
              << std::endl;
}

void MenuProveedores::actualizarProveedor()
{
    const int id = utils.validarId("Ingrese el id del proveedor a actualizar");
    if (id <= 0) {
        Menu::printError("El Id ingresado no existe. Actualizacion cancelada.");
        return;
    }

    auto result = repositories.proveedores.leerPorId(id);
    if (std::holds_alternative<std::string>(result)) {
        Menu::printError("Error: " + std::get<std::string>(result));
        return;
    }

    Proveedor proveedor = std::get<Proveedor>(result);
    while (true) {
        std::cout << COLOR_CYAN << "\n--- Proveedor actual ---" << COLOR_RESET << std::endl;
        std::cout << std::format("{}ID: {}{}", COLOR_YELLOW, COLOR_GREEN, proveedor.getId())
                  << std::endl;
        std::cout << std::format("{}Nombre: {}{}", COLOR_YELLOW, COLOR_GREEN, proveedor.getNombre())
                  << std::endl;
        std::cout << std::format("{}RIF: {}{}", COLOR_YELLOW, COLOR_GREEN, proveedor.getRif())
                  << std::endl;
        std::cout << std::format("{}Telefono: {}{}", COLOR_YELLOW, COLOR_GREEN,
                                 proveedor.getTelefono())
                  << std::endl;
        std::cout << std::format("{}Email: {}{}", COLOR_YELLOW, COLOR_GREEN, proveedor.getEmail())
                  << std::endl;
        std::cout << std::format("{}Direccion: {}{}", COLOR_YELLOW, COLOR_GREEN,
                                 proveedor.getDireccion())
                  << std::endl;

        std::cout << COLOR_CYAN << "\nSeleccione una propiedad a actualizar:" << COLOR_RESET
                  << std::endl;
        std::cout << COLOR_YELLOW << "1." << COLOR_RESET << " Nombre" << std::endl;
        std::cout << COLOR_YELLOW << "2." << COLOR_RESET << " RIF" << std::endl;
        std::cout << COLOR_YELLOW << "3." << COLOR_RESET << " Telefono" << std::endl;
        std::cout << COLOR_YELLOW << "4." << COLOR_RESET << " Email" << std::endl;
        std::cout << COLOR_YELLOW << "5." << COLOR_RESET << " Direccion" << std::endl;
        std::cout << COLOR_RED << "0." << COLOR_RESET << " Salir" << std::endl;

        const std::string opcionText = readLine("Opcion: ");
        int opcion = -1;
        if (!utils.parsePositiveInt(opcionText, opcion, true) || opcion > 5) {
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

                std::cout << std::format("Nombre actual: {} | Nuevo nombre: {}",
                                         proveedor.getNombre(), nuevoNombre)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de nombre? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Proveedor proveedorActualizado = proveedor;
                if (!proveedorActualizado.setNombre(nuevoNombre.c_str())) {
                    Menu::printError("Nombre invalido.");
                    break;
                }

                proveedorActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.proveedores.actualizar(id, proveedorActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar nombre: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                proveedor = proveedorActualizado;
                Menu::printSuccess("Nombre actualizado con exito.");
                break;
            }
            case 2: {
                const std::string nuevoRif = readLine("Nuevo RIF (q o enter para cancelar): ");
                if (nuevoRif == "q" || nuevoRif == "Q" || nuevoRif.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                if (rifDuplicado(nuevoRif, id)) {
                    Menu::printError("Ya existe un proveedor con el RIF ingresado.");
                    break;
                }

                std::cout << std::format("RIF actual: {} | Nuevo RIF: {}", proveedor.getRif(),
                                         nuevoRif)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de RIF? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Proveedor proveedorActualizado = proveedor;
                if (!proveedorActualizado.setRif(nuevoRif.c_str())) {
                    Menu::printError("RIF invalido.");
                    break;
                }

                proveedorActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.proveedores.actualizar(id, proveedorActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar RIF: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                proveedor = proveedorActualizado;
                Menu::printSuccess("RIF actualizado con exito.");
                break;
            }
            case 3: {
                const std::string nuevoTelefono =
                    readLine("Nuevo telefono (q o enter para cancelar): ");
                if (nuevoTelefono == "q" || nuevoTelefono == "Q" || nuevoTelefono.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                std::cout << std::format("Telefono actual: {} | Nuevo telefono: {}",
                                         proveedor.getTelefono(), nuevoTelefono)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de telefono? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Proveedor proveedorActualizado = proveedor;
                if (!proveedorActualizado.setTelefono(nuevoTelefono.c_str())) {
                    Menu::printError("Telefono invalido.");
                    break;
                }

                proveedorActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.proveedores.actualizar(id, proveedorActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar telefono: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                proveedor = proveedorActualizado;
                Menu::printSuccess("Telefono actualizado con exito.");
                break;
            }
            case 4: {
                const std::string nuevoEmail = readLine("Nuevo email (q o enter para cancelar): ");
                if (nuevoEmail == "q" || nuevoEmail == "Q" || nuevoEmail.empty()) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Proveedor validadorEmail;
                if (!validadorEmail.setEmail(nuevoEmail.c_str())) {
                    Menu::printError("Email invalido.");
                    break;
                }

                std::cout << std::format("Email actual: {} | Nuevo email: {}", proveedor.getEmail(),
                                         nuevoEmail)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de email? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Proveedor proveedorActualizado = proveedor;
                if (!proveedorActualizado.setEmail(nuevoEmail.c_str())) {
                    Menu::printError("Email invalido.");
                    break;
                }

                proveedorActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.proveedores.actualizar(id, proveedorActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar email: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                proveedor = proveedorActualizado;
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
                                         proveedor.getDireccion(), nuevaDireccion)
                          << std::endl;
                if (!confirmAction("Confirma actualizacion de direccion? (s/n): ")) {
                    Menu::printError("Actualizacion cancelada.");
                    break;
                }

                Proveedor proveedorActualizado = proveedor;
                if (!proveedorActualizado.setDireccion(nuevaDireccion.c_str())) {
                    Menu::printError("Direccion invalida.");
                    break;
                }

                proveedorActualizado.setFechaUltimaModificacion(std::chrono::system_clock::now());
                auto updateResult = repositories.proveedores.actualizar(id, proveedorActualizado);
                if (std::holds_alternative<std::string>(updateResult)) {
                    Menu::printError("Error al actualizar direccion: " +
                                     std::get<std::string>(updateResult));
                    break;
                }

                proveedor = proveedorActualizado;
                Menu::printSuccess("Direccion actualizada con exito.");
                break;
            }
            default:
                Menu::printError("Opcion invalida.");
                break;
        }
    }
}

void MenuProveedores::listarProveedores()
{
    auto proveedoresHeader = repositories.proveedores.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(proveedoresHeader)) {
        Menu::printError("Error: " + std::get<std::string>(proveedoresHeader));
        return;
    }

    HeaderFile stats = std::get<HeaderFile>(proveedoresHeader);
    if (stats.registrosActivos == 0) {
        Menu::printError("No hay proveedores registrados.");
        return;
    }

    std::cout << std::format("{}--- Lista de Proveedores ({}{}{}) ---", COLOR_CYAN, COLOR_GREEN,
                             COLOR_YELLOW, stats.registrosActivos)
              << std::endl;
    std::cout << std::format("{}{:<5} | {:<20} | {:<15} | {:<15} | {:<25} | {:<20}", COLOR_YELLOW,
                             "ID", "Nombre", "RIF", "Telefono", "Email", "Direccion")
              << std::endl;
    std::cout << "---------------------------------------------------------------------------------"
                 "----------------------------"
              << std::endl;

    for (int id = 1; id < stats.proximoID; ++id) {
        auto result = repositories.proveedores.leerPorId(id);
        if (!std::holds_alternative<Proveedor>(result)) {
            continue;
        }

        Proveedor proveedor = std::get<Proveedor>(result);
        std::cout << std::format("{}{:<5} | {:<20} | {:<15} | {:<15} | {:<25} | {:<20}",
                                 COLOR_GREEN, proveedor.getId(), proveedor.getNombre(),
                                 proveedor.getRif(), proveedor.getTelefono(), proveedor.getEmail(),
                                 proveedor.getDireccion())
                  << std::endl;
    }
}

void MenuProveedores::eliminarProveedor()
{
    const int id = utils.validarId("Ingrese el id del proveedor a eliminar");
    if (id <= 0) {
        Menu::printError("Eliminacion cancelada.");
        return;
    }

    auto proveedorResult = repositories.proveedores.leerPorId(id);
    if (std::holds_alternative<std::string>(proveedorResult)) {
        Menu::printError("Error: " + std::get<std::string>(proveedorResult));
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
        if (transaccion.getTipoTransaccion() == COMPRA && transaccion.getIdRelacionado() == id) {
            tieneTransaccionesActivas = true;
            break;
        }
    }

    if (tieneTransaccionesActivas) {
        Menu::printError("No se puede eliminar el proveedor porque tiene transacciones activas.");
        return;
    }

    Proveedor proveedor = std::get<Proveedor>(proveedorResult);
    std::cout << std::format("Proveedor a eliminar: {} ({})", proveedor.getNombre(),
                             proveedor.getRif())
              << std::endl;

    if (!confirmAction("Confirma eliminacion? (s/n): ")) {
        Menu::printError("Eliminacion cancelada.");
        return;
    }

    auto deleteResult = repositories.proveedores.eliminarLogicamente(id);
    if (std::holds_alternative<std::string>(deleteResult)) {
        Menu::printError("Error al eliminar: " + std::get<std::string>(deleteResult));
        return;
    }

    Menu::printSuccess("Proveedor eliminado con exito.");
}

void MenuProveedores::showMenu()
{
    setOption(0, "Crear Proveedor", [this]() { crearProveedor(); });
    setOption(1, "Buscar Proveedor", [this]() { buscarProveedor(); });
    setOption(2, "Actualizar Proveedor", [this]() { actualizarProveedor(); });
    setOption(3, "Listar Proveedores", [this]() { listarProveedores(); });
    setOption(4, "Eliminar Proveedor", [this]() { eliminarProveedor(); });
    drawMenu();
}
