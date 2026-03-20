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
bool productoTieneTransaccionesActivas(int idProducto);
bool proveedorTieneTransaccionesActivas(int idProveedor);
bool clienteTieneTransaccionesActivas(int idCliente);
bool leerTienda(Tienda& tienda);
bool actualizarTiendaRegistro(const Tienda& tienda);
void sincronizarContadoresTienda();
void mostrarResumenTienda();

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
    int cantidadTiposDeProductos; // Cuantos elementos de los arrays anteriores
                                  // se están usando

    // metadata
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Tienda {
    int id;
    char nombre[100];
    char rif[20];
    int totalProductosActivos;
    int totalProveedoresActivos;
    int totalClientesActivos;
    int totalTransaccionesActivas;
    float montoTotalVentas;
    float montoTotalCompras;
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
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
    string paths[5] = {PRODUCTOS_PATH, PROVEEDORES_PATH, CLIENTES_PATH, TRANSACCIONES_PATH,
                       TIENDA_PATH};

    // Inicializar todos los archivos tan pronto como inicie el programa
    for (string path : paths) {
        inicializarArchivo(path.c_str());
    }

    ArchivoHeader tiendaHeader = leerHeader(TIENDA_PATH);
    if (tiendaHeader.cantidadRegistros == 0) {
        Tienda tienda = {};
        tienda.id = 1;
        copiarString(tienda.nombre, nombre);
        copiarString(tienda.rif, rif);
        tienda.totalProductosActivos = 0;
        tienda.totalProveedoresActivos = 0;
        tienda.totalClientesActivos = 0;
        tienda.totalTransaccionesActivas = 0;
        tienda.montoTotalVentas = 0;
        tienda.montoTotalCompras = 0;
        tienda.eliminado = false;
        tienda.fechaCreacion = time(nullptr);
        tienda.fechaUltimaModificacion = tienda.fechaCreacion;

        ofstream archivoOut(TIENDA_PATH, ios::binary | ios::app);
        archivoOut.write(reinterpret_cast<const char*>(&tienda), sizeof(Tienda));
        archivoOut.close();

        tiendaHeader.cantidadRegistros = 1;
        tiendaHeader.registrosActivos = 1;
        tiendaHeader.proximoID = 2;
        tiendaHeader.version = 1;
        actualizarHeader(TIENDA_PATH, tiendaHeader);
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
        cout << format("{} {} Error: No hay proveedores registrados. Debe crear "
                       "al menos un "
                       "proveedor antes de registrar un producto. {}",
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

    int stockMinimo;
    asignarPropiedadNum("Ingrese el stock mínimo ideal del producto: ", stockMinimo);

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
    producto.stockMinimo = stockMinimo;
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
    // actualiza las entidades activas dentro de tienda
    sincronizarContadoresTienda();

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
                         << "El valor debe ser numérico. Intente "
                            "nuevamente: "
                         << COLOR_RESET << endl;
                    continue;
                }
                if (cantidad > 0)
                    break;
                cout << CLEAR_SCREEN << COLOR_RED
                     << "La cantidad debe ser mayor a 0. Intente "
                        "nuevamente: "
                     << COLOR_RESET << endl;
            }

            int nuevoStock = producto.stock;
            if (opcion == '1') {
                nuevoStock += cantidad;
            }

            if (opcion == '2') {
                nuevoStock -= cantidad;
                if (nuevoStock < 0) {
                    cout << CLEAR_SCREEN << COLOR_RED
                         << format("Error: El stock no puede ser negativo. "
                                   "Faltan unidades. Stock "
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
    archivo.read(reinterpret_cast<char*>(&prodHeader),
                 sizeof(ArchivoHeader)); // Saltar header

    if (prodHeader.registrosActivos == 0) {
        cout << "No hay productos registrados." << endl;
        return;
    }

    cout << "--- Lista de Productos (" << prodHeader.registrosActivos << ") ---" << endl;
    cout << format("{:<5} | {:<20} | {:<15} | {:<10} | {:<5} | {:<15}", "ID", "Nombre", "Codigo",
                   "Precio", "Stock", "Proveedores")
         << endl;
    cout << "------------------------------------------------------------------"
            "----"
         << endl;

    Producto producto;
    while (archivo.read(reinterpret_cast<char*>(&producto), sizeof(Producto))) {
        string nombresProveedores = "";
        ifstream proveedoresFile(PROVEEDORES_PATH, ios::binary);
        if (!proveedoresFile.is_open()) {
            cout << COLOR_RED
                 << "Error al listar los productos: No se pudo abrir el "
                    "archivo de "
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
    cout << "------------------------------------------------------------------"
            "----"
         << endl;
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

    bool tieneTransacciones = productoTieneTransaccionesActivas(id);

    if (tieneTransacciones) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: No se puede eliminar el producto porque tiene "
                "transacciones asociadas. "
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
    sincronizarContadoresTienda();

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
    sincronizarContadoresTienda();

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
    cout << "------------------------------------------------------------------"
            "----------------"
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
    cout << "------------------------------------------------------------------"
            "----------------"
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

    bool tieneTransacciones = proveedorTieneTransaccionesActivas(id);

    if (tieneTransacciones) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: No se puede eliminar el proveedor porque tiene "
                "transacciones asociadas. "
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
        sincronizarContadoresTienda();

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
        sincronizarContadoresTienda();

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
    cout << "------------------------------------------------------------------"
            "----------------"
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
    cout << "------------------------------------------------------------------"
            "----------------"
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

    bool tieneTransacciones = clienteTieneTransaccionesActivas(id);

    if (tieneTransacciones) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: No se puede eliminar el cliente porque tiene "
                "transacciones asociadas. "
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
        sincronizarContadoresTienda();

        cout << "Cliente eliminado exitosamente." << endl;
    } else {
        cout << "Eliminación cancelada." << endl;
    }
}

// agrega una transacción al historial de transacciones de un cliente
// 1) No agrega IDs inválidos ni cuando el contador está corrupto.
// 2) Si el ID ya existe, no duplica y retorna true.
// 3) Si hay espacio, inserta al final y aumenta el contador.
// 4) Si no hay espacio, retorna false.
bool agregarIdAArreglo(int transactionId, int wholeTransactions[], int& cantidad,
                       int capacidad = 100) {
    if (transactionId <= 0 || cantidad < 0 || cantidad > capacidad) {
        return false;
    }

    for (int i = 0; i < cantidad; i++) {
        if (wholeTransactions[i] == transactionId) {
            return true;
        }
    }

    if (cantidad >= capacidad) {
        return false;
    }

    wholeTransactions[cantidad] = transactionId;
    cantidad++;
    return true;
}

// elimina una transacción al historial de transacciones de un cliente
// Elimina un ID de un arreglo fijo manteniendo continuidad.
// Busca el ID, hace shift a la izquierda para evitar huecos,
// limpia la última posición y disminuye el contador.
bool removerIdDeArreglo(int transactionId, int wholeTransactions[], int& cantidad) {
    if (transactionId <= 0 || cantidad <= 0) {
        return false;
    }

    int index = -1;
    for (int i = 0; i < cantidad; i++) {
        if (wholeTransactions[i] == transactionId) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return false;
    }

    for (int i = index; i < cantidad - 1; i++) {
        wholeTransactions[i] = wholeTransactions[i + 1];
    }

    wholeTransactions[cantidad - 1] = 0;
    cantidad--;
    return true;
}

int buscarProductoEnTransaccion(const Transaccion& transaccion, int idProducto) {
    for (int i = 0; i < transaccion.cantidadTiposDeProductos; i++) {
        if (transaccion.productosIds[i] == idProducto) {
            return i;
        }
    }
    return -1;
}

// agregar producto en la transaccion previamente seleccionado.
bool agregarProductoConsolidado(Transaccion& transaccion, int idProducto, int cantidad,
                                float precioUnitario) {
    if (idProducto <= 0 || cantidad <= 0) {
        return false;
    }

    int idx = buscarProductoEnTransaccion(transaccion, idProducto);
    if (idx != -1) {
        transaccion.cantidades[idx] += cantidad;
        return true;
    }

    if (transaccion.cantidadTiposDeProductos >= 100) {
        return false;
    }

    int pos = transaccion.cantidadTiposDeProductos;
    transaccion.productosIds[pos] = idProducto;
    transaccion.cantidades[pos] = cantidad;
    transaccion.preciosUnitarios[pos] = precioUnitario;
    transaccion.cantidadTiposDeProductos++;
    return true;
}

bool productoTieneTransaccionesActivas(int idProducto) {
    if (idProducto <= 0) {
        return false;
    }

    ifstream archivo(TRANSACCIONES_PATH, ios::binary);
    if (!archivo.is_open()) {
        return false;
    }

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    Transaccion transaccion;
    // revisa todos las transacciones
    while (archivo.read(reinterpret_cast<char*>(&transaccion), sizeof(Transaccion))) {
        if (transaccion.eliminado) {
            continue;
        }

        int cantidadItems = transaccion.cantidadTiposDeProductos;
        if (cantidadItems < 0 || cantidadItems > 100) {
            continue;
        }

        // revisa si el producto solicitado tiene alguna transaccion
        for (int i = 0; i < cantidadItems; i++) {
            if (transaccion.productosIds[i] == idProducto) {
                return true;
            }
        }
    }

    return false;
}

bool proveedorTieneTransaccionesActivas(int idProveedor) {
    if (idProveedor <= 0) {
        return false;
    }

    ifstream archivo(TRANSACCIONES_PATH, ios::binary);
    if (!archivo.is_open()) {
        return false;
    }

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    Transaccion transaccion;
    while (archivo.read(reinterpret_cast<char*>(&transaccion), sizeof(Transaccion))) {
        if (!transaccion.eliminado && transaccion.tipo == COMPRA &&
            transaccion.idRelacionado == idProveedor) {
            return true;
        }
    }

    return false;
}

bool clienteTieneTransaccionesActivas(int idCliente) {
    if (idCliente <= 0) {
        return false;
    }

    ifstream archivo(TRANSACCIONES_PATH, ios::binary);
    if (!archivo.is_open()) {
        return false;
    }

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    Transaccion transaccion;
    while (archivo.read(reinterpret_cast<char*>(&transaccion), sizeof(Transaccion))) {
        if (!transaccion.eliminado && transaccion.tipo == VENTA &&
            transaccion.idRelacionado == idCliente) {
            return true;
        }
    }

    return false;
}

bool leerTienda(Tienda& tienda) {
    ifstream archivo(TIENDA_PATH, ios::binary);
    if (!archivo.is_open()) {
        return false;
    }

    archivo.seekg(sizeof(ArchivoHeader), ios::beg);
    archivo.read(reinterpret_cast<char*>(&tienda), sizeof(Tienda));
    return archivo.good() && !tienda.eliminado;
}

bool actualizarTiendaRegistro(const Tienda& tienda) {
    fstream archivo(TIENDA_PATH, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        return false;
    }

    archivo.seekp(sizeof(ArchivoHeader), ios::beg);
    archivo.write(reinterpret_cast<const char*>(&tienda), sizeof(Tienda));
    return archivo.good();
}

// Actualiza los registros activos de todas las entidades en la tienda
void sincronizarContadoresTienda() {
    Tienda tienda;
    if (!leerTienda(tienda)) {
        return;
    }

    ArchivoHeader headerProductos = leerHeader(PRODUCTOS_PATH);
    ArchivoHeader headerProveedores = leerHeader(PROVEEDORES_PATH);
    ArchivoHeader headerClientes = leerHeader(CLIENTES_PATH);
    ArchivoHeader headerTransacciones = leerHeader(TRANSACCIONES_PATH);

    tienda.totalProductosActivos = headerProductos.registrosActivos;
    tienda.totalProveedoresActivos = headerProveedores.registrosActivos;
    tienda.totalClientesActivos = headerClientes.registrosActivos;
    tienda.totalTransaccionesActivas = headerTransacciones.registrosActivos;
    tienda.fechaUltimaModificacion = time(nullptr);

    actualizarTiendaRegistro(tienda);
}

void mostrarResumenTienda() {
    Tienda tienda;
    if (!leerTienda(tienda)) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: No se pudo leer el registro de tienda."
             << COLOR_RESET << endl;
        return;
    }

    cout << CLEAR_SCREEN;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;
    cout << COLOR_CYAN << "         RESUMEN DE TIENDA             " << COLOR_RESET << endl;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;
    cout << COLOR_YELLOW << "ID: " << COLOR_RESET << tienda.id << endl;
    cout << COLOR_YELLOW << "Nombre: " << COLOR_RESET << tienda.nombre << endl;
    cout << COLOR_YELLOW << "RIF: " << COLOR_RESET << tienda.rif << endl;
    cout << COLOR_YELLOW << "Productos activos: " << COLOR_RESET << tienda.totalProductosActivos
         << endl;
    cout << COLOR_YELLOW << "Proveedores activos: " << COLOR_RESET << tienda.totalProveedoresActivos
         << endl;
    cout << COLOR_YELLOW << "Clientes activos: " << COLOR_RESET << tienda.totalClientesActivos
         << endl;
    cout << COLOR_YELLOW << "Transacciones activas: " << COLOR_RESET
         << tienda.totalTransaccionesActivas << endl;
    cout << COLOR_YELLOW << "Monto total ventas: " << COLOR_RESET
         << format("${:.2f}", tienda.montoTotalVentas) << endl;
    cout << COLOR_YELLOW << "Monto total compras: " << COLOR_RESET
         << format("${:.2f}", tienda.montoTotalCompras) << endl;
}

bool mostrarProductosPorProveedor(int idProveedor) {
    ifstream archivo(PRODUCTOS_PATH, ios::binary);
    if (!archivo.is_open()) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: No se pudo abrir el archivo de productos."
             << COLOR_RESET << endl;
        return false;
    }

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    bool hayProductos = false;
    Producto producto;
    cout << "\n--- Productos del proveedor seleccionado ---" << endl;
    cout << format("{:<5} | {:<22} | {:<15} | {:<8} | {:<10}", "ID", "Nombre", "Código", "Stock",
                   "Precio")
         << endl;
    cout << "------------------------------------------------------------------"
            "-----"
         << endl;
    while (archivo.read(reinterpret_cast<char*>(&producto), sizeof(Producto))) {
        if (!producto.eliminado && producto.idProveedor == idProveedor) {
            cout << format("{:<5} | {:<22} | {:<15} | {:<8} | ${:<9.2f}", producto.id,
                           producto.nombre, producto.codigo, producto.stock, producto.precio)
                 << endl;
            hayProductos = true;
        }
    }
    cout << "------------------------------------------------------------------"
            "-----"
         << endl;

    if (!hayProductos) {
        cout << COLOR_RED
             << "No hay productos activos asociados a este proveedor para "
                "registrar una compra."
             << COLOR_RESET << endl;
    }

    return hayProductos;
}

void registrarCompra() {
    cout << "\n===REGISTRAR COMPRA (Entrada de Mercancia)===" << endl;

    mostrarListaEntidades<Proveedor>("Proveedores", PROVEEDORES_PATH, PorAmbos);
    int idProv = leerId("Ingrese ID del Proveedor para esta compra");
    if (idProv <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }

    if (buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, idProv) == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: El proveedor no existe." << COLOR_RESET
             << endl;
        return;
    }

    if (!mostrarProductosPorProveedor(idProv)) {
        return;
    }

    Transaccion transaccion = {};
    transaccion.tipo = COMPRA;
    transaccion.idRelacionado = idProv;

    while (true) {
        int idProd = leerId("Ingrese ID del Producto");
        if (idProd <= 0) {
            if (transaccion.cantidadTiposDeProductos == 0) {
                cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
                return;
            }
            break;
        }

        int idxProd = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, idProd);
        if (idxProd == -1) {
            cout << CLEAR_SCREEN << COLOR_RED << "Error: El producto con ID " << idProd
                 << " no existe." << COLOR_RESET << endl;
            continue;
        }

        Producto producto;
        ifstream archivoProdRead(PRODUCTOS_PATH, ios::binary);
        archivoProdRead.seekg(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
        archivoProdRead.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

        if (!archivoProdRead || producto.eliminado) {
            cout << CLEAR_SCREEN << COLOR_RED
                 << "Error: No se pudo leer el producto o está eliminado." << COLOR_RESET << endl;
            continue;
        }

        if (producto.idProveedor != idProv) {
            cout << CLEAR_SCREEN << COLOR_RED
                 << "Error: El producto no pertenece al proveedor seleccionado." << COLOR_RESET
                 << endl;
            continue;
        }

        int cantidad = leerId("Cantidad comprada para este producto");
        if (cantidad <= 0) {
            cout << CLEAR_SCREEN << COLOR_RED << "Cantidad inválida. Producto omitido."
                 << COLOR_RESET << endl;
            continue;
        }

        if (!agregarProductoConsolidado(transaccion, idProd, cantidad, producto.precio)) {
            cout << CLEAR_SCREEN << COLOR_RED
                 << "Error: No se pudo agregar más productos (límite alcanzado)." << COLOR_RESET
                 << endl;
            return;
        }

        cout << COLOR_GREEN << "Producto agregado a la compra." << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "¿Desea agregar otro producto? (s/n): " << COLOR_RESET;
        char continuar;
        cin >> continuar;
        if (tolower(continuar) != 's') {
            break;
        }

        mostrarProductosPorProveedor(idProv);
    }

    ArchivoHeader transHeader = leerHeader(TRANSACCIONES_PATH);

    transaccion.id = transHeader.proximoID;
    transaccion.total = 0;
    for (int i = 0; i < transaccion.cantidadTiposDeProductos; i++) {
        transaccion.total += transaccion.cantidades[i] * transaccion.preciosUnitarios[i];
    }
    transaccion.eliminado = false;
    transaccion.fechaCreacion = time(nullptr);
    transaccion.fechaUltimaModificacion = transaccion.fechaCreacion;
    obtenerFechaActual(transaccion.fecha);
    transHeader.cantidadRegistros++;
    transHeader.registrosActivos++;
    transHeader.proximoID++;

    ofstream archivoOut(TRANSACCIONES_PATH, ios::binary | ios::app);
    archivoOut.write(reinterpret_cast<const char*>(&transaccion), sizeof(Transaccion));
    archivoOut.close();

    actualizarHeader(TRANSACCIONES_PATH, transHeader);
    sincronizarContadoresTienda();

    Tienda tiendaCompra;
    if (leerTienda(tiendaCompra)) {
        tiendaCompra.montoTotalCompras += transaccion.total;
        tiendaCompra.fechaUltimaModificacion = time(nullptr);
        actualizarTiendaRegistro(tiendaCompra);
    }

    fstream archivoProdWrite(PRODUCTOS_PATH, ios::binary | ios::in | ios::out);
    if (!archivoProdWrite.is_open()) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: No se pudo abrir archivo de productos para actualizar "
                "stock."
             << COLOR_RESET << endl;
        return;
    }

    for (int i = 0; i < transaccion.cantidadTiposDeProductos; i++) {
        int idxProd = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, transaccion.productosIds[i]);
        if (idxProd == -1) {
            continue;
        }

        Producto producto;
        archivoProdWrite.seekg(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
        archivoProdWrite.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

        if (!archivoProdWrite || producto.eliminado) {
            archivoProdWrite.clear();
            continue;
        }

        producto.stock += transaccion.cantidades[i];
        producto.fechaUltimaModificacion = time(nullptr);

        archivoProdWrite.seekp(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
        archivoProdWrite.write(reinterpret_cast<const char*>(&producto), sizeof(Producto));
        archivoProdWrite.clear();
    }

    cout << "Compra registrada ID: " << transaccion.id << endl;
    cout << "Total de productos distintos en la compra: " << transaccion.cantidadTiposDeProductos
         << endl;
}

void registrarVenta() {
    cout << "\n===REGISTRAR VENTA (Salida de Mercancia)===" << endl;

    ArchivoHeader clientesHeader = leerHeader(CLIENTES_PATH);
    if (clientesHeader.cantidadRegistros == 0) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: No hay clientes registrados. Para registrar una venta, "
                "primero debe "
                "registrar un cliente."
             << COLOR_RESET << endl;
        return;
    }

    mostrarListaEntidades<Producto>("Productos", PRODUCTOS_PATH, PorAmbos);
    // validar cliente
    mostrarListaEntidades<Cliente>("Clientes", CLIENTES_PATH, PorAmbos);

    int idCli = leerId("Ingrese ID del Cliente: ");
    if (idCli <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }
    int idxCli = buscarEntidadPorId<Cliente>(CLIENTES_PATH, idCli);
    if (idxCli == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: El cliente no existe." << COLOR_RESET << endl;
        return;
    }

    Cliente cliente;
    fstream clientesFile(CLIENTES_PATH, ios::binary | ios::in | ios::out);
    if (!clientesFile.is_open()) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: No se pudo abrir el archivo de clientes."
             << COLOR_RESET << endl;
        return;
    }

    clientesFile.seekg(sizeof(ArchivoHeader) + idxCli * sizeof(Cliente), ios::beg);
    clientesFile.read(reinterpret_cast<char*>(&cliente), sizeof(Cliente));

    if (!clientesFile || cliente.eliminado) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: No se pudo leer el cliente o está eliminado."
             << COLOR_RESET << endl;
        return;
    }

    Transaccion transaccion = {};
    transaccion.tipo = VENTA;
    transaccion.idRelacionado = idCli;

    while (true) {
        int idProd = leerId("Ingrese ID del Producto");
        if (idProd <= 0 && transaccion.cantidadTiposDeProductos == 0) {
            cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
            return;
        }

        int idxProd = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, idProd);
        if (idxProd == -1) {
            cout << CLEAR_SCREEN << COLOR_RED << "Error: El producto no existe." << COLOR_RESET
                 << endl;
            continue;
        }

        Producto producto;
        ifstream archivoProdRead(PRODUCTOS_PATH, ios::binary);
        archivoProdRead.seekg(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
        archivoProdRead.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

        if (!archivoProdRead || producto.eliminado) {
            cout << CLEAR_SCREEN << COLOR_RED
                 << "Error: No se pudo leer el producto o está eliminado." << COLOR_RESET << endl;
            continue;
        }

        int cantidad = leerId("Cantidad a vender para este producto");
        if (cantidad <= 0) {
            cout << CLEAR_SCREEN << COLOR_RED << "Cantidad inválida. Producto omitido."
                 << COLOR_RESET << endl;
            continue;
        }

        if (!agregarProductoConsolidado(transaccion, idProd, cantidad, producto.precio)) {
            cout << CLEAR_SCREEN << COLOR_RED
                 << "Error: No se pudo agregar más productos (límite alcanzado)." << COLOR_RESET
                 << endl;
            return;
        }

        cout << COLOR_GREEN << "Producto agregado a la venta." << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "¿Desea agregar otro producto? (s/n): " << COLOR_RESET;
        char continuar;
        cin >> continuar;
        if (tolower(continuar) != 's') {
            break;
        }
    }

    for (int i = 0; i < transaccion.cantidadTiposDeProductos; i++) {
        int idxProd = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, transaccion.productosIds[i]);
        if (idxProd == -1) {
            cout << CLEAR_SCREEN << COLOR_RED << "Error: Producto no encontrado durante validación."
                 << COLOR_RESET << endl;
            return;
        }

        Producto producto;
        ifstream archivoProdRead(PRODUCTOS_PATH, ios::binary);
        archivoProdRead.seekg(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
        archivoProdRead.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

        if (!archivoProdRead || producto.eliminado) {
            cout << CLEAR_SCREEN << COLOR_RED
                 << "Error: Producto inválido durante validación de stock." << COLOR_RESET << endl;
            return;
        }

        if (producto.stock < transaccion.cantidades[i]) {
            cout << CLEAR_SCREEN << COLOR_RED << "Error: Stock insuficiente para "
                 << producto.nombre << ". Solo hay " << producto.stock << " unidades."
                 << COLOR_RESET << endl;
            return;
        }
    }

    ArchivoHeader transHeader = leerHeader(TRANSACCIONES_PATH);
    transaccion.id = transHeader.proximoID;
    transaccion.total = 0;
    for (int i = 0; i < transaccion.cantidadTiposDeProductos; i++) {
        transaccion.total += transaccion.cantidades[i] * transaccion.preciosUnitarios[i];
    }
    transaccion.eliminado = false;
    transaccion.fechaCreacion = time(nullptr);
    transaccion.fechaUltimaModificacion = transaccion.fechaCreacion;
    obtenerFechaActual(transaccion.fecha);

    if (!agregarIdAArreglo(transaccion.id, cliente.transaccionesIds,
                           cliente.cantidadTransacciones)) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: El historial del cliente está lleno. No se puede "
                "registrar la venta."
             << COLOR_RESET << endl;
        return;
    }

    cliente.totalCompras += transaccion.total;
    cliente.fechaUltimaModificacion = time(nullptr);

    transHeader.cantidadRegistros++;
    transHeader.registrosActivos++;
    transHeader.proximoID++;

    ofstream archivoOut(TRANSACCIONES_PATH, ios::binary | ios::app);
    archivoOut.write(reinterpret_cast<const char*>(&transaccion), sizeof(Transaccion));
    archivoOut.close();

    actualizarHeader(TRANSACCIONES_PATH, transHeader);
    sincronizarContadoresTienda();

    Tienda tiendaVenta;
    if (leerTienda(tiendaVenta)) {
        tiendaVenta.montoTotalVentas += transaccion.total;
        tiendaVenta.fechaUltimaModificacion = time(nullptr);
        actualizarTiendaRegistro(tiendaVenta);
    }

    fstream archivoProdWrite(PRODUCTOS_PATH, ios::binary | ios::in | ios::out);
    if (!archivoProdWrite.is_open()) {
        cout << CLEAR_SCREEN << COLOR_RED
             << "Error: No se pudo abrir el archivo de productos para "
                "actualizar stock."
             << COLOR_RESET << endl;
        return;
    }

    for (int i = 0; i < transaccion.cantidadTiposDeProductos; i++) {
        int idxProd = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, transaccion.productosIds[i]);
        if (idxProd == -1) {
            continue;
        }

        Producto producto;
        archivoProdWrite.seekg(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
        archivoProdWrite.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

        if (!archivoProdWrite || producto.eliminado) {
            archivoProdWrite.clear();
            continue;
        }

        producto.stock -= transaccion.cantidades[i];
        producto.totalVendido += transaccion.cantidades[i];
        producto.fechaUltimaModificacion = time(nullptr);

        archivoProdWrite.seekp(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
        archivoProdWrite.write(reinterpret_cast<const char*>(&producto), sizeof(Producto));
        archivoProdWrite.clear();
    }

    clientesFile.seekp(sizeof(ArchivoHeader) + idxCli * sizeof(Cliente), ios::beg);
    clientesFile.write(reinterpret_cast<const char*>(&cliente), sizeof(Cliente));

    // confirmamos venta
    cout << "¡Venta exitosa! Total: " << transaccion.total
         << " | Productos distintos vendidos: " << transaccion.cantidadTiposDeProductos << endl;
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
    cout << "\n================================================================"
            "================"
            "====="
         << endl;
    cout << "                         HISTORIAL COMPLETO DE MOVIMIENTOS" << endl;
    cout << "=================================================================="
            "================"
            "==="
         << endl;
    cout << format("{:<5} | {:<8} | {:<12} | {:<11} | {:<12} | {:<15}", "ID", "Tipo", "Fecha",
                   "Items", "Total", "Asociado ID")
         << endl;
    cout << "------------------------------------------------------------------"
            "----------------"
            "---"
         << endl;

    ifstream archivo(TRANSACCIONES_PATH, ios::binary);
    archivo.seekg(sizeof(ArchivoHeader), ios::beg);
    Transaccion t;
    int count = 0;
    while (archivo.read(reinterpret_cast<char*>(&t), sizeof(Transaccion))) {
        if (!t.eliminado) {
            const char* tipoStr = (t.tipo == COMPRA) ? "COMPRA" : "VENTA";

            cout << format("{:<5} | {:<8} | {:<12} | {:<11} | ${:<11.2f} | {:<15}", t.id, tipoStr,
                           t.fecha, t.cantidadTiposDeProductos, t.total, t.idRelacionado)
                 << endl;
            count++;
        }
    }
    cout << "=================================================================="
            "================"
            "==="
         << endl;
    cout << "Total de registros: " << count << endl;
}

