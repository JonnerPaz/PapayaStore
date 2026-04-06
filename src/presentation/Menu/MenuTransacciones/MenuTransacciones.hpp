#pragma once
#include <string>
#include <vector>

#include "presentation/CliUtils.hpp"
#include "presentation/Menu/Menu.hpp"

class MenuTransacciones : public Menu
{
   private:
    bool readValidCantidad(const char* prompt, int& outCantidad);
    bool getItemsFromTransaccion(const Transaccion& transaccion,
                                 std::vector<TransaccionDTO>& outItems, std::string& outError);
    bool aplicarCambiosStock(const std::vector<TransaccionDTO>& items, bool incrementarStock,
                             bool ajustarTotalVendido, std::vector<Producto>& productosOriginales,
                             std::string& outError);
    bool rollbackStock(const std::vector<Producto>& productosOriginales);
    float calcularTotalTransaccion(const std::vector<TransaccionDTO>& items, std::string& outError);
    void imprimirDetalleTransaccion(const Transaccion& transaccion);

   public:
    CliUtils utils;
    explicit MenuTransacciones(AppRepositories& repositories, CliUtils utils = CliUtils());

    void registrarCompra();
    void registrarVenta();
    void buscarTransacciones();
    void listarTransacciones();
    void cancelarTransaccion();

    void showMenu() override;
    ~MenuTransacciones() override = default;
};
