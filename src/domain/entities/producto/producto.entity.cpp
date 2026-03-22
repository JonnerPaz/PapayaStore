#include "domain/entities/producto/producto.entity.hpp"
#include <cstring>
#include <iostream>

#include "domain/constants.hpp"

using namespace std;
using namespace Constants::ASCII_CODES;
using namespace Constants::PATHS;

Producto::Producto(int id, const char* nombre, const std::string& codigo, const char* descripcion) {
    this->setId(id);
    this->setNombre(nombre);
    strncpy(this->codigo, codigo.c_str(), 20);
    strncpy(this->descripcion, descripcion, 200);
    this->fechaRegistro[0] = '\0';
    this->precio = 0;
    this->stock = 0;
    this->idProveedor = 0;
    this->stockMinimo = 0;
    this->totalVendido = 0;
    this->setEliminado(false);
}

Producto Producto::crearProducto() {
}

enum Busqueda {
    BusquedaId = '1',
    BusquedaNombre = '2',
    BusquedaMostrar = '3',
    BusquedaCancelada = '0'
};

Producto Producto::buscarProducto(int id) {
    char opcion;
    do {
        cout << COLOR_CYAN << "Seleccione el criterio de busqueda: " << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "1." << COLOR_RESET << " Id" << endl
             << COLOR_YELLOW << "2." << COLOR_RESET << " Nombre" << endl
             << COLOR_YELLOW << "3." << COLOR_RESET << " Listar todos" << endl
             << COLOR_RED << "0." << COLOR_RESET << " Cancelar" << endl;
        cout << "Seleccione una opción: ";

        cin >> opcion;
        switch (opcion) {
        case BusquedaId: {
            mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PorId);
            int id = leerId("Ingrese el id del producto");
            if (id <= 0) {
                cout << CLEAR_SCREEN << COLOR_RED << "Búsqueda cancelada." << COLOR_RESET << endl;
                break;
            }

            int index = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, id);
            if (index == -1) {
                cout << "Producto no encontrado." << endl;
                break;
            }

            Producto producto;
            ifstream archivo(PRODUCTOS_PATH, ios::binary);
            archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Producto), ios::beg);
            archivo.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

            cout << "Producto encontrado:" << endl;
            cout << COLOR_YELLOW << "Id: " << COLOR_RESET << producto.id << endl;
            cout << COLOR_YELLOW << "Nombre: " << COLOR_RESET << producto.nombre << endl;
            cout << COLOR_YELLOW << "Codigo: " << COLOR_RESET << producto.codigo << endl;
            cout << COLOR_YELLOW << "Descripcion: " << COLOR_RESET << producto.descripcion << endl;
            cout << COLOR_YELLOW << "Precio: " << COLOR_RESET << producto.precio << endl;
            cout << COLOR_YELLOW << "Stock: " << COLOR_RESET << producto.stock << endl << endl;
            break;
        }
        case BusquedaNombre: {
            mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PorNombre);
            cout << "Ingrese el nombre del producto: ";
            char nombre[100];
            if (cin.peek() == '\n')
                cin.ignore();
            cin.getline(nombre, 100);

            // array de indices del producto. Asumimos que index[0] es la
            // cantidad de encontrados y a partir de index[1] estan los
            // indices reales en el array de la tienda
            int* index = buscarProductosPorNombre(nombre);
            if (index == nullptr || index[0] == 0) {
                cout << "Producto no encontrado." << endl;
                if (index != nullptr)
                    delete[] index;
                break;
            }

            cout << "Productos encontrados:" << endl;
            ifstream archivo(PRODUCTOS_PATH, ios::binary);
            for (int i = 1; i <= index[0]; i++) {
                Producto producto;
                archivo.seekg(sizeof(ArchivoHeader) + index[i] * sizeof(Producto), ios::beg);
                archivo.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

                cout << COLOR_YELLOW << "Id: " << COLOR_RESET << producto.id << endl;
                cout << COLOR_YELLOW << "Nombre: " << COLOR_RESET << producto.nombre << endl;
                cout << COLOR_YELLOW << "Codigo: " << COLOR_RESET << producto.codigo << endl;
                cout << COLOR_YELLOW << "Descripcion: " << COLOR_RESET << producto.descripcion
                     << endl;
                cout << COLOR_YELLOW << "Precio: " << COLOR_RESET << producto.precio << endl;
                cout << COLOR_YELLOW << "Stock: " << COLOR_RESET << producto.stock << endl << endl;
            }
            delete[] index;
            break;
        }

        case BusquedaMostrar: {
            listarProductos();
            break;
        }
        case BusquedaCancelada: {
            cout << COLOR_GREEN << "Cancelada la búsqueda" << COLOR_RESET << endl;
            return;
        }
        default: {
            cout << CLEAR_SCREEN << COLOR_RED << "Opcion no valida" << COLOR_RESET << endl;
            break;
        }
        }
    } while (opcion != BusquedaCancelada);
}
