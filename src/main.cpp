#include <cstring>
#include <format>
#include <iostream>
#include <limits>
#include <type_traits>

using namespace std;

struct Tienda;
int buscarProductoPorId(Tienda* tienda, int id);
int buscarProveedorPorId(Tienda* tienda, int id);
int buscarClientePorId(Tienda* tienda, int id);
int leerId(const char* msg);
void obtenerFechaActual(char* fecha);
// retorna array de indices del producto
int* buscarProductosPorNombre(Tienda* tienda, const char* nombre);
void listarProductos(Tienda* tienda);
void convertirAMinusculas(char* cadena);
bool contieneSubstring(const char* cadena, const char* subcadena);
void redimensionarProductos(Tienda* tienda);
void redimensionarProveedores(Tienda* tienda);
void redimensionarClientes(Tienda* tienda);
void redimensionarTransacciones(Tienda* tienda);

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

// templates utilizados en el programa
template <typename T>
// remove_reference_t remueve las referencias de T y devuelve T
// Ej. si T es int& -> devuelve int
concept AsignarNum = std::is_arithmetic_v<std::remove_reference_t<T>>;

void asignarPropiedadNum(string msg, AsignarNum auto& prop) {
    cout << msg << endl;
    while (true) {
        cin >> prop;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "El valor debe ser numérico. Intente nuevamente: ";
            continue;
        }
        if (prop >= 0) {
            break;
        }
        cout << "El valor debe ser mayor o igual a 0. Intente nuevamente: ";
    }
    cout << endl;
}

void asignarPropiedadString(string msg, char* prop, int str_length) {
    cout << msg << endl;
    if (cin.peek() == '\n') {
        cin.ignore();
    }
    cin.getline(prop, str_length);
    cout << endl;
}

///// FUNCIONES CRUD DEL PROGRAMA
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

