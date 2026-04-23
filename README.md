<div align="center">
  <h1>Papaya Store</h1>
  <p><b>Sistema CLI de gestion de tienda en C++20 con persistencia binaria</b></p>
  <p><i>Proyecto de Programacion II - Universidad Rafael Urdaneta (URU)</i></p>
  <p>Desarrollado por <b>Jonner Paz</b> y <b>Andres Martinez</b></p>
</div>

---

## Descripcion general

Papaya Store es una aplicacion de consola para administrar una tienda con modulos de:

- Productos
- Proveedores
- Clientes
- Transacciones (compras y ventas)
- Reportes operativos y de seguridad
- Resumen global de tienda

Toda la informacion se persiste en archivos binarios dentro de `data/`, con acceso aleatorio por offset.

## Funcionalidades principales

### Productos

- CRUD completo con borrado logico.
- Relacion obligatoria con proveedor.
- Control de stock, stock minimo y total vendido.
- Restriccion de eliminacion cuando existe en transacciones activas.

### Proveedores

- CRUD completo con validaciones de formato.
- Relacion con productos y transacciones de compra.
- Restriccion de eliminacion con integridad referencial.

### Clientes

- CRUD completo con validaciones de formato.
- Historial de transacciones de venta.
- Restriccion de eliminacion con integridad referencial.

### Transacciones

- Registro de compras y ventas con multiples productos por movimiento.
- Validacion de stock para ventas.
- Impacto directo en estadisticas de tienda.

### Reportes y administracion

- Verificacion de integridad referencial.
- Reporte de stock critico.
- Backup de archivos `.bin`.
- Sincronizacion y resumen de contadores globales en `tienda.bin`.

---

## Arquitectura detallada

El proyecto usa una arquitectura por capas con responsabilidades claras y bajo acoplamiento.

### 1) Como esta constituida

- `src/domain/`: entidades (`Producto`, `Proveedor`, `Cliente`, `Transaccion`, `Tienda`), `HeaderFile`, constantes y contratos (`I*Repository`, `IDatabaseAdmin`).
- `src/infrastructure/`: implementaciones concretas de repositorios en filesystem (`FS*Repository`), repositorio generico (`FSBaseRepository`) y mapeo binario (`EntityTraits`).
- `src/presentation/`: menus y utilidades CLI para la interaccion del usuario.
- `src/Bootstrapper.*`: composicion de dependencias, inicializacion de almacenamiento y lanzamiento de la app.
- `src/main.cpp`: entrypoint minimo que delega la ejecucion a `Bootstrapper`.

### 2) Como se conectan los modulos

- `main.cpp` crea `Bootstrapper`.
- `Bootstrapper` instancia repositorios concretos (`FSProductoRepository`, etc.) y `FSDatabaseAdmin`.
- `Bootstrapper` agrupa dependencias en `AppRepositories` (inyeccion por referencias a interfaces).
- `MainMenu` y submenus consumen `AppRepositories` para ejecutar casos de uso.
- Cada `FS*Repository` delega CRUD comun en `FSBaseRepository<T>`.
- `FSBaseRepository<T>` delega serializacion/deserializacion en `EntityTraits<T>`.
- `EntityTraits<T>` define layout binario estable para cada entidad en sus archivos `.bin`.

### 4) Por que se usan `EntityTraits`

`EntityTraits` es la pieza clave para desacoplar dominio y persistencia binaria.

Cada especializacion (`EntityTraits<Producto>`, `EntityTraits<Cliente>`, etc.) define:

- `recordSize()`: tamano fijo y determinista del registro en disco.
- `writeToStream(...)`: como se serializa la entidad en orden fijo de campos.
- `readFromStream(...)`: como se reconstruye la entidad desde bytes.
- `getId(...)`, `isDeleted(...)`, `setDeleted(...)`: metadatos comunes para CRUD generico.

Beneficios concretos:

- Permite que `FSBaseRepository<T>` sea reutilizable para multiples entidades sin duplicar logica.
- Evita persistir objetos OOP completos de forma cruda, reduciendo riesgo por padding/layout interno.
- Conserva encapsulamiento: la entidad se reconstruye por setters, no por acceso directo a memoria.
- Facilita evolucionar cada entidad sin romper el contrato generico del repositorio base.

Entonces: `FSBaseRepository` sabe *cuándo* y *dónde* leer/escribir; `EntityTraits` sabe *cómo* traducir cada entidad.

### 5) Flujo de una operacion (ejemplo: crear producto)

