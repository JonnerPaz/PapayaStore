# REPÚBLICA BOLIVARIANA DE VENEZUELA
## UNIVERSIDAD RAFAEL URDANETA
## FACULTAD DE INGENIERÍA
## ESCUELA DE INGENIERÍA DE COMPUTACIÓN

**ASIGNATURA:** PROGRAMACIÓN 2  
**PROFESOR:** ING. VICTOR KNEIDER  
**TRIMESTRE:** 2026-A

---

# PROYECTO 2: SISTEMA DE GESTIÓN DE INVENTARIO CON PERSISTENCIA Y ACCESO ALEATORIO

## DESCRIPCIÓN GENERAL

Evoluciona tu sistema de gestión de inventario del Proyecto 1 implementando **persistencia de datos mediante archivos binarios** y **acceso aleatorio**. El sistema debe demostrar dominio de operaciones de lectura/escritura binaria, navegación eficiente en archivos usando `seekg()/seekp()`, y gestión de relaciones entre múltiples archivos binarios sin depender de la carga total en memoria (RAM).

---

## OBJETIVOS DE APRENDIZAJE

1. Implementar persistencia de datos con archivos binarios.
2. Dominar operaciones de lectura/escritura binaria con la librería `<fstream>`.
3. Aplicar acceso aleatorio y cálculo de offsets usando `seekg()` y `seekp()`.
4. Comprender el mapeo entre estructuras en memoria y su representación física en disco.
5. Gestionar índices, posiciones y borrado lógico en archivos binarios.
6. Implementar integridad referencial entre múltiples archivos mediante IDs.

---

## FILOSOFÍA DE ACCESO ALEATORIO

### Concepto Fundamental

Este proyecto **NO carga todos los datos en memoria**. A diferencia del Proyecto 1, donde todo el sistema vivía en la RAM usando arreglos dinámicos, aquí los datos permanecen en disco y **solo se carga un registro a la vez en memoria cuando se necesita procesar**.

### Ciclo de Vida de una Operación

Para cualquier acción que involucre un registro (leer, modificar, eliminar lógicamente), tu programa debe seguir estrictamente este flujo:
1. Abrir el archivo binario correspondiente.
2. Calcular la posición exacta en bytes del registro buscado.
3. Desplazar el puntero de lectura/escritura (`seekg` o `seekp`).
4. Leer o escribir **únicamente ese registro**.
5. Cerrar el archivo inmediatamente.

### Ventajas de este enfoque

- **Eficiencia de Memoria:** El uso de RAM es mínimo (apenas unos bytes para las variables temporales) sin importar si gestionas 10 o 100,000 registros.
- **Persistencia Inmediata:** Si ocurre una falla eléctrica (muy común) o el programa crashea, la información ya procesada está segura en el disco.

---

## 1. MODELO DE PERSISTENCIA Y ARQUITECTURA

El sistema debe crear **un archivo binario independiente por cada estructura principal**: `tienda.bin`, `productos.bin`, `proveedores.bin`, `clientes.bin` y `transacciones.bin`.

### 1.1 Estructura Interna de los Archivos

Cada archivo binario debe contar con un **Header (Encabezado)** al principio, seguido de los registros de datos:

```
[HEADER: 16 bytes] → Metadata administrativa del archivo
[REGISTRO 0]
[REGISTRO 1]
...
[REGISTRO N]
```

**Estructura obligatoria del Header:**

```cpp
struct ArchivoHeader {
    int cantidadRegistros;      // Total histórico de registros
    int proximoID;              // Siguiente ID a asignar (Autoincremental)
    int registrosActivos;       // Registros que no están marcados como eliminados
    int version;                // Control de versión del archivo
};
```

El Header evita tener que recorrer todo el archivo secuencialmente para saber cuántos elementos existen o cuál es el próximo ID válido.

---

## 2. ADAPTACIÓN DE ESTRUCTURAS PARA PERSISTENCIA

