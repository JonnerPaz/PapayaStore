# REPÚBLICA BOLIVARIANA DE VENEZUELA
## UNIVERSIDAD RAFAEL URDANETA
## FACULTAD DE INGENIERÍA
## ESCUELA DE INGENIERÍA DE COMPUTACIÓN

**ASIGNATURA:** PROGRAMACIÓN 2  
**PROFESOR:** ING. VICTOR KNEIDER  
**TRIMESTRE:** 2026-A

---

# PROYECTO 3: POO Y MODULARIZACIÓN DEL SISTEMA DE INVENTARIO

## OBJETIVOS DE APRENDIZAJE

- Aplicar principios de Programación Orientada a Objetos (POO) en C++
- Convertir estructuras (`struct`) a clases (`class`) con encapsulamiento
- Separar declaraciones (`.hpp`) de implementaciones (`.cpp`)
- Organizar el proyecto en múltiples archivos por dominio/servicio
- Dominar constructores, destructores y métodos de acceso
- Comprender el flujo de compilación multi-archivo

---

## FILOSOFÍA DE ESTA ETAPA

### ¿Qué cambia y qué se mantiene?

**✅ SE MANTIENE:**
- Toda la funcionalidad de archivos binarios del Proyecto 2
- Acceso aleatorio con `seekg()` y `seekp()`
- Headers en archivos binarios
- Persistencia de datos y operaciones CRUD completas

**🔄 EVOLUCIONA:**
- `struct` → `class` (con encapsulamiento)
- Código monolítico → Módulos organizados por dominio
- Funciones sueltas → Métodos de clase
- Acceso directo a atributos → Getters y Setters
- Todo en `main.cpp` → Arquitectura modular

---

## 1. ESTRUCTURA DEL PROYECTO

### 1.1 Organización por Dominios

```
ProyectoTienda_v3/
│
├── tienda/
│   ├── Tienda.hpp
│   └── Tienda.cpp
│
├── productos/
│   ├── Producto.hpp
│   ├── Producto.cpp
│   ├── operacionesProductos.hpp
│   └── operacionesProductos.cpp
│
├── proveedores/
│   ├── Proveedor.hpp
│   ├── Proveedor.cpp
│   ├── operacionesProveedores.hpp
│   └── operacionesProveedores.cpp
│
├── clientes/
│   ├── Cliente.hpp
│   ├── Cliente.cpp
│   ├── operacionesClientes.hpp
│   └── operacionesClientes.cpp
│
├── transacciones/
│   ├── Transaccion.hpp
│   ├── Transaccion.cpp
│   ├── operacionesTransacciones.hpp
│   └── operacionesTransacciones.cpp
│
├── interfaz/
│   ├── Interfaz.hpp
│   └── Interfaz.cpp
│
├── persistencia/
│   ├── GestorArchivos.hpp
│   ├── GestorArchivos.cpp
│   └── Constantes.hpp
│
├── utilidades/
│   ├── Validaciones.hpp
│   ├── Validaciones.cpp
│   ├── Formatos.hpp
│   └── Formatos.cpp
│
├── main.cpp
├── Makefile
│
└── datos/                  # Archivos binarios (generados)
    ├── tienda.bin
    ├── productos.bin
    ├── proveedores.bin
    ├── clientes.bin
    └── transacciones.bin
```

### 1.2 Separación de Responsabilidades

Cada módulo se compone de:

- **Entidad** (`Producto.hpp/cpp`): Representa los datos con getters/setters y validaciones propias.
- **Operaciones** (`operacionesProductos.hpp/cpp`): Contiene la lógica de negocio y flujos (registrar, buscar, listar, etc.).
- **Persistencia** (`GestorArchivos`): Centraliza todas las operaciones de archivos binarios.
- **Interfaz** (`Interfaz`): Encapsula todos los menús y la navegación del sistema.
- **main.cpp**: Solo inicializa el sistema y llama a `Interfaz::ejecutar()`. Debe ser un archivo mínimo.

