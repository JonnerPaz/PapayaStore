#include <chrono>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

using namespace std;
namespace fs = std::filesystem;

// ANSI Escape Codes
const char* CLEAR_SCREEN = "\033[2J\033[H";
const char* COLOR_RESET = "\033[0m";
const char* COLOR_CYAN = "\033[36m";
const char* COLOR_GREEN = "\033[32m";
const char* COLOR_YELLOW = "\033[33m";
const char* COLOR_RED = "\033[31m";
const char* COLOR_MAGENTA = "\033[35m";

// File paths
const char* PRODUCTOS_PATH = "./data/productos.bin";
const char* PROVEEDORES_PATH = "./data/proveedores.bin";
const char* CLIENTES_PATH = "./data/clientes.bin";
const char* TRANSACCIONES_PATH = "./data/transacciones.bin";
const char* TIENDA_PATH = "./data/tienda.bin";
const char* BACKUP_PATH = "./data/backup.bin";

struct Tienda;
template <typename T> int buscarEntidadPorId(const char* path, int id);
int leerId(const char* msg);
void obtenerFechaActual(char* fecha);
int* buscarProductosPorNombre(Tienda* tienda, const char* nombre);
void listarProductos(Tienda* tienda);
void convertirAMinusculas(char* cadena);
bool contieneSubstring(const char* cadena, const char* subcadena);
template <typename T, size_t N>
bool existeStringDuplicado(const char* path, const char* valorBusqueda, char (T::*miembro)[N]);
bool existeProveedor(Tienda* tienda, int id);
bool existeCliente(Tienda* tienda, int id);
bool codigoDuplicado(Tienda* tienda, const char* codigo);
bool rifDuplicado(Tienda* tienda, const char* rif);
bool validarEmail(const char* email);

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

struct ArchivoHeader {
    int cantidadRegistros; // Total histórico de registros
    int proximoID;         // Siguiente ID a asignar (Autoincremental)
    int registrosActivos;  // Registros que no están marcados como eliminados
    int version;           // Control de versión del archivo;
};

struct Producto {
    int id;                 // Identificador único (autoincremental)
    char nombre[100];       // Nombre del producto
    char codigo[20];        // Código del producto (ej: "PROD-001")
    char descripcion[200];  // Descripción del producto
    char fechaRegistro[11]; // Formato: YYYY-MM-DD
    float precio;           // Precio unitario
    int stock;              // Cantidad en inventario

    // llaves foraneas
    int idProveedor; // ID del proveedor asociado

    // estadisticas
    int stockMinimo;
    int totalVendido;

