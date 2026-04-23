import flet as ft


def ProvidersView(page: ft.Page) -> ft.Control:
    return ft.Column(
        [
            ft.Text("Proveedores", size=32, weight=ft.FontWeight.BOLD),
            ft.Divider(),
            ft.Text(
                "Aquí se mostrará la gestión de proveedores...",
                color=ft.Colors.ON_SURFACE_VARIANT,
            ),
        ],
        expand=True,
    )


def register_routes(router) -> None:
    router.register(
        path="/providers",
        label="Proveedores",
        icon=ft.Icons.ANALYTICS_OUTLINED,
        selected_icon=ft.Icons.ANALYTICS,
        view_factory=ProvidersView,
    )
