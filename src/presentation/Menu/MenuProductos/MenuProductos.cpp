#include "MenuProductos.hpp"
#include "domain/constants.hpp"
#include "domain/repositories/AppRepositories.hpp"
#include "presentation/CliUtils.hpp"
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
                      << std::std::endl;
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

        if (CliUtils::existeDuplicado<Producto>(Constants::PATHS::PRODUCTOS_PATH, nombre,
                                                PorAmbos)) {
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
                      << std::std::endl;
            continue;
        }

        if (codigoDuplicado(codigo)) {
            std::cout << COLOR_RED << "Error: Ya existe un producto con el código " << codigo
                      << COLOR_RESET << std::std::endl;
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

    CliUtils::mostrarListaEntidades<Proveedor>("Proveedores", Constants::PATHS::PROVEEDORES_PATH);

    while (true) {
        idProveedor = leerId("Ingresar el id del proveedor");
        if (idProveedor <= 0) {
            std::cout << CLEAR_SCREEN << COLOR_RED << "Creación de producto cancelada."
                      << COLOR_RESET << std::std::endl;
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
    char opcion;
    do {
        std::cout << COLOR_CYAN << "Seleccione el criterio de busqueda: " << COLOR_RESET
                  << std::std::endl;
        std::cout << COLOR_YELLOW << "1." << COLOR_RESET << " Id" << std::std::endl
                  << COLOR_YELLOW << "2." << COLOR_RESET << " Nombre" << std::std::endl
                  << COLOR_YELLOW << "3." << COLOR_RESET << " Listar todos" << std::std::endl
                  << COLOR_RED << "0." << COLOR_RESET << " Cancelar" << std::std::endl;
        std::cout << "Seleccione una opción: ";

        std::cin >> opcion;
        switch (opcion) {
        case BusquedaId: {
            mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PorId);
            int id = leerId("Ingrese el id del producto");
            if (id <= 0) {
                std::cout << CLEAR_SCREEN << COLOR_RED << "Búsqueda cancelada." << COLOR_RESET
                          << std::endl;
                break;
            }

            int index = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, id);
            if (index == -1) {
                std::cout << "Producto no encontrado." << std::endl;
                break;
            }

            Producto producto;
            ifstream archivo(PRODUCTOS_PATH, ios::binary);
            archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Producto), ios::beg);
            archivo.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

            std::cout << "Producto encontrado:" << std::endl;
            std::cout << COLOR_YELLOW << "Id: " << COLOR_RESET << producto.id << std::endl;
            std::cout << COLOR_YELLOW << "Nombre: " << COLOR_RESET << producto.nombre << std::endl;
            std::cout << COLOR_YELLOW << "Codigo: " << COLOR_RESET << producto.codigo << std::endl;
            std::cout << COLOR_YELLOW << "Descripcion: " << COLOR_RESET << producto.descripcion
                      << std::endl;
            std::cout << COLOR_YELLOW << "Precio: " << COLOR_RESET << producto.precio << std::endl;
            std::cout << COLOR_YELLOW << "Stock: " << COLOR_RESET << producto.stock << std::endl
                      << std::endl;
            break;
        }
        case BusquedaNombre: {
            mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PorNombre);
            std::cout << "Ingrese el nombre del producto: ";
            char nombre[100];
            if (std::cin.peek() == '\n')
                std::cin.ignore();
            std::cin.getline(nombre, 100);

            // array de indices del producto. Asumimos que index[0] es la
            // cantidad de encontrados y a partir de index[1] estan los
            // indices reales en el array de la tienda
            int* index = buscarProductosPorNombre(nombre);
            if (index == nullptr || index[0] == 0) {
                std::cout << "Producto no encontrado." << std::endl;
                if (index != nullptr)
                    delete[] index;
                break;
            }

            std::cout << "Productos encontrados:" << std::endl;
            ifstream archivo(PRODUCTOS_PATH, ios::binary);
            for (int i = 1; i <= index[0]; i++) {
                Producto producto;
                archivo.seekg(sizeof(ArchivoHeader) + index[i] * sizeof(Producto), ios::beg);
                archivo.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

                std::cout << COLOR_YELLOW << "Id: " << COLOR_RESET << producto.id << std::endl;
                std::cout << COLOR_YELLOW << "Nombre: " << COLOR_RESET << producto.nombre
                          << std::endl;
                std::cout << COLOR_YELLOW << "Codigo: " << COLOR_RESET << producto.codigo
                          << std::endl;
                std::cout << COLOR_YELLOW << "Descripcion: " << COLOR_RESET << producto.descripcion
                          << std::endl;
                std::cout << COLOR_YELLOW << "Precio: " << COLOR_RESET << producto.precio
                          << std::endl;
                std::cout << COLOR_YELLOW << "Stock: " << COLOR_RESET << producto.stock << std::endl
                          << std::endl;
            }
            delete[] index;
            break;
        }

        case BusquedaMostrar: {
            listarProductos();
            break;
        }
        case BusquedaCancelada: {
            std::cout << COLOR_GREEN << "Cancelada la búsqueda" << COLOR_RESET << std::endl;
            return;
        }
        default: {
            std::cout << CLEAR_SCREEN << COLOR_RED << "Opcion no valida" << COLOR_RESET
                      << std::endl;
            break;
        }
        }
    } while (opcion != BusquedaCancelada);
}

void MenuProductos::actualizarProducto() {
    std::cout << "Actualizar Producto" << std::std::endl;
}

void MenuProductos::eliminarProducto() {
    std::cout << "Eliminar Producto" << std::std::endl;
}

void MenuProductos::showMenu() {
    this->setOption(0, "Crear Producto", [this]() { crearProducto(); });
    this->setOption(1, "Buscar Producto", [this]() { buscarProducto(); });
    this->setOption(2, "Actualizar Producto", [this]() { actualizarProducto(); });
    this->setOption(3, "Listar Productos", [this]() { listarProductos(); });
    this->setOption(4, "Crear Producto", [this]() { eliminarProducto(); });

    drawMenu();
}
