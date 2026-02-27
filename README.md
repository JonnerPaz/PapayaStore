<div align="center">
  <h1>🍎 Papaya Store</h1>
  <p><b>Sistema de Gestión de Inventario en C++20</b></p>
  <p><i>Proyecto de Programación II - Universidad Rafael Urdaneta (URU)</i></p>
  <p>Desarrollado por <b>Jonner Paz</b> y <b>Andrés Martínez</b></p>
</div>

---

¡Bienvenido a **Papaya Store**! Este es un robusto sistema de gestión empresarial desarrollado en consola con **C++**. El software permite administrar el ciclo completo de una tienda: desde la llegada de mercancía por proveedores hasta la venta final al cliente, garantizando la integridad de los datos y una gestión eficiente de la memoria dinámica.

## 🚀 Características Principales

*   **📦 Gestión Integral:** Control total sobre **Productos**, **Proveedores** y **Clientes**.
*   **💳 Sistema de Transacciones Inteligente:**
    *   Registro de **Compras** y **Ventas**.
    *   **Validación de Stock:** No se permiten ventas si no hay existencias.
*   **🔍 Búsquedas Avanzadas:**
    *   Búsqueda exacta por ID.
    *   Búsqueda por nombre
*   **🛡️ Validaciones Robustas:**
    *   Verificación de email.
    *   Validación de fechas 
    *   Prevención RIF duplicados.

## 🛠️ Detalles Técnicos

El código implementa conceptos de C++ para asegurar escalabilidad y limpieza, adhiriéndose a restricciones educativas:

*   **Memoria Dinámica Manual:** Uso exclusivo de punteros (`new[]` / `delete[]`) y arrays dinámicos que crecen automáticamente (`redimensionarEntidad`) al alcanzar su capacidad.
*   **Templates y Conceptos:** Implementación de funciones genéricas para buscar cualquier entidad por ID y validación de tipos aritméticos mediante `std::concepts`.
*   **Seguridad de Entrada:** Manejo riguroso de `cin.fail()` y limpieza de buffer para evitar bucles infinitos por entradas de usuario erróneas.
*   **Formato Moderno:** Uso de `std::format` (C++20) para una salida de consola elegante, tabulada y legible.

### 📂 Arquitectura Central
El corazón del sistema reside en la estructura `Tienda`, que actúa como contenedor principal:

```cpp
struct Tienda {
    Producto* productos;        // Array dinámico
    Proveedor* proveedores;     // Array dinámico
    Cliente* clientes;          // Array dinámico
    Transaccion* transacciones; // Historial de movimientos
    // ... contadores (num) y capacidades máximas
};
```

## 🚦 Instalación y Uso

### Requisitos
*   Compilador compatible con **C++20** (GCC 10+, Clang 10+ o MSVC 19.29+).

### Compilación
Abre tu terminal en la raíz del proyecto y ejecuta:

```bash
g++ -std=c++20 -Wall -Wextra src/main.cpp -o papaya
```

### Ejecución
```bash
./papaya
```

## 📖 Guía de Uso Rápida

1.  **Inicialización:** El sistema arranca cargando la configuración base de la tienda en memoria.
2.  **Registro:** Antes de poder vender algo, asegúrate de registrar al menos un **Proveedor** y un **Producto**.
3.  **Compras (Abastecimiento):** Usa el módulo de transacciones para cargar el stock inicial de tus productos mediante una nueva "Compra".
4.  **Ventas:** Registra a un **Cliente** en el sistema y procede a realizar la venta. Papaya Store calculará los totales y descontará el inventario automáticamente.
5.  **Mantenimiento:** Puedes actualizar precios, descripciones o datos de contacto en sus respectivos módulos en cualquier momento.
