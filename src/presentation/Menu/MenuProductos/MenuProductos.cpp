#include "MenuProductos.hpp"
#include "domain/Utils.hpp"
#include "domain/constants.hpp"
#include "domain/repositories/AppRepositories.hpp"
#include <format>
#include <iostream>

MenuProductos::MenuProductos(AppRepositories& repository) : Menu(repository) {
    Menu::setTitle("Gestión de Productos");
    Menu::setTexToExit("Salir");
    Menu::setNumOptions(5);
}

bool MenuProductos::asignarOpcion(auto& opcion) {
    while (true) {
        CliUtils::asignarPropiedadString("Ingrese el nombre del producto (q para cancelar): ",
                                         nombre);
        if (opcion[0] == 'q' && opcion[1] == '\0')
            return;

        if (CliUtils::existeDuplicado<Producto>(opcion, &Producto::nombre)) {
            std::cout << COLOR_RED << "Error: Ya existe un producto con ese nombre" << COLOR_RESET
                      << std::endl;
            continue;
        }
        break;
    }
}

void MenuProductos::crearProducto() {
    auto productosStats = this->obtenerEntidadHeader<Producto>();
    auto proveedoresStats = this->obtenerEntidadHeader<Proveedor>();

    if (std::holds_alternative<std::string>(productosStats) ||
        std::holds_alternative<std::string>(proveedoresStats)) {
        std::cout << "Error: " << std::get<std::string>(productosStats) << "\n";
        return;
    }

    ArchivoStats proveedoresHeader = std::get<ArchivoStats>(proveedoresStats);
    ArchivoStats productosHeader = std::get<ArchivoStats>(productosStats);

    if (proveedoresHeader.registrosActivos == 0) {
        std::cout << std::format("{} {} Error: No hay proveedores registrados. Debe crear "
                                 "al menos un "
                                 "proveedor antes de registrar un producto. {}",
                                 CLEAR_SCREEN, COLOR_RED, COLOR_RESET)
                  << std::endl;
        return;
    }

    char confirmar, nombre[100], codigo[20], descripcion[200];
    int stock, idProveedor, stockMinimo;
    float precio;

    while (true) {
        CliUtils::asignarPropiedadString("Ingrese el nombre del producto (q para cancelar): ",
                                         nombre);
        if (nombre[0] == 'q' && nombre[1] == '\0')
            return;

        if (CliUtils::existeDuplicado<Producto>(Constants::PATHS::PRODUCTOS_PATH, nombre)) {
            std::cout << COLOR_RED << "Error: Ya existe un producto con ese nombre" << COLOR_RESET
                      << std::endl;
            continue;
        }
        break;
    }

    while (true) {
        CliUtils::asignarPropiedadString("Ingrese el código del producto (q para cancelar): ",
                                         codigo);
        if (codigo[0] == 'q' && codigo[1] == '\0')
            return;

        if (codigo[0] == '\0') {
            std::cout << COLOR_RED << "Error: El código no puede estar vacío." << COLOR_RESET
                      << std::endl;
            continue;
        }

        if (codigoDuplicado(codigo)) {
            std::cout << COLOR_RED << "Error: Ya existe un producto con el código " << codigo
                      << COLOR_RESET << std::endl;
            continue;
        }
        break;
    }

    CliUtils::asignarPropiedadString("Ingrese la descripcion del producto (q para cancelar): ",
                                     descripcion);
    if (descripcion[0] == 'q' && descripcion[1] == '\0')
        return;

    CliUtils::asignarPropiedadNum("Ingrese el precio del producto: ", precio);

    CliUtils::asignarPropiedadNum("Ingrese el stock del producto: ", stock);

    CliUtils::asignarPropiedadNum("Ingrese el stock mínimo ideal del producto: ", stockMinimo);

    mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH);

    while (true) {
        idProveedor = leerId("Ingresar el id del proveedor");
        if (idProveedor <= 0) {
            std::cout << CLEAR_SCREEN << COLOR_RED << "Creación de producto cancelada."
                      << COLOR_RESET << std::endl;
            return;
        }

        if (existeProveedor(idProveedor)) {
            break;
        }

        std::cout << CLEAR_SCREEN << COLOR_RED << "Error: El proveedor con ID " << idProveedor
                  << " no existe. Intente nuevamente." << COLOR_RESET << std::endl;

        mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH);
    }
}

void MenuProductos::listarProductos() {
    std::cout << "Listar Productos" << std::endl;
}

void MenuProductos::buscarProducto() {
    std::cout << "Buscar Producto" << std::endl;
}

void MenuProductos::actualizarProducto() {
    std::cout << "Actualizar Producto" << std::endl;
}

void MenuProductos::eliminarProducto() {
    std::cout << "Eliminar Producto" << std::endl;
}

void MenuProductos::showMenu() {
    this->setOption(0, "Crear Producto", [this]() { crearProducto(); });
    this->setOption(1, "Buscar Producto", [this]() { buscarProducto(); });
    this->setOption(2, "Actualizar Producto", [this]() { actualizarProducto(); });
    this->setOption(3, "Listar Productos", [this]() { listarProductos(); });
    this->setOption(4, "Crear Producto", [this]() { eliminarProducto(); });

    drawMenu();
}
