<div align="center">
  <h1>Papaya Store</h1>
  <p><b>Sistema de gestion de inventario en C++ con persistencia binaria</b></p>
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
- Reportes, respaldo e integridad
- Resumen de tienda

El sistema persiste toda la informacion en archivos binarios dentro de `data/`.

## Arquitectura actual

El proyecto sigue una arquitectura modular por capas:

- `src/domain/`: entidades, contratos de repositorio, constantes y modelos comunes.
- `src/infrastructure/`: datasource de archivos binarios (`.bin`) y administracion de datos.
- `src/presentation/`: menus, utilidades CLI y flujo de interaccion con usuario.
- `src/main.cpp`: entrypoint actual del sistema.

### Principio clave de persistencia

Las entidades de dominio permanecen agnosticas de la capa de datos.

La serializacion binaria se resuelve en infraestructura mediante `EntityTraits` y `FSBaseRepository`,
evitando serializacion cruda de clases completas con `reinterpret_cast` sobre objetos OOP.

Esto permite:

- Mantener clases de dominio encapsuladas.
- Tener offsets binarios estables por registro.
- Evitar problemas por vtable/padding al guardar clases en disco.

## Entidades y persistencia

Archivos operativos en `data/`:

- `data/productos.bin`
- `data/proveedores.bin`
- `data/clientes.bin`
- `data/transacciones.bin`
- `data/tienda.bin`

Cada archivo inicia con `HeaderFile`:

```cpp
struct HeaderFile {
    int cantidadRegistros;
    int proximoID;
    int registrosActivos;
    int version;
};
```

Luego se guardan registros con acceso aleatorio:

`offset = sizeof(HeaderFile) + (indice * recordSize)`

## Funcionalidades principales

### Productos

- Crear, buscar, actualizar, listar y eliminar logicamente.
- Relacion obligatoria con proveedor.
- Control de stock, stock minimo y total vendido.
- Restriccion de borrado si el producto aparece en transacciones activas.

### Proveedores

- CRUD completo.
- Validacion de email.
- Restriccion de borrado si tiene transacciones de compra activas.

### Clientes

- CRUD completo.
- Validacion de email.
- Restriccion de borrado si tiene transacciones de venta activas.

### Transacciones

- Registro de compras y ventas con multiples productos por transaccion.
- Validacion de stock para ventas.
- Impacto en estadisticas y sincronizacion de contadores de tienda.

### Reportes y administracion

- Verificacion de integridad referencial.
- Reporte de productos con stock critico.
- Backup de archivos binarios.
- Resumen general de tienda.

## Requisitos

- CMake 3.16+
- Compilador C++20 con soporte de `std::format`

## Compilacion y ejecucion

Desde la raiz del proyecto:

```bash
cmake -S . -B build
cmake --build build
./build/PapayaStore
```

## Estado actual

- Se mantiene `src/main.cpp` como entrypoint.
- El proyecto compila correctamente por CMake.
- La persistencia de la capa OOP se maneja desde datasource sin acoplar entidades a archivos.

## Notas de uso

- Si hay datos antiguos serializados con formato previo, puede ser necesario regenerar `data/*.bin`.
- El sistema usa borrado logico, por lo que no reescribe historial ni compacta archivos.
