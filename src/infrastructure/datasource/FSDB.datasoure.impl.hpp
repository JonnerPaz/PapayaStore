#pragma once
#include "domain/datasource/DB.datasource.hpp"
#include "presentation/HeaderFile.hpp"
#include <filesystem>
#include <variant>

namespace fs = std::filesystem;

class FSDB : public DB {
  public:
    FSDB();
    void crearBackup() override;
    void verificarIntegridadReferencial() override;
    void leerEntidad() override;
    void escribirEntidad() override;
    void guardarEntidad() override;

    bool initDB();
    std::variant<HeaderFile, std::string> leerHeader(fs::path path);
    std::variant<HeaderFile, std::string> actualizarHeader(fs::path path);

    void reporteStockCritico() override;
    void reporteHistorialCliente() override;
    ~FSDB();
};
