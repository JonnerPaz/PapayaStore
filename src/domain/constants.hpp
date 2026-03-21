#pragma once
#include <filesystem>

namespace fs = std::filesystem;

namespace Constants {

namespace ASCII_CODES {
inline constexpr const char* CLEAR_SCREEN = "\033[2J\033[H";
inline constexpr const char* COLOR_RESET = "\033[0m";
inline constexpr const char* COLOR_CYAN = "\033[36m";
inline constexpr const char* COLOR_GREEN = "\033[32m";
inline constexpr const char* COLOR_YELLOW = "\033[33m";
inline constexpr const char* COLOR_RED = "\033[31m";
inline constexpr const char* COLOR_MAGENTA = "\033[35m";
}; // namespace ASCII_CODES

namespace PATHS {
inline const fs::path CLIENTES_PATH = "./data/clientes.bin";
inline const fs::path PROVEEDORES_PATH = "./data/proveedores.bin";
inline const fs::path PRODUCTOS_PATH = "./data/productos.bin";
inline const fs::path TRANSACCIONES_PATH = "./data/transacciones.bin";
inline const fs::path TIENDA_PATH = "./data/tienda.bin";
inline const fs::path BACKUP_PATH = "./backup/";
}; // namespace PATHS

} // namespace Constants