void cancelarTransaccion() {
    ArchivoHeader transactionsHeader = leerHeader(TRANSACCIONES_PATH);
    if (transactionsHeader.registrosActivos == 0) {
        cout << "\n[!] No hay transacciones para cancelar." << endl;
        return;
    }

    int idBuscar = leerId("\nIngrese el ID de la transaccion a cancelar");
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

    if (transaccion.cantidadTiposDeProductos <= 0) {
        cout << "Error: La transaccion no tiene productos asociados." << endl;
        return;
    }

    for (int i = 0; i < transaccion.cantidadTiposDeProductos; i++) {
        int idProducto = transaccion.productosIds[i];
        int cantidad = transaccion.cantidades[i];
        int idxProd = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, idProducto);

        if (idxProd == -1) {
            cout << CLEAR_SCREEN << COLOR_RED << "Error: El producto ID " << idProducto
                 << " de esta transaccion ya no existe en el sistema." << COLOR_RESET << endl;
            return;
        }

        fstream archivoProd(PRODUCTOS_PATH, ios::binary | ios::in | ios::out);
        if (!archivoProd.is_open()) {
            cout << CLEAR_SCREEN << COLOR_RED << "Error: No se pudo abrir productos.bin"
                 << COLOR_RESET << endl;
            return;
        }

        Producto producto;
        archivoProd.seekg(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
        archivoProd.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

        if (!archivoProd || producto.eliminado) {
            cout << CLEAR_SCREEN << COLOR_RED << "Error: Producto inválido al cancelar transacción."
                 << COLOR_RESET << endl;
            return;
        }

        if (transaccion.tipo == VENTA) {
            producto.stock += cantidad;
            if (producto.totalVendido >= cantidad) {
                producto.totalVendido -= cantidad;
            } else {
                producto.totalVendido = 0;
            }
        } else if (transaccion.tipo == COMPRA) {
            if (producto.stock < cantidad) {
                cout << "Error: No se puede cancelar la compra. El stock "
                        "actual de "
                     << producto.nombre << " es menor a lo que quieres quitar." << endl;
                return;
            }
            producto.stock -= cantidad;
        }

        producto.fechaUltimaModificacion = time(nullptr);
        archivoProd.seekp(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
        archivoProd.write(reinterpret_cast<const char*>(&producto), sizeof(Producto));
    }

    Cliente cliente;
    int idxCli = -1;
    bool clienteDisponible = false;
    fstream archivoCli;
    if (transaccion.tipo == VENTA) {
        idxCli = buscarEntidadPorId<Cliente>(CLIENTES_PATH, transaccion.idRelacionado);
        if (idxCli != -1) {
            archivoCli.open(CLIENTES_PATH, ios::binary | ios::in | ios::out);
            if (archivoCli.is_open()) {
                archivoCli.seekg(sizeof(ArchivoHeader) + idxCli * sizeof(Cliente), ios::beg);
                archivoCli.read(reinterpret_cast<char*>(&cliente), sizeof(Cliente));
                clienteDisponible = archivoCli.good() && !cliente.eliminado;
            }
        }
    }

    if (transaccion.tipo == VENTA) {
        if (clienteDisponible) {
            if (cliente.totalCompras >= transaccion.total) {
                cliente.totalCompras -= transaccion.total;
            } else {
                cliente.totalCompras = 0;
            }
            removerIdDeArreglo(transaccion.id, cliente.transaccionesIds,
                               cliente.cantidadTransacciones);
            cliente.fechaUltimaModificacion = time(nullptr);

            archivoCli.seekp(sizeof(ArchivoHeader) + idxCli * sizeof(Cliente), ios::beg);
            archivoCli.write(reinterpret_cast<const char*>(&cliente), sizeof(Cliente));
        } else {
            cout << COLOR_YELLOW
                 << "Advertencia: No se pudo actualizar el historial del "
                    "cliente al cancelar la "
                    "venta."
                 << COLOR_RESET << endl;
        }
    }

    if (archivoCli.is_open()) {
        archivoCli.close();
    }

    if (transaccion.tipo == VENTA) {
        cout << "[SISTEMA] Venta cancelada. Se devolvió el stock de todos los "
                "productos asociados."
             << endl;
    } else if (transaccion.tipo == COMPRA) {
        cout << "[SISTEMA] Compra cancelada. Se retiró el stock de todos los "
                "productos asociados."
             << endl;
    }

    // marcar transaccion eliminada
    transaccion.eliminado = true;
    transaccion.fechaUltimaModificacion = time(nullptr);
    archivoTrans.seekp(sizeof(ArchivoHeader) + idx * sizeof(Transaccion), ios::beg);
    archivoTrans.write(reinterpret_cast<const char*>(&transaccion), sizeof(Transaccion));

    transactionsHeader.registrosActivos--;
    actualizarHeader(TRANSACCIONES_PATH, transactionsHeader);
    sincronizarContadoresTienda();

    Tienda tiendaCancelacion;
    if (leerTienda(tiendaCancelacion)) {
        if (transaccion.tipo == VENTA) {
            if (tiendaCancelacion.montoTotalVentas >= transaccion.total) {
                tiendaCancelacion.montoTotalVentas -= transaccion.total;
            } else {
                tiendaCancelacion.montoTotalVentas = 0;
            }
        } else if (transaccion.tipo == COMPRA) {
            if (tiendaCancelacion.montoTotalCompras >= transaccion.total) {
                tiendaCancelacion.montoTotalCompras -= transaccion.total;
            } else {
                tiendaCancelacion.montoTotalCompras = 0;
            }
        }
        tiendaCancelacion.fechaUltimaModificacion = time(nullptr);
        actualizarTiendaRegistro(tiendaCancelacion);
    }

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

    // Formateamos como "YYYY-MM-DD" y lo guardamos en el buffer
    // (máximo 11 caracteres)
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
                     << "El id debe ser numérico y mayor a 0. Intente "
                        "nuevamente."
                     << COLOR_RESET << endl;
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
        int fileSize = fs::file_size(path);
        string baseName = path.stem().string();
        fs::path ymd_suffix =
            BACKUP_PATH / fs::path(format("{}.{:%Y-%m-%d}.{:%T}.bin", baseName, ymd, localTime));
        fs::copy(path, ymd_suffix);
        cout << format("{}{:>20} {}Respaldado en {}\"{}\"{}", COLOR_CYAN, path.stem().string(),
                       COLOR_GREEN, COLOR_CYAN, ymd_suffix.string(), COLOR_RESET)
             << endl;
        cout << format("{} Tamaño del archivo: {}{} Bytes{}", COLOR_YELLOW, COLOR_CYAN, fileSize,
                       COLOR_RESET)
             << endl;
    }

    cout << format("{}Backup realizado con exito en {}\"{}\"{}", COLOR_GREEN, COLOR_CYAN,
                   BACKUP_PATH.string(), COLOR_RESET)
         << endl;
    cout << format("{}Fecha y hora del backup (Año-Mes-Día) "
                   "(Hora:Minutos:Segundos):{} \"{} {:%T}\"{}",
                   COLOR_YELLOW, COLOR_CYAN, ymd, localTime, COLOR_RESET)
         << endl;
}

