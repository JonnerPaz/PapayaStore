<div align="center">
  <h1>🍎 Papaya Store</h1>
  <p><b>Sistema de Gestión de Inventario con persistencia binaria</b></p>
  <p><i>Proyecto de Programación II - Universidad Rafael Urdaneta (URU)</i></p>
  <p>Desarrollado por <b>Jonner Paz</b> y <b>Andrés Martínez</b></p>
</div>

---

Papaya Store es un sistema de consola en C++ enfocado en gestión de inventario con persistencia real en disco. Es un sistema ligero y estable capaz de almacenar una gran cantidad de productos

## Qué hace el sistema

- Gestiona **Productos**, **Proveedores**, **Clientes**, **Transacciones** y **Tienda**.
- Guarda todo en archivos `.bin` independientes dentro de `data/`.
- Aplica **borrado lógico** (`eliminado = true`) para preservar offsets físicos.
- Permite registrar **compras y ventas con múltiples productos por transacción**.
- Mantiene estadísticas por entidad (ej. `totalVendido`, `totalCompras`) y resumen global en `tienda.bin`.
- Incluye diagnóstico de integridad referencial, backup y reportes analíticos.

## Estructura de persistencia

Archivos operativos:

- `data/productos.bin`
- `data/proveedores.bin`
- `data/clientes.bin`
- `data/transacciones.bin`
- `data/tienda.bin`

Cada archivo inicia con un `ArchivoHeader` y luego los registros de su estructura correspondiente.

## Funcionalidades destacadas

### Transacciones

- Registro de compra y venta.
- Soporte de múltiples productos por transacción.
- Validación de stock por producto antes de confirmar una venta.
- Cancelación de transacción con reversión de stock y métricas.

### Integridad y seguridad de datos

- Verificación de integridad referencial entre productos, proveedores, clientes y transacciones.
- Restricción de borrado lógico cuando existen transacciones activas asociadas.
- Sistema de backup de los 5 archivos binarios con timestamp.

### Reportes

- Productos con stock crítico.
- Historial de cliente con detalle de transacciones y productos.
- Resumen general de tienda (contadores globales y montos acumulados).

## Requisitos

- Compilador con soporte para C++23 (o C++20 con soporte suficiente de `std::format` y `<chrono>` moderno).

## Compilación y ejecución

Desde la raíz del proyecto:

```bash
g++ -std=c++23 -Wall -Wextra src/main.cpp -o papaya
./papaya
```
