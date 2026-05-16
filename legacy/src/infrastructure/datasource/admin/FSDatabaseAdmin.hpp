#pragma once
#include <string>

#include "domain/HeaderFile.hpp"
#include "domain/entities/tienda/tienda.entity.hpp"
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

    /// Lee el encabezado de tienda.bin para obtener contadores y version.
    std::variant<HeaderFile, std::string> leerHeaderTienda();

    /// Lee el registro principal de tienda.bin (despues del header).
    std::variant<Tienda, std::string> leerRegistroTienda();

    /// Persiste header y registro de tienda de forma consistente en tienda.bin.
    std::variant<bool, std::string> guardarRegistroTienda(const Tienda& tienda,
                                                          const HeaderFile& header);

   public:
    FSDatabaseAdmin(IProductoRepository& productos, IClienteRepository& clientes,
                    IProveedorRepository& proveedores, ITransaccionRepository& transacciones);

    void crearBackup() override;
    std::tuple<int, int, int, int> verificarIntegridadReferencial() override;
    int reporteStockCritico() override;
    void reporteHistorialCliente(int idCliente) override;
    bool sincronizarContadoresTienda() override;
};