1. `MenuProductos` valida entrada del usuario.
2. Solicita estadisticas al repositorio para calcular `proximoID`.
3. Construye `Producto` y llama `repositories.productos.guardar(...)`.
4. `FSProductoRepository` delega en `FSBaseRepository<Producto>::guardarTemplate(...)`.
5. `FSBaseRepository` usa `EntityTraits<Producto>::writeToStream(...)` y actualiza `HeaderFile`.
6. Se sincronizan contadores globales con `repositories.admin.sincronizarContadoresTienda()`.

---

## Persistencia binaria

Archivos operativos:

- `data/productos.bin`
- `data/proveedores.bin`
- `data/clientes.bin`
- `data/transacciones.bin`
- `data/tienda.bin`

Todos los archivos binarios arrancan con `HeaderFile`:

```cpp
struct HeaderFile {
    int cantidadRegistros;
    int proximoID;
    int registrosActivos;
    int version;
};
```

Acceso aleatorio:

```cpp
offset = sizeof(HeaderFile) + (id - 1) * EntityTraits<T>::recordSize();
```

---

## Estructura del proyecto (diagrama tipo arbol)

```text
papaya/
├── CMakeLists.txt
├── README.md
├── requirements.md
├── data/
├── build/
└── src/
    ├── main.cpp
    ├── Bootstrapper.hpp
    ├── Bootstrapper.cpp
    ├── domain/
    │   ├── HeaderFile.hpp
    │   ├── constants.hpp
    │   ├── utils/
    │   ├── entities/
    │   │   ├── entidad.entity.hpp
    │   │   ├── entidad.entity.cpp
    │   │   ├── producto/
    │   │   ├── proveedor/
    │   │   ├── cliente/
    │   │   ├── transaccion/
    │   │   └── tienda/
    │   └── repositories/
    │       ├── AppRepositories.hpp
    │       ├── IProductoRepository.hpp
    │       ├── IProveedorRepository.hpp
    │       ├── IClienteRepository.hpp
    │       ├── ITransaccionRepository.hpp
    │       └── IDatabaseAdmin.hpp
    ├── infrastructure/
    │   └── datasource/
    │       ├── EntityTraits.hpp
    │       ├── FSBaseRepository.hpp
    │       ├── producto/
    │       ├── proveedor/
    │       ├── cliente/
    │       ├── transaccion/
    │       └── admin/
    └── presentation/
        ├── CliUtils.hpp
        ├── CliUtils.cpp
        └── Menu/
            ├── Menu.hpp
            ├── Menu.cpp
            ├── MainMenu/
            ├── MenuProductos/
            ├── MenuProveedores/
            ├── MenuClientes/
            ├── MenuTransacciones/
            └── MenuReportes/
```

---

## Requisitos

- CMake 3.16+
- Compilador compatible con C++20
- Python 3.12+ (gestionado con `uv`)
- `uv` instalado

## Dependencias Python (uv)

Desde la raiz del proyecto:

```bash
uv sync
```

Si necesitas agregar paquetes manualmente:

```bash
uv add pybind11 flet
```

## Compilacion (CLI + modulo Python)

Compila el core, el ejecutable CLI y el modulo Python:

```bash
cmake --fresh -S . -B build
cmake --build build -j4
```

Esto genera:

- `build/PapayaStore` (app CLI)
- `build/papaya_backend*.so` (modulo Python compilado con Pybind11)

## Ejecucion

### 1) Ejecutar la app CLI

```bash
./build/PapayaStore
```

### 2) Probar el backend desde Python

Desde la raiz del proyecto:

```bash
uv run python test_backend.py
```

## Uso rapido del modulo desde Python

```python
import sys
sys.path.append("build")

import papaya_backend as pb

ctx = pb.PapayaContext()

ok = ctx.init_database()
print("DB inicializada:", ok)

# Repositorios desde el contexto
repo_productos = ctx.productos
estadisticas = repo_productos.obtener_estadisticas()
print("Estadisticas:", estadisticas)

# Tareas administrativas
integridad = ctx.admin.verificar_integridad_referencial()
print("Integridad:", integridad)
```

- `PapayaContext` es la forma recomendada de usar el backend desde Python, porque centraliza repositorios (`productos`, `proveedores`, `clientes`, `transacciones`) y administracion (`admin`) en un unico punto.
- Los repositorios directos (`ProductoRepository()`, etc.) siguen disponibles para compatibilidad.

## Nota sobre CMake y Python

Si cambias la version de Python del entorno `uv` o aparece un sufijo de modulo incorrecto (`cpython-3xx`), recompila con configuracion limpia:

```bash
cmake --fresh -S . -B build
cmake --build build -j4
```

## Notas de uso

- El sistema usa borrado logico (`eliminado`) y mantiene historial de registros.
- Si cambia el layout binario de una entidad, se recomienda regenerar los `.bin` de entorno de desarrollo.
- Los backups se guardan en `backup/` con nombre basado en fecha y hora.
