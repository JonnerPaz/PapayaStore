#include <concepts>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <variant>

using namespace std;

struct Tienda;
int buscarProductoPorId(Tienda* tienda, int id);
// retorna array de indices del producto
int* buscarProductosPorNombre(Tienda* tienda, const char* nombre);
void listarProductos(Tienda* tienda);
void convertirAMinusculas(char* cadena);
bool contieneSubstring(const char* cadena, const char* subcadena);

enum TipoDeTransaccion { COMPRA, VENTA };
enum Busqueda {
    BusquedaId = '1',
    BusquedaNombre = '2',
    BusquedaMostrar = '3',
    BusquedaCancelada = '0'
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

void inicializarTienda(Tienda* tienda, const char* nombre, const char* rif) {
    const int CAPACIDAD_INICIAL = 5;

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

    // empiezan en 0 porque es el primer elemento del array
    tienda->siguienteIdProducto = 0;
    tienda->siguienteIdProveedor = 0;
    tienda->siguienteIdCliente = 0;
    tienda->siguienteIdTransaccion = 0;
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

    tienda->siguienteIdProducto = 0;
    tienda->siguienteIdProveedor = 0;
    tienda->siguienteIdCliente = 0;
    tienda->siguienteIdTransaccion = 0;
}

void crearProducto(Tienda* tienda) {
    if (tienda == nullptr) {
        cout << "Error: La tienda no ha sido creada." << endl;
        return;
    }

    int index = tienda->siguienteIdProducto;
    char confirmar;

    char* nombre = new char[100];
    asignarPropiedadString("Ingrese el nombre del producto (q para cancelar): ", nombre, 100);

    char* codigo = new char[20];
    asignarPropiedadString("Ingrese el código del producto (q para cancelar): ", codigo, 20);

    char* descripcion = new char[200];
    asignarPropiedadString("Ingrese la descripcion del producto (q para cancelar): ", descripcion,
                           200);

    float* precio = new float;
    asignarPropiedadNum<float&>("Ingrese el precio del producto (q para cancelar): ", *precio);

    int* stock = new int;
    asignarPropiedadNum<int&>("Ingrese el stock del producto (q para cancelar): ", *stock);

    int* idProveedor = new int;
    asignarPropiedadNum<int&>("Ingresar el id del proveedor (q para cancelar): ", *idProveedor);

    cout << "Los datos del producto son: " << endl;
    cout << "Nombre: " << nombre << endl;
    cout << "Codigo: " << codigo << endl;
    cout << "Descripcion: " << descripcion << endl;
    cout << "Precio: " << precio << endl;
    cout << "Stock: " << stock << endl;
    cout << "Proveedor: " << *idProveedor << endl;

    cout << "Está seguro de crear el producto? (s/n): ";
    cin >> confirmar;

    if (confirmar == 's' || confirmar == 'S') {
        Producto& producto = tienda->productos[index];
        producto.id = tienda->siguienteIdProducto;
        strncpy(producto.nombre, nombre, sizeof(producto.nombre) - 1);
        strncpy(producto.codigo, codigo, sizeof(producto.codigo) - 1);
        strncpy(producto.descripcion, descripcion, sizeof(producto.descripcion) - 1);
        producto.precio = *precio;
        producto.stock = *stock;

        // siguiente producto tiene su espacio en el array
        tienda->siguienteIdProducto++;
    } else {
        cout << "Producto no creado." << endl;
    }

    delete[] nombre;
    delete[] codigo;
    delete[] descripcion;
    delete idProveedor;
    nombre = nullptr;
    codigo = nullptr;
    descripcion = nullptr;
    idProveedor = nullptr;
}

void buscarProducto(Tienda* tienda) {
    while (true) {
        cout << "Buscar producto" << endl;
        cout << "1. Id" << endl
             << "2. Nombre" << endl
             << "3. Listar todos" << endl
             << "0. Cancelar" << endl;
        cout << "Seleccione una opción: ";

        char opcion;
        cin >> opcion;

        switch (opcion) {
            // id
        case BusquedaId: {
            cout << "Ingrese el id del producto: ";

            int id;
            cin >> id;
            if (id <= 0) {
                cout << "El id debe ser mayor a 0. Intente nuevamente: ";
                break;
            }

            int index = buscarProductoPorId(tienda, id);
            if (index == -1) {
                cout << "Producto no encontrado." << endl;
                break;
            }

            Producto& producto = tienda->productos[index];
            cout << "Producto encontrado:" << endl;
            cout << "Id: " << producto.id << endl;
            cout << "Nombre: " << producto.nombre << endl;
            cout << "Codigo: " << producto.codigo << endl;
            cout << "Descripcion: " << producto.descripcion << endl;
            cout << "Precio: " << producto.precio << endl;
            cout << "Stock: " << producto.stock << endl;
            break;
        }
        case BusquedaNombre: {
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
                cout << "Id: " << producto.id << endl;
                cout << "Nombre: " << producto.nombre << endl;
                cout << "Codigo: " << producto.codigo << endl;
                cout << "Descripcion: " << producto.descripcion << endl;
                cout << "Precio: " << producto.precio << endl;
                cout << "Stock: " << producto.stock << endl;
            }
            delete[] index;
            break;
        }

        case BusquedaMostrar: {
            listarProductos(tienda);
            break;
        }
        case BusquedaCancelada: {
            cout << "Cancelada la búsqueda" << endl;
            return;
        }
        default: {
            cout << "Opcion no valida" << endl;
            break;
        }
        }
    }
}