    // metadata
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Proveedor {
    int id;                 // Identificador único (autoincremental)
    char nombre[100];       // Nombre del proveedor
    char rif[20];           // RIF o identificación fiscal
    char telefono[20];      // Teléfono de contacto
    char email[100];        // Correo electrónico
    char direccion[200];    // Dirección física
    char fechaRegistro[11]; // Formato: YYYY-MM-DD

    int productosIds[100];
    int cantidadProductos;

    // metadata
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Cliente {
    int id;                 // Identificador único (autoincremental)
    char nombre[100];       // Nombre completo del cliente
    char cedula[20];        // Cédula o RIF
    char telefono[20];      // Teléfono de contacto
    char email[100];        // Correo electrónico
    char direccion[200];    // Dirección física
    char fechaRegistro[11]; // Formato: YYYY-MM-DD

    float totalCompras;

    int transaccionesIds[100];
    int cantidadTransacciones;
    // metadata
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Transaccion {
    int id;                 // Identificador único (autoincremental)
    TipoDeTransaccion tipo; // COMPRA o VENTA
    int idRelacionado;      // ID del proveedor (compra) o cliente (venta)
    float total;            // cantidad * precioUnitario
    char fecha[11];         // Formato: YYYY-MM-DD
    char descripcion[200];  // Notas adicionales (opcional)

    // Hasta 100 productos por transacción
    int productosIds[100];
    int cantidades[100];          // Cantidades de cada producto
    float preciosUnitarios[100];  // Precios unitarios de cada producto
    int cantidadTiposDeProductos; // Cuantos elementos de los arreys anteriores se están usando

    // metadata
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Tienda {
    char nombre[100]; // Nombre de la tienda
    char rif[20];     // RIF de la tienda
    float totalTransacciones;
};

enum TipoPropiedadLista { PropiedadId, PropiedadNombre, PropiedadAmbos };

template <typename T>
void mostrarListaEntidades(const char* titulo, const char* path,
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

///// FUNCIONES CRUD DEL PROGRAMA
bool inicializarArchivo(const char* path) {
    if (fs::exists(path)) {
        cout << format("El archivo {} ya existe.", path) << endl;
        return true;
    }

    ofstream archivo(path, ios::binary | ios::app);
    ArchivoHeader header = {
        .cantidadRegistros = 0, .proximoID = 1, .registrosActivos = 0, .version = 1};
    archivo.write(reinterpret_cast<const char*>(&header), sizeof(ArchivoHeader));
    archivo.close();

    return fs::exists(path);
}

ArchivoHeader leerHeader(const char* path) {
    try {
        ArchivoHeader header;
        ifstream archivo(path, ios::binary);
        archivo.seekg(0);
        archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
        return header;
    } catch (const std::exception& e) {
        cerr << "Error al leer el archivo " << path << ": " << e.what() << endl;
        return ArchivoHeader();
    }
}

bool actualizarHeader(const char* path, ArchivoHeader header) {
    try {
        ofstream archivo(path, ios::binary);
        archivo.seekp(0);
        archivo.write(reinterpret_cast<const char*>(&header), sizeof(ArchivoHeader));
        return true;
    } catch (const std::exception& e) {
        cerr << "Error al actualizar el archivo: " << e.what() << endl;
        return false;
    }
}

void inicializarTienda(const char* nombre, const char* rif) {
    Tienda tienda = {};
    strncpy(tienda.nombre, nombre, sizeof(tienda.nombre) - 1);
    tienda.nombre[sizeof(tienda.nombre) - 1] = '\0';

    strncpy(tienda.rif, rif, sizeof(tienda.rif) - 1);
    tienda.rif[sizeof(tienda.rif) - 1] = '\0';

    string paths[5] = {PRODUCTOS_PATH, PROVEEDORES_PATH, CLIENTES_PATH, TRANSACCIONES_PATH,
                       TIENDA_PATH};

    // Inicializar todos los archivos tan pronto como inicie el programa
    for (string path : paths) {
        inicializarArchivo(path.c_str());
    }
}

void crearProducto(Tienda* tienda) {
    ifstream archivo(PRODUCTOS_PATH, ios::binary);
    if (!archivo) {
        cout << format("{} {} Error: No se pudo abrir el archivo de productos. {}", CLEAR_SCREEN,
                       COLOR_RED, COLOR_RESET);
        return;
    }

    ArchivoHeader productosHeader = leerHeader(PRODUCTOS_PATH);
    ArchivoHeader proveedoresHeader = leerHeader(PROVEEDORES_PATH);

    if (proveedoresHeader.registrosActivos == 0) {
        cout << format(
                    "{} {} Error: No hay proveedores registrados. Debe crear al menos un proveedor "
                    "antes de registrar un producto. {}",
                    CLEAR_SCREEN, COLOR_RED, COLOR_RESET)
             << endl;
        return;
    }

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
    mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH);

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

        mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH);
    }

    cout << "Los datos del producto son: " << endl;
    cout << format("Nombre: {}", nombre) << endl;
    cout << format("Codigo: {}", codigo) << endl;
    cout << format("Descripcion: {}", descripcion) << endl;
    cout << format("Precio: {}", precio) << endl;
    cout << format("Stock: {}", stock) << endl;
    cout << format("Proveedor: {}", idProveedor) << endl;

    cout << COLOR_YELLOW << "Está seguro de crear el producto? (s/n): " << COLOR_RESET;
    cin >> confirmar;

    if (confirmar == 's' || confirmar == 'S') {
        Producto producto = {};
        producto.id = productosHeader.proximoID;
        producto.idProveedor = idProveedor;
        copiarString(producto.nombre, nombre);
        copiarString(producto.codigo, codigo);
        copiarString(producto.descripcion, descripcion);
        producto.precio = precio;
        producto.stock = stock;
        producto.eliminado = false;
        auto now = chrono::system_clock::now();
        producto.fechaCreacion = chrono::system_clock::to_time_t(now);
        producto.fechaUltimaModificacion = producto.fechaCreacion;
        obtenerFechaActual(producto.fechaRegistro);

        // actualizar el header
        productosHeader.cantidadRegistros++;
        productosHeader.registrosActivos++;
        productosHeader.proximoID++;

        ofstream archivoOut(PRODUCTOS_PATH, ios::binary | ios::app);
        archivoOut.write(reinterpret_cast<const char*>(&producto), sizeof(Producto));
        archivoOut.close();

        actualizarHeader(PRODUCTOS_PATH, productosHeader);

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
            mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PropiedadId);
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
            mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PropiedadNombre);
            cout << "Ingrese el nombre del producto: ";
            char nombre[100];
            if (cin.peek() == '\n')
                cin.ignore();
            cin.getline(nombre, 100);

            // array de indices del producto. Asumimos que index[0] es la cantidad de
            // encontrados y a partir de index[1] estan los indices reales en el array de la
            // tienda
            int* index = buscarProductosPorNombre(tienda, nombre);
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

    mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PropiedadAmbos);

    int id = leerId("Ingresa el id del producto a actualizar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Actualización cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Producto a actualizar no encontrado." << COLOR_RESET
             << endl;
        return;
    }

    Producto producto;
    fstream archivo(PRODUCTOS_PATH, ios::binary | ios::in | ios::out);
    archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Producto), ios::beg);
    archivo.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

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
            mostrarListaEntidades<Proveedor>("Proveedores Disponibles", PROVEEDORES_PATH,
                                             PropiedadAmbos);
            int idNuevoProveedor = leerId("Ingrese el ID del nuevo proveedor (q para cancelar)");
            if (idNuevoProveedor <= 0) {
                cout << "Actualización cancelada." << endl;
            } else {
                int provIndex = buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, idNuevoProveedor);
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

        if (opcion >= '1' && opcion <= '6') {
            producto.fechaUltimaModificacion = time(nullptr);
            archivo.seekp(sizeof(ArchivoHeader) + index * sizeof(Producto), ios::beg);
            archivo.write(reinterpret_cast<const char*>(&producto), sizeof(Producto));
        }

    } while (opcion != ActualizarCancelada);
}