---

## 2. CONVERSIÓN DE ESTRUCTURAS A CLASES

### 2.1 Reglas Generales

Cada clase de entidad debe tener:

- **Atributos privados:** Todos los campos del `struct` del Proyecto 2.
- **Constructores:** Por defecto, con parámetros principales, y de copia.
- **Destructor.**
- **Getters (`const`)** y **Setters (con validación)** para cada atributo.
- **Métodos de validación** propios de la entidad.
- **Métodos de presentación** (`mostrarInformacionBasica()`, `mostrarInformacionCompleta()`).
- **Métodos de gestión de relaciones** (ej. `agregarTransaccionID()`, `eliminarProductoID()`).
- **Método estático** `obtenerTamano()` que retorna `sizeof` de la clase.

**La clase NO debe conocer operaciones de archivos.** Eso es responsabilidad exclusiva de `GestorArchivos`.

Aplica estos principios a todas las entidades: `Producto`, `Proveedor`, `Cliente`, `Transaccion` y `Tienda`.

---

## 3. PRINCIPIOS DE POO A APLICAR

### 3.1 Encapsulamiento

Los datos son privados, solo accesibles mediante métodos públicos.

**Antes (Proyecto 2):**
```cpp
Producto p;
p.precio = -50;  // ❌ Sin validación
```

**Ahora (Proyecto 3):**
```cpp
Producto p;
p.setPrecio(-50);  // ✅ El setter rechaza valores inválidos
```

### 3.2 Abstracción

El usuario de la clase llama `producto.agregarTransaccionID(15)` y no necesita saber que internamente se verifica espacio, se valida el ID, se actualiza un contador y se marca la fecha de modificación.

### 3.3 Responsabilidad Única

| Clase | Responsabilidad |
|-------|----------------|
| `Producto` | Representar y gestionar datos de un producto |
| `Proveedor` | Representar y gestionar datos de un proveedor |
| `Cliente` | Representar y gestionar datos de un cliente |
| `Transaccion` | Representar y gestionar datos de una transacción |
| `Tienda` | Información general y contadores del sistema |
| `GestorArchivos` | Operaciones de persistencia en archivos binarios |
| `Interfaz` | Menús, navegación y flujo de interacción con el usuario |
| `Validaciones` | Validar formatos de datos generales |

**⚠️ ANTIPATRÓN — NO HACER:**
```cpp
class Producto {
    void guardarEnArchivo();  // ❌ Responsabilidad de GestorArchivos
    void mostrarMenu();       // ❌ Responsabilidad de main.cpp
};
```

---

## 4. MÓDULO DE PERSISTENCIA

### 4.1 Clase GestorArchivos

Centraliza **todas** las operaciones de archivos binarios. Mantiene la misma estructura de `ArchivoHeader` del Proyecto 2.

### 4.2 Uso Obligatorio de Templates

Si observas la lógica de `GestorArchivos`, las operaciones CRUD a nivel de archivo son prácticamente idénticas para todas las entidades: abrir archivo, calcular offset, leer/escribir `sizeof(T)` bytes, cerrar archivo. La única diferencia es el **tipo de dato** y la **ruta del archivo**.

En lugar de escribir `guardarProducto()`, `guardarProveedor()`, `guardarCliente()`, `guardarTransaccion()` con código casi idéntico, debes utilizar **templates (plantillas de C++)** para crear métodos genéricos que funcionen con cualquier entidad:

```cpp
template <typename T>
bool guardarRegistro(const char* archivo, T& registro);

template <typename T>
bool leerRegistroPorIndice(const char* archivo, int indice, T& registro);

template <typename T>
bool actualizarRegistro(const char* archivo, int indice, T& registro);
```

Con templates, escribes la lógica **una sola vez** y el compilador genera la versión específica para cada tipo cuando la invocas. Esto reduce drásticamente la cantidad de código repetido y facilita el mantenimiento: si corriges un bug en el método genérico, se corrige para todas las entidades.

