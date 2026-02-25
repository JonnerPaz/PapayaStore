#include <cstring>
#include <format>
#include <iostream>
#include <limits>
#include <type_traits>
#include <chrono>
#include <string>
#include <sstream>

using namespace std;

struct Tienda;
template <typename T> int buscarEntidadPorId(T* array, int count, int id);
int leerId(const char* msg);
void obtenerFechaActual(char* fecha);
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
void asignarPropiedadNum(const char* msg, AsignarNum auto& prop) {
    cout << msg << endl;
    while (true) {
        cin >> prop;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "El valor debe ser numérico. Intente nuevamente: ";
            continue;
        }
        // Clean buffer no matter if cin fail or not
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (prop >= 0) {
            break;
        }
        cout << "El valor debe ser mayor o igual a 0. Intente nuevamente: ";
    }
    cout << endl;
}

template <size_t N> void asignarPropiedadString(const char* msg, char (&prop)[N]) {
    cout << msg << endl;
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

void crearProducto(Tienda* tienda) {
    if (tienda == nullptr) {
        cout << "Error: La tienda no ha sido creada." << endl;
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
    asignarPropiedadString("Ingrese el código del producto (q para cancelar): ", codigo);
    if (codigo[0] == 'q' && codigo[1] == '\0')
        return;

    char descripcion[200];
    asignarPropiedadString("Ingrese la descripcion del producto (q para cancelar): ", descripcion);
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
        copiarString(producto.nombre, nombre);
        copiarString(producto.codigo, codigo);
        copiarString(producto.descripcion, descripcion);
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

            int index = buscarEntidadPorId(tienda->productos, tienda->numProductos, id);
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

    int index = buscarEntidadPorId(tienda->productos, tienda->numProductos, id);
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

    int index = buscarEntidadPorId(tienda->productos, tienda->numProductos, id);
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

    int index = buscarEntidadPorId(tienda->productos, tienda->numProductos, id);
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
        eliminarElementoDeArray(tienda->productos, index, tienda->numProductos);
        cout << "Producto eliminado exitosamente." << endl;
    } else {
        cout << "Eliminación cancelada." << endl;
    }
}

void crearProveedor(Tienda* tienda) {
    if (tienda == nullptr) {
        cout << "Error: La tienda no ha sido creada." << endl;
        return;
    }
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
    asignarPropiedadString("Ingrese el RIF del proveedor (q para cancelar): ", rif);
    if (rif[0] == 'q' && rif[1] == '\0')
        return;

    char telefono[20];
    asignarPropiedadString("Ingrese el telefono del proveedor: ", telefono);
    char email[100];
    asignarPropiedadString("Ingrese el email del proveedor: ", email);
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
    cout << "Ingrese el id del proveedor a buscar: ";
    int id;
    cin >> id;
    if (cin.fail() || id <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Id invalido." << endl;
        return;
    }
    int index = buscarEntidadPorId(tienda->proveedores, tienda->numProveedores, id);
    if (index == -1) {
        cout << "Proveedor no encontrado." << endl;
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
    cout << "Ingrese el id del proveedor a actualizar: ";
    int id;
    cin >> id;
    if (cin.fail() || id <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Id invalido." << endl;
        return;
    }
    int index = buscarEntidadPorId(tienda->proveedores, tienda->numProveedores, id);
    if (index == -1) {
        cout << "Proveedor no encontrado." << endl;
        return;
    }
    Proveedor& p = tienda->proveedores[index];

    char opcion;
    do {
        cout << "¿Qué desea actualizar?: " << endl;
        cout << "1. Nombre\n2. RIF\n3. Telefono\n4. Email\n5. Direccion\n0. Cancelar\n";
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
            cout << "Saliendo..." << endl;
            break;
        default:
            cout << "Opcion no valida" << endl;
        }
    } while (opcion != '0');
}

void listarProveedores(Tienda* tienda) {
    if (tienda == nullptr || tienda->numProveedores == 0) {
        cout << "No hay proveedores registrados." << endl;
        return;
    }
    cout << "--- Lista de Proveedores (" << tienda->numProveedores << ") ---" << endl;
    for (int i = 0; i < tienda->numProveedores; ++i) {
        Proveedor& p = tienda->proveedores[i];
        cout << "Id: " << p.id << " | Nombre: " << p.nombre << " | RIF: " << p.rif
             << " | Telefono: " << p.telefono << " | Email: " << p.email << endl;
    }
    cout << "--------------------------" << endl;
}

void eliminarProveedor(Tienda* tienda) {
    if (tienda == nullptr)
        return;
    cout << "Ingrese el id del proveedor a eliminar: ";
    int id;
    cin >> id;
    if (cin.fail() || id <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Id invalido." << endl;
        return;
    }
    int index = buscarEntidadPorId(tienda->proveedores, tienda->numProveedores, id);
    if (index == -1) {
        cout << "Proveedor no encontrado." << endl;
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
    if (tienda == nullptr) {
        cout << "Error: La tienda no ha sido creada." << endl;
        return;
    }
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
    asignarPropiedadString("Ingrese el email del cliente: ", email);
    char direccion[200];
    asignarPropiedadString("Ingrese la direccion del cliente: ", direccion);

    cout << "Está seguro de crear el cliente? (s/n): ";
    char confirmar;
    cin >> confirmar;
    if (confirmar == 's' || confirmar == 'S') {
        Cliente& cli = tienda->clientes[index];
        cli.id = tienda->siguienteIdCliente++;
        copiarString(cli.nombre, nombre);
        copiarString(cli.cedula, cedula);
        copiarString(cli.telefono, telefono);
        copiarString(cli.email, email);
        copiarString(cli.direccion, direccion);
        obtenerFechaActual(cli.fechaRegistro);
        tienda->numClientes++;
        cout << "Cliente creado con exito." << endl;
    } else {
        cout << "Cliente no creado." << endl;
    }
}

void buscarCliente(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    int id = leerId("Ingrese el id del cliente a buscar");
    if (id <= 0) {
        cout << "No se actualizara ningun producto." << endl;
        return;
    }

    int index = buscarEntidadPorId(tienda->clientes, tienda->numClientes, id);
    if (index == -1) {
        cout << "Cliente no encontrado." << endl;
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

    int id = leerId("Ingrese el id del cliente a actualizar");
    if (id <= 0) {
        cout << "No se actualizara ningun producto." << endl;
        return;
    }

    int index = buscarEntidadPorId(tienda->clientes, tienda->numClientes, id);
    if (index == -1) {
        cout << "Cliente no encontrado." << endl;
        return;
    }
    Cliente& clientes = tienda->clientes[index];

    char opcion;
    do {
        cout << "¿Qué desea actualizar?: " << endl;
        cout << "1. Nombre\n2. Cedula\n3. Telefono\n4. Email\n5. Direccion\n0. Cancelar\n";
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
            cout << "Saliendo..." << endl;
            break;
        default:
            cout << "Opcion no valida" << endl;
        }
    } while (opcion != '0');
}

void listarClientes(Tienda* tienda) {
    if (tienda == nullptr || tienda->numClientes == 0) {
        cout << "No hay clientes registrados." << endl;
        return;
    }
    cout << "--- Lista de Clientes (" << tienda->numClientes << ") ---" << endl;
    for (int i = 0; i < tienda->numClientes; ++i) {
        Cliente& cliente = tienda->clientes[i];
        cout << "Id: " << cliente.id << " | Nombre: " << cliente.nombre
             << " | Cedula: " << cliente.cedula << " | Telefono: " << cliente.telefono
             << " | Email: " << cliente.email << endl;
    }
    cout << "--------------------------" << endl;
}

void eliminarCliente(Tienda* tienda) {
    if (tienda == nullptr)
        return;

    int id = leerId("Ingrese el id del cliente a eliminar");
    if (id <= 0) {
        cout << "No se actualizara ningun producto." << endl;
        return;
    }

    int index = buscarEntidadPorId(tienda->clientes, tienda->numClientes, id);
    if (index == -1) {
        cout << "Cliente no encontrado." << endl;
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
}

void registrarVenta(Tienda* tienda) {
}

void buscarTransacciones(Tienda* tienda) {
}

void listarTransacciones(Tienda* tienda) {
}

void cancelarTransaccion(Tienda* tienda) {
}

///////////// FIN FUNCIONES CRUD DEL PROGRAMA //////////

// VALIDACIONES
bool validarEmail(const char* email) {
    //Seguridad para ver si el puntero existe
    if (email == nullptr)
    {
    return false;
    }
    int PosicionAt = -1;                 //verificar y darle valor al "@"
    bool TienePuntoDespues = false;      //para comprobar que tenga "." despues del "@"
    int longitud = strlen(email);        //para que el ciclo for sepa hasta donde buscar
    
    //la funcion "for" para recorrer todo el email
    for (int i = 0; i < longitud; i++)
    {
    if (email[i] == '@')
        {
    //verificacion si ya habia un @ y hay dos @
        if(PosicionAt != -1) return false;   
        PosicionAt = i;
        }
    //verificacion de si hay un '.' despues de ya tener el '@'   
    if (PosicionAt != -1 && email[i] == '.')
        {
        if(i > PosicionAt + 1)
            {
             TienePuntoDespues = true;
            }
        }
    
    }
    // verificacion de los errores 
    if (PosicionAt > 0 && PosicionAt < longitud -1 && TienePuntoDespues)
    {
    return true;
    }
    // si el email es invalido
    cout << "ERROR: Formato de email invalido." << endl;
    return false;
    
}
bool validarFecha(const char* fecha) {
    //estos solo estan vivios mientras se este utilizando esta funcion
    using namespace std;
    using namespace std::chrono;

    //extraemos los datos de YYYY-MM-DD
    stringstream ss(fecha);
    int y, m, d;
    char dash1, dash2;

    //verificar si es correcto el formato
    if (!(ss >> y >> dash1 >> m >> dash2 >> d)) {
        return false; 
    }

    //para ver que tenga los guiones donde van 
    if (dash1 != '-' || dash2 != '-') {
        return false;

        // year_month_day representa una fecha en el calendario civil
       year_month_day ymd{year{y}, month{(unsigned)m}, day{(unsigned)d}};
        return ymd.ok();

    }
}

bool existeProducto(Tienda* tienda, int id) {
   //verificacion de seguridad
   if (tienda == nullptr || tienda->productos == nullptr) {
        return false;

        for (int i = 0; i < tienda->numProductos; ++i) {
            if (tienda->productos[i].id == id) {
                return true;
            }
        }
    }
    return false;
}

bool existeProveedor(Tienda* tienda, int id) {
    //Verificación de seguridad básica
    if (tienda == nullptr || tienda->proveedores == nullptr) {
        return false;
    }
    //utilizamos el template anterior   
    return buscarEntidadPorId(tienda->proveedores, tienda->numProveedores, id) != -1;
}

bool existeCliente(Tienda* tienda, int id) {
    //Verificación de seguridad
    if (tienda == nullptr || tienda->clientes == nullptr) {
        return false;
    }
    return buscarEntidadPorId(tienda->clientes, tienda->numClientes, id) != -1;
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

        // si el primer caracter es 'q', se sale
        if (cin.peek() == 'q' || cin.peek() == 'Q') {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return -1;
        }

        int id;
        cin >> id;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "El id debe ser un número." << endl;
            cout << "¿Habrás querido salir? Presiona 'q' para salir." << endl;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (id <= 0) {
            cout << "El id debe ser numérico y mayor a 0. Intente nuevamente." << endl;
            continue;
        }

        return id;
    }
}

void menuProductos(Tienda* tienda) {
    char opcion;
    do {
        cout << "\n=== Gestión de Productos ===" << endl;
        cout << "1. Crear Producto" << endl;
        cout << "2. Buscar Producto" << endl;
        cout << "3. Actualizar Producto" << endl;
        cout << "4. Actualizar Stock" << endl;
        cout << "5. Listar Productos" << endl;
        cout << "6. Eliminar Producto" << endl;
        cout << "0. Volver al Menú Principal" << endl;
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
            cout << "Volviendo..." << endl;
            break;
        default:
            cout << "Opción inválida." << endl;
        }
    } while (opcion != '0');
}

void menuProveedores(Tienda* tienda) {
    char opcion;
    do {
        cout << "\n=== Gestión de Proveedores ===" << endl;
        cout << "1. Crear Proveedor" << endl;
        cout << "2. Buscar Proveedor" << endl;
        cout << "3. Actualizar Proveedor" << endl;
        cout << "4. Listar Proveedores" << endl;
        cout << "5. Eliminar Proveedor" << endl;
        cout << "0. Volver al Menú Principal" << endl;
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
            cout << "Volviendo..." << endl;
            break;
        default:
            cout << "Opción inválida." << endl;
        }
    } while (opcion != '0');
}

