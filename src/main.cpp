#include <chrono>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

using namespace std;

// ANSI Escape Codes
const char* CLEAR_SCREEN = "\033[2J\033[H";
const char* COLOR_RESET = "\033[0m";
const char* COLOR_CYAN = "\033[36m";
const char* COLOR_GREEN = "\033[32m";
const char* COLOR_YELLOW = "\033[33m";
const char* COLOR_RED = "\033[31m";
const char* COLOR_MAGENTA = "\033[35m";

struct Tienda;
template <typename T> int buscarEntidadPorId(T* array, int count, int id);
int leerId(const char* msg);
void obtenerFechaActual(char* fecha);
// retorna array de indices del producto
int* buscarProductosPorNombre(Tienda* tienda, const char* nombre);
void listarProductos(Tienda* tienda);
void convertirAMinusculas(char* cadena);
bool contieneSubstring(const char* cadena, const char* subcadena);
template <typename T, size_t N>
bool existeStringDuplicado(T* array, int count, const char* valorBusqueda, char (T::*miembro)[N]);
bool existeProveedor(Tienda* tienda, int id);
bool existeCliente(Tienda* tienda, int id);

enum TipoDeTransaccion { COMPRA, VENTA };
enum Busqueda {
    BusquedaId = '1',
    BusquedaNombre = '2',
    BusquedaMostrar = '3',
    BusquedaCancelada = '0'
};
enum Actualizar {
    ActualizarNombre = '1',
    ActualizarCodigo = '2',
    ActualizarDescripcion = '3',
    ActualizarPrecio = '4',
    ActualizarStock = '5',
    ActualizarProveedor = '6',
    ActualizarCancelada = '0'
};

struct Producto {
    int id;                 // Identificador único (autoincremental)
    char nombre[100];       // Nombre del producto
    char codigo[20];        // Código del producto (ej: "PROD-001")
    char descripcion[200];  // Descripción del producto
    char fechaRegistro[11]; // Formato: YYYY-MM-DD
    int idProveedor;        // ID del proveedor asociado
    float precio;           // Precio unitario
    int stock;              // Cantidad en inventario
};

struct Proveedor {
    int id;                 // Identificador único (autoincremental)
    char nombre[100];       // Nombre del proveedor
    char rif[20];           // RIF o identificación fiscal
    char telefono[20];      // Teléfono de contacto
    char email[100];        // Correo electrónico
    char direccion[200];    // Dirección física
    char fechaRegistro[11]; // Formato: YYYY-MM-DD
};

struct Cliente {
    int id;                 // Identificador único (autoincremental)
    char nombre[100];       // Nombre completo del cliente
    char cedula[20];        // Cédula o RIF
    char telefono[20];      // Teléfono de contacto
    char email[100];        // Correo electrónico
    char direccion[200];    // Dirección física
    char fechaRegistro[11]; // Formato: YYYY-MM-DD
};

struct Transaccion {
    int id;                 // Identificador único (autoincremental)
    TipoDeTransaccion tipo; // COMPRA o VENTA
    int idProducto;         // ID del producto involucrado
    int idRelacionado;      // ID del proveedor (compra) o cliente (venta)
    int cantidad;           // Cantidad de unidades
    float precioUnitario;   // Precio por unidad en esta transacción
    float total;            // cantidad * precioUnitario
    char fecha[11];         // Formato: YYYY-MM-DD
    char descripcion[200];  // Notas adicionales (opcional)
};

struct Tienda {
    char nombre[100]; // Nombre de la tienda
    char rif[20];     // RIF de la tienda

    // Arrays dinámicos de entidades
    Producto* productos;
    Proveedor* proveedores;
    Cliente* clientes;
    Transaccion* transacciones;

    // Control de Memoria
    int numProductos, capacidadProductos;
    int numClientes, capacidadClientes;
    int numTransacciones, capacidadTransacciones;
    int numProveedores, capacidadProveedores;

    // Contadores para IDs autoincrementales
    int siguienteIdProducto;
    int siguienteIdProveedor;
    int siguienteIdCliente;
    int siguienteIdTransaccion;
};

enum TipoPropiedadLista { PropiedadId, PropiedadNombre, PropiedadAmbos };

template <typename T>
void mostrarListaEntidades(const char* titulo, T* array, int count,
                           TipoPropiedadLista tipoProp = PropiedadAmbos);

// templates utilizados en el programa
template <typename T>
// remove_reference_t remueve las referencias de T y devuelve T
// Ej. si T es int& -> devuelve int
concept AsignarNum = std::is_arithmetic_v<std::remove_reference_t<T>>;
void asignarPropiedadNum(const char* msg, AsignarNum auto& prop) {
    cout << msg;
    while (true) {
        cin >> prop;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << CLEAR_SCREEN << COLOR_RED
                 << "El valor debe ser numérico. Intente nuevamente: " << COLOR_RESET << endl;
            continue;
        }
        // Clean buffer no matter if cin fail or not
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (prop >= 0) {
            break;
        }
        cout << CLEAR_SCREEN << COLOR_RED
             << "El valor debe ser mayor o igual a 0. Intente nuevamente: " << COLOR_RESET << endl;
    }
    cout << endl;
}

template <size_t N> void asignarPropiedadString(const char* msg, char (&prop)[N]) {
    cout << msg;
    if (cin.peek() == '\n') {
        cin.ignore();
    }

    cin.getline(prop, N);

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << endl;
}

// Helper template para copiar strings de forma segura
// Se pasa el arreglo de char como referencia para evitar el decaimiento (decay)
// y no copiar el array original.
template <size_t N> void copiarString(char (&destino)[N], const char* origen) {
    strncpy(destino, origen, N - 1);
    destino[N - 1] = '\0';
}

// Helper template para desplazar elementos a la izquierda al eliminar
// Actualiza el count a la cantidad de elementos restantes.
// CUIDADO DE PASAR LA CUENTA DE UN ARRAY DIFERENTE AL ARRAY
template <typename T> void eliminarElementoDeArray(T* array, int index, int& count) {
    for (int i = index; i < count - 1; ++i) {
        array[i] = array[i + 1];
    }
    count--;
}

// Helper template para redimensionar arrays dinámicos
template <typename T> void redimensionarEntidad(T*& array, int& capacidad, int numElementos) {
    int nuevaCapacidad = capacidad * 2;
    T* nuevoArray = new T[nuevaCapacidad];
    for (int i = 0; i < numElementos; ++i) {
        nuevoArray[i] = array[i];
    }
    delete[] array;
    array = nuevoArray;
    capacidad = nuevaCapacidad;
}

///// FUNCIONES CRUD DEL PROGRAMA
void inicializarTienda(Tienda* tienda, const char* nombre, const char* rif) {
    const int CAPACIDAD_INICIAL = 5;

    strncpy(tienda->nombre, nombre, 100);
    strncpy(tienda->rif, rif, 20);

    tienda->capacidadClientes = CAPACIDAD_INICIAL;
    tienda->capacidadProductos = CAPACIDAD_INICIAL;
    tienda->capacidadProveedores = CAPACIDAD_INICIAL;
    tienda->capacidadTransacciones = CAPACIDAD_INICIAL;

    // Inicializar los arrays dinámicos
    tienda->productos = new Producto[CAPACIDAD_INICIAL];
    tienda->proveedores = new Proveedor[CAPACIDAD_INICIAL];
    tienda->clientes = new Cliente[CAPACIDAD_INICIAL];
    tienda->transacciones = new Transaccion[CAPACIDAD_INICIAL];

    tienda->numTransacciones = 0;
    tienda->numProductos = 0;
    tienda->numProveedores = 0;
    tienda->numClientes = 0;

    // empiezan en 1 porque el id debe ser mayor a 0
    tienda->siguienteIdProducto = 1;
    tienda->siguienteIdProveedor = 1;
    tienda->siguienteIdCliente = 1;
    tienda->siguienteIdTransaccion = 1;
}