void crearProducto(Tienda* tienda) {
    if (tienda == nullptr) {
        cout << "Error: La tienda no ha sido creada." << endl;
        return;
    }

    if (tienda->numProductos >= tienda->capacidadProductos) {
        redimensionarProductos(tienda);
    }

    int index = tienda->numProductos;
    char confirmar;

    char nombre[100];
    asignarPropiedadString("Ingrese el nombre del producto (q para cancelar): ", nombre, 100);
    if (nombre[0] == 'q' && nombre[1] == '\0')
        return;

    char codigo[20];
    asignarPropiedadString("Ingrese el código del producto (q para cancelar): ", codigo, 20);
    if (codigo[0] == 'q' && codigo[1] == '\0')
        return;

    char descripcion[200];
    asignarPropiedadString("Ingrese la descripcion del producto (q para cancelar): ", descripcion,
                           200);
    if (descripcion[0] == 'q' && descripcion[1] == '\0')
        return;

    float precio;
    asignarPropiedadNum("Ingrese el precio del producto: ", precio);

    int stock;
    asignarPropiedadNum("Ingrese el stock del producto: ", stock);

    int idProveedor;
    asignarPropiedadNum("Ingresar el id del proveedor: ", idProveedor);

    cout << "Los datos del producto son: " << endl;
    cout << "Nombre: " << nombre << endl;
    cout << "Codigo: " << codigo << endl;
    cout << "Descripcion: " << descripcion << endl;
    cout << "Precio: " << precio << endl;
    cout << "Stock: " << stock << endl;
    cout << "Proveedor: " << idProveedor << endl;

    cout << "Está seguro de crear el producto? (s/n): ";
    cin >> confirmar;

    if (confirmar == 's' || confirmar == 'S') {
        Producto& producto = tienda->productos[index];
        producto.id = tienda->siguienteIdProducto;
        producto.idProveedor = idProveedor;
        strncpy(producto.nombre, nombre, sizeof(producto.nombre) - 1);
        producto.nombre[sizeof(producto.nombre) - 1] = '\0';
        strncpy(producto.codigo, codigo, sizeof(producto.codigo) - 1);
        producto.codigo[sizeof(producto.codigo) - 1] = '\0';
        strncpy(producto.descripcion, descripcion, sizeof(producto.descripcion) - 1);
        producto.descripcion[sizeof(producto.descripcion) - 1] = '\0';
        producto.precio = precio;
        producto.stock = stock;

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
        cout << "Buscar producto" << endl;
        cout << "1. Id" << endl
             << "2. Nombre" << endl
             << "3. Listar todos" << endl
             << "0. Cancelar" << endl;
        cout << "Seleccione una opción: ";

        cin >> opcion;
        switch (opcion) {
            // id
        case BusquedaId: {
            cout << "Ingrese el id del producto: ";

            int id;
            cin >> id;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "El id debe ser numérico. Intente nuevamente: ";
                break;
            }

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
    } while (opcion != BusquedaCancelada);
}

template <typename T> void manejarPropiedad(const string& nombrePropiedad, T& propiedad) {
    char confirmar;

    //  Descripcion, Nombre, Codigo
    if constexpr (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>) {
        char tempProp[200];
        cout << format("Ingrese el nuevo {}: ", nombrePropiedad);
        if (cin.peek() == '\n')
            cin.ignore();
        cin.getline(tempProp, sizeof(T));

        cout << format("Viejo {}: {}", nombrePropiedad, propiedad) << endl;
        cout << format("Nuevo {}: {}", nombrePropiedad, tempProp) << endl;

        cout << format("Está seguro que desea actualizar el {}? (s/n): ", nombrePropiedad);
        cin >> confirmar;
        if (confirmar == 's' || confirmar == 'S') {
            strncpy(propiedad, tempProp, sizeof(T) - 1);
            propiedad[sizeof(T) - 1] = '\0';
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
                cout << "El valor debe ser numérico. Intente nuevamente: ";
                continue;
            }
            if (tempProp >= 0)
                break;
            cout << "El valor debe ser mayor o igual a 0. Intente nuevamente: ";
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

    int id = leerId("Ingresa el id del producto a actualizar");
    if (id <= 0) {
        cout << "No se actualizara ningun producto." << endl;
        return;
    }

    int index = buscarProductoPorId(tienda, id);
    if (index == -1) {
        cout << "Producto a actualizar no encontrado." << endl;
        return;
    }

    Producto& producto = tienda->productos[index];
    cout << format("Producto con el id {} encontrado: {}", id, producto.nombre) << endl;

    char opcion;
    do {
        cout << "¿Qué desea actualizar?: " << endl;
        cout << "1. Nombre" << endl;
        cout << "2. Codigo" << endl;
        cout << "3. Descripcion" << endl;
        cout << "4. Precio" << endl;
        cout << "5. Stock" << endl;
        cout << "0. Cancelar" << endl;

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
        case ActualizarCancelada:
            cout << "Se canceló la actualizacion." << endl;
            break;
        default:
            cout << "Opcion no valida" << endl;
            break;
        }
    } while (opcion != ActualizarCancelada);
}

void actualizarStockProducto(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    cout << "Ingrese el id del producto para actualizar el stock: ";
    int id;
    do {
        cin >> id;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "El id debe ser numérico. Intente nuevamente." << endl;
        }
    } while (id <= 0);

    if (id <= 0) {
        cout << "El id debe ser mayor a 0. Intente nuevamente." << endl;
        return;
    }

    int index = buscarProductoPorId(tienda, id);
    if (index == -1) {
        cout << "Producto no encontrado." << endl;
        return;
    }

    Producto& producto = tienda->productos[index];
    cout << format("Producto: {} | Stock actual: {}", producto.nombre, producto.stock) << endl;

    char opcion;
    do {
        cout << "Seleccione la operación a realizar: " << endl;
        cout << "1. Incrementar stock" << endl;
        cout << "2. Disminuir stock" << endl;
        cout << "0. Cancelar" << endl;
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
                    cout << "El valor debe ser numérico. Intente nuevamente: ";
                    continue;
                }
                if (cantidad > 0)
                    break;
                cout << "La cantidad debe ser mayor a 0. Intente nuevamente: ";
            }

            int nuevoStock = producto.stock;
            if (opcion == '1') {
                nuevoStock += cantidad;
            }

            if (opcion == '2') {
                nuevoStock -= cantidad;
                if (nuevoStock < 0) {
                    cout << format("Error: El stock no puede ser negativo. Faltan unidades. Stock "
                                   "actual: {}",
                                   producto.stock)
                         << endl;
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
            cout << "Operación cancelada." << endl;
            break;
        default:
            cout << "Opción no válida." << endl;
            break;
        }

    } while (opcion != '0');
}

