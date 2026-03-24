# TODO.md - Plan de migracion OOP/MVC

Este archivo resume el plan general para sesiones de trabajo.
Objetivo: migrar el flujo legacy de `src/main.cpp` a la arquitectura OOP ya creada
(`domain` + `infrastructure` + `presentation`) sin romper funcionalidad ni datos.

## Estado actual (resumen rapido)
- `src/main.cpp` sigue teniendo la logica legacy completa.
- `src/main_oop.cpp` ya existe como nuevo entrypoint de migracion.
- El menu principal de `main_oop` ya enruta Productos por OOP.
- `MenuProductos` ya tiene implementacion funcional base (CRUD principal).
- El resto de menus siguen en bridge legacy o parcialmente en desarrollo.

## Objetivo de arquitectura
- Dejar `src/main.cpp` como legacy/deprecado y mantenerlo solo temporalmente.
- Dejar `src/main_oop.cpp` como composition root definitivo.
- Menus en `presentation` como UI/orquestacion.
- Reglas de negocio y validaciones en domain/repositories/services.
- Persistencia en `infrastructure/datasource`.

## Principios de ejecucion
- Migracion incremental por modulo.
- No mezclar dos fuentes de verdad para la misma regla.
- Cada paso debe compilar y poder probarse manualmente.

## Orden recomendado de menus
1. Productos (base de referencia, ya avanzado)
2. Proveedores
3. Clientes
4. Transacciones
5. Reportes
6. Tienda

## Plan por menu

### 1) MenuProductos
Archivo: `src/presentation/Menu/MenuProductos/MenuProductos.cpp`

Pendiente de consolidar:
- Revisar validaciones de duplicados (nombre/codigo) contra repositorio.
- Revisar consistencia con proveedor asociado al crear/actualizar.
- Confirmar mensajes UX en espanol y flujo cancelable con `q`.

Switch principal:
- `src/main_oop.cpp` debe llamar solo a `MenuProductos.showMenu()` (ya hecho).

Definicion de cierre:
- Crear, buscar, actualizar, listar y eliminar funcionan solo por OOP.

### 2) MenuProveedores
Archivos:
- `src/presentation/Menu/MenuProveedores/MenuProveedores.cpp`
- `src/main_oop.cpp`

Tareas:
- Implementar `crearProveedor`, `buscarProveedor`, `actualizarProveedor`,
  `listarProveedores`, `eliminarProveedor`.
- Validar restriccion de eliminacion si hay transacciones activas.
- Cambiar bridge en `main_oop` para usar `MenuProveedores.showMenu()`.

Definicion de cierre:
- Opcion Proveedores ya no usa `menuProveedores()` legacy.

### 3) MenuClientes
Archivos:
- `src/presentation/Menu/MenuClientes/MenuClientes.cpp`
- `src/main_oop.cpp`

Tareas:
- Completar `crearCliente`, `buscarCliente`, `actualizarCliente`, `eliminarCliente`.
- Mantener/ajustar `listarClientes` para consistencia de salida.
- Cambiar bridge en `main_oop` para usar `MenuClientes.showMenu()`.

Definicion de cierre:
- Opcion Clientes deja de depender de `menuClientes()` legacy.

### 4) MenuTransacciones
Archivos:
- `src/presentation/Menu/MenuTransacciones/MenuTransacciones.cpp`
- `src/main_oop.cpp`

Tareas:
- Implementar `registrarCompra`.
- Implementar `registrarVenta`.
- Implementar `buscarTransacciones` y `listarTransacciones`.
- Implementar `cancelarTransaccion` con validaciones de reversa.
- Cambiar bridge en `main_oop` a `MenuTransacciones.showMenu()`.

Definicion de cierre:
- Flujo de transacciones 100% OOP y consistente en inventario.

### 5) MenuReportes
Archivos:
- `src/presentation/Menu/MenuReportes/MenuReportes.cpp`
- `src/infrastructure/datasource/FSDatabaseAdmin.cpp`
- `src/main_oop.cpp`

Tareas:
- Completar implementaciones de admin:
  - `verificarIntegridadReferencial`
  - `crearBackup`
  - `reporteStockCritico`
  - `reporteHistorialCliente`
  - `sincronizarContadoresTienda`
- Conectar opcion Reportes por OOP en `main_oop`.

Definicion de cierre:
- Opcion Reportes sin bridge legacy.

### 6) MenuTienda
Archivos:
- `src/presentation/Menu/MenuTienda/MenuTienda.cpp`
- `src/main_oop.cpp`

Tareas:
- Implementar resumen real de tienda usando repos/admin.
- Conectar opcion Tienda por OOP en `main_oop`.

Definicion de cierre:
- Opcion Tienda sin bridge legacy.

## Cierre final de migracion
Archivos:
- `src/main.cpp`
- `src/main_oop.cpp`

Tareas:
- Cuando todos los menus esten en OOP, retirar bridges legacy restantes.
- Mantener `main.cpp` minimo o marcarlo como deprecated.
- Dejar `main_oop.cpp` como arranque unico del programa.

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