void liberarTienda(Tienda* tienda) {
    delete[] tienda->productos;
    delete[] tienda->proveedores;
    delete[] tienda->clientes;
    delete[] tienda->transacciones;

    tienda->clientes = nullptr;
    tienda->productos = nullptr;
    tienda->proveedores = nullptr;
    tienda->transacciones = nullptr;

    tienda->numTransacciones = 0;
    tienda->numClientes = 0;
    tienda->numProductos = 0;
    tienda->numProveedores = 0;

    tienda->siguienteIdProducto = 1;
    tienda->siguienteIdProveedor = 1;
    tienda->siguienteIdCliente = 1;
    tienda->siguienteIdTransaccion = 1;
}

// Forward declarations para validaciones
bool codigoDuplicado(Tienda* tienda, const char* codigo);
bool rifDuplicado(Tienda* tienda, const char* rif);
bool validarEmail(const char* email);

void crearProducto(Tienda* tienda) {
    if (tienda == nullptr) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: La tienda no ha sido creada." << COLOR_RESET
             << endl;
        return;
    }

    if (tienda->numProveedores == 0) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: No hay proveedores registrados. Debe crear al menos un proveedor antes de "
                "registrar un producto."
             << COLOR_RESET << endl;
        return;
    }

    int& totalDeProductos = tienda->numProductos;
    int& capacidadProductos = tienda->capacidadProductos;
    Producto*& productos = tienda->productos;

    if (totalDeProductos >= capacidadProductos) {
        redimensionarEntidad(productos, capacidadProductos, totalDeProductos);
    }

    int index = tienda->numProductos;
    char confirmar;

    char nombre[100];
    asignarPropiedadString("Ingrese el nombre del producto (q para cancelar): ", nombre);
    if (nombre[0] == 'q' && nombre[1] == '\0')
        return;

    char codigo[20];
    while (true) {
        asignarPropiedadString("Ingrese el código del producto (q para cancelar): ", codigo);
        if (codigo[0] == 'q' && codigo[1] == '\0')
            return;
        if (codigo[0] == '\0') {
            cout << COLOR_RED << "Error: El código no puede estar vacío." << COLOR_RESET << endl;
            continue;
        }
        if (codigoDuplicado(tienda, codigo)) {
            cout << COLOR_RED << "Error: Ya existe un producto con el código " << codigo
                 << COLOR_RESET << endl;
        } else {
            break;
        }
    }

    char descripcion[200];
    asignarPropiedadString("Ingrese la descripcion del producto (q para cancelar): ", descripcion);
    if (descripcion[0] == 'q' && descripcion[1] == '\0')
        return;

    float precio;
    asignarPropiedadNum("Ingrese el precio del producto: ", precio);

    int stock;
    asignarPropiedadNum("Ingrese el stock del producto: ", stock);

    // muestra los proveedores que hay
    // para que el usuario escoja el proveedor del producto más abajo
    mostrarListaEntidades("Proveedores", tienda->proveedores, tienda->numProveedores);

    int idProveedor;
    while (true) {
        idProveedor = leerId("Ingresar el id del proveedor");
        if (idProveedor <= 0) {
            cout << CLEAR_SCREEN << COLOR_RED << "Creación de producto cancelada." << COLOR_RESET
                 << endl;
            return;
        }

        if (existeProveedor(tienda, idProveedor)) {
            break;
        }

        cout << CLEAR_SCREEN << COLOR_RED << "Error: El proveedor con ID " << idProveedor
             << " no existe. Intente nuevamente." << COLOR_RESET << endl;

        mostrarListaEntidades("Proveedores", tienda->proveedores, tienda->numProveedores);
    }

    cout << "Los datos del producto son: " << endl;
    cout << "Nombre: " << nombre << endl;
    cout << "Codigo: " << codigo << endl;
    cout << "Descripcion: " << descripcion << endl;
    cout << "Precio: " << precio << endl;
    cout << "Stock: " << stock << endl;
    cout << "Proveedor: " << idProveedor << endl;

    cout << COLOR_YELLOW << "Está seguro de crear el producto? (s/n): " << COLOR_RESET;
    cin >> confirmar;

    if (confirmar == 's' || confirmar == 'S') {
        Producto& producto = tienda->productos[index];
        producto.id = tienda->siguienteIdProducto;
        producto.idProveedor = idProveedor;
        copiarString(producto.nombre, nombre);
        copiarString(producto.codigo, codigo);
        copiarString(producto.descripcion, descripcion);
        producto.precio = precio;
        producto.stock = stock;
        obtenerFechaActual(producto.fechaRegistro);

        tienda->siguienteIdProducto++;
        tienda->numProductos++;
        cout << "Producto creado con exito." << endl;
    } else {
        cout << "Producto no creado." << endl;
    }
}

