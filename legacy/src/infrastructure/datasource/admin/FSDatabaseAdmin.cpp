#include "FSDatabaseAdmin.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <variant>

#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"
#include "domain/entities/producto/producto.entity.hpp"
#include "domain/entities/tienda/tienda.entity.hpp"
#include "domain/entities/transaccion/transaccion.entity.hpp"
#include "infrastructure/datasource/EntityTraits.hpp"

namespace fs = std::filesystem;
using namespace Constants::ASCII_CODES;
using namespace Constants::PATHS;
using namespace std::chrono;

FSDatabaseAdmin::FSDatabaseAdmin(IProductoRepository& productos, IClienteRepository& clientes,
                                 IProveedorRepository& proveedores,
                                 ITransaccionRepository& transacciones)
    : productos(productos),
      clientes(clientes),
      proveedores(proveedores),
      transacciones(transacciones)
{
}

std::variant<HeaderFile, std::string> FSDatabaseAdmin::leerHeaderTienda()
{
    std::ifstream tiendaFile(TIENDA_PATH, std::ios::binary);
    if (!tiendaFile.is_open()) {
        return "No se pudo abrir tienda.bin";
    }

    HeaderFile header = {};
    tiendaFile.read(reinterpret_cast<char*>(&header), sizeof(HeaderFile));
    if (!tiendaFile) {
        return "No se pudo leer header de tienda.bin";
    }

    return header;
}

std::variant<Tienda, std::string> FSDatabaseAdmin::leerRegistroTienda()
{
    auto headerResult = leerHeaderTienda();
    if (std::holds_alternative<std::string>(headerResult)) {
        return std::get<std::string>(headerResult);
    }

    HeaderFile header = std::get<HeaderFile>(headerResult);
    if (header.cantidadRegistros <= 0) {
        return "tienda.bin no tiene registros activos";
    }

    std::ifstream tiendaFile(TIENDA_PATH, std::ios::binary);
    if (!tiendaFile.is_open()) {
        return "No se pudo abrir tienda.bin";
    }

    tiendaFile.seekg(sizeof(HeaderFile), std::ios::beg);
    Tienda tienda;
    if (!EntityTraits<Tienda>::readFromStream(tiendaFile, tienda)) {
        return "No se pudo leer registro de tienda";
    }

    return tienda;
}

std::variant<bool, std::string> FSDatabaseAdmin::guardarRegistroTienda(const Tienda& tienda,
                                                                       const HeaderFile& header)
{
    std::fstream tiendaFile(TIENDA_PATH, std::ios::binary | std::ios::in | std::ios::out);
    if (!tiendaFile.is_open()) {
        return "No se pudo abrir tienda.bin";
    }

    tiendaFile.seekp(0, std::ios::beg);
    tiendaFile.write(reinterpret_cast<const char*>(&header), sizeof(HeaderFile));
    if (!tiendaFile) {
        return "No se pudo escribir header de tienda.bin";
    }

    tiendaFile.seekp(sizeof(HeaderFile), std::ios::beg);
    if (!EntityTraits<Tienda>::writeToStream(tiendaFile, tienda)) {
        return "No se pudo escribir registro de tienda";
    }

    return true;
}

void FSDatabaseAdmin::crearBackup()
{
    fs::create_directories(BACKUP_PATH);

    const fs::path paths[5] = {PRODUCTOS_PATH, PROVEEDORES_PATH, CLIENTES_PATH, TRANSACCIONES_PATH,
                               TIENDA_PATH};

    const auto now = system_clock::now();
    const auto currentDate = time_point_cast<days>(now);
    const year_month_day ymd{currentDate};
    const auto nowSec = time_point_cast<seconds>(now);
    const zoned_time localTime{current_zone(), nowSec};

    for (const fs::path& path : paths) {
        if (!fs::exists(path)) {
            continue;
        }

        const std::string baseName = path.stem().string();
        const fs::path backupName =
            fs::path(std::format("{}.{:%Y-%m-%d}.{:%T}.bin", baseName, ymd, localTime));
        const fs::path backupPath = BACKUP_PATH / backupName;

        fs::copy_file(path, backupPath, fs::copy_options::overwrite_existing);
    }
}

