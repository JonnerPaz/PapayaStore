import flet as ft


def ProductsView(page: ft.Page) -> ft.Control:
    return ft.Column(
        [
            ft.Row(
                [
                    ft.Text(
                        "Gestión de Productos",
                        size=32,
                        weight=ft.FontWeight.BOLD,
                    ),
                    ft.ElevatedButton("Nuevo Producto", icon=ft.Icons.ADD),
                ],
                alignment=ft.MainAxisAlignment.SPACE_BETWEEN,
            ),
            ft.Divider(),
            ft.Text(
                "Aquí se mostrará la tabla de inventario...",
                color=ft.Colors.ON_SURFACE_VARIANT,
            ),
            # A futuro aquí irá un ft.DataTable
        ],
        expand=True,
    )