void verificarIntegridadReferencial() {
    cout << CLEAR_SCREEN;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;
    cout << COLOR_CYAN << "   DIAGNÓSTICO DE SALUD DE BASE DE DATOS  " << COLOR_RESET << endl;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;

    int erroresProductosProveedor = 0;
    int erroresTransaccionRelacionado = 0;
    int erroresTransaccionProducto = 0;
    int erroresTipoTransaccion = 0;

    // 1. Recorrer productos activos y validar proveedor
    ifstream productosFile(PRODUCTOS_PATH, ios::binary);
    if (!productosFile.is_open()) {
        cout << COLOR_RED << "Error: No se pudo abrir el archivo de productos." << COLOR_RESET
             << endl;
        return;
    }

    ArchivoHeader headerProductos;
    productosFile.read(reinterpret_cast<char*>(&headerProductos), sizeof(ArchivoHeader));

    Producto temp;
    while (productosFile.read(reinterpret_cast<char*>(&temp), sizeof(Producto))) {
        if (temp.eliminado) {
            continue;
        }

        int hasProveedor = buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, temp.idProveedor) != -1;
        if (temp.idProveedor > 0 && !hasProveedor) {
            cout << COLOR_RED << "Error: El proveedor con id " << temp.idProveedor << " no existe."
                 << COLOR_RESET << endl;
            erroresProductosProveedor++;
        }
    }
    productosFile.close();

    // 2. Recorrer transacciones activas y validar relaciones
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
            bool tipoValido = trans.tipo == COMPRA || trans.tipo == VENTA;
            if (!tipoValido) {
                cout << COLOR_RED << "[ERROR - Transaccion ID " << trans.id << "] " << COLOR_RESET
                     << "Tipo de transaccion inválido en datos persistidos." << endl;
                erroresTipoTransaccion++;
            }

            // 2.1 Verificar Cliente/Proveedor relacionado
            if (trans.tipo == COMPRA &&
                buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, trans.idRelacionado) == -1) {
                cout << COLOR_RED << "[ERROR - Transaccion ID " << trans.id << "] " << COLOR_RESET
                     << "Es una COMPRA pero el Proveedor ID " << trans.idRelacionado
                     << " NO EXISTE." << endl;
                erroresTransaccionRelacionado++;
            }

            if (trans.tipo == VENTA &&
                buscarEntidadPorId<Cliente>(CLIENTES_PATH, trans.idRelacionado) == -1) {
                cout << COLOR_RED << "[ERROR - Transaccion ID " << trans.id << "] " << COLOR_RESET
                     << "Es una VENTA pero el Cliente ID " << trans.idRelacionado << " NO EXISTE."
                     << endl;
                erroresTransaccionRelacionado++;
            }

            // 2.2 Verificar Productos asociados a la transacción
            int cantidadProductos = trans.cantidadTiposDeProductos;
            if (cantidadProductos < 0 || cantidadProductos > 100) {
                cout << COLOR_RED << "[ERROR - Transaccion ID " << trans.id << "] " << COLOR_RESET
                     << "cantidadTiposDeProductos fuera de rango: " << cantidadProductos << endl;
                erroresTransaccionProducto++;
                if (cantidadProductos < 0) {
                    cantidadProductos = 0;
                } else {
                    cantidadProductos = 100;
                }
            }

            for (int i = 0; i < cantidadProductos; i++) {
                int idProducto = trans.productosIds[i];
                int hasProductId = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, idProducto) != -1;
                if (!hasProductId) {
                    cout << COLOR_RED << "[ERROR - Transaccion ID " << trans.id << "] "
                         << COLOR_RESET << "Contiene el Producto ID " << idProducto
                         << " que NO EXISTE." << endl;
                    erroresTransaccionProducto++;
                }
            }
        }
    }
    transaccionesFile.close();

    int erroresEncontrados = erroresProductosProveedor + erroresTransaccionRelacionado +
                             erroresTransaccionProducto + erroresTipoTransaccion;

    // 3. Reporte final con subtotales
    cout << "\n----------------------------------------" << endl;
    if (erroresEncontrados == 0) {
        cout << COLOR_GREEN
             << "[+] SALUDABLE: No se encontraron referencias rotas. La "
                "integridad referencial "
                "está intacta."
             << COLOR_RESET << endl;
    } else {
        cout << COLOR_RED << "[-] ADVERTENCIA: Se detectaron " << erroresEncontrados
             << " referencias rotas en la base de datos." << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "Subtotales de errores:" << COLOR_RESET << endl;
        cout << " - Productos -> Proveedor inexistente: " << erroresProductosProveedor << endl;
        cout << " - Transacciones -> Cliente/Proveedor inexistente: "
             << erroresTransaccionRelacionado << endl;
        cout << " - Transacciones -> Producto inexistente / cantidad inválida: "
             << erroresTransaccionProducto << endl;
        cout << " - Transacciones -> Tipo inválido: " << erroresTipoTransaccion << endl;
    }
    cout << "----------------------------------------" << endl;
}