void menuClientes(Tienda* tienda) {
    char opcion;
    do {
        cout << "\n=== Gestión de Clientes ===" << endl;
        cout << "1. Crear Cliente" << endl;
        cout << "2. Buscar Cliente" << endl;
        cout << "3. Actualizar Cliente" << endl;
        cout << "4. Listar Clientes" << endl;
        cout << "5. Eliminar Cliente" << endl;
        cout << "0. Volver al Menú Principal" << endl;
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
            cout << "Volviendo..." << endl;
            break;
        default:
            cout << "Opción inválida." << endl;
        }
    } while (opcion != '0');
}

void menuTransacciones(Tienda* tienda) {
    char opcion;
    do {
        cout << "\n=== Gestión de Transacciones ===" << endl;
        cout << "1. Registrar Compra" << endl;
        cout << "2. Registrar Venta" << endl;
        cout << "3. Buscar Transacciones" << endl;
        cout << "4. Listar Transacciones" << endl;
        cout << "5. Cancelar Transacción" << endl;
        cout << "0. Volver al Menú Principal" << endl;
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
            cout << "Volviendo..." << endl;
            break;
        default:
            cout << "Opción inválida." << endl;
        }
    } while (opcion != '0');
}

int main() {
    Tienda tienda;
    inicializarTienda(&tienda, "Papaya Store", "J-123456789");

    char opcion;
    do {
        cout << "\n=== PAPAYA STORE - Menú Principal ===" << endl;
        cout << "1. Gestión de Productos" << endl;
        cout << "2. Gestión de Proveedores" << endl;
        cout << "3. Gestión de Clientes" << endl;
        cout << "4. Gestión de Transacciones" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opción: ";
        cin >> opcion;

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
            cout << "Saliendo del sistema..." << endl;
            break;
        default:
            cout << "Opción inválida. Intente de nuevo." << endl;
        }
    } while (opcion != '0');

    liberarTienda(&tienda);
    cout << "Saliendo del sistema..." << endl;
    return 0;
}
