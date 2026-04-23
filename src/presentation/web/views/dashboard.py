import flet as ft


def DashboardView(page: ft.Page) -> ft.Control:
    return ft.Column(
        [
            ft.Text("Dashboard", size=32, weight=ft.FontWeight.BOLD),
            ft.Text(
                "Bienvenido al panel de control de Papaya Store.",
                color=ft.Colors.ON_SURFACE_VARIANT,
            ),
            ft.Divider(),
            ft.Row(
                [
                    ft.Card(
                        content=ft.Container(
                            padding=20,
                            content=ft.Column(
                                [
                                    ft.Text("Total Productos", size=16),
                                    ft.Text(
                                        "120",
                                        size=24,
                                        weight=ft.FontWeight.BOLD,
                                    ),
                                ]
                            ),
                        )
                    ),
                    ft.Card(
                        content=ft.Container(
                            padding=20,
                            content=ft.Column(
                                [
                                    ft.Text("Ventas Hoy", size=16),
                                    ft.Text("15", size=24, weight=ft.FontWeight.BOLD),
                                ]
                            ),
                        )
                    ),
                ]
            ),
        ],
        expand=True,
    )


def register_routes(router) -> None:
    router.register(
        path="/",
        label="Inicio",
        icon=ft.Icons.DASHBOARD_OUTLINED,
        selected_icon=ft.Icons.DASHBOARD,
        view_factory=DashboardView,
    )
