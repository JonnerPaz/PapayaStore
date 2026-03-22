#pragma once

class IDatabaseAdmin {
  public:
    virtual void crearBackup() = 0;
    virtual void verificarIntegridadReferencial() = 0;
    virtual void reporteStockCritico() = 0;
    virtual void reporteHistorialCliente() = 0;
    virtual void sincronizarContadoresTienda() = 0;
    virtual ~IDatabaseAdmin() = default;
};
