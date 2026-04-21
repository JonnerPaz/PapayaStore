import sys
import os

# Agregamos la ruta del módulo compilado para que Python lo encuentre
sys.path.append(os.path.join(os.path.dirname(__file__), "build"))

try:
    import papaya_backend
except ImportError as e:
    print("Error importando el módulo de C++:", e)
    sys.exit(1)


def run_tests():
    print("Inicializando Base de Datos (Binarios)...")
    if not papaya_backend.init_database():
        print("Fallo al inicializar la base de datos.")
        return
    print("Base de datos lista.\n")

    print("Creando Repositorio de Productos...")
    repo_productos = papaya_backend.ProductoRepository()

    estadisticas_result = repo_productos.obtener_estadisticas()
    if isinstance(estadisticas_result, str):
        print("Error obteniendo estadísticas:", estadisticas_result)
        return

    estadisticas = estadisticas_result
    print(
        f"Estadísticas -> Registros Activos: {estadisticas.registros_activos}, Próximo ID: {estadisticas.proximo_id}"
    )

    print("\n--- Creando un Producto de Prueba ---")
    nuevo_producto = papaya_backend.Producto()
    nuevo_producto.set_id(estadisticas.proximo_id)
    nuevo_producto.set_nombre("Mango")
    nuevo_producto.set_codigo("MNG-001")
    nuevo_producto.set_descripcion("Mango maduro")
    nuevo_producto.set_precio(2.50)
    nuevo_producto.set_stock(100)
    nuevo_producto.set_stock_minimo(10)
    nuevo_producto.set_id_proveedor(1)

    resultado = repo_productos.guardar(nuevo_producto)
    if isinstance(resultado, str):
        print("Error guardando producto:", resultado)
    else:
        print("Producto guardado exitosamente!")

    print("\n--- Buscando el Producto por Nombre ---")
    busqueda = repo_productos.leer_por_nombre("Mango")
    if isinstance(busqueda, str):
        print("No se encontró o error:", busqueda)
    else:
        print(
            f"¡Encontrado! ID: {busqueda.get_id()}, Nombre: {busqueda.get_nombre()}, Precio: {busqueda.get_precio():.2f}"
        )


if __name__ == "__main__":
    run_tests()
