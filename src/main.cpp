struct Producto {
    int id;                 // Identificador único (autoincremental)
    char codigo[20];        // Código del producto (ej: "PROD-001")
    char nombre[100];       // Nombre del producto
    char descripcion[200];  // Descripción del producto
    int idProveedor;        // ID del proveedor asociado
    float precio;           // Precio unitario
    int stock;              // Cantidad en inventario
    char fechaRegistro[11]; // Formato: YYYY-MM-DD
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
    int id;                // Identificador único (autoincremental)
    char tipo[10];         // "COMPRA" o "VENTA"
    int idProducto;        // ID del producto involucrado
    int idRelacionado;     // ID del proveedor (compra) o cliente (venta)
    int cantidad;          // Cantidad de unidades
    float precioUnitario;  // Precio por unidad en esta transacción
    float total;           // cantidad * precioUnitario
    char fecha[11];        // Formato: YYYY-MM-DD
    char descripcion[200]; // Notas adicionales (opcional)
};

struct Tienda {
    char nombre[100]; // Nombre de la tienda
    char rif[20];     // RIF de la tienda

    // Arrays dinámicos de entidades
    Producto* productos;
    int numProductos;
    int capacidadProductos;

    Proveedor* proveedores;
    int numProveedores;
    int capacidadProveedores;

    Cliente* clientes;
    int numClientes;
    int capacidadClientes;

    Transaccion* transacciones;
    int numTransacciones;
    int capacidadTransacciones;

    // Contadores para IDs autoincrementales
    int siguienteIdProducto;
    int siguienteIdProveedor;
    int siguienteIdCliente;
    int siguienteIdTransaccion;
};

void inicializarTienda(Tienda* tienda, const char* nombre, const char* rif) {
    // Inicializar los arrays dinámicos
    tienda->productos = new Producto[5];
    tienda->proveedores = new Proveedor[5];
    tienda->clientes = new Cliente[5];
    tienda->transacciones = new Transaccion[5];
}

void liberarTienda(Tienda* tienda) {
    // Liberar los arrays dinámicos
    delete[] tienda->productos;
    delete[] tienda->proveedores;
    delete[] tienda->clientes;
    delete[] tienda->transacciones;
}

void crearProducto(Tienda* tienda) {
    // Implementar la lógica para crear un nuevo producto
}

void buscarProducto(Tienda* tienda) {
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
