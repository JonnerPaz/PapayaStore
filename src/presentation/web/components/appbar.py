import flet as ft


def create_appbar(page: ft.Page) -> ft.AppBar:
    def toggle_theme(e):
        page.theme_mode = (
            ft.ThemeMode.LIGHT
            if page.theme_mode == ft.ThemeMode.DARK
            else ft.ThemeMode.DARK
        )
        page.update()

    return ft.AppBar(
        title=ft.Text("Papaya Store"),
        center_title=False,
        bgcolor=ft.Colors.ON_SURFACE_VARIANT,
        actions=[
            ft.IconButton(
                ft.Icons.LIGHTBULB_OUTLINE,
                on_click=toggle_theme,
                tooltip="Cambiar Tema",
            ),
        ],
    )
