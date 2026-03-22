#include "presentation/MenuTienda.hpp"
#include <iostream>

void MenuTienda::mostrarResumenTienda() {
    if (!leerTienda(tienda)) {
        cout << CLEAR_SCREEN << COLOR_RED << "Error: No se pudo leer el registro de tienda."
             << COLOR_RESET << endl;
        return;
    }

    std::cout << CLEAR_SCREEN;
    std::cout << COLOR_CYAN << "========================================" << COLOR_RESET
              << std::endl;
    std::cout << COLOR_CYAN << "         RESUMEN DE TIENDA             " << COLOR_RESET
              << std::endl;
    std::cout << COLOR_CYAN << "========================================" << COLOR_RESET
              << std::endl;
    std::cout << COLOR_YELLOW << "ID: " << COLOR_RESET << tienda.id << std::endl;
    std::cout << COLOR_YELLOW << "Nombre: " << COLOR_RESET << tienda.nombre << std::endl;
    std::cout << COLOR_YELLOW << "RIF: " << COLOR_RESET << tienda.rif << std::endl;
    std::cout << COLOR_YELLOW << "Productos activos: " << COLOR_RESET
              << tienda.totalProductosActivos << std::endl;
    std::cout << COLOR_YELLOW << "Proveedores activos: " << COLOR_RESET
              << tienda.totalProveedoresActivos << std::endl;
    std::cout << COLOR_YELLOW << "Clientes activos: " << COLOR_RESET << tienda.totalClientesActivos
              << std::endl;
    std::cout << COLOR_YELLOW << "Transacciones activas: " << COLOR_RESET
              << tienda.totalTransaccionesActivas << std::endl;
    std::cout << COLOR_YELLOW << "Monto total ventas: " << COLOR_RESET
              << format("${:.2f}", tienda.montoTotalVentas) << std::endl;
    std::cout << COLOR_YELLOW << "Monto total compras: " << COLOR_RESET
              << format("${:.2f}", tienda.montoTotalCompras) << std::endl;
}