void actualizarStockProducto(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PropiedadAmbos);

    int id = leerId("Ingrese el id del producto para actualizar el stock");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Producto no encontrado." << COLOR_RESET << endl;
        return;
    }

    Producto producto;
    fstream archivo(PRODUCTOS_PATH, ios::binary | ios::in | ios::out);
    archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Producto), ios::beg);
    archivo.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

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
                producto.fechaUltimaModificacion = time(nullptr);
                archivo.seekp(sizeof(ArchivoHeader) + index * sizeof(Producto), ios::beg);
                archivo.write(reinterpret_cast<const char*>(&producto), sizeof(Producto));
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
    if (tienda == nullptr)
        return;

    ifstream archivo(PRODUCTOS_PATH, ios::binary);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo de productos." << endl;
        return;
    }
    ArchivoHeader header = leerHeader(PRODUCTOS_PATH);
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader)); // Saltar header

    if (header.registrosActivos == 0) {
        cout << "No hay productos registrados." << endl;
        return;
    }

    cout << "--- Lista de Productos (" << header.registrosActivos << ") ---" << endl;
    cout << format("{:<5} | {:<20} | {:<15} | {:<10} | {:<5}", "ID", "Nombre", "Codigo", "Precio",
                   "Stock")
         << endl;
    cout << "----------------------------------------------------------------------" << endl;

    Producto producto;
    while (archivo.read(reinterpret_cast<char*>(&producto), sizeof(Producto))) {
        if (!producto.eliminado) {
            cout << format("{:<5} | {:<20} | {:<15} | ${:<9.2f} | {:<5}", producto.id,
                           producto.nombre, producto.codigo, producto.precio, producto.stock)
                 << endl;
        }
    }
    cout << "----------------------------------------------------------------------" << endl;
    archivo.close();
}

void eliminarProducto(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PropiedadAmbos);

    int id = leerId("Ingresa el id del producto a eliminar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Eliminación cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Producto no encontrado." << COLOR_RESET << endl;
        return;
    }

    Producto producto;
    fstream archivo(PRODUCTOS_PATH, ios::binary | ios::in | ios::out);
    archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Producto), ios::beg);
    archivo.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

    cout << "Datos del producto a eliminar:" << endl;
    cout << "Id: " << producto.id << endl;
    cout << "Nombre: " << producto.nombre << endl;
    cout << "Codigo: " << producto.codigo << endl;
    cout << "Stock actual: " << producto.stock << endl;

    bool tieneTransacciones = false;
    // TODO: Comprobar transacciones usando archivos

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
        producto.eliminado = true;
        producto.fechaUltimaModificacion = time(nullptr);
        archivo.seekp(sizeof(ArchivoHeader) + index * sizeof(Producto), ios::beg);
        archivo.write(reinterpret_cast<char*>(&producto), sizeof(Producto));

        ArchivoHeader header = leerHeader(PRODUCTOS_PATH);
        header.registrosActivos--;
        actualizarHeader(PRODUCTOS_PATH, header);

        cout << "Producto eliminado exitosamente." << endl;
    } else {
        cout << "Eliminación cancelada." << endl;
    }
}

void crearProveedor(Tienda* tienda) {
    fstream archivo;
    try {
        ifstream archivo(PROVEEDORES_PATH, ios::binary);
    } catch (const std::exception& e) {
        ofstream archivo(PROVEEDORES_PATH, ios::binary);
    }

    ArchivoHeader proveedoresHeader = leerHeader(PROVEEDORES_PATH);

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
        Proveedor prov = {};
        prov.id = proveedoresHeader.proximoID;
        copiarString(prov.nombre, nombre);
        copiarString(prov.rif, rif);
        copiarString(prov.telefono, telefono);
        copiarString(prov.email, email);
        copiarString(prov.direccion, direccion);
        prov.eliminado = false;
        prov.fechaCreacion = time(nullptr);
        prov.fechaUltimaModificacion = prov.fechaCreacion;
        obtenerFechaActual(prov.fechaRegistro);

        proveedoresHeader.cantidadRegistros++;
        proveedoresHeader.registrosActivos++;
        proveedoresHeader.proximoID++;

        ofstream archivoOut(PROVEEDORES_PATH, ios::binary | ios::app);
        archivoOut.write(reinterpret_cast<const char*>(&prov), sizeof(Proveedor));
        archivoOut.close();

        actualizarHeader(PROVEEDORES_PATH, proveedoresHeader);

        cout << "Proveedor creado con exito." << endl;
    } else {
        cout << "Proveedor no creado." << endl;
    }
}

void buscarProveedor(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH, PropiedadId);

    int id = leerId("Ingrese el id del proveedor a buscar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Búsqueda cancelada." << COLOR_RESET << endl;
        return;
    }
    int index = buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Proveedor no encontrado." << COLOR_RESET << endl;
        return;
    }

    Proveedor p;
    ifstream archivo(PROVEEDORES_PATH, ios::binary);
    archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Proveedor), ios::beg);
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));

    cout << "Proveedor encontrado:" << endl;
    cout << "Id: " << p.id << "\nNombre: " << p.nombre << "\nRIF: " << p.rif
         << "\nTelefono: " << p.telefono << "\nEmail: " << p.email << "\nDireccion: " << p.direccion
         << "\nFecha de Registro: " << p.fechaRegistro << endl;
}