Para guardar estructuras en archivos binarios, estas no pueden contener punteros, arreglos dinámicos, ni strings de C++ (`std::string`). Su tamaño en bytes (`sizeof`) debe ser estático y predecible.

A continuación, se muestra cómo debe evolucionar la estructura Producto de tu Proyecto 1 para soportar la fase 2. Debes aplicar esta misma lógica de transformación para Proveedor, Cliente y Transacción.

```cpp
struct Producto {
    // 1. Datos básicos (Usar arreglos de char estáticos, NO std::string)
    int id;
    char codigo[20];
    char nombre[100];
    char descripcion[200];
    float precio;
    int stock;
    
    // 2. Llaves Foráneas (Relaciones)
    int idProveedor;                 
    
    // 4. Estadísticas del registro
    int stockMinimo;                 
    int totalVendido;                
    
    // 5. Metadata de Control Obligatoria
    bool eliminado;                  // Para BORRADO LÓGICO
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};
```

### 2.2 Estructura Transacción con Múltiples Productos (Archivo: transacciones.bin)
Originalmente en la primera fase del proyecto, una transacción podía unicamente con tener UN SOLO PRODUCTO, pero deben convertirla para que pueda almacenar más de un producto por transacción. (Se debe mantener los mismos patrones de llaves foráneas)

### 2.2 Principios para adaptar el resto de estructuras
- **Tienda (`tienda.bin`):** Contendrá un único registro. Elimina todos los punteros a los arreglos de entidades. Solo debe guardar datos de la empresa y contadores estadísticos globales.
- **Arreglos de Relaciones:** Si un Proveedor tiene múltiples Productos, usa un arreglo fijo de enteros (ej. `int productosIDs[100]`) para guardar los IDs, acompañado de su respectivo contador (`int cantidadProductos`).
- **Borrado Lógico:** Toda estructura debe tener el flag `bool eliminado`. En un archivo binario, borrar físicamente un registro del medio corrompe las posiciones matemáticas de los demás.

---

## 3. LÓGICA DE OPERACIONES CON ARCHIVOS (CRUD)

A continuación se detallan los requerimientos lógicos para las operaciones base. Es tu responsabilidad como ingeniero traducir estos algoritmos a código funcional en C++.

### 3.1 Inicialización y Gestión de Headers
Debes implementar funciones aisladas para leer y actualizar el header de cualquier archivo:

- `bool inicializarArchivo(const char* nombreArchivo)`: Crea el archivo si no existe y escribe un `ArchivoHeader` inicializado en 0.
- `ArchivoHeader leerHeader(const char* nombreArchivo)`: Abre el archivo, lee los primeros bytes correspondientes al header y lo retorna.
- `bool actualizarHeader(const char* nombreArchivo, ArchivoHeader header)`: Sobrescribe únicamente la sección del header.

### 3.2 Acceso Aleatorio y Cálculo de Offsets
La clave del proyecto es la función matemática para ubicar un registro sin recorrer el archivo.

**Fórmula de posición en bytes:**
```
sizeof(ArchivoHeader) + (indice * sizeof(TuEstructura))
```

> Nota: El indice (posición física 0, 1, 2...) no siempre es igual al ID del registro, especialmente si hay borrados lógicos. Debes hacer una función de búsqueda secuencial que reciba un ID y retorne su indice físico.

### 3.3 Creación de un Registro (Append)
Lógica requerida:

- Leer el header actual para obtener el proximoID.
- Asignar ese ID a tu nueva estructura y marcarla como `eliminado = false`.
- Abrir el archivo en modo adición/binario (`ios::app | ios::binary` o usar `seekp(0, ios::end)`).
- Escribir la estructura.
- Actualizar los contadores en el header y guardarlo nuevamente.

### 3.4 Actualización (Update) y Borrado Lógico (Delete)
Lógica requerida:

