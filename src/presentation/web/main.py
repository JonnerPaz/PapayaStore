import flet as ft
from components.appbar import create_appbar
from components.sidebar import create_sidebar
from router import AppRouter
from views.dashboard import register_routes as register_dashboard_routes
from views.products import register_routes as register_products_routes
from views.providers import register_routes as register_providers_routes
from views.settings import register_routes as register_settings_routes
from views.transactions import register_routes as register_transactions_routes
from views.clients import register_routes as register_clients_routes


def main(page: ft.Page):
    # Configuración principal
    page.title = "Papaya Store - Panel Admin"
    page.theme_mode = ft.ThemeMode.DARK
    page.padding = 12

    # Contenedor dinámico principal
    content_area = ft.Container(expand=True, padding=20)

    router = AppRouter(page)
    register_dashboard_routes(router)
    register_products_routes(router)
    register_providers_routes(router)
    register_clients_routes(router)
    register_transactions_routes(router)
    register_settings_routes(router)

    sidebar = create_sidebar(page, router)
    router.bind_layout(content_area=content_area, sidebar=sidebar)
    page.on_route_change = router.handle_route_change

    # Ensamblar Layout Principal
    page.appbar = create_appbar(page)
    page.add(
        ft.Row(
            controls=[sidebar, ft.VerticalDivider(width=1), content_area],
            expand=True,
        )
    )

    router.ensure_initial_route()


if __name__ == "__main__":
    # Ejecutar la aplicación web en el puerto 8000
    ft.run(main, view=ft.AppView.WEB_BROWSER, port=8000)