void actualizarProveedor(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH, PropiedadAmbos);

    int id = leerId("Ingrese el id del proveedor a actualizar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Actualización cancelada." << COLOR_RESET << endl;
        return;
    }
    int index = buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Proveedor no encontrado." << COLOR_RESET << endl;
        return;
    }

    Proveedor p;
    fstream archivo(PROVEEDORES_PATH, ios::binary | ios::in | ios::out);
    archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Proveedor), ios::beg);
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));

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

        if (opcion >= '1' && opcion <= '5') {
            p.fechaUltimaModificacion = time(nullptr);
            archivo.seekp(sizeof(ArchivoHeader) + index * sizeof(Proveedor), ios::beg);
            archivo.write(reinterpret_cast<const char*>(&p), sizeof(Proveedor));
        }

    } while (opcion != '0');
}

void listarProveedores(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    ifstream archivo(PROVEEDORES_PATH, ios::binary);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo de proveedores." << endl;
        return;
    }
    ArchivoHeader header = leerHeader(PROVEEDORES_PATH);
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    if (header.registrosActivos == 0) {
        cout << "No hay proveedores registrados." << endl;
        return;
    }

    cout << "--- Lista de Proveedores (" << header.registrosActivos << ") ---" << endl;
    cout << format("{:<5} | {:<20} | {:<15} | {:<15} | {:<25} | {:<15}", "ID", "Nombre", "RIF",
                   "Telefono", "Email", "Direccion")
         << endl;
    cout << "----------------------------------------------------------------------------------"
            "----"
            "-----------------"
         << endl;

    Proveedor proveedor;
    while (archivo.read(reinterpret_cast<char*>(&proveedor), sizeof(Proveedor))) {
        if (!proveedor.eliminado) {
            cout << format("{:<5} | {:<20} | {:<15} | {:<15} | {:<25} | {:<15}", proveedor.id,
                           proveedor.nombre, proveedor.rif, proveedor.telefono, proveedor.email,
                           proveedor.direccion)
                 << endl;
        }
    }
    cout << "----------------------------------------------------------------------------------"
            "----"
            "-----------------"
         << endl;
}

void eliminarProveedor(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH, PropiedadAmbos);

    int id = leerId("Ingrese el id del proveedor a eliminar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Eliminación cancelada." << COLOR_RESET << endl;
        return;
    }
    int index = buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Proveedor no encontrado." << COLOR_RESET << endl;
        return;
    }

    bool tieneTransacciones = false;
    // TODO: Comprobar en archivo de transacciones

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
        Proveedor p;
        fstream archivo(PROVEEDORES_PATH, ios::binary | ios::in | ios::out);
        archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Proveedor), ios::beg);
        archivo.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));

        p.eliminado = true;
        p.fechaUltimaModificacion = time(nullptr);

        archivo.seekp(sizeof(ArchivoHeader) + index * sizeof(Proveedor), ios::beg);
        archivo.write(reinterpret_cast<const char*>(&p), sizeof(Proveedor));

        ArchivoHeader header = leerHeader(PROVEEDORES_PATH);
        header.registrosActivos--;
        actualizarHeader(PROVEEDORES_PATH, header);

        cout << "Proveedor eliminado exitosamente." << endl;
    } else {
        cout << "Eliminación cancelada." << endl;
    }
}

void crearCliente(Tienda* tienda) {
    fstream archivo;
    try {
        ifstream archivo(CLIENTES_PATH, ios::binary);
    } catch (const std::exception& e) {
        ofstream archivo(CLIENTES_PATH, ios::binary);
    }

    ArchivoHeader clientesHeader = leerHeader(CLIENTES_PATH);

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
        Cliente cliente = {};
        cliente.id = clientesHeader.proximoID;
        copiarString(cliente.nombre, nombre);
        copiarString(cliente.cedula, cedula);
        copiarString(cliente.telefono, telefono);
        copiarString(cliente.email, email);
        copiarString(cliente.direccion, direccion);
        cliente.eliminado = false;
        cliente.fechaCreacion = time(nullptr);
        cliente.fechaUltimaModificacion = cliente.fechaCreacion;
        obtenerFechaActual(cliente.fechaRegistro);

        clientesHeader.cantidadRegistros++;
        clientesHeader.registrosActivos++;
        clientesHeader.proximoID++;

        ofstream archivoOut(CLIENTES_PATH, ios::binary | ios::app);
        archivoOut.write(reinterpret_cast<const char*>(&cliente), sizeof(Cliente));
        archivoOut.close();

        actualizarHeader(CLIENTES_PATH, clientesHeader);

        cout << "Cliente creado con exito." << endl;
    } else {
        cout << "Cliente no creado." << endl;
    }
}

void buscarCliente(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades<Cliente>("Clientes", CLIENTES_PATH, PropiedadId);

    int id = leerId("Ingrese el id del cliente a buscar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Búsqueda cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId<Cliente>(CLIENTES_PATH, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Cliente no encontrado." << COLOR_RESET << endl;
        return;
    }

    Cliente c;
    ifstream archivo(CLIENTES_PATH, ios::binary);
    archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Cliente), ios::beg);
    archivo.read(reinterpret_cast<char*>(&c), sizeof(Cliente));

    cout << "Cliente encontrado:" << endl;
    cout << "Id: " << c.id << "\nNombre: " << c.nombre << "\nCedula: " << c.cedula
         << "\nTelefono: " << c.telefono << "\nEmail: " << c.email << "\nDireccion: " << c.direccion
         << "\nFecha de Registro: " << c.fechaRegistro << endl;
}

