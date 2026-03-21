#pragma once

class DB {

  public:
    virtual void crearBackup() = 0;
    virtual void verificarIntegridadReferencial() = 0;
    virtual void leerEntidad() = 0;
    virtual void escribirEntidad() = 0;
    virtual void guardarEntidad() = 0;

    virtual void reporteStockCritico() = 0;
    virtual void reporteHistorialCliente() = 0;
    virtual ~DB() = default;
};
