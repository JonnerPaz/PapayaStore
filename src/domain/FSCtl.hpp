#pragma once

/**
 * @class - FSCtl
 * @file - FSCtl.hpp
 *  @description - Controlador de base de datos
 */
class FSCtl {
  public:
    virtual void crearBackup() = 0;
    virtual void verificarIntegridadReferencial() = 0;
    virtual void reporteStockCritico() = 0;
    virtual void reporteHistorialCliente() = 0;
    virtual void mostrarResumenTienda() = 0;
};