void actualizarCliente(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades<Cliente>("Clientes", CLIENTES_PATH, PropiedadAmbos);

    int id = leerId("Ingrese el id del cliente a actualizar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Actualización cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId<Cliente>(CLIENTES_PATH, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Cliente no encontrado." << COLOR_RESET << endl;
        return;
    }

    Cliente clientes;
    fstream archivo(CLIENTES_PATH, ios::binary | ios::in | ios::out);
    archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Cliente), ios::beg);
    archivo.read(reinterpret_cast<char*>(&clientes), sizeof(Cliente));

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

        if (opcion >= '1' && opcion <= '5') {
            clientes.fechaUltimaModificacion = time(nullptr);
            archivo.seekp(sizeof(ArchivoHeader) + index * sizeof(Cliente), ios::beg);
            archivo.write(reinterpret_cast<const char*>(&clientes), sizeof(Cliente));
        }

    } while (opcion != '0');
}

void listarClientes(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    ifstream archivo(CLIENTES_PATH, ios::binary);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo de clientes." << endl;
        return;
    }
    ArchivoHeader header = leerHeader(CLIENTES_PATH);
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    if (header.registrosActivos == 0) {
        cout << "No hay clientes registrados." << endl;
        return;
    }

    cout << "--- Lista de Clientes (" << header.registrosActivos << ") ---" << endl;
    cout << format("{:<5} | {:<20} | {:<15} | {:<15} | {:<25}", "ID", "Nombre", "Cedula",
                   "Telefono", "Email")
         << endl;
    cout << "----------------------------------------------------------------------------------"
            "----"
            "--"
         << endl;

    Cliente cliente;
    while (archivo.read(reinterpret_cast<char*>(&cliente), sizeof(Cliente))) {
        if (!cliente.eliminado) {
            cout << format("{:<5} | {:<20} | {:<15} | {:<15} | {:<25}", cliente.id, cliente.nombre,
                           cliente.cedula, cliente.telefono, cliente.email)
                 << endl;
        }
    }
    cout << "----------------------------------------------------------------------------------"
            "----"
            "--"
         << endl;
}

void eliminarCliente(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    mostrarListaEntidades<Cliente>("Clientes", CLIENTES_PATH, PropiedadAmbos);

    int id = leerId("Ingrese el id del cliente a eliminar");
    if (id <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Eliminación cancelada." << COLOR_RESET << endl;
        return;
    }

    int index = buscarEntidadPorId<Cliente>(CLIENTES_PATH, id);
    if (index == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Cliente no encontrado." << COLOR_RESET << endl;
        return;
    }

    bool tieneTransacciones = false;
    // TODO: Comprobar transacciones

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
        Cliente clientes;
        fstream archivo(CLIENTES_PATH, ios::binary | ios::in | ios::out);
        archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Cliente), ios::beg);
        archivo.read(reinterpret_cast<char*>(&clientes), sizeof(Cliente));

        clientes.eliminado = true;
        clientes.fechaUltimaModificacion = time(nullptr);

        archivo.seekp(sizeof(ArchivoHeader) + index * sizeof(Cliente), ios::beg);
        archivo.write(reinterpret_cast<const char*>(&clientes), sizeof(Cliente));

        ArchivoHeader header = leerHeader(CLIENTES_PATH);
        header.registrosActivos--;
        actualizarHeader(CLIENTES_PATH, header);

        cout << "Cliente eliminado exitosamente." << endl;
    } else {
        cout << "Eliminación cancelada." << endl;
    }
}

void registrarCompra(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    cout << "\n===REGISTRAR COMPRA (Entrada de Mercancia)===" << endl;

    mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PropiedadAmbos);

    // buscar el producto
    int idProd = leerId("Ingrese ID del Producto: ");
    if (idProd <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }
    int idxProd = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, idProd);

    if (idxProd == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: El producto con ID " << idProd << " no existe."
             << COLOR_RESET << endl;
        return;
    }

    Producto producto;
    fstream archivoProd(PRODUCTOS_PATH, ios::binary | ios::in | ios::out);
    archivoProd.seekg(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
    archivoProd.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

    mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH, PropiedadAmbos);

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

    fstream archivoTrans;
    try {
        ifstream check(TRANSACCIONES_PATH, ios::binary);
    } catch (...) {
        ofstream create(TRANSACCIONES_PATH, ios::binary);
    }
    ArchivoHeader transHeader = leerHeader(TRANSACCIONES_PATH);

    Transaccion transaccion = {};
    transaccion.id = transHeader.proximoID;
    transaccion.tipo = COMPRA;
    transaccion.idRelacionado = idProv;
    transaccion.cantidadTiposDeProductos = 1;
    transaccion.productosIds[0] = idProd;
    transaccion.cantidades[0] = cantidad;
    transaccion.preciosUnitarios[0] = producto.precio;
    transaccion.total = cantidad * producto.precio;
    transaccion.eliminado = false;
    transaccion.fechaCreacion = time(nullptr);
    transaccion.fechaUltimaModificacion = transaccion.fechaCreacion;
    obtenerFechaActual(transaccion.fecha);

    producto.stock += cantidad;
    producto.fechaUltimaModificacion = time(nullptr);

    archivoProd.seekp(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
    archivoProd.write(reinterpret_cast<const char*>(&producto), sizeof(Producto));

    transHeader.cantidadRegistros++;
    transHeader.registrosActivos++;
    transHeader.proximoID++;

    ofstream archivoOut(TRANSACCIONES_PATH, ios::binary | ios::app);
    archivoOut.write(reinterpret_cast<const char*>(&transaccion), sizeof(Transaccion));
    archivoOut.close();

    actualizarHeader(TRANSACCIONES_PATH, transHeader);

    cout << "Compra registrada ID: " << transaccion.id << endl;
    cout << "Stock actualizado de " << producto.nombre << ": " << producto.stock << endl;
}