void buscarProducto(Tienda* tienda) {
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
            mostrarListaEntidades("Productos", tienda->productos, tienda->numProductos,
                                  PropiedadId);
            int id = leerId("Ingrese el id del producto");
            if (id <= 0) {
                cout << CLEAR_SCREEN << COLOR_RED << "Búsqueda cancelada." << COLOR_RESET << endl;
                break;
            }

            int index = buscarEntidadPorId(tienda->productos, tienda->numProductos, id);
            if (index == -1) {
                cout << "Producto no encontrado." << endl;
                break;
            }

            Producto& producto = tienda->productos[index];
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
            mostrarListaEntidades("Productos", tienda->productos, tienda->numProductos,
                                  PropiedadNombre);
            cout << "Ingrese el nombre del producto: ";
            char nombre[100];
            if (cin.peek() == '\n')
                cin.ignore();
            cin.getline(nombre, 100);

            // array de indices del producto. Asumimos que index[0] es la cantidad de encontrados
            // y a partir de index[1] estan los indices reales en el array de la tienda
            int* index = buscarProductosPorNombre(tienda, nombre);
            if (index == nullptr || index[0] == 0) {
                cout << "Producto no encontrado." << endl;
                if (index != nullptr)
                    delete[] index;
                break;
            }

            cout << "Productos encontrados:" << endl;
            for (int i = 1; i <= index[0]; i++) {
                Producto& producto = tienda->productos[index[i]];

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
            listarProductos(tienda);
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

template <typename T> void manejarPropiedad(const string& nombrePropiedad, T& propiedad) {
    char confirmar;

    //  Descripcion, Nombre, Codigo
    if constexpr (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>) {
        char tempProp[200];
        cout << format("Ingrese el nuevo {} (q para cancelar): ", nombrePropiedad);
        if (cin.peek() == '\n')
            cin.ignore();
        cin.getline(tempProp, sizeof(T));

        if (tempProp[0] == 'q' && tempProp[1] == '\0') {
            cout << "Actualización cancelada." << endl;
            return;
        }

        cout << format("Viejo {}: {}", nombrePropiedad, propiedad) << endl;
        cout << format("Nuevo {}: {}", nombrePropiedad, tempProp) << endl;

        cout << format("Está seguro que desea actualizar el {}? (s/n): ", nombrePropiedad);
        cin >> confirmar;
        if (confirmar == 's' || confirmar == 'S') {
            copiarString(propiedad, tempProp);
            cout << format("{} actualizado a: {}", nombrePropiedad, propiedad) << endl;
        } else {
            cout << "Actualización cancelada." << endl;
        }
        // Int, Float
    } else if constexpr (std::is_arithmetic_v<T>) {
        T tempProp;
        cout << "Ingrese el nuevo " << nombrePropiedad << ": ";
        while (true) {
            cin >> tempProp;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << CLEAR_SCREEN << COLOR_RED
                     << "El valor debe ser numérico. Intente nuevamente: " << COLOR_RESET << endl;
                continue;
            }
            if (tempProp >= 0)
                break;
            cout << CLEAR_SCREEN << COLOR_RED
                 << "El valor debe ser mayor o igual a 0. Intente nuevamente: " << COLOR_RESET
                 << endl;
        }

        cout << format("Viejo {}: {}", nombrePropiedad, propiedad) << endl;
        cout << format("Nuevo {}: {}", nombrePropiedad, tempProp) << endl;

        cout << format("Está seguro que desea actualizar el {}? (s/n): ", nombrePropiedad);
        cin >> confirmar;
        if (confirmar == 's' || confirmar == 'S') {
            propiedad = tempProp;
            cout << format("{} actualizado a: {}", nombrePropiedad, propiedad) << endl;
        } else {
            cout << "Actualizacion cancelada." << endl;
        }
    }
}

void actualizarProducto(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades("Productos", tienda->productos, tienda->numProductos);

    int id = leerId("Ingresa el id del producto a actualizar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Actualización cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId(tienda->productos, tienda->numProductos, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Producto a actualizar no encontrado." << COLOR_RESET
             << endl;
        return;
    }

    Producto& producto = tienda->productos[index];
    cout << format("Producto con el id {} encontrado: {}", id, producto.nombre) << endl;

    char opcion;
    do {
        cout << COLOR_CYAN << "¿Qué desea actualizar?: " << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "1." << COLOR_RESET << " Nombre" << endl;
        cout << COLOR_YELLOW << "2." << COLOR_RESET << " Codigo" << endl;
        cout << COLOR_YELLOW << "3." << COLOR_RESET << " Descripcion" << endl;
        cout << COLOR_YELLOW << "4." << COLOR_RESET << " Precio" << endl;
        cout << COLOR_YELLOW << "5." << COLOR_RESET << " Stock" << endl;
        cout << COLOR_YELLOW << "6." << COLOR_RESET << " Proveedor" << endl;
        cout << COLOR_RED << "0." << COLOR_RESET << " Cancelar" << endl;
        cout << "Seleccione una opción: ";

        cin >> opcion;

        switch (opcion) {
        case ActualizarNombre:
            manejarPropiedad("nombre", producto.nombre);
            break;
        case ActualizarCodigo:
            manejarPropiedad("codigo", producto.codigo);
            break;
        case ActualizarDescripcion:
            manejarPropiedad("descripcion", producto.descripcion);
            break;
        case ActualizarPrecio:
            manejarPropiedad("precio", producto.precio);
            break;
        case ActualizarStock:
            manejarPropiedad("stock", producto.stock);
            break;
        case ActualizarProveedor: {
            mostrarListaEntidades("Proveedores Disponibles", tienda->proveedores,
                                  tienda->numProveedores, PropiedadAmbos);
            int idNuevoProveedor = leerId("Ingrese el ID del nuevo proveedor (q para cancelar)");
            if (idNuevoProveedor <= 0) {
                cout << "Actualización cancelada." << endl;
            } else {
                int provIndex = buscarEntidadPorId(tienda->proveedores, tienda->numProveedores,
                                                   idNuevoProveedor);
                if (provIndex == -1) {
                    cout << COLOR_RED << "Error: Proveedor no encontrado." << COLOR_RESET << endl;
                } else {
                    producto.idProveedor = idNuevoProveedor;
                    cout << "Proveedor actualizado con éxito." << endl;
                }
            }
            break;
        }
        case ActualizarCancelada:
            cout << COLOR_GREEN << "Se canceló la actualizacion." << COLOR_RESET << endl;
            break;
        default:
            cout << CLEAR_SCREEN << COLOR_RED << "Opcion no valida" << COLOR_RESET << endl;
            break;
        }
    } while (opcion != ActualizarCancelada);
}

void actualizarStockProducto(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades("Productos", tienda->productos, tienda->numProductos);

    int id = leerId("Ingrese el id del producto para actualizar el stock");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId(tienda->productos, tienda->numProductos, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Producto no encontrado." << COLOR_RESET << endl;
        return;
    }

    Producto& producto = tienda->productos[index];
    cout << format("Producto: {} | Stock actual: {}", producto.nombre, producto.stock) << endl;

    char opcion;
    do {
        cout << COLOR_CYAN << "Seleccione la operación a realizar: " << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "1." << COLOR_RESET << " Incrementar stock" << endl;
        cout << COLOR_YELLOW << "2." << COLOR_RESET << " Disminuir stock" << endl;
        cout << COLOR_RED << "0." << COLOR_RESET << " Cancelar" << endl;
        cout << "Seleccione una opción: ";

        cin >> opcion;

        switch (opcion) {
        case '1': // Hecho a propósito
        case '2': {
            int cantidad;
            cout << "Ingrese la cantidad a modificar: ";

            while (true) {
                cin >> cantidad;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << CLEAR_SCREEN << COLOR_RED
                         << "El valor debe ser numérico. Intente nuevamente: " << COLOR_RESET
                         << endl;
                    continue;
                }
                if (cantidad > 0)
                    break;
                cout << CLEAR_SCREEN << COLOR_RED
                     << "La cantidad debe ser mayor a 0. Intente nuevamente: " << COLOR_RESET
                     << endl;
            }

            int nuevoStock = producto.stock;
            if (opcion == '1') {
                nuevoStock += cantidad;
            }

            if (opcion == '2') {
                nuevoStock -= cantidad;
                if (nuevoStock < 0) {
                    cout << CLEAR_SCREEN << COLOR_RED
                         << format("Error: El stock no puede ser negativo. Faltan unidades. Stock "
                                   "actual: {}",
                                   producto.stock)
                         << COLOR_RESET << endl;
                    break;
                }
            }

            cout << format("Stock actual: {} | Nuevo stock: {}", producto.stock, nuevoStock)
                 << endl;
            cout << "¿Está seguro de aplicar este cambio al stock? (s/n): ";
            char confirmar;
            cin >> confirmar;

            if (confirmar == 's' || confirmar == 'S') {
                producto.stock = nuevoStock;
                cout << "Stock actualizado exitosamente." << endl;

            } else {
                cout << "Operación cancelada." << endl;
            }
            break;
        }
        case '0':
            cout << COLOR_GREEN << "Operación cancelada." << COLOR_RESET << endl;
            break;
        default:
            cout << CLEAR_SCREEN << COLOR_RED << "Opción no válida." << COLOR_RESET << endl;
            break;
        }

    } while (opcion != '0');
}

void listarProductos(Tienda* tienda) {
    if (tienda == nullptr) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: La tienda no ha sido creada." << COLOR_RESET
             << endl;
        return;
    }

    if (tienda->numProductos == 0) {
        cout << "No hay productos registrados." << endl;
        return;
    }

    cout << "--- Lista de Productos (" << tienda->numProductos << ") ---" << endl;
    cout << format("{:<5} | {:<20} | {:<15} | {:<10} | {:<5}", "ID", "Nombre", "Codigo", "Precio",
                   "Stock")
         << endl;
    cout << "----------------------------------------------------------------------" << endl;
    for (int i = 0; i < tienda->numProductos; ++i) {
        Producto& producto = tienda->productos[i];
        cout << format("{:<5} | {:<20} | {:<15} | ${:<9.2f} | {:<5}", producto.id, producto.nombre,
                       producto.codigo, producto.precio, producto.stock)
             << endl;
    }
    cout << "----------------------------------------------------------------------" << endl;
}

void eliminarProducto(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades("Productos", tienda->productos, tienda->numProductos);

    int id = leerId("Ingresa el id del producto a eliminar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Eliminación cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId(tienda->productos, tienda->numProductos, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Producto no encontrado." << COLOR_RESET << endl;
        return;
    }

    Producto& producto = tienda->productos[index];
    cout << "Datos del producto a eliminar:" << endl;
    cout << "Id: " << producto.id << endl;
    cout << "Nombre: " << producto.nombre << endl;
    cout << "Codigo: " << producto.codigo << endl;
    cout << "Stock actual: " << producto.stock << endl;

    bool tieneTransacciones = false;
    for (int i = 0; i < tienda->numTransacciones; ++i) {
        if (tienda->transacciones[i].idProducto == id) {
            tieneTransacciones = true;
            break;
        }
    }

    if (tieneTransacciones) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: No se puede eliminar el producto porque tiene transacciones asociadas. "
             << "Cancele las transacciones primero si desea eliminarlo." << COLOR_RESET << endl;
        return;
    }

    cout << "\n¿Está seguro que desea eliminar este producto? (s/n): ";
    char confirmar;
    cin >> confirmar;

    if (confirmar == 's' || confirmar == 'S') {
        eliminarElementoDeArray(tienda->productos, index, tienda->numProductos);
        cout << "Producto eliminado exitosamente." << endl;
    } else {
        cout << "Eliminación cancelada." << endl;
    }
}

