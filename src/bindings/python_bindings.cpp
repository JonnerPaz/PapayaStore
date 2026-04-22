#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // Para std::variant, std::string, etc.

#include <array>
#include <filesystem>
#include <fstream>

#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"
#include "domain/entities/producto/producto.entity.hpp"
#include "domain/entities/tienda/tienda.entity.hpp"
#include "infrastructure/datasource/EntityTraits.hpp"
#include "infrastructure/datasource/producto/FSProductoRepository.hpp"

// Necesario para usar las las librerías en python
namespace py = pybind11;
namespace fs = std::filesystem;

// Helpers para inicializar los binarios
bool ensureFileWithHeader(const fs::path& path)
{
    if (path.has_parent_path()) {
        std::error_code ec;
        fs::create_directories(path.parent_path(), ec);
        if (ec) return false;
    }
    if (fs::exists(path)) return true;

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) return false;

    const HeaderFile header{0, 1, 0, 1};
    file.write(reinterpret_cast<const char*>(&header), sizeof(HeaderFile));
    return static_cast<bool>(file);
}

bool ensureTiendaRecord()
{
    std::fstream file(Constants::PATHS::TIENDA_PATH,
                      std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) return false;

    HeaderFile header = {};
    file.read(reinterpret_cast<char*>(&header), sizeof(HeaderFile));
    if (!file) return false;

    if (header.cantidadRegistros > 0) return true;

    Tienda tienda;
    tienda.setId(1);
    tienda.setNombre("Papaya Store");
    tienda.setRif("N/A");

    file.seekp(sizeof(HeaderFile), std::ios::beg);
    if (!EntityTraits<Tienda>::writeToStream(file, tienda)) return false;

    header.cantidadRegistros = 1;
    header.proximoID = 2;
    header.registrosActivos = 1;
    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(&header), sizeof(HeaderFile));
    return static_cast<bool>(file);
}

bool init_database()
{
    const std::array<fs::path, 5> paths = {
        Constants::PATHS::PRODUCTOS_PATH, Constants::PATHS::PROVEEDORES_PATH,
        Constants::PATHS::CLIENTES_PATH,  Constants::PATHS::TRANSACCIONES_PATH,
        Constants::PATHS::TIENDA_PATH,
    };
    bool ok = true;
    for (const fs::path& path : paths) {
        ok = ensureFileWithHeader(path) && ok;
    }
    if (ok) {
        ok = ensureTiendaRecord() && ok;
    }
    return ok;
}

PYBIND11_MODULE(papaya_backend, m)
{
    m.doc() = "PapayaStore Backend Bindings";

    m.def("init_database", &init_database, "Inicializa la base de datos de archivos binarios");

    // Exponer HeaderFile
    py::class_<HeaderFile>(m, "HeaderFile")
        .def_readwrite("proximo_id", &HeaderFile::proximoID)
        .def_readwrite("cantidad_registros", &HeaderFile::cantidadRegistros)
        .def_readwrite("registros_activos", &HeaderFile::registrosActivos)
        .def_readwrite("version", &HeaderFile::version);

    // Exponer EntidadBase
    py::class_<EntidadBase>(m, "EntidadBase")
        .def("get_id", &EntidadBase::getId)
        .def("set_id", &EntidadBase::setId)
        .def("get_nombre", [](const EntidadBase& e) { return std::string(e.getNombre()); })
        .def("set_nombre",
             [](EntidadBase& e, const std::string& nombre) { e.setNombre(nombre.c_str()); })
        .def("get_eliminado", &EntidadBase::getEliminado)
        .def("set_eliminado", &EntidadBase::setEliminado)
        .def("get_fecha_creacion", &EntidadBase::getFechaCreacion)
        .def("set_fecha_creacion", &EntidadBase::setFechaCreacion)
        .def("get_fecha_ultima_modificacion", &EntidadBase::getFechaUltimaModificacion)
        .def("set_fecha_ultima_modificacion", &EntidadBase::setFechaUltimaModificacion);

    // Exponer Producto
    py::class_<Producto, EntidadBase>(m, "Producto")
        .def(py::init<>())
        .def("get_precio", &Producto::getPrecio)
        .def("set_precio", &Producto::setPrecio)
        .def("get_stock", &Producto::getStock)
        .def("set_stock", &Producto::setStock)
        .def("get_id_proveedor", &Producto::getIdProveedor)
        .def("set_id_proveedor", &Producto::setIdProveedor)
        .def("get_stock_minimo", &Producto::getStockMinimo)
        .def("set_stock_minimo", &Producto::setStockMinimo)
        .def("get_total_vendido", &Producto::getTotalVendido)
        .def("set_total_vendido", &Producto::setTotalVendido)
        .def("get_codigo", [](const Producto& p) { return std::string(p.getCodigo()); })
        .def("set_codigo",
             [](Producto& p, const std::string& codigo) { p.setCodigo(codigo.c_str()); })
        .def("get_descripcion", [](const Producto& p) { return std::string(p.getDescripcion()); })
        .def("set_descripcion",
             [](Producto& p, const std::string& desc) { p.setDescripcion(desc.c_str()); })
        .def("__repr__", [](const Producto& p) {
            return std::format("Producto({}, {})", p.getNombre(), p.getId());
        });

    // Exponer FSProductoRepository
    py::class_<FSProductoRepository>(m, "ProductoRepository")
        .def(py::init<>())
        .def("leer_por_id", &FSProductoRepository::leerPorId)
        .def("leer_por_nombre", &FSProductoRepository::leerPorNombre)
        .def("guardar", &FSProductoRepository::guardar)
        .def("actualizar", &FSProductoRepository::actualizar)
        .def("eliminar_logicamente", &FSProductoRepository::eliminarLogicamente)
        .def("obtener_estadisticas", &FSProductoRepository::obtenerEstadisticas);
}
