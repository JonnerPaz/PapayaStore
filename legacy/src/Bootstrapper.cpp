#include "Bootstrapper.hpp"

#include <array>
#include <fstream>
#include <iostream>

#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"
#include "domain/entities/tienda/tienda.entity.hpp"
#include "infrastructure/datasource/EntityTraits.hpp"

using namespace Constants::PATHS;

Bootstrapper::Bootstrapper()
    : admin(productos, clientes, proveedores, transacciones),
      repositories{productos, clientes, proveedores, transacciones, admin},
      mainMenu(repositories)
{
}

Bootstrapper& Bootstrapper::bootstrapContext()
{
    static Bootstrapper context;
    return context;
}

bool Bootstrapper::ensureFileWithHeader(const fs::path& path)
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
    // si la escritura es correcta, la variable file es true
    return static_cast<bool>(file);
}

bool Bootstrapper::bootstrapStorage()
{
    const std::array<fs::path, 5> paths = {
        PRODUCTOS_PATH, PROVEEDORES_PATH, CLIENTES_PATH, TRANSACCIONES_PATH, TIENDA_PATH,
    };

    bool ok = true;
    for (const fs::path& path : paths) {
        ok = this->ensureFileWithHeader(path) && ok;
    }

    if (ok) {
        ok = this->ensureTiendaRecord() && ok;
    }

    return ok;
}

bool Bootstrapper::ensureTiendaRecord()
{
    std::fstream file(TIENDA_PATH, std::ios::binary | std::ios::in | std::ios::out);
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

void Bootstrapper::runMainLoop()
{
    if (!this->bootstrapStorage()) {
        std::cout << "Error inicializando almacenamiento base\n";
        return;
    }

    mainMenu.showMenu();
}