void listarProductos(Tienda* tienda) {
    if (tienda == nullptr) {
        cout << "Error: La tienda no ha sido creada." << endl;
        return;
    }

    if (tienda->numProductos == 0) {
        cout << "No hay productos registrados." << endl;
        return;
    }

    cout << "--- Lista de Productos (" << tienda->numProductos << ") ---" << endl;
    for (int i = 0; i < tienda->numProductos; ++i) {
        Producto& p = tienda->productos[i];
        cout << "Id: " << p.id << " | Nombre: " << p.nombre << " | Codigo: " << p.codigo
             << " | Precio: $" << p.precio << " | Stock: " << p.stock << endl;
    }
    cout << "--------------------------" << endl;
}

void eliminarProducto(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    int id = leerId("Ingresa el id del producto a actualizar");
    if (id <= 0) {
        cout << "No se eliminó el producto." << endl;
        return;
    }

    int index = buscarProductoPorId(tienda, id);
    if (index == -1) {
        cout << "Producto no encontrado." << endl;
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
        cout << "\n¡ADVERTENCIA! Este producto tiene transacciones asociadas." << endl;
        cout << "Eliminarlo puede causar inconsistencias en el historial de transacciones." << endl;
    }

    cout << "\n¿Está seguro que desea eliminar este producto? (s/n): ";
    char confirmar;
    cin >> confirmar;

    if (confirmar == 's' || confirmar == 'S') {
        // Eliminar físicamente moviendo elementos a la izquierda
        for (int i = index; i < tienda->numProductos - 1; ++i) {
            tienda->productos[i] = tienda->productos[i + 1];
        }
        tienda->numProductos--;
        cout << "Producto eliminado exitosamente." << endl;
    } else {
        cout << "Eliminación cancelada." << endl;
    }
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
    if (tienda == nullptr)
        return;
    // doble capacidad
    int nuevaCapacidad = tienda->capacidadProductos * 2;
    Producto* nuevosProductos = new Producto[nuevaCapacidad];
    for (int i = 0; i < tienda->numProductos; ++i) {
        nuevosProductos[i] = tienda->productos[i];
    }
    delete[] tienda->productos;
    tienda->productos = nuevosProductos;
    tienda->capacidadProductos = nuevaCapacidad;
}

void redimensionarProveedores(Tienda* tienda) {
    if (tienda == nullptr)
        return;
    int nuevaCapacidad = tienda->capacidadProveedores * 2;
    Proveedor* nuevosProveedores = new Proveedor[nuevaCapacidad];
    for (int i = 0; i < tienda->numProveedores; ++i) {
        nuevosProveedores[i] = tienda->proveedores[i];
    }
    delete[] tienda->proveedores;
    tienda->proveedores = nuevosProveedores;
    tienda->capacidadProveedores = nuevaCapacidad;
}

void redimensionarClientes(Tienda* tienda) {
    if (tienda == nullptr)
        return;
    int nuevaCapacidad = tienda->capacidadClientes * 2;
    Cliente* nuevosClientes = new Cliente[nuevaCapacidad];
    for (int i = 0; i < tienda->numClientes; ++i) {
        nuevosClientes[i] = tienda->clientes[i];
    }
    delete[] tienda->clientes;
    tienda->clientes = nuevosClientes;
    tienda->capacidadClientes = nuevaCapacidad;
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
    if (tienda == nullptr)
        return -1;
    for (int i = 0; i < tienda->numProductos; ++i) {
        if (tienda->productos[i].id == id) {
            return i;
        }
    }
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
        int id;
        cin >> id;

        if (cin.peek() == 'q') {
            return -1;
        }

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "El id debe ser un número" << endl;
            cout << "Habrás querido salir? presiona 'q' para salir" << endl;
            continue;
        }

        if (id <= 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "El id debe ser numérico y mayor o igual a 0. Intente nuevamente" << endl;
        }

        return id;
    }
}

int main() {
    return 0;
}
