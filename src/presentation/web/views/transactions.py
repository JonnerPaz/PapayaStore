import flet as ft


def TransactionsView(page: ft.Page) -> ft.Control:
    return ft.Column(
        [
            ft.Text("Transacciones", size=32, weight=ft.FontWeight.BOLD),
            ft.Divider(),
            ft.Text(
                "Aquí se mostrará la gestión de transacciones...",
                color=ft.Colors.ON_SURFACE_VARIANT,
            ),
        ],
        expand=True,
    )


def register_routes(router) -> None:
    router.register(
        path="/transactions",
        label="Transacciones",
        icon=ft.Icons.MONETIZATION_ON_OUTLINED,
        selected_icon=ft.Icons.MONETIZATION_ON,
        view_factory=TransactionsView,
    )