- Buscar el índice del registro mediante su ID.
- Calcular su posición exacta en bytes.
- Posicionar el cursor de escritura (`seekp`) en ese byte.
- Escribir la estructura modificada (o la estructura con `eliminado = true`) sobrescribiendo los datos anteriores.

---

## 4. GESTIÓN DE RELACIONES Y TRANSACCIONES

### 4.1 Registrar una Compra/Venta
El proceso de registrar una transacción ahora involucra múltiples accesos a disco para mantener la coherencia.

**Pasos algorítmicos obligatorios para una Venta:**

1. Buscar y leer el Producto y el Cliente desde sus respectivos archivos.
2. Validar reglas de negocio (ej. ¿Hay stock suficiente? ¿El cliente existe?).
3. Si es válido, instanciar un registro Transaccion.
4. Guardar la Transaccion al final de `transacciones.bin`.
5. Modificar el Producto en memoria (restar stock, sumar estadísticas, añadir el ID de la transacción a su arreglo de historial) y actualizar su registro en el archivo binario.
6. Modificar el Cliente en memoria (sumar gastos, añadir ID de transacción) y actualizar su registro en el archivo binario.

---

## 5. MANTENIMIENTO E INTEGRIDAD (NUEVAS FUNCIONALIDADES)

### 5.1 Integridad Referencial
Debes programar un módulo de diagnóstico `verificarIntegridadReferencial()` que detecte "referencias rotas".

¿Qué debe hacer el algoritmo?

- Recorrer `productos.bin`. Por cada producto activo, extraer su `idProveedor`.
- Buscar en `proveedores.bin` si ese ID existe y no está eliminado. Si no existe, registrar el error.
- Repetir la lógica cruzada para Transacciones (verificar que el `idProducto` y el `idCliente/Proveedor` aún existan).
- Imprimir un reporte de salud de la base de datos al finalizar.

### 5.2 Respaldo de Datos (Backup)
Implementar una función `crearBackup()`:

- Debe crear una carpeta (o usar un prefijo en el nombre) con la fecha y hora actual.
- Debe copiar byte a byte los 5 archivos .bin operativos a este nuevo destino seguro.

### 5.3 Reportes Analíticos (Lectura en Lote)
Se requiere un submenú para leer datos y generar estadísticas:

- **Productos con stock crítico:** Recorrer productos y filtrar los que tengan `stock <= stockMinimo`.
- **Historial de Cliente:** Dado un ID de cliente, imprimir sus datos básicos, buscar todas las transacciones asociadas a su arreglo `comprasIDs[]` e imprimir el detalle recuperando el nombre del producto involucrado.

---

## 6. EXPERIENCIA DE USUARIO

### 6.1. IMPRESIÓN DE DATOS
- Todos los datos que se muestren en el sistema deben estar correctamente formateados usando tablas, colores, caracteres, etc

### 6.2. PRACTICIDAD
- Se debe tomar en cuenta que el sistema busca ser práctico, tal vez opciones de los submenus deberian repensarse para poder brindar la mejor experiencia de usuario. Por ejemplo: Cuando muestras la información de un producto ¿No sería bueno mostrar también la información del proveedor que lo vende, o solamente su id te parece suficiente?

## 7. ENTREGABLES

### 7.1 Código Fuente
- Archivo .cpp principal (o modularizado en .h / .cpp).
- Uso estricto de nomenclatura y comentarios descriptivos sobre los cálculos lógicos de bytes.

### 7.2 Archivos de Datos de Prueba
- Tu entrega debe venir acompañada de los archivos .bin pre-poblados con al menos: 15 productos, 5 proveedores, 8 clientes y 25 transacciones que demuestren el funcionamiento.

### 7.3 Documentación Técnica (`README_V2.md`)
- Diagrama de las estructuras y sus tamaños en bytes (`sizeof`).
- Manual de usuario sencillo.
