#include "FSDB.datasoure.impl.hpp"
#include "domain/constants.hpp"
#include "presentation/HeaderFile.hpp"
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <variant>

namespace fs = std::filesystem;
using namespace Constants::ASCII_CODES;
using namespace Constants::PATHS;

FSDB::FSDB() {
}

bool FSDB::initDB() {
    fs::path paths[5] = {PRODUCTOS_PATH, PROVEEDORES_PATH, CLIENTES_PATH, TRANSACCIONES_PATH,
                         TIENDA_PATH};

    try {
        for (fs::path path : paths) {
            if (path.has_parent_path() && !fs::exists(path.parent_path())) {
                fs::create_directories(path.parent_path());
            }

            std::ofstream file(path, std::ios::binary | std::ios::app);
            if (!file) {
                throw std::runtime_error(
                    std::format("No se pudo crear el archivo {}", path.string()));
                return false;
            }

            HeaderFile header = {
                .cantidadRegistros = 0, .proximoID = 1, .registrosActivos = 0, .version = 1};
            file.write(reinterpret_cast<const char*>(&header), sizeof(HeaderFile));
            file.close();
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << std::format("Error al inicializar el archivo: {}", e.what()) << std::endl;
        return false;
    }
}

std::variant<HeaderFile, std::string> FSDB::leerHeader(fs::path path) {
    try {
        std::ifstream file(path, std::ios::binary | std::ios::in);
        if (!file.is_open()) {
            throw std::runtime_error(std::format("Error al abrir el archivo {}", path.string()));
        }

        HeaderFile header;

        // lee el header
        file.seekg(0);
        file.read(reinterpret_cast<char*>(&header), sizeof(HeaderFile));

        return header;
    } catch (const std::exception& error) {
        std::cerr << std::format("Error al leer el archivo {}: {}", path.string(), error.what())
                  << std::endl;
        return HeaderFile();
    }
}

std::variant<HeaderFile, std::string> FSDB::actualizarHeader(fs::path path) {
    try {
        std::ofstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open()) {
            throw std::runtime_error(std::format("Error al abrir el archivo {}", path.string()));
        }
        HeaderFile header;
        file.seekp(0);
        // Reescribe todo el header
        file.write(reinterpret_cast<const char*>(&header), sizeof(HeaderFile));
        file.close();
        return header;
    } catch (const std::exception& e) {
        std::cerr << std::format("Error al actualizar el archivo: {}", e.what()) << std::endl;
        return e.what();
    }
}

void FSDB::crearBackup() {
    fs::create_directories(BACKUP_PATH);

    const std::string paths[5] = {PRODUCTOS_PATH, PROVEEDORES_PATH, CLIENTES_PATH,
                                  TRANSACCIONES_PATH, TIENDA_PATH};

    auto now = std::chrono::system_clock::now();
    auto current_date = std::chrono::time_point_cast<std::chrono::days>(now);
    std::chrono::year_month_day ymd{current_date};
    auto nowSec = std::chrono::time_point_cast<std::chrono::seconds>(now);
    std::chrono::zoned_time localTime{std::chrono::current_zone(), nowSec};

    for (fs::path path : paths) {
        std::string baseName = path.stem().string();
        fs::path backupName =
            fs::path(format("{}.{:%Y-%m-%d}.{:%T}.bin", baseName, ymd, localTime));
        fs::path ymd_suffix = BACKUP_PATH / backupName;

        fs::copy(path, ymd_suffix);
    }
}

FSDB::~FSDB() {
}
