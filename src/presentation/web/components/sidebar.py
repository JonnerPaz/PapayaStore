import flet as ft
from flet import NavigationRail

from router import AppRouter


def create_sidebar(
    page: ft.Page, router: AppRouter, selected_index: int = 0
) -> ft.NavigationRail:
    def on_nav_change(e: ft.Event[NavigationRail]) -> None:
        router.navigate_by_index(e.control.selected_index)

    return NavigationRail(
        selected_index=selected_index,
        label_type=ft.NavigationRailLabelType.ALL,
        min_width=100,
        min_extended_width=200,
        destinations=router.destinations(),
        on_change=on_nav_change,
    )