void crearProveedor(Tienda* tienda) {
    int& totalDeProveedores = tienda->numProveedores;
    int& capacidadProveedores = tienda->capacidadProveedores;
    // es un puntero DE LA REFERENCIA
    Proveedor*& proveedores = tienda->proveedores;

    // Aumentar espacio si es necesario
    if (totalDeProveedores >= capacidadProveedores) {
        redimensionarEntidad(proveedores, capacidadProveedores, totalDeProveedores);
    }
    int index = tienda->numProveedores;

    char nombre[100];
    asignarPropiedadString("Ingrese el nombre del proveedor (q para cancelar): ", nombre);
    if (nombre[0] == 'q' && nombre[1] == '\0')
        return;

    char rif[20];
    while (true) {
        asignarPropiedadString("Ingrese el RIF del proveedor (q para cancelar): ", rif);
        if (rif[0] == 'q' && rif[1] == '\0')
            return;
        if (rif[0] == '\0') {
            cout << COLOR_RED << "Error: El RIF no puede estar vacío." << COLOR_RESET << endl;
            continue;
        }
        if (rifDuplicado(tienda, rif)) {
            cout << COLOR_RED << "Error: Ya existe un proveedor con el RIF " << rif << COLOR_RESET
                 << endl;
        } else {
            break;
        }
    }

    char telefono[20];
    asignarPropiedadString("Ingrese el telefono del proveedor: ", telefono);
    char email[100];
    while (true) {
        asignarPropiedadString("Ingrese el email del proveedor (vacio para omitir): ", email);
        if (email[0] == '\0') {
            break; // Email opcional
        }
        if (validarEmail(email)) {
            break;
        }
    }
    char direccion[200];
    asignarPropiedadString("Ingrese la direccion del proveedor: ", direccion);

    cout << "Está seguro de crear el proveedor? (s/n): ";
    char confirmar;
    cin >> confirmar;
    if (confirmar == 's' || confirmar == 'S') {
        Proveedor& prov = tienda->proveedores[index];
        prov.id = tienda->siguienteIdProveedor++;
        copiarString(prov.nombre, nombre);
        copiarString(prov.rif, rif);
        copiarString(prov.telefono, telefono);
        copiarString(prov.email, email);
        copiarString(prov.direccion, direccion);
        obtenerFechaActual(prov.fechaRegistro);
        tienda->numProveedores++;
        cout << "Proveedor creado con exito." << endl;
    } else {
        cout << "Proveedor no creado." << endl;
    }
}

void buscarProveedor(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades("Proveedores", tienda->proveedores, tienda->numProveedores, PropiedadId);

    int id = leerId("Ingrese el id del proveedor a buscar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Búsqueda cancelada." << COLOR_RESET << endl;
        return;
    }
    int index = buscarEntidadPorId(tienda->proveedores, tienda->numProveedores, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Proveedor no encontrado." << COLOR_RESET << endl;
        return;
    }
    Proveedor& p = tienda->proveedores[index];
    cout << "Proveedor encontrado:" << endl;
    cout << "Id: " << p.id << "\nNombre: " << p.nombre << "\nRIF: " << p.rif
         << "\nTelefono: " << p.telefono << "\nEmail: " << p.email << "\nDireccion: " << p.direccion
         << "\nFecha de Registro: " << p.fechaRegistro << endl;
}

void actualizarProveedor(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades("Proveedores", tienda->proveedores, tienda->numProveedores);

    int id = leerId("Ingrese el id del proveedor a actualizar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Actualización cancelada." << COLOR_RESET << endl;
        return;
    }
    int index = buscarEntidadPorId(tienda->proveedores, tienda->numProveedores, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Proveedor no encontrado." << COLOR_RESET << endl;
        return;
    }
    Proveedor& p = tienda->proveedores[index];

    char opcion;
    do {
        cout << COLOR_CYAN << "¿Qué desea actualizar?: " << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "1." << COLOR_RESET << " Nombre\n"
             << COLOR_YELLOW << "2." << COLOR_RESET << " RIF\n"
             << COLOR_YELLOW << "3." << COLOR_RESET << " Telefono\n"
             << COLOR_YELLOW << "4." << COLOR_RESET << " Email\n"
             << COLOR_YELLOW << "5." << COLOR_RESET << " Direccion\n"
             << COLOR_RED << "0." << COLOR_RESET << " Cancelar\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;
        switch (opcion) {
        case '1':
            manejarPropiedad("nombre", p.nombre);
            break;
        case '2':
            manejarPropiedad("rif", p.rif);
            break;
        case '3':
            manejarPropiedad("telefono", p.telefono);
            break;
        case '4':
            manejarPropiedad("email", p.email);
            break;
        case '5':
            manejarPropiedad("direccion", p.direccion);
            break;
        case '0':
            cout << COLOR_GREEN << "Saliendo..." << COLOR_RESET << endl;
            break;
        default:
            cout << CLEAR_SCREEN << COLOR_RED << "Opcion no valida" << COLOR_RESET << endl;
        }
    } while (opcion != '0');
}

void listarProveedores(Tienda* tienda) {
    if (tienda == nullptr || tienda->numProveedores == 0) {
        cout << "No hay proveedores registrados." << endl;
        return;
    }
    cout << "--- Lista de Proveedores (" << tienda->numProveedores << ") ---" << endl;
    cout << format("{:<5} | {:<20} | {:<15} | {:<15} | {:<25} | {:<15}", "ID", "Nombre", "RIF",
                   "Telefono", "Email", "Direccion")
         << endl;
    cout << "--------------------------------------------------------------------------------------"
            "-----------------"
         << endl;
    for (int i = 0; i < tienda->numProveedores; ++i) {
        Proveedor& proveedor = tienda->proveedores[i];
        cout << format("{:<5} | {:<20} | {:<15} | {:<15} | {:<25} | {:<15}", proveedor.id,
                       proveedor.nombre, proveedor.rif, proveedor.telefono, proveedor.email,
                       proveedor.direccion)
             << endl;
    }
    cout << "--------------------------------------------------------------------------------------"
            "-----------------"
         << endl;
}

void eliminarProveedor(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades("Proveedores", tienda->proveedores, tienda->numProveedores);

    int id = leerId("Ingrese el id del proveedor a eliminar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Eliminación cancelada." << COLOR_RESET << endl;
        return;
    }
    int index = buscarEntidadPorId(tienda->proveedores, tienda->numProveedores, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Proveedor no encontrado." << COLOR_RESET << endl;
        return;
    }
    bool tieneTransacciones = false;
    for (int i = 0; i < tienda->numTransacciones; ++i) {
        if (tienda->transacciones[i].idRelacionado == id &&
            tienda->transacciones[i].tipo == COMPRA) {
            tieneTransacciones = true;
            break;
        }
    }

    if (tieneTransacciones) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: No se puede eliminar el proveedor porque tiene transacciones asociadas. "
             << "Cancele las transacciones primero si desea eliminarlo." << COLOR_RESET << endl;
        return;
    }

    cout << "¿Está seguro que desea eliminar este proveedor? (s/n): ";
    char confirmar;
    cin >> confirmar;
    if (confirmar == 's' || confirmar == 'S') {
        eliminarElementoDeArray(tienda->proveedores, index, tienda->numProveedores);
        cout << "Proveedor eliminado exitosamente." << endl;
    } else {
        cout << "Eliminación cancelada." << endl;
    }
}