void actualizarProducto(Tienda* tienda) {
}

void actualizarStockProducto(Tienda* tienda) {
}

void listarProductos(Tienda* tienda) {
}

void eliminarProducto(Tienda* tienda) {
}

void crearProveedor(Tienda* tienda) {
}

void buscarProveedor(Tienda* tienda) {
}

void actualizarProveedor(Tienda* tienda) {
}

void listarProveedores(Tienda* tienda) {
}

void eliminarProveedor(Tienda* tienda) {
}

void crearCliente(Tienda* tienda) {
}

void buscarCliente(Tienda* tienda) {
}

void actualizarCliente(Tienda* tienda) {
}

void listarClientes(Tienda* tienda) {
}

void eliminarCliente(Tienda* tienda) {
}

void registrarCompra(Tienda* tienda) {
}

void registrarVenta(Tienda* tienda) {
}

void buscarTransacciones(Tienda* tienda) {
}

void listarTransacciones(Tienda* tienda) {
}

void cancelarTransaccion(Tienda* tienda) {
}

void redimensionarProductos(Tienda* tienda) {
}

void redimensionarProveedores(Tienda* tienda) {
}

void redimensionarClientes(Tienda* tienda) {
}

void redimensionarTransacciones(Tienda* tienda) {
}

///////////// FIN FUNCIONES CRUD DEL PROGRAMA //////////

// VALIDACIONES
bool validarEmail(const char* email) {
    // Implementar la lógica para validar el correo electrónica
    return true;
}

bool validarFecha(const char* fecha) {
    // Implementar la lógica para validar la fecha
    return true;
}

bool existeProducto(Tienda* tienda, int id) {
    // Implementar la lógica para verificar la existencia del producto
    return true;
}

bool existeProveedor(Tienda* tienda, int id) {
    // Implementar la lógica para verificar la existencia del proveedor
    return true;
}

bool existeCliente(Tienda* tienda, int id) {
    // Implementar la lógica para verificar la existencia del cliente
    return true;
}

bool codigoDuplicado(Tienda* tienda, const char* codigo) {
    // Implementar la lógica para verificar la duplicidad de códigos
    return true;
}

bool rifDuplicado(Tienda* tienda, const char* rif) {
    // Implementar la lógica para verificar la duplicidad de RIF
    return true;
}

// Búsquedas
int buscarProductoPorId(Tienda* tienda, int id) {
    return -1;
}

int buscarProveedorPorId(Tienda* tienda, int id) {
    return -1;
}

int buscarClientePorId(Tienda* tienda, int id) {
    return -1;
}

// retorna array de indices del producto
int* buscarProductosPorNombre(Tienda* tienda, const char* nombre) {
    return nullptr;
}

//// UTILIDADES

// formato YYYY-MM-DD
void obtenerFechaActual(char* fecha) {
}

void convertirAMinusculas(char* cadena) {
}

bool contieneSubstring(const char* cadena, const char* subcadena) {
    return false;
}

int main() {
    return 0;
}