**Nota:** Los métodos template deben implementarse en el archivo `.hpp` (no en `.cpp`), ya que el compilador necesita ver la implementación completa al momento de instanciar la plantilla.

---

## 5. ARCHIVOS DE OPERACIONES

Cada módulo tiene su archivo de operaciones con la lógica de negocio. Estas funciones son las que `main.cpp` invoca.

**Ejemplo: `productos/operacionesProductos.hpp`**

```cpp
#ifndef OPERACIONES_PRODUCTOS_HPP
#define OPERACIONES_PRODUCTOS_HPP

#include "../tienda/Tienda.hpp"

void registrarProducto(Tienda& tienda);
void buscarProducto(Tienda& tienda);
void actualizarProducto(Tienda& tienda);
void listarProductos(Tienda& tienda);
void eliminarProducto(Tienda& tienda);
void productosStockCritico(Tienda& tienda);

#endif
```

Las implementaciones en `operacionesProductos.cpp` contienen toda la lógica: solicitar datos al usuario, validar, llamar a `GestorArchivos` para persistir, y mostrar resultados. Replicar este patrón para proveedores, clientes y transacciones.

---

## 6. MÓDULO DE UTILIDADES

### Clase Validaciones

Funciones **estáticas** para validaciones generales de formato:

- `validarEmail(const char*)` → Contiene '@' y '.' después del @
- `validarRIF(const char*)` → Formato válido
- `validarTelefono(const char*)` → Formato válido
- `validarRango(int valor, int min, int max)` → Dentro de rango

**Diferencia clave:**
- **Validaciones (clase estática):** Reglas generales de formato. ¿Es un email válido?
- **Métodos de entidad:** Reglas de negocio específicas. ¿Este producto tiene datos completos?

### Clase Formatos

Funciones **estáticas** para formateo de salida:

- `formatearFecha(time_t)` → "DD/MM/AAAA"
- `formatearMoneda(float)` → Formato con separador de miles
- `convertirAMayusculas(char*)`
- `limpiarBuffer()`
- `pausar()` → Espera Enter del usuario

---

## 7. CLASE INTERFAZ Y main.cpp

### 7.1 Clase Interfaz

Todos los menús del sistema deben estar encapsulados en una clase `Interfaz` dentro de su propio módulo:

```
├── interfaz/
│   ├── Interfaz.hpp
│   └── Interfaz.cpp
```

Esta clase centraliza la presentación de menús y la lectura de opciones del usuario. Internamente, cada submenú delega la acción correspondiente a las funciones de `operacionesXXX`. De esta forma, ni `main.cpp` ni las clases de entidad conocen la lógica de navegación.

**Responsabilidades de `Interfaz`:**
- Mostrar el menú principal y los submenús de cada módulo (productos, proveedores, clientes, transacciones, mantenimiento).
- Leer y validar la opción seleccionada por el usuario.
- Invocar las funciones de operaciones correspondientes según la selección.
- Contener el loop principal del programa (`ejecutar()`).

### 7.2 main.cpp

Con la clase `Interfaz`, `main.cpp` queda reducido a su mínima expresión: inicializar el sistema y delegar todo el flujo.

```cpp
#include "persistencia/GestorArchivos.hpp"
#include "interfaz/Interfaz.hpp"
#include <iostream>

int main() {
    if (!GestorArchivos::inicializarSistemaArchivos()) {
        cerr << "Error al inicializar archivos" << endl;
        return 1;
    }

    Interfaz::ejecutar();
    return 0;
}
```

**`main.cpp` no debe contener lógica de negocio, menús, ni switches.** Todo eso vive en `Interfaz`.

---

## 8. GUARDAS DE INCLUSIÓN Y COMPILACIÓN

### 8.1 Guardas de Inclusión (OBLIGATORIO en todo `.hpp`)

```cpp
#ifndef PRODUCTO_HPP
#define PRODUCTO_HPP
// ... contenido ...
#endif
```

