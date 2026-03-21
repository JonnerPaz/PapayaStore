#pragma once
#include <filesystem>

namespace fs = std::filesystem;
namespace Constants {

// ANSI Escape Codes
const char* CLEAR_SCREEN = "\033[2J\033[H";
const char* COLOR_RESET = "\033[0m";
const char* COLOR_CYAN = "\033[36m";
const char* COLOR_GREEN = "\033[32m";
const char* COLOR_YELLOW = "\033[33m";
const char* COLOR_RED = "\033[31m";
const char* COLOR_MAGENTA = "\033[35m";

// File paths
const fs::path CLIENTES_PATH = "./data/clientes.bin";
const fs::path PROVEEDORES_PATH = "./data/proveedores.bin";
const fs::path PRODUCTOS_PATH = "./data/productos.bin";
const fs::path TRANSACCIONES_PATH = "./data/transacciones.bin";
const fs::path TIENDA_PATH = "./data/tienda.bin";
} // namespace Constants
