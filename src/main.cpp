#include <cctype>
#include <chrono>
#include <clocale>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <format>
#include <fstream>
#include <iosfwd>
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
const fs::path PARENT_PATH = "./data/";
const fs::path PRODUCTOS_PATH = PARENT_PATH / "productos.bin";
const fs::path PROVEEDORES_PATH = PARENT_PATH / "proveedores.bin";
const fs::path CLIENTES_PATH = PARENT_PATH / "clientes.bin";
const fs::path TRANSACCIONES_PATH = PARENT_PATH / "transacciones.bin";
const fs::path TIENDA_PATH = PARENT_PATH / "tienda.bin";
const fs::path BACKUP_PATH = "./backup/";

struct Tienda;
template <typename T> int buscarEntidadPorId(fs::path path, int id);
template <typename T> void menuActualizar(T& entidad, fstream& archivo, int index);
int leerId(const char* msg);
void obtenerFechaActual(char* fecha);
int* buscarProductosPorNombre(const char* nombre);
void listarProductos();
void convertirAMinusculas(char* cadena);
bool contieneSubstring(const char* cadena, const char* subcadena);
template <typename T, size_t N>
bool existeStringDuplicado(fs::path path, const char* valorBusqueda, char (T::*miembro)[N]);
bool existeProveedor(int id);
bool existeCliente(int id);
bool codigoDuplicado(const char* codigo);
bool rifDuplicado(const char* rif);
bool validarEmail(const char* email);
bool nombreProductoDuplicado(const char* nombre);

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
    int idProveedor;

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
    int cantidadTiposDeProductos; // Cuantos elementos de los arrays anteriores se están usando

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

enum ListarPorPropiedad { PorId, PorNombre, PorAmbos };

template <typename T>
void mostrarListaEntidades(const char* titulo, fs::path path,
                           ListarPorPropiedad tipoProp = PorAmbos);

