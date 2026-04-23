from __future__ import annotations

from dataclasses import dataclass
from typing import Callable

import flet as ft

ViewFactory = Callable[[ft.Page], ft.Control]


@dataclass(frozen=True)
class RouteEntry:
    path: str
    label: str
    icon: ft.IconDataOrControl
    selected_icon: ft.IconDataOrControl
    view_factory: ViewFactory


class AppRouter:
    def __init__(self, page: ft.Page) -> None:
        self.page = page
        self._routes: list[RouteEntry] = []
        self._route_to_index: dict[str, int] = {}
        self._content_area: ft.Container | None = None
        self._sidebar: ft.NavigationRail | None = None

    def register(
        self,
        *,
        path: str,
        label: str,
        icon: ft.IconDataOrControl,
        selected_icon: ft.IconDataOrControl,
        view_factory: ViewFactory,
    ) -> None:
        if path in self._route_to_index:
            raise ValueError(f"Route already registered: {path}")

        self._route_to_index[path] = len(self._routes)
        self._routes.append(
            RouteEntry(
                path=path,
                label=label,
                icon=icon,
                selected_icon=selected_icon,
                view_factory=view_factory,
            )
        )

    def destinations(self) -> list[ft.NavigationRailDestination]:
        return [
            ft.NavigationRailDestination(
                icon=route.icon,
                selected_icon=route.selected_icon,
                label=route.label,
            )
            for route in self._routes
        ]

    def bind_layout(
        self, *, content_area: ft.Container, sidebar: ft.NavigationRail
    ) -> None:
        self._content_area = content_area
        self._sidebar = sidebar

    def navigate_by_index(self, index: int | None) -> None:
        if index is None or index < 0 or index >= len(self._routes):
            return

        route = self._routes[index].path
        if route != self.page.route:
            self.page.run_task(self.page.push_route, route)

    def handle_route_change(self, e: ft.RouteChangeEvent) -> None:
        self._render_current_route()

    def ensure_initial_route(self) -> None:
        if not self._routes:
            raise RuntimeError("No routes registered")

        if self.page.route in self._route_to_index:
            self._render_current_route()
            return

        self.page.run_task(self.page.push_route, self._routes[0].path)

    def _render_current_route(self) -> None:
        if self._content_area is None or self._sidebar is None:
            raise RuntimeError("Router layout is not bound")

        index = self._route_to_index.get(self.page.route)
        if index is None:
            index = 0
            fallback = self._routes[index].path
            if fallback != self.page.route:
                self.page.run_task(self.page.push_route, fallback)

        route = self._routes[index]
        self._sidebar.selected_index = index
        self._content_area.content = route.view_factory(self.page)
        self.page.update()