std::tuple<int, int, int, int> FSDatabaseAdmin::verificarIntegridadReferencial()
{
    int erroresProductosProveedor = 0, erroresTransaccionRelacionado = 0,
        erroresTransaccionProducto = 0, erroresTipoTransaccion = 0;

    auto productosHeader = this->productos.obtenerEstadisticas();
    if (std::get_if<std::string>(&productosHeader)) {
        throw std::runtime_error(std::get<std::string>(productosHeader));
    }

    const HeaderFile productosStats = std::get<HeaderFile>(productosHeader);
    for (int id = 1; id < productosStats.proximoID; ++id) {
        auto productoResult = productos.leerPorId(id);
        if (!std::holds_alternative<Producto>(productoResult)) {
            continue;
        }

        const Producto& producto = std::get<Producto>(productoResult);
        const int proveedorId = producto.getIdProveedor();
        if (proveedorId > 0) {
            auto proveedorResult = proveedores.leerPorId(proveedorId);
            if (std::holds_alternative<std::string>(proveedorResult)) {
                ++erroresProductosProveedor;
            }
        }
    }

    auto transactionsHeader = this->transacciones.obtenerEstadisticas();
    if (std::get_if<std::string>(&transactionsHeader)) {
        throw std::runtime_error(std::get<std::string>(transactionsHeader));
    }

    const HeaderFile transaccionesStats = std::get<HeaderFile>(transactionsHeader);
    for (int id = 1; id < transaccionesStats.proximoID; ++id) {
        auto transaccionResult = transacciones.leerPorId(id);
        if (!std::holds_alternative<Transaccion>(transaccionResult)) {
            continue;
        }

        const Transaccion& transaccion = std::get<Transaccion>(transaccionResult);
        const auto tipo = transaccion.getTipoTransaccion();
        if (tipo != COMPRA && tipo != VENTA) {
            ++erroresTipoTransaccion;
            continue;
        }

        const int relacionadoId = transaccion.getIdRelacionado();
        if (tipo == COMPRA) {
            auto proveedorResult = proveedores.leerPorId(relacionadoId);
            if (std::holds_alternative<std::string>(proveedorResult)) {
                ++erroresTransaccionRelacionado;
            }
        } else {
            auto clienteResult = clientes.leerPorId(relacionadoId);
            if (std::holds_alternative<std::string>(clienteResult)) {
                ++erroresTransaccionRelacionado;
            }
        }

        const int productosTotales = transaccion.getProductosTotales();
        for (int i = 0; i < productosTotales; ++i) {
            TransaccionDTO productoTransaccion = {};
            if (!transaccion.getProductoEnIndice(i, productoTransaccion)) {
                ++erroresTransaccionProducto;
                continue;
            }

            auto productoResult = productos.leerPorId(productoTransaccion.productoId);
            if (std::holds_alternative<std::string>(productoResult)) {
                ++erroresTransaccionProducto;
            }
        }
    }

    return {erroresProductosProveedor, erroresTransaccionRelacionado, erroresTransaccionProducto,
            erroresTipoTransaccion};
}

int FSDatabaseAdmin::reporteStockCritico()
{
    const auto productosHeader = this->productos.obtenerEstadisticas();
    if (std::get_if<std::string>(&productosHeader)) {
        throw std::runtime_error(std::get<std::string>(productosHeader));
    }

    int totalCriticos = 0;

    const HeaderFile productosStats = std::get<HeaderFile>(productosHeader);
    for (int id = 1; id < productosStats.proximoID; ++id) {
        auto productoResult = productos.leerPorId(id);
        if (!std::holds_alternative<Producto>(productoResult)) {
            continue;
        }

        const Producto& producto = std::get<Producto>(productoResult);

        if (producto.getStock() <= producto.getStockMinimo()) {
            totalCriticos++;
        }
    }

    return totalCriticos;
}