template <typename T>
// remove_reference_t remueve las referencias de T y devuelve T
// Ej. si T es int& -> devuelve int
concept AsignarNum = std::is_arithmetic_v<std::remove_reference_t<T>>;
void asignarPropiedadNum(const char* msg, AsignarNum auto& prop) {
    cout << COLOR_YELLOW << msg << COLOR_RESET;
    while (true) {
        cin >> prop;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << COLOR_RED << "El valor debe ser numérico. Intente nuevamente: " << COLOR_RESET
                 << endl;
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
    cout << COLOR_YELLOW << msg << COLOR_RESET;
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

template <typename T> void mostrarDetallesEntidad(const T& entidad) {
    if constexpr (std::is_same_v<T, Producto>) {
        cout << format("{}Nombre: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.nombre) << endl;
        cout << format("{}Codigo: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.codigo) << endl;
        cout << format("{}Descripcion: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.descripcion)
             << endl;
        cout << format("{}Precio: {}{:.2f}", COLOR_YELLOW, COLOR_RESET, entidad.precio) << endl;
        cout << format("{}Stock: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.stock) << endl;
        cout << format("{}ID de Proveedor asociado: {}{}", COLOR_YELLOW, COLOR_RESET,
                       entidad.idProveedor)
             << endl;
    } else if constexpr (std::is_same_v<T, Proveedor>) {
        cout << format("{}Nombre: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.nombre) << endl;
        cout << format("{}RIF: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.rif) << endl;
        cout << format("{}Teléfono: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.telefono) << endl;
        cout << format("{}Email: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.email) << endl;
        cout << format("{}Dirección: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.direccion) << endl;
    } else if constexpr (std::is_same_v<T, Cliente>) {
        cout << format("{}Nombre: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.nombre) << endl;
        cout << format("{}Cédula/RIF: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.cedula) << endl;
        cout << format("{}Teléfono: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.telefono) << endl;
        cout << format("{}Email: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.email) << endl;
        cout << format("{}Dirección: {}{}", COLOR_YELLOW, COLOR_RESET, entidad.direccion) << endl;
    }
}

///// FUNCIONES CRUD DEL PROGRAMA
bool inicializarArchivo(fs::path path) {
    if (path.has_parent_path() && !fs::exists(path.parent_path())) {
        fs::create_directories(path.parent_path());
    }

    if (fs::exists(path)) {
        cout << format("El archivo {} ya existe.", path.c_str()) << endl;
        return true;
    }

    ofstream file(path, ios::binary | ios::app);
    if (!file) {
        cout << "Error al crear el archivo " << path << endl;
        return false;
    }

    ArchivoHeader header = {
        .cantidadRegistros = 0, .proximoID = 1, .registrosActivos = 0, .version = 1};
    file.write(reinterpret_cast<const char*>(&header), sizeof(ArchivoHeader));
    file.close();

    return fs::exists(path);
}

ArchivoHeader leerHeader(fs::path path) {
    try {
        ArchivoHeader header;
        ifstream archivo(path, ios::binary | ios::in);
        if (!archivo.is_open()) {
            cerr << "Error al abrir el archivo " << path << endl;
            return ArchivoHeader();
        }
        // lee el header
        archivo.seekg(0);
        archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
        return header;
    } catch (const std::exception& error) {
        cerr << "Error al leer el archivo " << path << ": " << error.what() << endl;
        return ArchivoHeader();
    }
}

bool actualizarHeader(fs::path path, ArchivoHeader header) {
    try {
        fstream archivo(path, ios::binary | ios::in | ios::out);
        if (!archivo.is_open()) {
            cerr << "Error al abrir el archivo " << path << endl;
            return false;
        }
        archivo.seekp(0);
        // Reescribe todo el header
        archivo.write(reinterpret_cast<const char*>(&header), sizeof(ArchivoHeader));
        archivo.close();
        return true;
    } catch (const std::exception& e) {
        cerr << "Error al actualizar el archivo: " << e.what() << endl;
        return false;
    }
}

void inicializarTienda(const char* nombre, const char* rif) {
    Tienda tienda = {};
    copiarString(tienda.nombre, nombre);
    copiarString(tienda.rif, rif);

    string paths[5] = {PRODUCTOS_PATH, PROVEEDORES_PATH, CLIENTES_PATH, TRANSACCIONES_PATH,
                       TIENDA_PATH};

    // Inicializar todos los archivos tan pronto como inicie el programa
    for (string path : paths) {
        inicializarArchivo(path.c_str());
    }
}

void crearProducto() {
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
    while (true) {
        asignarPropiedadString("Ingrese el nombre del producto (q para cancelar): ", nombre);
        if (nombre[0] == 'q' && nombre[1] == '\0')
            return;

        if (nombreProductoDuplicado(nombre)) {
            cout << COLOR_RED << "Error: Ya existe un producto con ese nombre" << COLOR_RESET
                 << endl;
            continue;
        }
        break;
    }

    char codigo[20];
    while (true) {
        asignarPropiedadString("Ingrese el código del producto (q para cancelar): ", codigo);
        if (codigo[0] == 'q' && codigo[1] == '\0')
            return;

        if (codigo[0] == '\0') {
            cout << COLOR_RED << "Error: El código no puede estar vacío." << COLOR_RESET << endl;
            continue;
        }

        if (codigoDuplicado(codigo)) {
            cout << COLOR_RED << "Error: Ya existe un producto con el código " << codigo
                 << COLOR_RESET << endl;
            continue;
        }
        break;
    }

    char descripcion[200];
    asignarPropiedadString("Ingrese la descripcion del producto (q para cancelar): ", descripcion);
    if (descripcion[0] == 'q' && descripcion[1] == '\0')
        return;

    float precio;
    asignarPropiedadNum("Ingrese el precio del producto: ", precio);

    int stock;
    asignarPropiedadNum("Ingrese el stock del producto: ", stock);

    mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH);

    int idProveedor;
    while (true) {
        idProveedor = leerId("Ingresar el id del proveedor");
        if (idProveedor <= 0) {
            cout << CLEAR_SCREEN << COLOR_RED << "Creación de producto cancelada." << COLOR_RESET
                 << endl;
            return;
        }

        if (existeProveedor(idProveedor)) {
            break;
        }

        cout << CLEAR_SCREEN << COLOR_RED << "Error: El proveedor con ID " << idProveedor
             << " no existe. Intente nuevamente." << COLOR_RESET << endl;

        mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH);
    }

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

    mostrarDetallesEntidad(producto);

    cout << COLOR_YELLOW << "Está seguro de crear el producto? (s/n): " << COLOR_RESET;
    cin >> confirmar;

    if (tolower(confirmar) != 's') {
        cout << COLOR_RED << "Creación de producto cancelada." << COLOR_RESET << endl;
        return;
    }

    // actualizar el header
    productosHeader.cantidadRegistros++;
    productosHeader.registrosActivos++;
    productosHeader.proximoID++;
    ofstream archivoOut(PRODUCTOS_PATH, ios::binary | ios::app);
    archivoOut.write(reinterpret_cast<const char*>(&producto), sizeof(Producto));
    archivoOut.close();
    actualizarHeader(PRODUCTOS_PATH, productosHeader);

    // Actualizar el proveedor con el nuevo producto
    int provIndex = buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, idProveedor);
    if (provIndex != -1) {
        Proveedor prov;
        fstream provArchivo(PROVEEDORES_PATH, ios::binary | ios::in | ios::out);
        provArchivo.seekg(sizeof(ArchivoHeader) + provIndex * sizeof(Proveedor), ios::beg);
        provArchivo.read(reinterpret_cast<char*>(&prov), sizeof(Proveedor));

        if (prov.cantidadProductos < 100) {
            prov.productosIds[prov.cantidadProductos++] = producto.id;
            prov.fechaUltimaModificacion = time(nullptr);
            provArchivo.seekp(sizeof(ArchivoHeader) + provIndex * sizeof(Proveedor), ios::beg);
            provArchivo.write(reinterpret_cast<const char*>(&prov), sizeof(Proveedor));
        }
    }

    cout << "Producto creado con exito." << endl;
}

void buscarProducto() {
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

void actualizarProducto() {
    mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PorAmbos);

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

    menuActualizar(producto, archivo, index);

    archivo.close();
}

void actualizarStockProducto() {
    mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PorAmbos);

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

void listarProductos() {
    ifstream archivo(PRODUCTOS_PATH, ios::binary);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo de productos." << endl;
        return;
    }
    ArchivoHeader prodHeader = leerHeader(PRODUCTOS_PATH);
    archivo.read(reinterpret_cast<char*>(&prodHeader), sizeof(ArchivoHeader)); // Saltar header

    if (prodHeader.registrosActivos == 0) {
        cout << "No hay productos registrados." << endl;
        return;
    }

    cout << "--- Lista de Productos (" << prodHeader.registrosActivos << ") ---" << endl;
    cout << format("{:<5} | {:<20} | {:<15} | {:<10} | {:<5} | {:<15}", "ID", "Nombre", "Codigo",
                   "Precio", "Stock", "Proveedores")
         << endl;
    cout << "----------------------------------------------------------------------" << endl;

    Producto producto;
    // TODO: Usar O(1) para buscar los proveedores (si es que se puede)
    while (archivo.read(reinterpret_cast<char*>(&producto), sizeof(Producto))) {
        string nombresProveedores = "";
        ifstream proveedoresFile(PROVEEDORES_PATH, ios::binary);
        if (!proveedoresFile.is_open()) {
            cout << COLOR_RED
                 << "Error al listar los productos: No se pudo abrir el archivo de "
                    "proveedores.\nCerrando..."
                 << COLOR_RESET << endl;
            return;
        }

        int proveedorIdx = buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, producto.idProveedor);

        if (proveedorIdx != -1) {
            Proveedor proveedor;
            proveedoresFile.seekg(sizeof(ArchivoHeader) + (proveedorIdx * sizeof(Proveedor)),
                                  ios::beg);
            proveedoresFile.read(reinterpret_cast<char*>(&proveedor), sizeof(Proveedor));
            nombresProveedores = proveedor.nombre;
        }
        proveedoresFile.close();

        if (!producto.eliminado) {
            cout << format("{:<5} | {:<20} | {:<15} | ${:<9.2f} | {:<5} | {:<15}", producto.id,
                           producto.nombre, producto.codigo, producto.precio, producto.stock,
                           nombresProveedores)
                 << endl;
        }
    }
    cout << "----------------------------------------------------------------------" << endl;
    archivo.close();
}

void eliminarProducto() {
    mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PorAmbos);

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

    cout << COLOR_YELLOW << "Datos del producto a eliminar:" << COLOR_RESET << endl;
    mostrarDetallesEntidad(producto);

    bool tieneTransacciones = false;
    // TODO: Comprobar transacciones usando archivos

    if (tieneTransacciones) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: No se puede eliminar el producto porque tiene transacciones asociadas. "
             << "Cancele las transacciones primero si desea eliminarlo." << COLOR_RESET << endl;
        return;
    }

    cout << COLOR_RED << "\n¿Deseas eliminar este producto? (s/n): " << COLOR_RESET;
    char confirmar;
    cin >> confirmar;
    cin.ignore();
    cin.clear();

    if (tolower(confirmar) != 's') {
        cout << "Eliminación cancelada." << endl;
        return;
    }

    if (tolower(confirmar) == 's') {
        cout << COLOR_RED << "¿Estás seguro? (s/n): " << COLOR_RESET;
        cin >> confirmar;
    }

    if (tolower(confirmar) != 's') {
        cout << COLOR_RED << "Eliminación cancelada." << COLOR_RESET << endl;
        return;
    }

    producto.eliminado = true;
    producto.fechaUltimaModificacion = time(nullptr);
    archivo.seekp(sizeof(ArchivoHeader) + index * sizeof(Producto), ios::beg);
    archivo.write(reinterpret_cast<char*>(&producto), sizeof(Producto));

    ArchivoHeader header = leerHeader(PRODUCTOS_PATH);
    header.registrosActivos--;
    actualizarHeader(PRODUCTOS_PATH, header);

    cout << COLOR_GREEN << "Producto eliminado exitosamente." << COLOR_RESET << endl;
}

void crearProveedor() {
    fstream archivo;
    try {
        ifstream archivo(PROVEEDORES_PATH, ios::binary);
    } catch (const std::exception& e) {
        ofstream archivo(PROVEEDORES_PATH, ios::binary);
    }

    ArchivoHeader proveedoresHeader = leerHeader(PROVEEDORES_PATH);

    char nombre[100], email[100], telefono[20], rif[20], direccion[200];

    asignarPropiedadString("Ingrese el nombre del proveedor (q para cancelar): ", nombre);
    if (nombre[0] == 'q' && nombre[1] == '\0')
        return;

    while (true) {
        asignarPropiedadString("Ingrese el RIF del proveedor (q para cancelar): ", rif);
        if (rif[0] == 'q' && rif[1] == '\0')
            return;
        if (rif[0] == '\0') {
            cout << COLOR_RED << "Error: El RIF no puede estar vacío." << COLOR_RESET << endl;
            continue;
        }
        if (rifDuplicado(rif)) {
            cout << COLOR_RED << "Error: Ya existe un proveedor con el RIF " << rif << COLOR_RESET
                 << endl;
        } else {
            break;
        }
    }

    asignarPropiedadString("Ingrese el telefono del proveedor: ", telefono);
    while (true) {
        asignarPropiedadString("Ingrese el email del proveedor (q para cancelar): ", email);
        if (email[0] == 'q') {
            return;
        }

        if (!validarEmail(email)) {
            continue;
        }
        break;
    }
    asignarPropiedadString("Ingrese la direccion del proveedor: ", direccion);

    Proveedor proveedor = {};
    proveedor.id = proveedoresHeader.proximoID;
    copiarString(proveedor.nombre, nombre);
    copiarString(proveedor.rif, rif);
    copiarString(proveedor.telefono, telefono);
    copiarString(proveedor.email, email);
    copiarString(proveedor.direccion, direccion);
    proveedor.eliminado = false;
    proveedor.fechaCreacion = time(nullptr);
    proveedor.fechaUltimaModificacion = proveedor.fechaCreacion;
    obtenerFechaActual(proveedor.fechaRegistro);

    mostrarDetallesEntidad(proveedor);

    cout << COLOR_YELLOW << "Está seguro de crear el proveedor? (s/n): " << COLOR_RESET;
    char confirmar;
    cin >> confirmar;

    if (tolower(confirmar) != 's') {
        cout << "Proveedor no creado." << endl;
        return;
    }

    proveedoresHeader.cantidadRegistros++;
    proveedoresHeader.registrosActivos++;
    proveedoresHeader.proximoID++;

    ofstream archivoOut(PROVEEDORES_PATH, ios::binary | ios::app);
    archivoOut.write(reinterpret_cast<const char*>(&proveedor), sizeof(Proveedor));
    archivoOut.close();

    actualizarHeader(PROVEEDORES_PATH, proveedoresHeader);

    cout << COLOR_GREEN << "Proveedor creado con exito." << COLOR_RESET << endl;
}

void buscarProveedor() {
    mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH, PorAmbos);

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
    archivo.close();

    cout << format("{}Datos del proveedor: {}", COLOR_GREEN, COLOR_RESET) << endl;
    cout << COLOR_YELLOW << "Id: " << COLOR_RESET << p.id << endl;
    cout << COLOR_YELLOW << "Nombre: " << COLOR_RESET << p.nombre << endl;
    cout << COLOR_YELLOW << "RIF: " << COLOR_RESET << p.rif << endl;
    cout << COLOR_YELLOW << "Telefono: " << COLOR_RESET << p.telefono << endl;
    cout << COLOR_YELLOW << "Email: " << COLOR_RESET << p.email << endl;
    cout << COLOR_YELLOW << "Direccion: " << COLOR_RESET << p.direccion << endl;
    cout << COLOR_YELLOW << "Fecha de Registro: " << COLOR_RESET << p.fechaRegistro << endl;
}

void actualizarProveedor() {
    mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH, PorAmbos);

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

    menuActualizar(p, archivo, index);

    archivo.close();
}

void listarProveedores() {
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

void eliminarProveedor() {
    mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH, PorAmbos);

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

void crearCliente() {
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

    mostrarDetallesEntidad(cliente);

    cout << "Está seguro de crear el cliente? (s/n): ";
    char confirmar;
    cin >> confirmar;
    if (confirmar == 's' || confirmar == 'S') {
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

void buscarCliente() {
    mostrarListaEntidades<Cliente>("Clientes", CLIENTES_PATH, PorId);

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

void actualizarCliente() {
    mostrarListaEntidades<Cliente>("Clientes", CLIENTES_PATH, PorAmbos);

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

    Cliente cliente;
    fstream archivo(CLIENTES_PATH, ios::binary | ios::in | ios::out);
    archivo.seekg(sizeof(ArchivoHeader) + index * sizeof(Cliente), ios::beg);
    archivo.read(reinterpret_cast<char*>(&cliente), sizeof(Cliente));

    menuActualizar(cliente, archivo, index);

    archivo.close();
}

void listarClientes() {
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

void eliminarCliente() {
    mostrarListaEntidades<Cliente>("Clientes", CLIENTES_PATH, PorAmbos);

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

void registrarCompra() {
    cout << "\n===REGISTRAR COMPRA (Entrada de Mercancia)===" << endl;

    mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PorAmbos);

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

    int idProv = producto.idProveedor;

    // Validar que el proveedor exista
    if (buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, idProv) == -1) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: El proveedor asociado al producto (ID: " << idProv << ") no existe."
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

void registrarVenta() {
    cout << "\n===REGISTRAR VENTA (Salida de Mercancia)===" << endl;

    mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PorAmbos);

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
    mostrarListaEntidades<Cliente>("Clientes", CLIENTES_PATH, PorAmbos);

    int idCli = leerId("Ingrese ID del Cliente: ");
    if (idCli <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }
    if (!existeCliente(idCli)) {
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

void buscarTransacciones() {
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

void listarTransacciones() {
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

void cancelarTransaccion() {
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

    cout << COLOR_RED << "Error: El email no es valido." << COLOR_RESET << endl;
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

bool existeProducto(int id) {
    return buscarEntidadPorId<Producto>(PRODUCTOS_PATH, id) != -1;
}

bool existeProveedor(int id) {
    return buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, id) != -1;
}

bool existeCliente(int id) {
    return buscarEntidadPorId<Cliente>(CLIENTES_PATH, id) != -1;
}

bool codigoDuplicado(const char* codigo) {
    return existeStringDuplicado<Producto>(PRODUCTOS_PATH, codigo, &Producto::codigo);
}

bool nombreProveedorDuplicado(const char* nombre) {
    return existeStringDuplicado<Proveedor>(PROVEEDORES_PATH, nombre, &Proveedor::nombre);
}

bool nombreProductoDuplicado(const char* nombre) {
    return existeStringDuplicado<Producto>(PRODUCTOS_PATH, nombre, &Producto::nombre);
}

bool rifDuplicado(const char* rif) {
    return existeStringDuplicado<Proveedor>(PROVEEDORES_PATH, rif, &Proveedor::rif);
}

// Búsquedas
// =======================================================
//                 SECCIÓN DE TEMPLATES
// =======================================================
// Casi todas las entidades tienen un id (cliente, proveedor, producto)
template <typename T> int buscarEntidadPorId(fs::path path, int id) {
    if (id <= 0)
        return -1;

    ifstream archivo(path, ios::binary);
    if (!archivo.is_open())
        return -1;

    int physicalIdx = id - 1;
    // Se salta el archivo header y se posiciona en la entidad
    streampos entityPos = sizeof(ArchivoHeader) + ((physicalIdx) * sizeof(T));
    archivo.seekg(entityPos, ios::beg);

    // No hay entidades en esta posición
    if (!archivo)
        return -1;

    T entidad;
    archivo.read(reinterpret_cast<char*>(&entidad), sizeof(T));

    if (archivo.gcount() == sizeof(T)) {
        if (!entidad.eliminado && entidad.id == id) {
            return physicalIdx;
        }
    }
    return -1;
}

template <typename T>
void mostrarListaEntidades(const char* titulo, fs::path path, ListarPorPropiedad tipoProp) {
    ifstream archivo(path, ios::binary);
    if (!archivo.is_open())
        return;

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    if (header.registrosActivos == 0) {
        cout << "\nNo hay " << titulo << " registrados." << endl;
        return;
    }

    cout << format("\n--- {} Disponibles ({})---\n", titulo, header.registrosActivos);

    // Imprimir cabecera
    switch (tipoProp) {
    case PorId:
        cout << format("{:<5}", "ID") << endl;
        break;
    case PorNombre:
        cout << format("{:<20}", "Nombre") << endl;
        break;
    case PorAmbos:
        cout << format("{:<5} | {:<20}", "ID", "Nombre") << endl;
        break;
    }
    cout << "-------------------------------" << endl;

    // datos
    T entidad;
    while (archivo.read(reinterpret_cast<char*>(&entidad), sizeof(T))) {
        if (!entidad.eliminado) {
            switch (tipoProp) {
            case PorId:
                cout << format("{:<5}", entidad.id) << endl;
                break;
            case PorNombre:
                cout << format("{:<20}", entidad.nombre) << endl;
                break;
            case PorAmbos:
                cout << format("{:<5} | {:<20}", entidad.id, entidad.nombre) << endl;
                break;
            }
        }
    }
    cout << "-------------------------------" << endl;
}

template <typename T, size_t N>
bool existeStringDuplicado(fs::path path, const char* valorBusqueda, char (T::*propiedad)[N]) {
    ifstream archivo(path, ios::binary);
    if (!archivo.is_open())
        return false;

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    T entidad;
    while (archivo.read(reinterpret_cast<char*>(&entidad), sizeof(T))) {
        if (!entidad.eliminado && strcmp(entidad.*propiedad, valorBusqueda) == 0) {
            return true;
        }
    }
    return false;
}

// retorna array de indices del producto
int* buscarProductosPorNombre(const char* nombre) {
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
        cout << COLOR_YELLOW << msg << " (q para salir): " << COLOR_RESET;
        string input;
        cin >> input;

        if (input == "q" || input == "Q") {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return -1;
        }

        try {
            int id = stoi(input);
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

void crearBackup() {
    char option;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;
    cout << COLOR_CYAN << "   CREACIÓN DE BACKUP DE BASE DE DATOS  " << COLOR_RESET << endl;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;
    do {
        cout << COLOR_YELLOW
             << "¿Desea crear un backup de la base de datos? (s/n): " << COLOR_RESET;

        cin >> option;
        if (tolower(option) == 'n') {
            cout << COLOR_RED << "Operación cancelada" << COLOR_RESET << endl;
            cout << COLOR_GREEN << "Saliendo..." << COLOR_RESET << endl;
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.clear();

    } while (tolower(option) != 's');
    cout << format("{}Comenzando backup...{}", COLOR_YELLOW, COLOR_RESET) << endl;

    fs::create_directories(BACKUP_PATH);

    const string paths[5] = {PRODUCTOS_PATH, PROVEEDORES_PATH, CLIENTES_PATH, TRANSACCIONES_PATH,
                             TIENDA_PATH};
    auto now = chrono::system_clock::now();
    auto current_date = chrono::time_point_cast<std::chrono::days>(now);
    chrono::year_month_day ymd{current_date};
    auto nowSec = chrono::time_point_cast<std::chrono::seconds>(now);
    chrono::zoned_time localTime{chrono::current_zone(), nowSec};

    for (fs::path path : paths) {
        string baseName = path.stem().string();
        fs::path ymd_suffix =
            BACKUP_PATH / fs::path(format("{}.{:%Y-%m-%d}.{:%T}.bin", baseName, ymd, localTime));
        fs::copy(path, ymd_suffix);
        cout << format("{}{:>20} {}Respaldado en {}\"{}\"{}", COLOR_CYAN, path.stem().string(),
                       COLOR_GREEN, COLOR_CYAN, ymd_suffix.string(), COLOR_RESET)
             << endl;
    }

    cout << format("{}Backup realizado con exito en {}\"{}\"{}", COLOR_GREEN, COLOR_CYAN,
                   BACKUP_PATH.string(), COLOR_RESET)
         << endl;
    cout << format(
                "{}Fecha y hora del backup (Año-Mes-Día) (Hora:Minutos:Segundos):{} \"{} {:%T}\"{}",
                COLOR_YELLOW, COLOR_CYAN, ymd, localTime, COLOR_RESET)
         << endl;
}

void verificarIntegridadReferencial() {
    cout << CLEAR_SCREEN;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;
    cout << COLOR_CYAN << "   DIAGNÓSTICO DE SALUD DE BASE DE DATOS  " << COLOR_RESET << endl;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;

    // 1. Recorrer productos
    ifstream productosFile(PRODUCTOS_PATH, ios::binary);
    if (!productosFile.is_open()) {
        cout << COLOR_RED << "Error: No se pudo abrir el archivo de productos." << COLOR_RESET
             << endl;
        return;
    }
    productosFile.seekg(0, ios::end);

    int erroresEncontrados = 0;
    Producto temp;
    while (productosFile.read(reinterpret_cast<char*>(&temp), sizeof(Producto))) {
        // Verificando que los proveedores existan
        int hasProveedor = buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, temp.idProveedor) != -1;
        if (!temp.eliminado && temp.idProveedor > 0 && !hasProveedor) {
            cout << COLOR_RED << "Error: El proveedor con id " << temp.idProveedor << " no existe."
                 << COLOR_RESET << endl;
            erroresEncontrados++;
            return;
        }
    }
    productosFile.close();

    // 2. Recorrer el archivo de transacciones
    ifstream transaccionesFile(TRANSACCIONES_PATH, ios::binary);
    if (!transaccionesFile.is_open()) {
        cout << COLOR_RED << "Error: No se pudo abrir el archivo de transacciones." << COLOR_RESET
             << endl;
        return;
    }

    ArchivoHeader headerTrans;
    transaccionesFile.read(reinterpret_cast<char*>(&headerTrans), sizeof(ArchivoHeader));

    Transaccion trans;
    while (transaccionesFile.read(reinterpret_cast<char*>(&trans), sizeof(Transaccion))) {
        if (!trans.eliminado) {
            // 2.1 Verificar Cliente o Proveedor asociado
            int hasRelatedId =
                buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, trans.idRelacionado) != -1;
            if (trans.tipo == COMPRA && !hasRelatedId) {
                cout << COLOR_RED << "[ERROR - Transaccion ID " << trans.id << "] " << COLOR_RESET
                     << "Es una COMPRA pero el Proveedor ID " << trans.idRelacionado
                     << " NO EXISTE." << endl;
                erroresEncontrados++;
            }

            if (trans.tipo == VENTA && !hasRelatedId) {
                cout << COLOR_RED << "[ERROR - Transaccion ID " << trans.id << "] " << COLOR_RESET
                     << "Es una VENTA pero el Cliente ID " << trans.idRelacionado << " NO EXISTE."
                     << endl;
                erroresEncontrados++;
            }

            // 2.2 Verificar Productos asociados a la transacción
            for (int i = 0; i < trans.cantidadTiposDeProductos; i++) {
                int idProducto = trans.productosIds[i];
                int hasProductId = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, idProducto) != -1;
                if (!hasProductId) {
                    cout << COLOR_RED << "[ERROR - Transaccion ID " << trans.id << "] "
                         << COLOR_RESET << "Contiene el Producto ID " << idProducto
                         << " que NO EXISTE." << endl;
                    erroresEncontrados++;
                }
            }
        }
    }
    transaccionesFile.close();

    // 3. Reporte final
    cout << "\n----------------------------------------" << endl;
    if (erroresEncontrados == 0) {
        cout << format("{} [+] SALUDABLE: {} No se encontraron referencias rotas. La integridad "
                       "referencial está intacta.",
                       CLEAR_SCREEN, COLOR_GREEN, COLOR_RESET);
    } else {
        cout << format(
            "{} [-] ADVERTENCIA: {} Se detectaron {} referencias rotas en la base de datos. Te "
            "sugerimos usar el sistema de respaldo y restaurar una base de datos estable",
            COLOR_RED, COLOR_RESET, erroresEncontrados);
    }
    cout << "----------------------------------------" << endl;
}

/// MENUS ////////////////////////////////////////////////////////////////////
template <typename T> void menuActualizar(T& entidad, fstream& archivo, int index) {
    char opcion;
    do {
        cout << format("{}Datos actuales: {}", COLOR_CYAN, COLOR_RESET) << endl;
        mostrarDetallesEntidad(entidad);
        cout << endl;

        cout << COLOR_CYAN << "¿Qué desea actualizar?: " << COLOR_RESET << endl;

        if constexpr (std::is_same_v<T, Producto>) {
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
                manejarPropiedad("nombre", entidad.nombre);
                break;
            case ActualizarCodigo:
                manejarPropiedad("codigo", entidad.codigo);
                break;
            case ActualizarDescripcion:
                manejarPropiedad("descripcion", entidad.descripcion);
                break;
            case ActualizarPrecio:
                manejarPropiedad("precio", entidad.precio);
                break;
            case ActualizarStock:
                manejarPropiedad("stock", entidad.stock);
                break;
            case ActualizarProveedor: {
                mostrarListaEntidades<Proveedor>("Proveedores Disponibles", PROVEEDORES_PATH,
                                                 PorAmbos);
                int newProvIdx = leerId("Ingrese el ID del nuevo proveedor");
                if (newProvIdx <= 0) {
                    cout << "Actualización cancelada." << endl;
                    break;
                }

                int provIdx = buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, newProvIdx);
                if (provIdx == -1) {
                    cout << COLOR_RED << "Error: Proveedor no encontrado." << COLOR_RESET << endl;
                    break;
                }

                // 1. Obtener ID del proveedor anterior
                int oldProvId = entidad.idProveedor;

                // 2. Actualizar el producto con el nuevo proveedor
                entidad.idProveedor = newProvIdx;
                cout << "Proveedor actualizado con éxito." << endl;

                // 3. Actualizar el NUEVO proveedor (agregar producto a su lista)
                Proveedor newProv;
                fstream provArchivo(PROVEEDORES_PATH, ios::binary | ios::in | ios::out);

                // Leer nuevo proveedor
                provArchivo.seekg(sizeof(ArchivoHeader) + provIdx * sizeof(Proveedor), ios::beg);
                provArchivo.read(reinterpret_cast<char*>(&newProv), sizeof(Proveedor));

                bool exists = false;
                if (newProv.cantidadProductos < 100) {
                    for (int i = 0; i < newProv.cantidadProductos; i++) {
                        if (newProv.productosIds[i] == entidad.id) {
                            exists = true;
                            break;
                        }
                    }

                    if (!exists) {
                        newProv.productosIds[newProv.cantidadProductos++] = entidad.id;
                        newProv.fechaUltimaModificacion = time(nullptr);
                        provArchivo.seekp(sizeof(ArchivoHeader) + provIdx * sizeof(Proveedor),
                                          ios::beg);
                        provArchivo.write(reinterpret_cast<const char*>(&newProv),
                                          sizeof(Proveedor));
                    }
                }

                // 4. Actualizar el VIEJO proveedor (remover producto de su lista)
                if (oldProvId > 0 && oldProvId != newProvIdx) {
                    int oldProvIdx = buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, oldProvId);
                    if (oldProvIdx != -1) {
                        Proveedor oldProv;
                        provArchivo.seekg(sizeof(ArchivoHeader) + oldProvIdx * sizeof(Proveedor),
                                          ios::beg);
                        provArchivo.read(reinterpret_cast<char*>(&oldProv), sizeof(Proveedor));

                        bool found = false;
                        int foundIdx = -1;
                        for (int i = 0; i < oldProv.cantidadProductos; i++) {
                            if (oldProv.productosIds[i] == entidad.id) {
                                found = true;
                                foundIdx = i;
                                break;
                            }
                        }

                        if (found) {
                            // Shift left to remove
                            for (int j = foundIdx; j < oldProv.cantidadProductos - 1; j++) {
                                oldProv.productosIds[j] = oldProv.productosIds[j + 1];
                            }
                            oldProv.cantidadProductos--;
                            oldProv.fechaUltimaModificacion = time(nullptr);

                            provArchivo.seekp(
                                sizeof(ArchivoHeader) + oldProvIdx * sizeof(Proveedor), ios::beg);
                            provArchivo.write(reinterpret_cast<const char*>(&oldProv),
                                              sizeof(Proveedor));
                        }
                    }
                }
                provArchivo.close();
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
                entidad.fechaUltimaModificacion = time(nullptr);
                archivo.seekp(sizeof(ArchivoHeader) + index * sizeof(Producto), ios::beg);
                archivo.write(reinterpret_cast<const char*>(&entidad), sizeof(Producto));
            }
        } else if constexpr (std::is_same_v<T, Proveedor>) {
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
                manejarPropiedad("nombre", entidad.nombre);
                break;
            case '2':
                manejarPropiedad("rif", entidad.rif);
                break;
            case '3':
                manejarPropiedad("telefono", entidad.telefono);
                break;
            case '4':
                manejarPropiedad("email", entidad.email);
                break;
            case '5':
                manejarPropiedad("direccion", entidad.direccion);
                break;
            case '0':
                cout << COLOR_GREEN << "Saliendo..." << COLOR_RESET << endl;
                break;
            default:
                cout << CLEAR_SCREEN << COLOR_RED << "Opcion no valida" << COLOR_RESET << endl;
            }

            if (opcion >= '1' && opcion <= '5') {
                entidad.fechaUltimaModificacion = time(nullptr);
                archivo.seekp(sizeof(ArchivoHeader) + index * sizeof(Proveedor), ios::beg);
                archivo.write(reinterpret_cast<const char*>(&entidad), sizeof(Proveedor));
            }
        } else if constexpr (std::is_same_v<T, Cliente>) {
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
                manejarPropiedad("nombre", entidad.nombre);
                break;
            case '2':
                manejarPropiedad("cedula", entidad.cedula);
                break;
            case '3':
                manejarPropiedad("telefono", entidad.telefono);
                break;
            case '4':
                manejarPropiedad("email", entidad.email);
                break;
            case '5':
                manejarPropiedad("direccion", entidad.direccion);
                break;
            case '0':
                cout << COLOR_GREEN << "Saliendo..." << COLOR_RESET << endl;
                break;
            default:
                cout << CLEAR_SCREEN << COLOR_RED << "Opcion no valida" << COLOR_RESET << endl;
            }

            if (opcion >= '1' && opcion <= '5') {
                entidad.fechaUltimaModificacion = time(nullptr);
                archivo.seekp(sizeof(ArchivoHeader) + index * sizeof(Cliente), ios::beg);
                archivo.write(reinterpret_cast<const char*>(&entidad), sizeof(Cliente));
            }
        }
    } while (opcion != '0');
}

struct OpcionMenu {
    const char* descripcion;
    void (*accion)();
};
void drawMenu(const char* title, OpcionMenu options[], int numOptions,
              const char* texToExit = "Volver al Menú Principal") {
    int option;
    do {
        cout << COLOR_CYAN << "\n=== " << title << " ===" << COLOR_RESET << endl;
        for (int i = 0; i < numOptions; ++i) {
            cout << COLOR_YELLOW << i + 1 << "." << COLOR_RESET << " " << options[i].descripcion
                 << endl;
        }
        cout << COLOR_RED << "0." << COLOR_RESET << " " << texToExit << endl;
        cout << "Seleccione una opción: ";

        if (!(cin >> option))
            break;

        if (option < 0 || option > numOptions) {
            cout << COLOR_RED << "Opción inválida" << COLOR_RESET << endl;
            continue;
        }

        int index = option - 1;
        if (index >= 0 && index < numOptions && options[index].accion != nullptr) {
            options[index].accion();
        }

        // Si el indice es -1 ((index = 0) -1) se sale del menu
        if (index == -1) {
            cout << COLOR_GREEN << (tolower(texToExit[0]) == 's' ? "Saliendo..." : "Volviendo...")
                 << COLOR_RESET << endl;
            break;
        }
    } while (option != 0);
}

void menuProductos() {
    OpcionMenu opciones[] = {
        {"Crear Producto", crearProducto},           {"Buscar Producto", buscarProducto},
        {"Actualizar Producto", actualizarProducto}, {"Actualizar Stock", actualizarStockProducto},
        {"Listar Productos", listarProductos},       {"Eliminar Producto", eliminarProducto}};

    ifstream proveedoresFile(PROVEEDORES_PATH, ios::binary);
    if (!proveedoresFile.is_open()) {
        cout << COLOR_RED << "Error: No se pudo abrir el archivo de proveedores." << COLOR_RESET;
        return;
    }

    ArchivoHeader proveedoresHeader = leerHeader(PROVEEDORES_PATH);
    if (!proveedoresHeader.registrosActivos) {

        cout << format("{} {} Error: No hay proveedores registrados. Debe crear al menos un "
                       "proveedor antes de registrar un producto. {}",
                       CLEAR_SCREEN, COLOR_RED, COLOR_RESET);
        return;
    }

    drawMenu("Gestión de Productos", opciones, 6);
}

void menuProveedores() {
    OpcionMenu options[] = {{"Crear Proveedor", crearProveedor},
                            {"Buscar Proveedor", buscarProveedor},
                            {"Actualizar Proveedor", actualizarProveedor},
                            {"Listar Proveedores", listarProveedores},
                            {"Eliminar Proveedor", eliminarProveedor}};
    drawMenu("Gestión de Proveedores", options, 5);
}

void menuClientes() {
    OpcionMenu options[] = {{"Crear Cliente", crearCliente},
                            {"Buscar Cliente", buscarCliente},
                            {"Actualizar Cliente", actualizarCliente},
                            {"Listar Clientes", listarClientes},
                            {"Eliminar Cliente", eliminarCliente}};
    drawMenu("Gestión de Clientes", options, 5);
}

void menuTransacciones() {
    OpcionMenu options[] = {{"Registrar Compra", registrarCompra},
                            {"Registrar Venta", registrarVenta},
                            {"Buscar Transacciones", buscarTransacciones},
                            {"Listar Transacciones", listarTransacciones},
                            {"Cancelar Transacción", cancelarTransaccion}};
    drawMenu("Gestión de Transacciones", options, 5);
}

void menuReportes() {
    OpcionMenu options[] = {{"Integridad Referencial", verificarIntegridadReferencial},
                            {"Crear Backup", crearBackup}};
    drawMenu("Reportes", options, 2);
}

int main() {
    setlocale(LC_ALL, "Spanish");
    inicializarTienda("Papaya Store", "J-123456789");

    OpcionMenu options[] = {{"Gestión de Productos", menuProductos},
                            {"Gestión de Proveedores", menuProveedores},
                            {"Gestión de Clientes", menuClientes},
                            {"Gestión de Transacciones", menuTransacciones},
                            {"Gestión de Reportes y seguridad", menuReportes}};

    drawMenu("PAPAYA STORE - Menú Principal", options, 5, "Salir");

    return 0;
}