void registrarVenta(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    cout << "\n===REGISTRAR VENTA (Salida de Mercancia)===" << endl;

    mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PropiedadAmbos);

    // buscar producto
    int idProd = leerId("Ingrese ID del Producto: ");
    if (idProd <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }
    int idxProd = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, idProd);

    if (idxProd == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: El producto no existe." << COLOR_RESET << endl;
        return;
    }

    Producto producto;
    fstream archivoProd(PRODUCTOS_PATH, ios::binary | ios::in | ios::out);
    archivoProd.seekg(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
    archivoProd.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

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
    mostrarListaEntidades<Cliente>("Clientes", CLIENTES_PATH, PropiedadAmbos);

    int idCli = leerId("Ingrese ID del Cliente: ");
    if (idCli <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }
    if (!existeCliente(tienda, idCli)) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: El cliente no existe." << COLOR_RESET << endl;
        return;
    }

    fstream archivoTrans;
    try {
        ifstream check(TRANSACCIONES_PATH, ios::binary);
    } catch (...) {
        ofstream create(TRANSACCIONES_PATH, ios::binary);
    }
    ArchivoHeader transHeader = leerHeader(TRANSACCIONES_PATH);

    Transaccion transaccion = {};
    transaccion.id = transHeader.proximoID;
    transaccion.tipo = VENTA;
    transaccion.idRelacionado = idCli;
    transaccion.cantidadTiposDeProductos = 1;
    transaccion.productosIds[0] = idProd;
    transaccion.cantidades[0] = cantidad;
    transaccion.preciosUnitarios[0] = producto.precio;
    transaccion.total = cantidad * producto.precio;
    transaccion.eliminado = false;
    transaccion.fechaCreacion = time(nullptr);
    transaccion.fechaUltimaModificacion = transaccion.fechaCreacion;
    obtenerFechaActual(transaccion.fecha);

    // restamos el espacio en el stock
    producto.stock -= cantidad;
    producto.fechaUltimaModificacion = time(nullptr);

    archivoProd.seekp(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
    archivoProd.write(reinterpret_cast<const char*>(&producto), sizeof(Producto));

    transHeader.cantidadRegistros++;
    transHeader.registrosActivos++;
    transHeader.proximoID++;

    ofstream archivoOut(TRANSACCIONES_PATH, ios::binary | ios::app);
    archivoOut.write(reinterpret_cast<const char*>(&transaccion), sizeof(Transaccion));
    archivoOut.close();

    actualizarHeader(TRANSACCIONES_PATH, transHeader);

    // confirmamos venta
    cout << "¡Venta exitosa! Total: " << transaccion.total
         << " | Stock restante: " << producto.stock << endl;
}

void buscarTransacciones(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    ArchivoHeader header = leerHeader(TRANSACCIONES_PATH);
    if (header.registrosActivos == 0) {
        cout << "\n[!] No hay transacciones registradas en el sistema." << endl;
        return;
    }

    int idBuscar = leerId("\nIngrese el ID de la transaccion a buscar: ");
    if (idBuscar <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Búsqueda cancelada." << COLOR_RESET << endl;
        return;
    }

    int idx = buscarEntidadPorId<Transaccion>(TRANSACCIONES_PATH, idBuscar);

    if (idx == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: La transaccion con ID " << idBuscar
             << " no existe." << COLOR_RESET << endl;
        return;
    }

    Transaccion transaccion;
    ifstream archivo(TRANSACCIONES_PATH, ios::binary);
    archivo.seekg(sizeof(ArchivoHeader) + idx * sizeof(Transaccion), ios::beg);
    archivo.read(reinterpret_cast<char*>(&transaccion), sizeof(Transaccion));

    const char* tipoStr = (transaccion.tipo == COMPRA) ? "COMPRA" : "VENTA";
    cout << "\n========================================" << endl;
    cout << "       DETALLE DE TRANSACCION #" << transaccion.id << endl;
    cout << "========================================" << endl;
    cout << "Tipo:       " << tipoStr << endl;
    cout << "Fecha:      " << transaccion.fecha << endl;
    cout << "ID Asociado: " << transaccion.idRelacionado << " (Cliente/Proveedor)" << endl;
    cout << "----------------------------------------" << endl;

    for (int i = 0; i < transaccion.cantidadTiposDeProductos; i++) {
        cout << "Producto ID: " << transaccion.productosIds[i] << endl;
        cout << "Cantidad:    " << transaccion.cantidades[i] << endl;
        cout << "Precio Un.:  $" << transaccion.preciosUnitarios[i] << endl;
        cout << "----------------------------------------" << endl;
    }
    cout << "TOTAL:       $" << transaccion.total << endl;
    cout << "========================================" << endl;
}

