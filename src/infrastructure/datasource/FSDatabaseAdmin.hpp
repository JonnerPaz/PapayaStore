#pragma once
#include "domain/repositories/IDatabaseAdmin.hpp"

class FSDatabaseAdmin : public IDatabaseAdmin {
  public:
    FSDatabaseAdmin();
    void crearBackup() override;
    void verificarIntegridadReferencial() override;
    void reporteStockCritico() override;
    void reporteHistorialCliente() override;
    void sincronizarContadoresTienda() override;
    ~FSDatabaseAdmin() override = default;
};