void reporteStockCritico() {
    ifstream productosFile(PRODUCTOS_PATH, ios::binary);
    if (!productosFile.is_open()) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: No se pudo abrir el archivo de productos."
             << COLOR_RESET << endl;
        return;
    }

    ArchivoHeader header;
    productosFile.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
    if (header.registrosActivos == 0) {
        cout << CLEAR_SCREEN << COLOR_YELLOW << "No hay productos registrados." << COLOR_RESET
             << endl;
        return;
    }

    cout << CLEAR_SCREEN;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;
    cout << COLOR_CYAN << "      REPORTE: STOCK CRÍTICO          " << COLOR_RESET << endl;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;
    cout << format("{:<5} | {:<22} | {:<8} | {:<12} | {:<20}", "ID", "Producto", "Stock",
                   "Stock Mín.", "Proveedor")
         << endl;
    cout << "------------------------------------------------------------------"
            "---------"
         << endl;

    int totalCriticos = 0;
    Producto producto;
    while (productosFile.read(reinterpret_cast<char*>(&producto), sizeof(Producto))) {
        if (producto.eliminado || producto.stock > producto.stockMinimo) {
            continue;
        }

        string nombreProveedor = "N/A";
        int idxProv = buscarEntidadPorId<Proveedor>(PROVEEDORES_PATH, producto.idProveedor);
        if (idxProv != -1) {
            ifstream proveedoresFile(PROVEEDORES_PATH, ios::binary);
            if (proveedoresFile.is_open()) {
                Proveedor proveedor;
                proveedoresFile.seekg(sizeof(ArchivoHeader) + idxProv * sizeof(Proveedor),
                                      ios::beg);
                proveedoresFile.read(reinterpret_cast<char*>(&proveedor), sizeof(Proveedor));
                if (proveedoresFile && !proveedor.eliminado) {
                    nombreProveedor = proveedor.nombre;
                }
            }
        }

        cout << format("{:<5} | {:<22} | {:<8} | {:<12} | {:<20}", producto.id, producto.nombre,
                       producto.stock, producto.stockMinimo, nombreProveedor)
             << endl;
        totalCriticos++;
    }

    cout << "------------------------------------------------------------------"
            "---------"
         << endl;
    if (totalCriticos == 0) {
        cout << COLOR_GREEN << "No hay productos en estado crítico." << COLOR_RESET << endl;
    } else {
        cout << COLOR_YELLOW << "Total de productos críticos: " << totalCriticos << COLOR_RESET
             << endl;
    }
}

