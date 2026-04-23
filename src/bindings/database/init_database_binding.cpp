#include <array>
#include <filesystem>
#include <fstream>

#include "bindings/bindings.hpp"
#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"
#include "domain/entities/tienda/tienda.entity.hpp"
#include "infrastructure/datasource/EntityTraits.hpp"

namespace fs = std::filesystem;

namespace {

bool ensure_file_with_header(const fs::path& path)
{
    if (path.has_parent_path()) {
        std::error_code ec;
        fs::create_directories(path.parent_path(), ec);
        if (ec) {
            return false;
        }
    }

    if (fs::exists(path)) {
        return true;
    }

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    const HeaderFile header{0, 1, 0, 1};
    file.write(reinterpret_cast<const char*>(&header), sizeof(HeaderFile));
    return static_cast<bool>(file);
}

bool ensure_tienda_record()
{
    std::fstream file(Constants::PATHS::TIENDA_PATH,
                      std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        return false;
    }

    HeaderFile header = {};
    file.read(reinterpret_cast<char*>(&header), sizeof(HeaderFile));
    if (!file) {
        return false;
    }

    if (header.cantidadRegistros > 0) {
        return true;
    }

    Tienda tienda;
    tienda.setId(1);
    tienda.setNombre("Papaya Store");
    tienda.setRif("N/A");

    file.seekp(sizeof(HeaderFile), std::ios::beg);
    if (!EntityTraits<Tienda>::writeToStream(file, tienda)) {
        return false;
    }

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
        Constants::PATHS::PRODUCTOS_PATH,
        Constants::PATHS::PROVEEDORES_PATH,
        Constants::PATHS::CLIENTES_PATH,
        Constants::PATHS::TRANSACCIONES_PATH,
        Constants::PATHS::TIENDA_PATH,
    };

    bool ok = true;
    for (const fs::path& path : paths) {
        ok = ensure_file_with_header(path) && ok;
    }

    if (ok) {
        ok = ensure_tienda_record() && ok;
    }

    return ok;
}

}  // namespace

void bind_database_init(py::module_& m)
{
    m.def("init_database", &init_database,
          "Inicializa la base de datos de archivos binarios");
}
