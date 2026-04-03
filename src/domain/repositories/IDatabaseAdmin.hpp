#pragma once

#include <tuple>

/**
 * @brief - Clase para tareas administrativas del sistema
 */
class IDatabaseAdmin
{
   public:
    virtual void crearBackup() = 0;
    virtual std::tuple<int, int, int, int> verificarIntegridadReferencial() = 0;
    virtual int reporteStockCritico() = 0;
    virtual void reporteHistorialCliente() = 0;
    virtual bool sincronizarContadoresTienda() = 0;
    virtual ~IDatabaseAdmin() = default;
};