void reporteHistorialCliente() {
    ArchivoHeader headerClientes = leerHeader(CLIENTES_PATH);
    if (headerClientes.registrosActivos == 0) {
        cout << CLEAR_SCREEN << COLOR_YELLOW << "No hay clientes registrados." << COLOR_RESET
             << endl;
        return;
    }

    mostrarListaEntidades<Cliente>("Clientes", CLIENTES_PATH, PorAmbos);
    int idCliente = leerId("Ingrese el id del cliente");
    if (idCliente <= 0) {
        cout << CLEAR_SCREEN << COLOR_RED << "Operación cancelada." << COLOR_RESET << endl;
        return;
    }

    int idxCliente = buscarEntidadPorId<Cliente>(CLIENTES_PATH, idCliente);
    if (idxCliente == -1) {
        cout << CLEAR_SCREEN << COLOR_RED << "Cliente no encontrado." << COLOR_RESET << endl;
        return;
    }

    Cliente cliente;
    ifstream clientesFile(CLIENTES_PATH, ios::binary);
    clientesFile.seekg(sizeof(ArchivoHeader) + idxCliente * sizeof(Cliente), ios::beg);
    clientesFile.read(reinterpret_cast<char*>(&cliente), sizeof(Cliente));
    if (!clientesFile || cliente.eliminado) {
        cout << CLEAR_SCREEN << COLOR_RED << "No se pudo leer el cliente seleccionado."
             << COLOR_RESET << endl;
        return;
    }

    cout << CLEAR_SCREEN;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;
    cout << COLOR_CYAN << "      REPORTE: HISTORIAL DE CLIENTE    " << COLOR_RESET << endl;
    cout << COLOR_CYAN << "========================================" << COLOR_RESET << endl;
    cout << COLOR_YELLOW << "ID: " << COLOR_RESET << cliente.id << endl;
    cout << COLOR_YELLOW << "Nombre: " << COLOR_RESET << cliente.nombre << endl;
    cout << COLOR_YELLOW << "Cédula: " << COLOR_RESET << cliente.cedula << endl;
    cout << COLOR_YELLOW << "Total Compras: " << COLOR_RESET
         << format("${:.2f}", cliente.totalCompras) << endl;
    cout << COLOR_YELLOW << "Transacciones guardadas: " << COLOR_RESET
         << cliente.cantidadTransacciones << endl;

    int cantidadHistorial = cliente.cantidadTransacciones;
    if (cantidadHistorial < 0) {
        cantidadHistorial = 0;
    }
    if (cantidadHistorial > 100) {
        cantidadHistorial = 100;
    }

    if (cantidadHistorial == 0) {
        cout << COLOR_GREEN << "Este cliente no tiene transacciones en su historial." << COLOR_RESET
             << endl;
        return;
    }

    cout << "\n---------------- TRANSACCIONES ----------------" << endl;
    int transaccionesMostradas = 0;
    for (int i = 0; i < cantidadHistorial; i++) {
        int idTransaccion = cliente.transaccionesIds[i];
        if (idTransaccion <= 0) {
            continue;
        }

        int idxTrans = buscarEntidadPorId<Transaccion>(TRANSACCIONES_PATH, idTransaccion);
        if (idxTrans == -1) {
            cout << COLOR_YELLOW << "[Aviso] Transacción ID " << idTransaccion
                 << " no encontrada o eliminada." << COLOR_RESET << endl;
            continue;
        }

        Transaccion transaccion;
        ifstream transaccionesFile(TRANSACCIONES_PATH, ios::binary);
        transaccionesFile.seekg(sizeof(ArchivoHeader) + idxTrans * sizeof(Transaccion), ios::beg);
        transaccionesFile.read(reinterpret_cast<char*>(&transaccion), sizeof(Transaccion));
        if (!transaccionesFile || transaccion.eliminado) {
            continue;
        }

        if (transaccion.tipo != VENTA || transaccion.idRelacionado != cliente.id) {
            cout << COLOR_YELLOW << "[Aviso] Transacción ID " << transaccion.id
                 << " no pertenece a una venta válida de este cliente." << COLOR_RESET << endl;
            continue;
        }

        const char* tipoStr = transaccion.tipo == COMPRA ? "COMPRA" : "VENTA";
        cout << "\nTransacción #" << transaccion.id << " | Tipo: " << tipoStr
             << " | Fecha: " << transaccion.fecha
             << " | Total: " << format("${:.2f}", transaccion.total) << endl;
        cout << format("{:<10} | {:<22} | {:<10} | {:<12} | {:<12}", "Prod ID", "Producto",
                       "Cantidad", "Precio U.", "Subtotal")
             << endl;
        cout << "--------------------------------------------------------------"
                "---------"
             << endl;

        int cantidadItems = transaccion.cantidadTiposDeProductos;
        if (cantidadItems < 0) {
            cantidadItems = 0;
        }
        if (cantidadItems > 100) {
            cantidadItems = 100;
        }

        for (int j = 0; j < cantidadItems; j++) {
            int idProducto = transaccion.productosIds[j];
            int cantidad = transaccion.cantidades[j];
            float precioUnitario = transaccion.preciosUnitarios[j];
            float subtotal = cantidad * precioUnitario;

            string nombreProducto = "(No encontrado)";
            int idxProd = buscarEntidadPorId<Producto>(PRODUCTOS_PATH, idProducto);
            if (idxProd != -1) {
                Producto producto;
                ifstream productosFile(PRODUCTOS_PATH, ios::binary);
                productosFile.seekg(sizeof(ArchivoHeader) + idxProd * sizeof(Producto), ios::beg);
                productosFile.read(reinterpret_cast<char*>(&producto), sizeof(Producto));
                if (productosFile && !producto.eliminado) {
                    nombreProducto = producto.nombre;
                }
            }

            cout << format("{:<10} | {:<22} | {:<10} | ${:<11.2f} | ${:<11.2f}", idProducto,
                           nombreProducto, cantidad, precioUnitario, subtotal)
                 << endl;
        }

        transaccionesMostradas++;
    }

    cout << "\nTransacciones válidas mostradas: " << transaccionesMostradas << endl;
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

                // 3. Actualizar el NUEVO proveedor (agregar producto a su
                // lista)
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

                // 4. Actualizar el VIEJO proveedor (remover producto de su
                // lista)
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
        cout << format("{} {} Error: No hay proveedores registrados. Debe crear al menos "
                       "un "
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
                            {"Crear Backup", crearBackup},
                            {"Productos con stock crítico", reporteStockCritico},
                            {"Historial de Cliente", reporteHistorialCliente},
                            {"Resumen de Tienda", mostrarResumenTienda}};
    drawMenu("Reportes", options, 5);
}

int main() {
    setlocale(LC_ALL, "Spanish");
    inicializarTienda("Papaya Store", "J-123456789");
    sincronizarContadoresTienda();

    OpcionMenu options[] = {{"Gestión de Productos", menuProductos},
                            {"Gestión de Proveedores", menuProveedores},
                            {"Gestión de Clientes", menuClientes},
                            {"Gestión de Transacciones", menuTransacciones},
                            {"Gestión de Reportes y seguridad", menuReportes}};

    drawMenu("PAPAYA STORE - Menú Principal", options, 5, "Salir");

    return 0;
}
