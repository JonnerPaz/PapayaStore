#pragma once
#include "domain/repositories/IClienteRepository.hpp"
#include "domain/repositories/IDatabaseAdmin.hpp"
#include "domain/repositories/IProductoRepository.hpp"
#include "domain/repositories/IProveedorRepository.hpp"
#include "domain/repositories/ITransaccionRepository.hpp"

class FSDatabaseAdmin : public IDatabaseAdmin
{
   private:
    IProductoRepository& productos;
    IClienteRepository& clientes;
    IProveedorRepository& proveedores;
    ITransaccionRepository& transacciones;

   public:
    FSDatabaseAdmin(IProductoRepository& productos, IClienteRepository& clientes,
                    IProveedorRepository& proveedores, ITransaccionRepository& transacciones);

    void crearBackup() override;
    std::tuple<int, int, int, int> verificarIntegridadReferencial() override;
    int reporteStockCritico() override;
    void reporteHistorialCliente() override;
    bool sincronizarContadoresTienda() override;
};
