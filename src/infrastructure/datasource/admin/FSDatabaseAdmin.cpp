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

namespace fs = std::filesystem;
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

    std::ifstream productosFile(PRODUCTOS_PATH, std::ios::binary);
    if (productosFile.is_open()) {
        productosFile.seekg(sizeof(HeaderFile), std::ios::beg);
        for (int i = 0; i < std::get<HeaderFile>(productosHeader).cantidadRegistros; i++) {
            Producto producto;
            productosFile.read(reinterpret_cast<char*>(&producto), sizeof(Producto));

            if (producto.getEliminado()) continue;

            const int proveedorId = producto.getIdProveedor();
            if (proveedorId > 0) {
                auto proveedorResult = proveedores.leerPorId(proveedorId);
                if (std::holds_alternative<std::string>(proveedorResult)) {
                    erroresProductosProveedor++;
                }
            }
        }
    }

    auto transactionsHeader = this->transacciones.obtenerEstadisticas();
    if (std::get_if<std::string>(&transactionsHeader)) {
        throw std::runtime_error(std::get<std::string>(transactionsHeader));
    }

    std::ifstream transaccionesFile(TRANSACCIONES_PATH, std::ios::binary);
    if (transaccionesFile.is_open()) {
        transaccionesFile.seekg(sizeof(HeaderFile), std::ios::beg);
        for (int i = 0; i < std::get<HeaderFile>(transactionsHeader).cantidadRegistros; ++i) {
            Transaccion transaccion;
            transaccionesFile.read(reinterpret_cast<char*>(&transaccion), sizeof(Transaccion));
            if (transaccion.getEliminado()) {
                continue;
            }

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

            (void) erroresTransaccionProducto;
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

    std::ifstream productosFile(PRODUCTOS_PATH, std::ios::binary);
    if (!productosFile.is_open()) {
        throw std::runtime_error("No se pudo abrir productos.bin");
    }

    productosFile.seekg(sizeof(HeaderFile), std::ios::beg);
    int totalCriticos = 0;

    for (int i = 0; i < std::get<HeaderFile>(productosHeader).cantidadRegistros; ++i) {
        Producto producto;
        productosFile.read(reinterpret_cast<char*>(&producto), sizeof(Producto));
        if (!productosFile) {
            break;
        }

        if (producto.getEliminado()) {
            continue;
        }

        if (producto.getStock() <= producto.getStockMinimo()) {
            totalCriticos++;
        }
    }

    return totalCriticos;
}

void FSDatabaseAdmin::reporteHistorialCliente()
{
    std::cout << "Reporte historial cliente: pendiente de parametrizacion por ID de cliente."
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

    Tienda tienda;
    std::fstream tiendaFile(TIENDA_PATH, std::ios::binary | std::ios::in | std::ios::out);
    if (!tiendaFile.is_open()) {
        throw std::runtime_error("No se pudo abrir tienda.bin");
        return false;
    }

    tiendaFile.seekg(sizeof(HeaderFile), std::ios::beg);
    tiendaFile.read(reinterpret_cast<char*>(&tienda), sizeof(Tienda));
    if (!tiendaFile) {
        throw std::runtime_error("No se pudo leer tienda.bin");
        return false;
    }

    tienda.setTotalProductosActivos(std::get<HeaderFile>(productosHeader).registrosActivos);
    tienda.setTotalProveedoresActivos(std::get<HeaderFile>(proveedoresHeader).registrosActivos);
    tienda.setTotalClientesActivos(std::get<HeaderFile>(clientesHeader).registrosActivos);
    tienda.setTotalTransaccionesActivas(std::get<HeaderFile>(transaccionesHeader).registrosActivos);
    tienda.setFechaUltimaModificacion(system_clock::now());

    // save it
    tiendaFile.seekg(0, std::ios::beg);
    tiendaFile.write(reinterpret_cast<char*>(&tienda), sizeof(Tienda));
    if (!tiendaFile) {
        throw std::runtime_error("No se pudo escribir tienda.bin");
    }

    return true;
}