void listarTransacciones(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    ArchivoHeader header = leerHeader(TRANSACCIONES_PATH);
    if (header.registrosActivos == 0) {
        cout << "\n[!] El historial de transacciones esta vacio." << endl;
        return;
    }
    cout << "\n================================================================================"
            "===="
            "====="
         << endl;
    cout << "                         HISTORIAL COMPLETO DE MOVIMIENTOS" << endl;
    cout << "=================================================================================="
            "===="
            "==="
         << endl;
    cout << format("{:<5} | {:<8} | {:<12} | {:<10} | {:<10} | {:<12} | {:<12} | {:<15}", "ID",
                   "Tipo", "Fecha", "Prod ID", "Cantidad", "Precio Un.", "Total", "Asociado ID")
         << endl;
    cout << "----------------------------------------------------------------------------------"
            "----"
            "---"
         << endl;

    ifstream archivo(TRANSACCIONES_PATH, ios::binary);
    archivo.seekg(sizeof(ArchivoHeader), ios::beg);
    Transaccion t;
    int count = 0;
    while (archivo.read(reinterpret_cast<char*>(&t), sizeof(Transaccion))) {
        if (!t.eliminado) {
            const char* tipoStr = (t.tipo == COMPRA) ? "COMPRA" : "VENTA";

            // For listing, we show the first product
            int prodId = t.cantidadTiposDeProductos > 0 ? t.productosIds[0] : 0;
            int cant = t.cantidadTiposDeProductos > 0 ? t.cantidades[0] : 0;
            float precioUn = t.cantidadTiposDeProductos > 0 ? t.preciosUnitarios[0] : 0.0f;

            cout << format("{:<5} | {:<8} | {:<12} | {:<10} | {:<10} | ${:<11.2f} | ${:<11.2f} | "
                           "{:<15}",
                           t.id, tipoStr, t.fecha, prodId, cant, precioUn, t.total, t.idRelacionado)
                 << endl;
            count++;
        }
    }
    cout << "=================================================================================="
            "===="
            "==="
         << endl;
    cout << "Total de registros: " << count << endl;
}

void cancelarTransaccion(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    ArchivoHeader header = leerHeader(TRANSACCIONES_PATH);
    if (header.registrosActivos == 0) {
        cout << "\n[!] No hay transacciones para cancelar." << endl;
        return;
    }

    int idBuscar = leerId("\nIngrese el ID de la transaccion a CANCELAR: ");
    if (idBuscar <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }

    int idx = buscarEntidadPorId<Transaccion>(TRANSACCIONES_PATH, idBuscar);

    if (idx == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: Transaccion no encontrada." << COLOR_RESET
             << endl;
        return;
    }

    Transaccion transaccion;
    fstream archivoTrans(TRANSACCIONES_PATH, ios::binary | ios::in | ios::out);
    archivoTrans.seekg(sizeof(ArchivoHeader) + idx * sizeof(Transaccion), ios::beg);
    archivoTrans.read(reinterpret_cast<char*>(&transaccion), sizeof(Transaccion));

    if (transaccion.cantidadTiposDeProductos == 0) {
        cout << "Error: La transaccion no tiene productos asociados." << endl;
        return;
    }

    int idProducto = transaccion.productosIds[0];
    int cantidad = transaccion.cantidades[0];

    int idxProd = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, idProducto);

    if (idxProd == -1) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: El producto de esta transaccion ya no existe en el sistema." << COLOR_RESET
             << endl;
        return;
    }

    Producto producto;
    fstream archivoProd(PRODUCTOS_PATH, ios::binary | ios::in | ios::out);
    archivoProd.seekg(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
    archivoProd.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

    // REVERTIR EL STOCK (La parte lógica)
    if (transaccion.tipo == VENTA) {
        producto.stock += cantidad;
        cout << "[SISTEMA] Venta cancelada. Se devolvieron " << cantidad << " unidades al stock."
             << endl;
    } else if (transaccion.tipo == COMPRA) {
        if (producto.stock < cantidad) {
            cout << "Error: No se puede cancelar la compra. El stock actual es menor a lo que "
                    "quieres quitar."
                 << endl;
            return;
        }
        producto.stock -= cantidad;
        cout << "[SISTEMA] Compra cancelada. Se retiraron " << cantidad << " unidades del stock."
             << endl;
    }

    // guardar stock modificado
    producto.fechaUltimaModificacion = time(nullptr);
    archivoProd.seekp(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
    archivoProd.write(reinterpret_cast<const char*>(&producto), sizeof(Producto));

    // marcar transaccion eliminada
    transaccion.eliminado = true;
    transaccion.fechaUltimaModificacion = time(nullptr);
    archivoTrans.seekp(sizeof(ArchivoHeader) + idx * sizeof(Transaccion), ios::beg);
    archivoTrans.write(reinterpret_cast<const char*>(&transaccion), sizeof(Transaccion));

    header.registrosActivos--;
    actualizarHeader(TRANSACCIONES_PATH, header);

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
    if (tienda == nullptr) {
        return false;
    }
    return buscarEntidadPorId<Producto>(PRODUCTOS_PATH, id) != -1;
}

bool existeProveedor(Tienda* tienda, int id) {
    if (tienda == nullptr) {
        return false;
    }
    return buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, id) != -1;
}