void FSDatabaseAdmin::reporteHistorialCliente(int idCliente)
{
    auto clienteResult = clientes.leerPorId(idCliente);
    if (std::holds_alternative<std::string>(clienteResult)) {
        std::cout << "No se pudo generar historial del cliente: "
                  << std::get<std::string>(clienteResult) << std::endl;
        return;
    }

    const Cliente& cliente = std::get<Cliente>(clienteResult);

    std::cout << COLOR_CYAN << "========================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_CYAN << "      REPORTE: HISTORIAL DE CLIENTE    " << COLOR_RESET << std::endl;
    std::cout << COLOR_CYAN << "========================================" << COLOR_RESET << std::endl;
    std::cout << std::format("{}ID Cliente: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getId())
              << COLOR_RESET << std::endl;
    std::cout << std::format("{}Nombre: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getNombre())
              << COLOR_RESET << std::endl;
    std::cout << std::format("{}Cedula: {}{}", COLOR_YELLOW, COLOR_GREEN, cliente.getCedula())
              << COLOR_RESET << std::endl;
    std::cout << std::format("{}Total compras acumuladas: {}${:.2f}", COLOR_YELLOW, COLOR_GREEN,
                             cliente.getTotalCompras())
              << COLOR_RESET << std::endl;

    auto transaccionesHeader = transacciones.obtenerEstadisticas();
    if (std::holds_alternative<std::string>(transaccionesHeader)) {
        std::cout << "No se pudo leer transacciones: " << std::get<std::string>(transaccionesHeader)
                  << std::endl;
        return;
    }

    const HeaderFile transStats = std::get<HeaderFile>(transaccionesHeader);
    int transaccionesMostradas = 0;

    for (int idTransaccion = 1; idTransaccion < transStats.proximoID; ++idTransaccion) {
        auto transaccionResult = transacciones.leerPorId(idTransaccion);
        if (!std::holds_alternative<Transaccion>(transaccionResult)) {
            continue;
        }

        const Transaccion& transaccion = std::get<Transaccion>(transaccionResult);
        if (transaccion.getTipoTransaccion() != VENTA ||
            transaccion.getIdRelacionado() != cliente.getId()) {
            continue;
        }

        ++transaccionesMostradas;
        std::cout << "\n" << COLOR_YELLOW << "Transaccion #" << COLOR_GREEN << transaccion.getId()
                  << COLOR_RESET << std::endl;
        std::cout << std::format("{}Tipo: {}VENTA", COLOR_YELLOW, COLOR_GREEN) << COLOR_RESET
                  << std::endl;
        std::cout << std::format("{}Total: {}${:.2f}", COLOR_YELLOW, COLOR_GREEN,
                                 transaccion.getTotal())
                  << COLOR_RESET << std::endl;
        std::cout << std::format("{}Descripcion: {}{}", COLOR_YELLOW, COLOR_GREEN,
                                 transaccion.getDescripcion())
                  << COLOR_RESET << std::endl;

        std::cout << std::format("{:<10} | {:<24} | {:<10} | {:<12} | {:<12}", "Prod ID",
                                 "Producto", "Cantidad", "Precio U.", "Subtotal")
                  << std::endl;
        std::cout << "-----------------------------------------------------------------------"
                  << std::endl;

        for (int i = 0; i < transaccion.getProductosTotales(); ++i) {
            TransaccionDTO item = {};
            if (!transaccion.getProductoEnIndice(i, item)) {
                std::cout << std::format("[Aviso] No se pudo leer item {} de la transaccion {}.",
                                         i + 1, transaccion.getId())
                          << std::endl;
                continue;
            }

            std::string nombreProducto = "(No encontrado)";
            auto productoResult = productos.leerPorId(item.productoId);
            if (std::holds_alternative<Producto>(productoResult)) {
                nombreProducto = std::get<Producto>(productoResult).getNombre();
            }

            const float subtotal = static_cast<float>(item.cantidad) * item.precio;
            std::cout << std::format("{:<10} | {:<24} | {:<10} | ${:<11.2f} | ${:<11.2f}",
                                     item.productoId, nombreProducto, item.cantidad, item.precio,
                                     subtotal)
                      << std::endl;
        }
    }

    if (transaccionesMostradas == 0) {
        std::cout << COLOR_GREEN << "Este cliente no tiene ventas asociadas." << COLOR_RESET
                  << std::endl;
        return;
    }

    std::cout << "\n" << std::format("Transacciones validas mostradas: {}", transaccionesMostradas)
              << std::endl;
}

bool FSDatabaseAdmin::sincronizarContadoresTienda()
{
    const auto productosHeader = this->productos.obtenerEstadisticas();
    const auto proveedoresHeader = this->proveedores.obtenerEstadisticas();
    const auto clientesHeader = this->clientes.obtenerEstadisticas();
    const auto transaccionesHeader = this->transacciones.obtenerEstadisticas();

    if (std::get_if<std::string>(&productosHeader) ||
        std::get_if<std::string>(&proveedoresHeader) || std::get_if<std::string>(&clientesHeader) ||
        std::get_if<std::string>(&transaccionesHeader)) {
        throw std::runtime_error("Error al obtener estadisticas");
    }

    auto headerResult = leerHeaderTienda();
    if (std::holds_alternative<std::string>(headerResult)) {
        throw std::runtime_error(std::get<std::string>(headerResult));
    }

    HeaderFile tiendaHeader = std::get<HeaderFile>(headerResult);
    Tienda tienda;
    float montoTotalVentas = 0.0f;
    float montoTotalCompras = 0.0f;

    if (tiendaHeader.cantidadRegistros > 0) {
        auto tiendaResult = leerRegistroTienda();
        if (std::holds_alternative<std::string>(tiendaResult)) {
            throw std::runtime_error(std::get<std::string>(tiendaResult));
        }
        tienda = std::get<Tienda>(tiendaResult);
    } else {
        tienda.setId(1);
        tienda.setNombre("Papaya Store");
        tienda.setRif("N/A");
        tiendaHeader.cantidadRegistros = 1;
        tiendaHeader.proximoID = 2;
        tiendaHeader.registrosActivos = 1;
    }

    tienda.setTotalProductosActivos(std::get<HeaderFile>(productosHeader).registrosActivos);
    tienda.setTotalProveedoresActivos(std::get<HeaderFile>(proveedoresHeader).registrosActivos);
    tienda.setTotalClientesActivos(std::get<HeaderFile>(clientesHeader).registrosActivos);
    tienda.setTotalTransaccionesActivas(std::get<HeaderFile>(transaccionesHeader).registrosActivos);

    const HeaderFile transStats = std::get<HeaderFile>(transaccionesHeader);
    for (int id = 1; id < transStats.proximoID; ++id) {
        auto transaccionResult = transacciones.leerPorId(id);
        if (!std::holds_alternative<Transaccion>(transaccionResult)) {
            continue;
        }

        const Transaccion& transaccion = std::get<Transaccion>(transaccionResult);
        if (transaccion.getTipoTransaccion() == VENTA) {
            montoTotalVentas += transaccion.getTotal();
        } else if (transaccion.getTipoTransaccion() == COMPRA) {
            montoTotalCompras += transaccion.getTotal();
        }
    }

    tienda.setMontoTotalVentas(montoTotalVentas);
    tienda.setMontoTotalCompras(montoTotalCompras);
    tienda.setFechaUltimaModificacion(system_clock::now());

    auto saveResult = guardarRegistroTienda(tienda, tiendaHeader);
    if (std::holds_alternative<std::string>(saveResult)) {
        throw std::runtime_error(std::get<std::string>(saveResult));
    }

    return true;
}
