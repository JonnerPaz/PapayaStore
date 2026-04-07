# TODO.md - Plan de migracion OOP/MVC

Este archivo resume el plan general para sesiones de trabajo.
Objetivo: migrar el flujo legacy de `src/main.cpp` a la arquitectura OOP ya creada
(`domain` + `infrastructure` + `presentation`) sin romper funcionalidad ni datos.

## Estado actual (resumen rapido)
- `src/main.cpp` es ahora el unico entrypoint compilado y contiene el unico `main()`.
- El `main loop` unico vive en `Bootstrap::runMainLoop()` dentro de `src/main.cpp`.
- `src/main.cpp` se mantiene como legacy temporal, pero ya no forma parte del build OOP.
- El menu principal de `main` ya enruta Productos, Proveedores, Clientes,
  Transacciones, Reportes y Tienda por menus OOP (sin bridge legacy en dispatch).
- `MenuProductos` ya tiene implementacion funcional base (CRUD principal).
- `MenuProveedores`, `MenuClientes`, `MenuTransacciones` y `MenuTienda` siguen
  con funcionalidades clave pendientes en sus handlers.
- `MenuReportes` ya esta conectado por OOP, pero la mayoria de funciones de
  `FSDatabaseAdmin` siguen pendientes.

## Objetivo de arquitectura
- Mantener `src/main.cpp` solo como referencia legacy/deprecada durante la migracion.
- Mantener `src/main.cpp` como composition root definitivo.
- Menus en `presentation` como UI/orquestacion.
- Reglas de negocio y validaciones en domain/repositories/services.
- Persistencia en `infrastructure/datasource`.

## Principios de ejecucion
- Migracion incremental por modulo.
- No mezclar dos fuentes de verdad para la misma regla.
- Cada paso debe compilar y poder probarse manualmente.

## Orden recomendado de menus
1. Productos (base de referencia, ya avanzado)
2. Proveedores (siguiente paso)
3. Clientes
4. Transacciones
5. Reportes
6. Tienda

## Siguiente paso recomendado (iteracion actual)

### Iteracion 1: Cierre funcional de `MenuProveedores`
Archivos:
- `src/presentation/Menu/MenuProveedores/MenuProveedores.cpp`
- `src/presentation/Menu/MenuProveedores/MenuProveedores.hpp` (si hace falta helpers)
- `src/main.cpp` (ya conectado, no deberia requerir cambios funcionales)

Objetivo:
- Completar CRUD funcional por OOP para Proveedores y cerrar restricciones de
  eliminacion relacionadas con transacciones activas.

Checklist:
- Implementar `crearProveedor`.
- Implementar `buscarProveedor`.
- Implementar `actualizarProveedor`.
- Implementar `listarProveedores`.
- Implementar `eliminarProveedor`.
- Validar que no se elimine proveedor con transacciones activas asociadas.
- Homologar mensajes de UX (errores, exito, cancelacion) en espanol.
- Mantener consistencia con manejo de IDs y borrado logico.

Definicion de cierre:
- Flujo `crear -> listar -> buscar -> actualizar -> eliminar` funcionando en OOP.
- Restriccion de eliminacion aplicada correctamente.
- Compila con `cmake -S . -B build && cmake --build build`.

## Plan por menu

### 1) MenuProductos
Archivo: `src/presentation/Menu/MenuProductos/MenuProductos.cpp`

Pendiente de consolidar:
- Revisar validaciones de duplicados (nombre/codigo) contra repositorio.
- Revisar consistencia con proveedor asociado al crear/actualizar.
- Confirmar mensajes UX en espanol y flujo cancelable con `q`.

Estado de routing:
- `src/main.cpp` ya llama solo a `MenuProductos.showMenu()`.

Definicion de cierre:
- Crear, buscar, actualizar, listar y eliminar funcionan solo por OOP.

### 2) MenuProveedores
Archivos:
- `src/presentation/Menu/MenuProveedores/MenuProveedores.cpp`
- `src/main.cpp`

Tareas:
- Implementar `crearProveedor`, `buscarProveedor`, `actualizarProveedor`,
  `listarProveedores`, `eliminarProveedor`.
- Validar restriccion de eliminacion si hay transacciones activas.

Estado de routing:
- Ya conectado por OOP en `main` (sin bridge legacy).

Definicion de cierre:
- Opcion Proveedores funcional por OOP y sin handlers en desarrollo.

### 3) MenuClientes
Archivos:
- `src/presentation/Menu/MenuClientes/MenuClientes.cpp`
- `src/main.cpp`

Tareas:
- Completar `crearCliente`, `buscarCliente`, `actualizarCliente`, `eliminarCliente`.
- Mantener/ajustar `listarClientes` para consistencia de salida.

Estado de routing:
- Ya conectado por OOP en `main` (sin bridge legacy).

Definicion de cierre:
- Opcion Clientes funcional por OOP y sin handlers en desarrollo.

### 4) MenuTransacciones
Archivos:
- `src/presentation/Menu/MenuTransacciones/MenuTransacciones.cpp`
- `src/main.cpp`

Tareas:
- Implementar `registrarCompra`.
- Implementar `registrarVenta`.
- Implementar `buscarTransacciones` y `listarTransacciones`.
- Implementar `cancelarTransaccion` con validaciones de reversa.

Estado de routing:
- Ya conectado por OOP en `main` (sin bridge legacy).

Definicion de cierre:
- Flujo de transacciones 100% OOP y consistente en inventario.

### 5) MenuReportes
Archivos:
- `src/presentation/Menu/MenuReportes/MenuReportes.cpp`
- `src/infrastructure/datasource/FSDatabaseAdmin.cpp`
- `src/main.cpp`

Tareas:
- Completar implementaciones de admin:
  - `verificarIntegridadReferencial`
  - `crearBackup` (revisar robustez y manejo de errores)
  - `reporteStockCritico`
  - `reporteHistorialCliente`
  - `sincronizarContadoresTienda`

Estado de routing:
- Ya conectado por OOP en `main` (sin bridge legacy).

Definicion de cierre:
- Opcion Reportes funcional por OOP con operaciones administrativas reales.

### 6) MenuTienda
Archivos:
- `src/presentation/Menu/MenuTienda/MenuTienda.cpp`
- `src/main.cpp`

Tareas:
- Implementar resumen real de tienda usando repos/admin.

Estado de routing:
- Ya conectado por OOP en `main` (sin bridge legacy).

Definicion de cierre:
- Opcion Tienda funcional por OOP con informacion real.

## Cierre final de migracion
Archivos:
- `src/main.cpp`
- `src/main.cpp`

Tareas:
- Cuando todos los menus esten completos en OOP, marcar `main.cpp` como
  deprecated de forma explicita en documentacion/comentario de cabecera.
- Mantener `main.cpp` como arranque unico del programa.

## Criterios de calidad por sesion
- Ejecutar siempre:
  - `cmake -S . -B build && cmake --build build`
- Probar manualmente el menu modificado:
  - crear -> listar -> buscar -> actualizar -> eliminar (si aplica)
- No introducir cambios de formato binario sin plan de migracion.

## Estrategia de trabajo sugerida
- Hacer un menu por PR/iteracion.
- No mezclar varios modulos en el mismo cambio.
- Mantener mensajes y UX de consola consistentes con el sistema actual.