### 8.2 Inclusiones en `.cpp`

Solo incluir lo que realmente se necesita:

```cpp
// productos/Producto.cpp
#include "Producto.hpp"     // ✅ Siempre su propio .hpp primero
#include <iostream>         // ✅ Solo si usas cout/cin
#include <cstring>          // ✅ Solo si usas strcpy/strlen
```

### 8.3 Compilación Multi-Archivo

Implementar un `Makefile` que compile todos los módulos y genere el ejecutable final.

---

## 9. REQUERIMIENTOS TÉCNICOS

### 9.1 Todos los Módulos Deben Tener

- Guardas de inclusión en `.hpp`
- Separación `.hpp` / `.cpp`
- Atributos privados, métodos públicos
- Constructores (por defecto + parametrizados) y destructor
- Getters `const` y Setters con validaciones
- Comentarios explicativos

### 9.2 Convenciones de Nombres

- **Clases:** PascalCase → `GestorArchivos`, `Producto`
- **Métodos:** camelCase → `registrarProducto()`, `buscarPorID()`
- **Atributos privados:** camelCase → `nombreCompleto`, `fechaCreacion`
- **Constantes:** UPPER_SNAKE_CASE → `MAX_PRODUCTOS`, `RUTA_ARCHIVO`

### 9.3 Uso de `const`

```cpp
int getId() const;                              // Getters siempre const
const char* getNombre() const;                  // Retorno const para char[]
void setNombre(const char* nombre);             // Parámetros que no se modifican
bool buscarPorID(int id, Producto& resultado) const;
```

---

## 10. DIFERENCIAS CLAVE CON PROYECTO 2

| Aspecto | Proyecto 2 | Proyecto 3 |
|---------|-----------|-----------|
| **Estructura de datos** | `struct` pública | `class` encapsulada |
| **Organización** | 1-2 archivos grandes | Módulos por dominio |
| **Acceso a datos** | `p.precio = 10` | `p.setPrecio(10)` |
| **Lógica de negocio** | En main o funciones sueltas | En `operacionesXXX.cpp` |
| **Persistencia** | Funciones sueltas | Clase `GestorArchivos` |
| **main.cpp** | 500+ líneas | 50-100 líneas |
| **Validaciones** | Manuales antes de usar | Dentro de setters |
| **Compilación** | `g++ archivo.cpp` | Multi-archivo con Makefile |

---

## 11. ENTREGABLES

### 11.1 Código Fuente Organizado

Estructura de carpetas completa según la sección 1.1. Todos los módulos completamente implementados. La organización propuesta es una guía; pueden adaptarla a su criterio siempre que mantengan la separación por dominios.

### 11.2 Archivos de Prueba

Incluir en carpeta `datos/` archivos `.bin` con al menos: 15 productos, 5 proveedores, 8 clientes y 25 transacciones.

### 11.3 Documentación

- `Makefile` funcional
- `README_V3.md` con instrucciones de compilación, estructura del proyecto y manual de usuario

---

## 12. ESTRATEGIA DE IMPLEMENTACIÓN SUGERIDA

1. **Fase 1:** Toma `struct Producto` del Proyecto 2, conviértelo a `class`, crea `operacionesProductos` con `registrarProducto()`, compila y prueba.
2. **Fase 2:** Implementa `GestorArchivos` con métodos básicos (guardar, leer, listar). Prueba con Producto.
3. **Fase 3:** Completa el módulo de Productos con todas las operaciones.
4. **Fase 4:** Replica el patrón para Proveedor, Cliente y Transacción.
5. **Fase 5:** Crea `main.cpp` limpio con menús, conecta todo.
6. **Fase 6:** Extrae validaciones y formatos a las clases utilitarias.

---

**IMPORTANTE:** No reinicies desde cero. Evoluciona tu Proyecto 2 aplicando estos conceptos gradualmente.

---

**Ing. Victor Kneider**  
Profesor de Programación 2  
Universidad Rafael Urdaneta