void crearCliente(Tienda* tienda) {
    if (tienda->numClientes >= tienda->capacidadClientes) {
        redimensionarEntidad(tienda->clientes, tienda->capacidadClientes, tienda->numClientes);
    }
    int index = tienda->numClientes;

    char nombre[100];
    asignarPropiedadString("Ingrese el nombre del cliente (q para cancelar): ", nombre);
    if (nombre[0] == 'q' && nombre[1] == '\0')
        return;

    char cedula[20];
    asignarPropiedadString("Ingrese la cedula del cliente (q para cancelar): ", cedula);
    if (cedula[0] == 'q' && cedula[1] == '\0')
        return;

    char telefono[20];
    asignarPropiedadString("Ingrese el telefono del cliente: ", telefono);
    char email[100];
    while (true) {
        asignarPropiedadString("Ingrese el email del cliente (vacio para omitir): ", email);
        if (email[0] == '\0') {
            break; // Email opcional
        }
        if (validarEmail(email)) {
            break;
        }
    }
    char direccion[200];
    asignarPropiedadString("Ingrese la direccion del cliente: ", direccion);

    cout << "Está seguro de crear el cliente? (s/n): ";
    char confirmar;
    cin >> confirmar;
    if (confirmar == 's' || confirmar == 'S') {
        Cliente& cliente = tienda->clientes[index];
        cliente.id = tienda->siguienteIdCliente++;
        copiarString(cliente.nombre, nombre);
        copiarString(cliente.cedula, cedula);
        copiarString(cliente.telefono, telefono);
        copiarString(cliente.email, email);
        copiarString(cliente.direccion, direccion);
        obtenerFechaActual(cliente.fechaRegistro);
        tienda->numClientes++;
        cout << "Cliente creado con exito." << endl;
    } else {
        cout << "Cliente no creado." << endl;
    }
}

void buscarCliente(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades("Clientes", tienda->clientes, tienda->numClientes, PropiedadId);

    int id = leerId("Ingrese el id del cliente a buscar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Búsqueda cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId(tienda->clientes, tienda->numClientes, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Cliente no encontrado." << COLOR_RESET << endl;
        return;
    }
    Cliente& c = tienda->clientes[index];
    cout << "Cliente encontrado:" << endl;
    cout << "Id: " << c.id << "\nNombre: " << c.nombre << "\nCedula: " << c.cedula
         << "\nTelefono: " << c.telefono << "\nEmail: " << c.email << "\nDireccion: " << c.direccion
         << "\nFecha de Registro: " << c.fechaRegistro << endl;
}

void actualizarCliente(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades("Clientes", tienda->clientes, tienda->numClientes);

    int id = leerId("Ingrese el id del cliente a actualizar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Actualización cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId(tienda->clientes, tienda->numClientes, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Cliente no encontrado." << COLOR_RESET << endl;
        return;
    }
    Cliente& clientes = tienda->clientes[index];

    char opcion;
    do {
        cout << COLOR_CYAN << "¿Qué desea actualizar?: " << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "1." << COLOR_RESET << " Nombre\n"
             << COLOR_YELLOW << "2." << COLOR_RESET << " Cedula\n"
             << COLOR_YELLOW << "3." << COLOR_RESET << " Telefono\n"
             << COLOR_YELLOW << "4." << COLOR_RESET << " Email\n"
             << COLOR_YELLOW << "5." << COLOR_RESET << " Direccion\n"
             << COLOR_RED << "0." << COLOR_RESET << " Cancelar\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;
        switch (opcion) {
        case '1':
            manejarPropiedad("nombre", clientes.nombre);
            break;
        case '2':
            manejarPropiedad("cedula", clientes.cedula);
            break;
        case '3':
            manejarPropiedad("telefono", clientes.telefono);
            break;
        case '4':
            manejarPropiedad("email", clientes.email);
            break;
        case '5':
            manejarPropiedad("direccion", clientes.direccion);
            break;
        case '0':
            cout << COLOR_GREEN << "Saliendo..." << COLOR_RESET << endl;
            break;
        default:
            cout << CLEAR_SCREEN << COLOR_RED << "Opcion no valida" << COLOR_RESET << endl;
        }
    } while (opcion != '0');
}

void listarClientes(Tienda* tienda) {
    if (tienda == nullptr || tienda->numClientes == 0) {
        cout << "No hay clientes registrados." << endl;
        return;
    }
    cout << "--- Lista de Clientes (" << tienda->numClientes << ") ---" << endl;
    cout << format("{:<5} | {:<20} | {:<15} | {:<15} | {:<25}", "ID", "Nombre", "Cedula",
                   "Telefono", "Email")
         << endl;
    cout << "--------------------------------------------------------------------------------------"
            "--"
         << endl;
    for (int i = 0; i < tienda->numClientes; ++i) {
        Cliente& cliente = tienda->clientes[i];
        cout << format("{:<5} | {:<20} | {:<15} | {:<15} | {:<25}", cliente.id, cliente.nombre,
                       cliente.cedula, cliente.telefono, cliente.email)
             << endl;
    }
    cout << "--------------------------------------------------------------------------------------"
            "--"
         << endl;
}

void eliminarCliente(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades("Clientes", tienda->clientes, tienda->numClientes);

    int id = leerId("Ingrese el id del cliente a eliminar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Eliminación cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId(tienda->clientes, tienda->numClientes, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Cliente no encontrado." << COLOR_RESET << endl;
        return;
    }

    bool tieneTransacciones = false;
    for (int i = 0; i < tienda->numTransacciones; ++i) {
        if (tienda->transacciones[i].idRelacionado == id &&
            tienda->transacciones[i].tipo == VENTA) {
            tieneTransacciones = true;
            break;
        }
    }

    if (tieneTransacciones) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: No se puede eliminar el cliente porque tiene transacciones asociadas. "
             << "Cancele las transacciones primero si desea eliminarlo." << COLOR_RESET << endl;
        return;
    }

    cout << "¿Está seguro que desea eliminar este cliente? (s/n): ";
    char confirmar;
    cin >> confirmar;
    if (confirmar == 's' || confirmar == 'S') {
        eliminarElementoDeArray(tienda->clientes, index, tienda->numClientes);
        cout << "Cliente eliminado exitosamente." << endl;
    } else {
        cout << "Eliminación cancelada." << endl;
    }
}

void registrarCompra(Tienda* tienda) {
    // pregunta de seguridad
    if (tienda == nullptr)
        return;

    // la operacion de mudanza de datos donde actualizamos el espacio y memoria de estos
    if (tienda->numTransacciones >= tienda->capacidadTransacciones) {
        redimensionarEntidad(tienda->transacciones, tienda->capacidadTransacciones,
                             tienda->numTransacciones);
    }
    cout << "\n===REGISTRAR COMPRA (Entrada de Mercancia)===" << endl;

    mostrarListaEntidades("Productos", tienda->productos, tienda->numProductos);

    // buscar el producto
    int idProd = leerId("Ingrese ID del Producto: ");
    if (idProd <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }
    int idxProd = buscarEntidadPorId(tienda->productos, tienda->numProductos, idProd);

    if (idxProd == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: El producto con ID " << idProd << " no existe."
             << COLOR_RESET << endl;
        return;
    }
    // referencia para hacer el texto mas legible
    Producto& producto = tienda->productos[idxProd];

    mostrarListaEntidades("Proveedores", tienda->proveedores, tienda->numProveedores);

    int idProv = leerId("Ingrese ID del Proveedor: ");
    if (idProv <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }
    if (!existeProveedor(tienda, idProv)) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: El proveedor no existe en el sistema."
             << COLOR_RESET << endl;
        return;
    }
    int cantidad = leerId("Cantidad comprada: ");
    if (cantidad <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }
    // referencia
    Transaccion& transaccion = tienda->transacciones[tienda->numTransacciones];

    transaccion.id = tienda->siguienteIdTransaccion++;
    transaccion.tipo = COMPRA;
    transaccion.idProducto = idProd;
    transaccion.idRelacionado = idProv; // En compras, guardamos el ID del proveedor
    transaccion.cantidad = cantidad;
    transaccion.precioUnitario = producto.precio;
    transaccion.total = transaccion.cantidad * transaccion.precioUnitario;
    obtenerFechaActual(transaccion.fecha);

    producto.stock += cantidad;

    tienda->numTransacciones++;

    cout << "Compra registrada ID: " << transaccion.id << endl;
    cout << "Stock actualizado de " << producto.nombre << ": " << producto.stock << endl;
}

