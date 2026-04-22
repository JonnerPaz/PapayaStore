import flet as ft
from components.appbar import create_appbar
from components.sidebar import create_sidebar
from views.dashboard import DashboardView
from views.products import ProductsView
from views.settings import SettingsView


def main(page: ft.Page):
    # Configuración principal
    page.title = "Papaya Store - Web Admin"
    page.theme_mode = ft.ThemeMode.DARK
    page.padding = 0

    # Contenedor dinámico principal
    content_area = ft.Container(expand=True, padding=20)

    # Menú lateral
    sidebar = create_sidebar(page)

    # Lógica de enrutamiento (Routing)
    def route_change(route):
        if page.route == "/":
            sidebar.selected_index = 0
            content_area.content = DashboardView(page)
        elif page.route == "/products":
            sidebar.selected_index = 1
            content_area.content = ProductsView(page)
        elif page.route == "/settings":
            sidebar.selected_index = 2
            content_area.content = SettingsView(page)

        page.update()

    page.on_route_change = route_change

    # Ensamblar Layout Principal
    page.appbar = create_appbar(page)
    page.add(
        ft.Row(
            controls=[sidebar, ft.VerticalDivider(width=1), content_area],
            expand=True,
        )
    )

    # Inicializar la ruta actual (por defecto carga Dashboard)
    page.go(page.route if page.route else "/")


if __name__ == "__main__":
    # Ejecutar la aplicación web en el puerto 8000
    ft.run(main, view=ft.AppView.WEB_BROWSER, port=8000)
