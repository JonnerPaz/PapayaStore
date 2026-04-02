#include "MenuTransacciones.hpp"

#include <iostream>

void MenuTransacciones::registrarCompra()
{
    std::cout << "Registrar compra (En desarrollo)" << std::endl;
}

void MenuTransacciones::registrarVenta()
{
    std::cout << "Registrar venta (En desarrollo)" << std::endl;
}

void MenuTransacciones::buscarTransacciones()
{
    std::cout << "Buscar transacciones (En desarrollo)" << std::endl;
}

void MenuTransacciones::listarTransacciones()
{
    std::cout << "Listar transacciones (En desarrollo)" << std::endl;
}

void MenuTransacciones::cancelarTransaccion()
{
    std::cout << "Cancelar transaccion (En desarrollo)" << std::endl;
}

void MenuTransacciones::showMenu()
{
    setOption(0, "Registrar Compra", [this]() { registrarCompra(); });
    setOption(1, "Registrar Venta", [this]() { registrarVenta(); });
    setOption(2, "Buscar Transacciones", [this]() { buscarTransacciones(); });
    setOption(3, "Listar Transacciones", [this]() { listarTransacciones(); });
    setOption(4, "Cancelar Transaccion", [this]() { cancelarTransaccion(); });
    drawMenu();
}