void registrarVenta(Tienda* tienda) {
    // pregunta de validacion
    if (tienda == nullptr)
        return;

    // la operacion de mudanza de datos donde actualizamos el espacio y memoria de estos
    if (tienda->numTransacciones >= tienda->capacidadTransacciones) {
        redimensionarEntidad(tienda->transacciones, tienda->capacidadTransacciones,
                             tienda->numTransacciones);
    }
    cout << "\n===REGISTRAR VENTA (Salida de Mercancia)===" << endl;

    mostrarListaEntidades("Productos", tienda->productos, tienda->numProductos);

    // buscar producto
    int idProd = leerId("Ingrese ID del Producto: ");
    if (idProd <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }
    int idxProd = buscarEntidadPorId(tienda->productos, tienda->numProductos, idProd);

    if (idxProd == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: El producto no existe." << COLOR_RESET << endl;
        return;
    }
    Producto& producto = tienda->productos[idxProd];

    // validar stock
    int cantidad = leerId("Cantidad a vender: ");
    if (cantidad <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }

    if (producto.stock < cantidad) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: Stock insuficiente. Solo hay "
             << producto.stock << " unidades." << COLOR_RESET << endl;
        return;
    }

    // validar cliente
    mostrarListaEntidades("Clientes", tienda->clientes, tienda->numClientes);

    int idCli = leerId("Ingrese ID del Cliente: ");
    if (idCli <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }
    if (!existeCliente(tienda, idCli)) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: El cliente no existe." << COLOR_RESET << endl;
        return;
    }
    // referencia
    Transaccion& transaccion = tienda->transacciones[tienda->numTransacciones];

    transaccion.id = tienda->siguienteIdTransaccion++;
    transaccion.tipo = VENTA;
    transaccion.idProducto = idProd;
    transaccion.idRelacionado = idCli; // Aquí guardamos al cliente
    transaccion.cantidad = cantidad;
    transaccion.precioUnitario = producto.precio;
    transaccion.total = transaccion.cantidad * transaccion.precioUnitario;
    obtenerFechaActual(transaccion.fecha);

    // restamos el espacio en el stock
    producto.stock -= cantidad;

    tienda->numTransacciones++;

    // confirmamos venta
    cout << "¡Venta exitosa! Total: " << transaccion.total
         << " | Stock restante: " << producto.stock << endl;
}

void buscarTransacciones(Tienda* tienda) {
    // pregunta de seguridad
    if (tienda == nullptr || tienda->numTransacciones == 0) {
        cout << "\n[!] No hay transacciones registradas en el sistema." << endl;
        return;
    }

    // funcion de leer el id
    int idBuscar = leerId("\nIngrese el ID de la transaccion a buscar: ");
    if (idBuscar <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Búsqueda cancelada." << COLOR_RESET << endl;
        return;
    }
    // template de buscar entidad
    int idx = buscarEntidadPorId(tienda->transacciones, tienda->numTransacciones, idBuscar);

    if (idx == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: La transaccion con ID " << idBuscar
             << " no existe." << COLOR_RESET << endl;
        return;
    }
    Transaccion& transaccion = tienda->transacciones[idx];
    // mostrar la informacion
    const char* tipoStr = (transaccion.tipo == COMPRA) ? "COMPRA" : "VENTA";
    cout << "\n========================================" << endl;
    cout << "       DETALLE DE TRANSACCION #" << transaccion.id << endl;
    cout << "========================================" << endl;
    cout << "Tipo:       " << tipoStr << endl;
    cout << "Fecha:      " << transaccion.fecha << endl;
    cout << "Producto ID: " << transaccion.idProducto << endl;
    cout << "ID Asociado: " << transaccion.idRelacionado << " (Cliente/Proveedor)" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Cantidad:    " << transaccion.cantidad << endl;
    cout << "Precio Un.:  $" << transaccion.precioUnitario << endl;
    cout << "TOTAL:       $" << transaccion.total << endl;
    cout << "========================================" << endl;
}

void listarTransacciones(Tienda* tienda) {
    // pregunda de verificacion
    if (tienda == nullptr || tienda->numTransacciones == 0) {
        cout << "\n[!] El historial de transacciones esta vacio." << endl;
        return;
    }
    cout << "\n===================================================================================="
            "====="
         << endl;
    cout << "                         HISTORIAL COMPLETO DE MOVIMIENTOS" << endl;
    cout << "======================================================================================"
            "==="
         << endl;
    cout << format("{:<5} | {:<8} | {:<12} | {:<10} | {:<10} | {:<12} | {:<12} | {:<15}", "ID",
                   "Tipo", "Fecha", "Prod ID", "Cantidad", "Precio Un.", "Total", "Asociado ID")
         << endl;
    cout << "--------------------------------------------------------------------------------------"
            "---"
         << endl;
    // El ciclo que recorre el arreglo
    for (int i = 0; i < tienda->numTransacciones; i++) {
        // referencia
        const Transaccion& t = tienda->transacciones[i];
        const char* tipoStr = (t.tipo == COMPRA) ? "COMPRA" : "VENTA";

        cout << format(
                    "{:<5} | {:<8} | {:<12} | {:<10} | {:<10} | ${:<11.2f} | ${:<11.2f} | {:<15}",
                    t.id, tipoStr, t.fecha, t.idProducto, t.cantidad, t.precioUnitario, t.total,
                    t.idRelacionado)
             << endl;
    }
    cout << "======================================================================================"
            "==="
         << endl;
    cout << "Total de registros: " << tienda->numTransacciones << endl;
}

void cancelarTransaccion(Tienda* tienda) {
    // pregunta de verificacion
    if (tienda == nullptr || tienda->numTransacciones == 0) {
        cout << "\n[!] No hay transacciones para cancelar." << endl;
        return;
    }

    // buscar producto
    // funcion leer id
    int idBuscar = leerId("\nIngrese el ID de la transaccion a CANCELAR: ");
    if (idBuscar <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }
    // template buscar entidad
    int idx = buscarEntidadPorId(tienda->transacciones, tienda->numTransacciones, idBuscar);

    if (idx == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: Transaccion no encontrada." << COLOR_RESET
             << endl;
        return;
    }

    Transaccion& transaccion = tienda->transacciones[idx];
    int idxProd =
        buscarEntidadPorId(tienda->productos, tienda->numProductos, transaccion.idProducto);

    if (idxProd == -1) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: El producto de esta transaccion ya no existe en el sistema." << COLOR_RESET
             << endl;
        return;
    }
    Producto& producto = tienda->productos[idxProd];

    // REVERTIR EL STOCK (La parte lógica)
    if (transaccion.tipo == VENTA) {
        // Si fue una VENTA -> Devolvemos al stock
        producto.stock += transaccion.cantidad;
        cout << "[SISTEMA] Venta cancelada. Se devolvieron " << transaccion.cantidad
             << " unidades al stock." << endl;
    } else if (transaccion.tipo == COMPRA) {
        // Si fue una COMPRA -> Quitamos del stock
        if (producto.stock < transaccion.cantidad) {
            cout << "Error: No se puede cancelar la compra. El stock actual es menor a lo que "
                    "quieres quitar."
                 << endl;
            return;
        }
        producto.stock -= transaccion.cantidad;
        cout << "[SISTEMA] Compra cancelada. Se retiraron " << transaccion.cantidad
             << " unidades del stock." << endl;
    }
    // eliminar la transaccion
    eliminarElementoDeArray(tienda->transacciones, idx, tienda->numTransacciones);
    cout << "\n[!] Transaccion #" << idBuscar << " eliminada del historial con exito." << endl;
}

///////////// FIN FUNCIONES CRUD DEL PROGRAMA //////////