bool existeCliente(Tienda* tienda, int id) {
    if (tienda == nullptr) {
        return false;
    }
    return buscarEntidadPorId<Cliente>(CLIENTES_PATH, id) != -1;
}

bool codigoDuplicado(Tienda* tienda, const char* codigo) {
    if (tienda == nullptr) {
        return false;
    }
    return existeStringDuplicado<Producto>(PRODUCTOS_PATH, codigo, &Producto::codigo);
}

bool rifDuplicado(Tienda* tienda, const char* rif) {
    if (tienda == nullptr) {
        return false;
    }
    return existeStringDuplicado<Proveedor>(PROVEEDORES_PATH, rif, &Proveedor::rif);
}

// Búsquedas
// =======================================================
//                 SECCIÓN DE TEMPLATES
// =======================================================
// Casi todas las entidades tienen un id (cliente, proveedor, producto)
template <typename T> int buscarEntidadPorId(const char* path, int id) {
    ifstream archivo(path, ios::binary);
    if (!archivo.is_open())
        return -1;

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    T entidad;
    int index = 0;
    while (archivo.read(reinterpret_cast<char*>(&entidad), sizeof(T))) {
        if (!entidad.eliminado && entidad.id == id) {
            return index;
        }
        index++;
    }
    return -1;
}

template <typename T>
void mostrarListaEntidades(const char* titulo, const char* path, TipoPropiedadLista tipoProp) {
    ifstream archivo(path, ios::binary);
    if (!archivo.is_open())
        return;

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    if (header.registrosActivos == 0) {
        cout << "\nNo hay " << titulo << " registrados." << endl;
        return;
    }

    cout << "\n--- " << titulo << " Disponibles ---" << endl;

    // Imprimir cabecera
    switch (tipoProp) {
    case PropiedadId:
        cout << format("{:<5}", "ID") << endl;
        break;
    case PropiedadNombre:
        cout << format("{:<20}", "Nombre") << endl;
        break;
    case PropiedadAmbos:
        cout << format("{:<5} | {:<20}", "ID", "Nombre") << endl;
        break;
    }
    cout << "-------------------------------" << endl;

    T entidad;
    while (archivo.read(reinterpret_cast<char*>(&entidad), sizeof(T))) {
        if (!entidad.eliminado) {
            switch (tipoProp) {
            case PropiedadId:
                cout << format("{:<5}", entidad.id) << endl;
                break;
            case PropiedadNombre:
                cout << format("{:<20}", entidad.nombre) << endl;
                break;
            case PropiedadAmbos:
                cout << format("{:<5} | {:<20}", entidad.id, entidad.nombre) << endl;
                break;
            }
        }
    }
    cout << "-------------------------------" << endl;
}

// para las entidades que revisan si si hay strngis duplicados(codigo,rif)
template <typename T, size_t N>
bool existeStringDuplicado(const char* path, const char* valorBusqueda, char (T::*miembro)[N]) {
    ifstream archivo(path, ios::binary);
    if (!archivo.is_open())
        return false;

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    T entidad;
    while (archivo.read(reinterpret_cast<char*>(&entidad), sizeof(T))) {
        if (!entidad.eliminado) {
            if (strcmp(entidad.*miembro, valorBusqueda) == 0) {
                return true;
            }
        }
    }
    return false;
}

// retorna array de indices del producto
int* buscarProductosPorNombre(Tienda* tienda, const char* nombre) {
    if (tienda == nullptr || nombre == nullptr)
        return nullptr;

    ifstream archivo(PRODUCTOS_PATH, ios::binary);
    if (!archivo.is_open())
        return nullptr;

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    int count = 0;
    Producto temp;
    while (archivo.read(reinterpret_cast<char*>(&temp), sizeof(Producto))) {
        if (!temp.eliminado && contieneSubstring(temp.nombre, nombre)) {
            count++;
        }
    }

    if (count == 0)
        return nullptr;

    int* result = new int[count + 1];
    result[0] = count; // el índice 0 guarda la cantidad de elementos

    archivo.clear();
    archivo.seekg(sizeof(ArchivoHeader), ios::beg);

    int pos = 1;
    int index = 0;
    while (archivo.read(reinterpret_cast<char*>(&temp), sizeof(Producto))) {
        if (!temp.eliminado && contieneSubstring(temp.nombre, nombre)) {
            result[pos++] = index;
        }
        index++;
    }

    return result;
}

int obtenerIndiceFisico(const char* path, int idSearch) {
    ifstream archivo(path, ios::binary);
    if (!archivo)
        return -1;

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    Producto temp;
    int indice = 0;
    while (archivo.read(reinterpret_cast<char*>(&temp), sizeof(Producto))) {
        if (!temp.eliminado && temp.id == idSearch) {
            return indice; // Retornamos la posición física (0, 1, 2...)
        }
        indice++;
    }
    return -1;
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
    inicializarTienda("Papaya Store", "J-123456789");

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

    return 0;
}
