# PapayaStore

PapayaStore es un programa de inventario realizado por ***Jonner Paz*** y ***Andrés Martínez***
para la materia de programación II de la Universidad Rafael Urdaneta.

🍎 Papaya Store - Sistema de Gestión de Inventario
¡Bienvenido a Papaya Store! Este es un robusto sistema de gestión empresarial desarrollado en C++20. El software permite administrar el ciclo completo de una tienda: desde la llegada de mercancía por proveedores hasta la venta final al cliente, garantizando la integridad de los datos y una gestión eficiente de la memoria dinámica.

🚀 Características Principales
Gestión Integral (CRUD): Control total sobre Productos, Proveedores y Clientes.

Sistema de Transacciones Inteligente: * Registro de Compras (Entradas) y Ventas (Salidas).

Validación de Stock: No se permiten ventas si no hay existencias.

Cancelación Reversiva: Al cancelar una transacción, el stock se restaura automáticamente.

Búsquedas Avanzadas: * Búsqueda por ID.

Búsqueda por coincidencia parcial de nombres (insensible a mayúsculas/minúsculas).

Validaciones Robustas: * Verificación de formato de Email.

Validación de fechas reales (usando std::chrono).

Prevención de códigos y RIF duplicados.

🛠️ Detalles Técnicos (Para Desarrolladores)
El código implementa conceptos avanzados de C++ para asegurar escalabilidad y limpieza:

Memoria Dinámica: Uso de arrays dinámicos que crecen automáticamente (redimensionarEntidad) cuando se alcanza el límite de capacidad.

Templates y Conceptos: Implementación de funciones genéricas para buscar cualquier entidad por ID y validación de tipos aritméticos mediante std::concepts.

Seguridad de Entrada: Manejo de cin.fail() y limpieza de buffer para evitar bucles infinitos por entradas erróneas.

C++ Moderno: Uso de std::format para una salida de consola elegante y legible.

📂 Estructura del Proyecto
El corazón del sistema reside en la estructura Tienda, que actúa como contenedor principal:

C++
struct Tienda {
    Producto* productos;       // Array dinámico
    Proveedor* proveedores;    // Array dinámico
    Cliente* clientes;         // Array dinámico
    Transaccion* transacciones; // Historial de movimientos
    // ... contadores y capacidades
};
🚦 Instalación y Uso
Requisitos
Compilador compatible con C++20 (GCC 10+, Clang 10+ o MSVC 19.29+).

Compilación
Clona o descarga el archivo y ejecuta en tu terminal:

Bash
g++ -std=c++20 main.cpp -o PapayaStore
Ejecución
Bash
./PapayaStore
📖 Guía de Uso Rápida
Inicialización: El sistema arranca con la configuración de "Papaya Store".

Registro: Antes de vender, asegúrate de registrar al menos un Proveedor y un Producto.

Compras: Usa el módulo de transacciones para cargar stock inicial mediante una "Compra".

Ventas: Registra a un Cliente y procede a realizar la venta. El sistema calculará los totales automáticamente.

Mantenimiento: Puedes actualizar precios o descripciones en el módulo de Productos en cualquier momento.

⚠️ Notas de Seguridad
Cierre Seguro: Siempre usa la opción "0. Salir" del menú principal. Esto dispara la función liberarTienda(), la cual libera todos los punteros y evita el consumo innecesario de RAM tras cerrar el programa.