// VALIDACIONES
bool validarEmail(const char* email) {
    // Seguridad para ver si el puntero existe
    if (email == nullptr) {
        return false;
    }
    int PosicionAt = -1;            // verificar y darle valor al "@"
    bool TienePuntoDespues = false; // para comprobar que tenga "." despues del "@"
    int longitud = strlen(email);   // para que el ciclo for sepa hasta donde buscar

    // la funcion "for" para recorrer todo el email
    for (int i = 0; i < longitud; i++) {
        if (email[i] == '@') {
            // verificacion si ya habia un @ y hay dos @
            if (PosicionAt != -1)
                return false;
            PosicionAt = i;
        }
        // verificacion de si hay un '.' despues de ya tener el '@'
        if (PosicionAt != -1 && email[i] == '.') {
            if (i > PosicionAt + 1) {
                TienePuntoDespues = true;
            }
        }
    }
    // verificacion de los errores
    if (PosicionAt > 0 && PosicionAt < longitud - 1 && TienePuntoDespues) {
        return true;
    }
    // si el email es invalido
    cout << "ERROR: Formato de email invalido." << endl;
    return false;
}
bool validarFecha(const char* fecha) {
    // estos solo estan vivios mientras se este utilizando esta funcion
    using namespace std;
    using namespace std::chrono;

    // extraemos los datos de YYYY-MM-DD
    stringstream ss(fecha);
    int y, m, d;
    char dash1, dash2;

    // verificar si es correcto el formato
    if (!(ss >> y >> dash1 >> m >> dash2 >> d)) {
        return false;
    }

    // para ver que tenga los guiones donde van
    if (dash1 != '-' || dash2 != '-') {
        return false;
    }
    // year_month_day representa una fecha en el calendario civil
    year_month_day ymd{year{y}, month{(unsigned)m}, day{(unsigned)d}};
    return ymd.ok();
}

bool existeProducto(Tienda* tienda, int id) {
    // verificacion de seguridad
    if (tienda == nullptr || tienda->productos == nullptr) {
        return false;
    }
    // usamos el template anterior
    return buscarEntidadPorId(tienda->productos, tienda->numProductos, id) != -1;
}

bool existeProveedor(Tienda* tienda, int id) {
    // Verificación de seguridad básica
    if (tienda == nullptr || tienda->proveedores == nullptr) {
        return false;
    }
    // utilizamos el template anterior
    return buscarEntidadPorId(tienda->proveedores, tienda->numProveedores, id) != -1;
}

bool existeCliente(Tienda* tienda, int id) {
    // Verificación de seguridad
    if (tienda == nullptr || tienda->clientes == nullptr) {
        return false;
    }
    // utilizamos el template anterior
    return buscarEntidadPorId(tienda->clientes, tienda->numClientes, id) != -1;
}

bool codigoDuplicado(Tienda* tienda, const char* codigo) {
    // Verificamos que la tienda exista
    if (tienda == nullptr) {
        return false;
    }
    // utilizamos el template anterior
    return existeStringDuplicado(tienda->productos, tienda->numProductos, codigo,
                                 &Producto::codigo);
}

bool rifDuplicado(Tienda* tienda, const char* rif) {
    // Verificación de seguridad
    if (tienda == nullptr) {
        return false;
    }
    // usamos el template anterior
    return existeStringDuplicado(tienda->proveedores, tienda->numProveedores, rif, &Proveedor::rif);
}

// Búsquedas
// =======================================================
//                 SECCIÓN DE TEMPLATES
// =======================================================
// Casi todas las entidades tienen un id (cliente, proveedor, producto)
template <typename T> int buscarEntidadPorId(T* array, int count, int id) {
    if (array == nullptr)
        return -1;
    for (int i = 0; i < count; ++i) {
        if (array[i].id == id)
            return i;
    }
    return -1;
}

template <typename T>
void mostrarListaEntidades(const char* titulo, T* array, int count, TipoPropiedadLista tipoProp) {
    if (count == 0)
        return;

    cout << "\n--- " << titulo << " Disponibles ---" << endl;
    switch (tipoProp) {
    case PropiedadId:
        cout << format("{:<5}", "ID") << endl;
        cout << "-------------------------------" << endl;
        for (int i = 0; i < count; ++i) {
            cout << format("{:<5}", array[i].id) << endl;
        }
        break;
    case PropiedadNombre:
        cout << format("{:<20}", "Nombre") << endl;
        cout << "-------------------------------" << endl;
        for (int i = 0; i < count; ++i) {
            cout << format("{:<20}", array[i].nombre) << endl;
        }
        break;
    case PropiedadAmbos:
        cout << format("{:<5} | {:<20}", "ID", "Nombre") << endl;
        cout << "-------------------------------" << endl;
        for (int i = 0; i < count; ++i) {
            cout << format("{:<5} | {:<20}", array[i].id, array[i].nombre) << endl;
        }
        break;
    }
    cout << "-------------------------------" << endl;
}

// para las entidades que revisan si si hay strngis duplicados(codigo,rif)
template <typename T, size_t N>
bool existeStringDuplicado(T* array, int count, const char* valorBusqueda, char (T::*miembro)[N]) {
    for (int i = 0; i < count; ++i) {
        // La sintaxis (array[i].*miembro) permite entrar al campo que le indiquemos
        if (strcmp(array[i].*miembro, valorBusqueda) == 0) {
            return true;
        }
    }
    return false;
}

// retorna array de indices del producto
int* buscarProductosPorNombre(Tienda* tienda, const char* nombre) {
    if (tienda == nullptr || nombre == nullptr)
        return nullptr;

    int count = 0;
    for (int i = 0; i < tienda->numProductos; ++i) {
        if (contieneSubstring(tienda->productos[i].nombre, nombre)) {
            count++;
        }
    }

    if (count == 0)
        return nullptr;

    int* result = new int[count + 1];
    result[0] = count; // el índice 0 guarda la cantidad de elementos

    int pos = 1;
    for (int i = 0; i < tienda->numProductos; ++i) {
        if (contieneSubstring(tienda->productos[i].nombre, nombre)) {
            result[pos++] = i;
        }
    }

    return result;
}

//// UTILIDADES

// formato YYYY-MM-DD
void obtenerFechaActual(char* fecha) {
    auto ahora = std::chrono::system_clock::now();
    // Convertimos el tiempo a formato de días (sys_days)
    auto sys_days_now = std::chrono::time_point_cast<std::chrono::days>(ahora);
    // Extraemos la fecha del calendario civil (year_month_day)
    std::chrono::year_month_day ymd{sys_days_now};

    // Convertimos las partes a enteros
    int anio = static_cast<int>(ymd.year());
    unsigned mes = static_cast<unsigned>(ymd.month());
    unsigned dia = static_cast<unsigned>(ymd.day());

    // Formateamos como "YYYY-MM-DD" y lo guardamos en el buffer (máximo 11 caracteres)
    snprintf(fecha, 11, "%04d-%02u-%02u", anio, mes, dia);
    fecha[10] = '\0'; // Aseguramos el null-termination por si acaso
}

void convertirAMinusculas(char* cadena) {
    if (cadena == nullptr)
        return;
    for (int i = 0; cadena[i] != '\0'; ++i) {
        if (cadena[i] >= 'A' && cadena[i] <= 'Z') {
            cadena[i] = cadena[i] + ('a' - 'A');
        }
    }
}

bool contieneSubstring(const char* cadena, const char* subcadena) {
    if (cadena == nullptr || subcadena == nullptr)
        return false;

    char* copyCadena = new char[strlen(cadena) + 1];
    strcpy(copyCadena, cadena);
    convertirAMinusculas(copyCadena);

    char* copySubcadena = new char[strlen(subcadena) + 1];
    strcpy(copySubcadena, subcadena);
    convertirAMinusculas(copySubcadena);

    bool result = (strstr(copyCadena, copySubcadena) != nullptr);

    delete[] copyCadena;
    delete[] copySubcadena;

    return result;
}

