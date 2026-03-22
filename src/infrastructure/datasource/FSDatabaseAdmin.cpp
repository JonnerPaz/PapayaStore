#include "FSDatabaseAdmin.hpp"
#include "domain/constants.hpp"
#include <chrono>
#include <filesystem>
#include <format>
#include <string>

namespace fs = std::filesystem;
using namespace Constants::ASCII_CODES;
using namespace Constants::PATHS;

FSDatabaseAdmin::FSDatabaseAdmin() {
}

void FSDatabaseAdmin::crearBackup() {
    fs::create_directories(BACKUP_PATH);

    const std::string paths[5] = {PRODUCTOS_PATH, PROVEEDORES_PATH, CLIENTES_PATH,
                                  TRANSACCIONES_PATH, TIENDA_PATH};

    auto now = std::chrono::system_clock::now();
    auto current_date = std::chrono::time_point_cast<std::chrono::days>(now);
    std::chrono::year_month_day ymd{current_date};
    auto nowSec = std::chrono::time_point_cast<std::chrono::seconds>(now);
    std::chrono::zoned_time localTime{std::chrono::current_zone(), nowSec};

    for (fs::path path : paths) {
        if (!fs::exists(path))
            continue;
        std::string baseName = path.stem().string();
        fs::path backupName =
            fs::path(std::format("{}.{:%Y-%m-%d}.{:%T}.bin", baseName, ymd, localTime));
        fs::path ymd_suffix = BACKUP_PATH / backupName;

        fs::copy(path, ymd_suffix);
    }
}

void FSDatabaseAdmin::verificarIntegridadReferencial() {
}

void FSDatabaseAdmin::reporteStockCritico() {
}

void FSDatabaseAdmin::reporteHistorialCliente() {
}

void FSDatabaseAdmin::sincronizarContadoresTienda() {
}
