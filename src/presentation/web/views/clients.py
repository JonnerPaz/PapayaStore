import flet as ft


def ClientsView(page: ft.Page) -> ft.Control:
    return ft.Column(
        [
            ft.Row(
                [
                    ft.Text(
                        "Gestión de Clientes",
                        size=32,
                        weight=ft.FontWeight.BOLD,
                    ),
                    ft.Button("Nuevo Cliente", icon=ft.Icons.ADD),
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


def register_routes(router) -> None:
    router.register(
        path="/clients",
        label="Clientes",
        icon=ft.Icons.CHILD_CARE_OUTLINED,
        selected_icon=ft.Icons.CHILD_CARE,
        view_factory=ClientsView,
    )