// lee un id y valida que sea un numero y mayor o igual a 0
// si el usuario ingresa 'q', retorna -1
int leerId(const char* msg) {
    while (true) {
        cout << msg << " (q para salir): ";
        string input;
        cin >> input;

        if (input == "q" || input == "Q") {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return -1;
        }

        try {
            int id = stoi(input);
            cout << std::format("id: {}", id) << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (id <= 0) {
                cout << CLEAR_SCREEN << COLOR_RED
                     << "El id debe ser numérico y mayor a 0. Intente nuevamente." << COLOR_RESET
                     << endl;
                continue;
            }

            return id;
        } catch (const invalid_argument& e) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << CLEAR_SCREEN << COLOR_RED << "El id debe ser numérico." << COLOR_RESET << endl;
            cout << COLOR_YELLOW << "¿Habrás querido salir? Presiona 'q' para salir." << COLOR_RESET
                 << endl;
            continue;
        }
    }
}

void menuProductos(Tienda* tienda) {
    char opcion;
    do {
        cout << COLOR_CYAN << "\n=== Gestión de Productos ===" << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "1." << COLOR_RESET << " Crear Producto" << endl;
        cout << COLOR_YELLOW << "2." << COLOR_RESET << " Buscar Producto" << endl;
        cout << COLOR_YELLOW << "3." << COLOR_RESET << " Actualizar Producto" << endl;
        cout << COLOR_YELLOW << "4." << COLOR_RESET << " Actualizar Stock" << endl;
        cout << COLOR_YELLOW << "5." << COLOR_RESET << " Listar Productos" << endl;
        cout << COLOR_YELLOW << "6." << COLOR_RESET << " Eliminar Producto" << endl;
        cout << COLOR_RED << "0." << COLOR_RESET << " Volver al Menú Principal" << endl;
        cout << "Seleccione una opción: ";
        cin >> opcion;

        switch (opcion) {
        case '1':
            crearProducto(tienda);
            break;
        case '2':
            buscarProducto(tienda);
            break;
        case '3':
            actualizarProducto(tienda);
            break;
        case '4':
            actualizarStockProducto(tienda);
            break;
        case '5':
            listarProductos(tienda);
            break;
        case '6':
            eliminarProducto(tienda);
            break;
        case '0':
            cout << COLOR_GREEN << "Volviendo..." << COLOR_RESET << endl;
            break;
        default:
            cout << CLEAR_SCREEN << COLOR_RED << "Opción inválida." << COLOR_RESET << endl;
        }
    } while (opcion != '0');
}

void menuProveedores(Tienda* tienda) {
    char opcion;
    do {
        cout << COLOR_CYAN << "\n=== Gestión de Proveedores ===" << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "1." << COLOR_RESET << " Crear Proveedor" << endl;
        cout << COLOR_YELLOW << "2." << COLOR_RESET << " Buscar Proveedor" << endl;
        cout << COLOR_YELLOW << "3." << COLOR_RESET << " Actualizar Proveedor" << endl;
        cout << COLOR_YELLOW << "4." << COLOR_RESET << " Listar Proveedores" << endl;
        cout << COLOR_YELLOW << "5." << COLOR_RESET << " Eliminar Proveedor" << endl;
        cout << COLOR_RED << "0." << COLOR_RESET << " Volver al Menú Principal" << endl;
        cout << "Seleccione una opción: ";
        cin >> opcion;

        switch (opcion) {
        case '1':
            crearProveedor(tienda);
            break;
        case '2':
            buscarProveedor(tienda);
            break;
        case '3':
            actualizarProveedor(tienda);
            break;
        case '4':
            listarProveedores(tienda);
            break;
        case '5':
            eliminarProveedor(tienda);
            break;
        case '0':
            cout << COLOR_GREEN << "Volviendo..." << COLOR_RESET << endl;
            break;
        default:
            cout << CLEAR_SCREEN << COLOR_RED << "Opción inválida." << COLOR_RESET << endl;
        }
    } while (opcion != '0');
}

void menuClientes(Tienda* tienda) {
    char opcion;
    do {
        cout << COLOR_CYAN << "\n=== Gestión de Clientes ===" << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "1." << COLOR_RESET << " Crear Cliente" << endl;
        cout << COLOR_YELLOW << "2." << COLOR_RESET << " Buscar Cliente" << endl;
        cout << COLOR_YELLOW << "3." << COLOR_RESET << " Actualizar Cliente" << endl;
        cout << COLOR_YELLOW << "4." << COLOR_RESET << " Listar Clientes" << endl;
        cout << COLOR_YELLOW << "5." << COLOR_RESET << " Eliminar Cliente" << endl;
        cout << COLOR_RED << "0." << COLOR_RESET << " Volver al Menú Principal" << endl;
        cout << "Seleccione una opción: ";
        cin >> opcion;

        switch (opcion) {
        case '1':
            crearCliente(tienda);
            break;
        case '2':
            buscarCliente(tienda);
            break;
        case '3':
            actualizarCliente(tienda);
            break;
        case '4':
            listarClientes(tienda);
            break;
        case '5':
            eliminarCliente(tienda);
            break;
        case '0':
            cout << COLOR_GREEN << "Volviendo..." << COLOR_RESET << endl;
            break;
        default:
            cout << CLEAR_SCREEN << COLOR_RED << "Opción inválida." << COLOR_RESET << endl;
        }
    } while (opcion != '0');
}

void menuTransacciones(Tienda* tienda) {
    char opcion;
    do {
        cout << COLOR_CYAN << "\n=== Gestión de Transacciones ===" << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "1." << COLOR_RESET << " Registrar Compra" << endl;
        cout << COLOR_YELLOW << "2." << COLOR_RESET << " Registrar Venta" << endl;
        cout << COLOR_YELLOW << "3." << COLOR_RESET << " Buscar Transacciones" << endl;
        cout << COLOR_YELLOW << "4." << COLOR_RESET << " Listar Transacciones" << endl;
        cout << COLOR_YELLOW << "5." << COLOR_RESET << " Cancelar Transacción" << endl;
        cout << COLOR_RED << "0." << COLOR_RESET << " Volver al Menú Principal" << endl;
        cout << "Seleccione una opción: ";
        cin >> opcion;

        switch (opcion) {
        case '1':
            registrarCompra(tienda);
            break;
        case '2':
            registrarVenta(tienda);
            break;
        case '3':
            buscarTransacciones(tienda);
            break;
        case '4':
            listarTransacciones(tienda);
            break;
        case '5':
            cancelarTransaccion(tienda);
            break;
        case '0':
            cout << COLOR_GREEN << "Volviendo..." << COLOR_RESET << endl;
            break;
        default:
            cout << CLEAR_SCREEN << COLOR_RED << "Opción inválida." << COLOR_RESET << endl;
        }
    } while (opcion != '0');
}

int main() {
    Tienda tienda;
    inicializarTienda(&tienda, "Papaya Store", "J-123456789");

    char opcion;
    do {
        cout << COLOR_CYAN << "\n=== PAPAYA STORE - Menú Principal ===" << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "1." << COLOR_RESET << " Gestión de Productos" << endl;
        cout << COLOR_YELLOW << "2." << COLOR_RESET << " Gestión de Proveedores" << endl;
        cout << COLOR_YELLOW << "3." << COLOR_RESET << " Gestión de Clientes" << endl;
        cout << COLOR_YELLOW << "4." << COLOR_RESET << " Gestión de Transacciones" << endl;
        cout << COLOR_RED << "0." << COLOR_RESET << " Salir" << endl;
        cout << "Seleccione una opción: ";
        if (!(cin >> opcion))
            break;

        switch (opcion) {
        case '1':
            menuProductos(&tienda);
            break;
        case '2':
            menuProveedores(&tienda);
            break;
        case '3':
            menuClientes(&tienda);
            break;
        case '4':
            menuTransacciones(&tienda);
            break;
        case '0':
            cout << COLOR_GREEN << "Saliendo..." << COLOR_RESET << endl;
            break;
        default:
            cout << CLEAR_SCREEN << COLOR_RED << "Opcion no valida" << COLOR_RESET << endl;
        }
    } while (opcion != '0');

    liberarTienda(&tienda);
    return 0;
}
