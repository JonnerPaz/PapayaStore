import flet as ft


def SettingsView(page: ft.Page) -> ft.Control:
    return ft.Column(
        [
            ft.Text("Configuración", size=32, weight=ft.FontWeight.BOLD),
            ft.Divider(),
            ft.Text("General", size=24, weight=ft.FontWeight.W_500),
            ft.Switch(
                label="Activar notificaciones de inventario bajo", value=True
            ),
            ft.Divider(height=40),
            ft.Text(
                "Copias de Seguridad (Backups)",
                size=24,
                weight=ft.FontWeight.W_500,
            ),
            ft.Text(
                "Gestiona los archivos binarios de la base de datos.",
                color=ft.Colors.ON_SURFACE_VARIANT,
            ),
            ft.Container(height=10),
            ft.Row(
                [
                    ft.Button(
                        "Crear Backup Nuevo",
                        icon=ft.Icons.SAVE,
                        style=ft.ButtonStyle(
                            color=ft.Colors.ON_PRIMARY,
                            bgcolor=ft.Colors.PRIMARY,
                        ),
                    ),
                    ft.OutlinedButton(
                        "Restaurar Backup", icon=ft.Icons.RESTORE
                    ),
                ]
            ),
            ft.Container(height=20),
            ft.Text(
                "Últimos backups realizados:",
                size=16,
                weight=ft.FontWeight.W_500,
            ),
            # Lista simulada de backups
            ft.ListView(
                [
                    ft.ListTile(
                        leading=ft.Icon(ft.Icons.DATA_USAGE),
                        title=ft.Text("backup_2026_04_21.bin"),
                        subtitle=ft.Text("Hace 1 día"),
                    ),
                    ft.ListTile(
                        leading=ft.Icon(ft.Icons.DATA_USAGE),
                        title=ft.Text("backup_2026_04_20.bin"),
                        subtitle=ft.Text("Hace 2 días"),
                    ),
                ],
                expand=True,
            ),
        ],
        expand=True,
    )


def register_routes(router) -> None:
    router.register(
        path="/settings",
        label="Ajustes",
        icon=ft.Icons.SETTINGS_OUTLINED,
        selected_icon=ft.Icons.SETTINGS,
        view_factory=SettingsView,
    )
