#pragma once

#include <chrono>
#include <cstdint>
#include <cstring>
#include <istream>
#include <ostream>

#include "domain/entities/cliente/Cliente.entity.hpp"
#include "domain/entities/producto/producto.entity.hpp"
#include "domain/entities/proveedor/Proveedor.entity.hpp"
#include "domain/entities/tienda/tienda.entity.hpp"
#include "domain/entities/transaccion/transaccion.entity.hpp"

template <typename T>
struct EntityTraits;

/// Adaptador binario para Producto.
/// Define metadatos para CRUD generico y serializacion deterministica.
template <>
struct EntityTraits<Producto> {
    /// Retorna el ID persistente de la entidad.
    static int getId(const Producto& p) { return p.getId(); }

    /// Indica si el registro debe tratarse como eliminado logicamente.
    static bool isDeleted(const Producto& p) { return p.getEliminado(); }

    /// Marca el estado de borrado logico de la entidad.
    static void setDeleted(Producto& p, bool val) { p.setEliminado(val); }

    /// Tamano fijo de un registro de Producto en disco.
    static std::streamoff recordSize()
    {
        return sizeof(int) + 100 + sizeof(std::int8_t) + sizeof(std::int64_t) +
               sizeof(std::int64_t) + 20 + 200 + sizeof(float) + sizeof(int) + sizeof(int) +
               sizeof(int) + sizeof(int);
    }

    /// Serializa Producto en orden fijo de campos para escritura binaria.
    static bool writeToStream(std::ostream& os, const Producto& p)
    {
        const int id = p.getId();
        const std::int8_t eliminado = p.getEliminado() ? 1 : 0;
        const std::int64_t fechaCreacion = std::chrono::duration_cast<std::chrono::seconds>(
                                               p.getFechaCreacion().time_since_epoch())
                                               .count();
        const std::int64_t fechaModificacion =
            std::chrono::duration_cast<std::chrono::seconds>(
                p.getFechaUltimaModificacion().time_since_epoch())
                .count();

        char nombre[100] = {0};
        std::strncpy(nombre, p.getNombre(), sizeof(nombre) - 1);
        char codigo[20] = {0};
        std::strncpy(codigo, p.getCodigo(), sizeof(codigo) - 1);
        char descripcion[200] = {0};
        std::strncpy(descripcion, p.getDescripcion(), sizeof(descripcion) - 1);
        const float precio = p.getPrecio();
        const int stock = p.getStock();
        const int idProveedor = p.getIdProveedor();
        const int stockMinimo = p.getStockMinimo();
        const int totalVendido = p.getTotalVendido();

        os.write(reinterpret_cast<const char*>(&id), sizeof(id));
        os.write(nombre, sizeof(nombre));
        os.write(reinterpret_cast<const char*>(&eliminado), sizeof(eliminado));
        os.write(reinterpret_cast<const char*>(&fechaCreacion), sizeof(fechaCreacion));
        os.write(reinterpret_cast<const char*>(&fechaModificacion), sizeof(fechaModificacion));
        os.write(codigo, sizeof(codigo));
        os.write(descripcion, sizeof(descripcion));
        os.write(reinterpret_cast<const char*>(&precio), sizeof(precio));
        os.write(reinterpret_cast<const char*>(&stock), sizeof(stock));
        os.write(reinterpret_cast<const char*>(&idProveedor), sizeof(idProveedor));
        os.write(reinterpret_cast<const char*>(&stockMinimo), sizeof(stockMinimo));
        os.write(reinterpret_cast<const char*>(&totalVendido), sizeof(totalVendido));

        return static_cast<bool>(os);
    }

    /// Deserializa un Producto desde stream binario y lo reconstruye via setters.
    static bool readFromStream(std::istream& is, Producto& p)
    {
        int id = 0;
        char nombre[100] = {0};
        std::int8_t eliminado = 0;
        std::int64_t fechaCreacion = 0;
        std::int64_t fechaModificacion = 0;
        char codigo[20] = {0};
        char descripcion[200] = {0};
        float precio = 0.0f;
        int stock = 0;
        int idProveedor = 0;
        int stockMinimo = 0;
        int totalVendido = 0;

        is.read(reinterpret_cast<char*>(&id), sizeof(id));
        is.read(nombre, sizeof(nombre));
        is.read(reinterpret_cast<char*>(&eliminado), sizeof(eliminado));
        is.read(reinterpret_cast<char*>(&fechaCreacion), sizeof(fechaCreacion));
        is.read(reinterpret_cast<char*>(&fechaModificacion), sizeof(fechaModificacion));
        is.read(codigo, sizeof(codigo));
        is.read(descripcion, sizeof(descripcion));
        is.read(reinterpret_cast<char*>(&precio), sizeof(precio));
        is.read(reinterpret_cast<char*>(&stock), sizeof(stock));
        is.read(reinterpret_cast<char*>(&idProveedor), sizeof(idProveedor));
        is.read(reinterpret_cast<char*>(&stockMinimo), sizeof(stockMinimo));
        is.read(reinterpret_cast<char*>(&totalVendido), sizeof(totalVendido));

        if (!is) {
            return false;
        }

        p.setId(id);
        p.setNombre(nombre);
        p.setEliminado(eliminado != 0);
        p.setFechaCreacion(
            std::chrono::system_clock::time_point(std::chrono::seconds(fechaCreacion)));
        p.setFechaUltimaModificacion(
            std::chrono::system_clock::time_point(std::chrono::seconds(fechaModificacion)));
        p.setCodigo(codigo);
        p.setDescripcion(descripcion);
        p.setPrecio(precio);
        p.setStock(stock);
        p.setIdProveedor(idProveedor);
        p.setStockMinimo(stockMinimo);
        p.setTotalVendido(totalVendido);

        return true;
    }
};

/// Adaptador binario para Cliente.
/// Encapsula conversion entre entidad de dominio y layout binario estable.
template <>
struct EntityTraits<Cliente> {
    /// Retorna el ID persistente de la entidad.
    static int getId(const Cliente& c) { return c.getId(); }

    /// Indica si el registro debe tratarse como eliminado logicamente.
    static bool isDeleted(const Cliente& c) { return c.getEliminado(); }

    /// Marca el estado de borrado logico de la entidad.
    static void setDeleted(Cliente& c, bool val) { c.setEliminado(val); }

    /// Tamano fijo de un registro de Cliente en disco.
    static std::streamoff recordSize()
    {
        return sizeof(int) + 100 + sizeof(std::int8_t) + sizeof(std::int64_t) +
               sizeof(std::int64_t) + 20 + 20 + 100 + 200 + sizeof(int) + (sizeof(int) * 100) +
               sizeof(float) + (sizeof(int) * 100) + sizeof(int);
    }

    /// Serializa Cliente en orden fijo de campos para escritura binaria.
    static bool writeToStream(std::ostream& os, const Cliente& c)
    {
        const int id = c.getId();
        const std::int8_t eliminado = c.getEliminado() ? 1 : 0;
        const std::int64_t fechaCreacion = std::chrono::duration_cast<std::chrono::seconds>(
                                               c.getFechaCreacion().time_since_epoch())
                                               .count();
        const std::int64_t fechaModificacion =
            std::chrono::duration_cast<std::chrono::seconds>(
                c.getFechaUltimaModificacion().time_since_epoch())
                .count();

        char nombre[100] = {0};
        std::strncpy(nombre, c.getNombre(), sizeof(nombre) - 1);
        char cedula[20] = {0};
        std::strncpy(cedula, c.getCedula(), sizeof(cedula) - 1);
        char telefono[20] = {0};
        std::strncpy(telefono, c.getTelefono(), sizeof(telefono) - 1);
        char email[100] = {0};
        std::strncpy(email, c.getEmail(), sizeof(email) - 1);
        char direccion[200] = {0};
        std::strncpy(direccion, c.getDireccion(), sizeof(direccion) - 1);
        const int cantidad = c.getCantidad();
        const float totalCompras = c.getTotalCompras();
        const int cantidadTransacciones = c.getCantidadTransacciones();

        os.write(reinterpret_cast<const char*>(&id), sizeof(id));
        os.write(nombre, sizeof(nombre));
        os.write(reinterpret_cast<const char*>(&eliminado), sizeof(eliminado));
        os.write(reinterpret_cast<const char*>(&fechaCreacion), sizeof(fechaCreacion));
        os.write(reinterpret_cast<const char*>(&fechaModificacion), sizeof(fechaModificacion));
        os.write(cedula, sizeof(cedula));
        os.write(telefono, sizeof(telefono));
        os.write(email, sizeof(email));
        os.write(direccion, sizeof(direccion));
        os.write(reinterpret_cast<const char*>(&cantidad), sizeof(cantidad));
        os.write(reinterpret_cast<const char*>(c.getHistorialIds()), sizeof(int) * 100);
        os.write(reinterpret_cast<const char*>(&totalCompras), sizeof(totalCompras));
        os.write(reinterpret_cast<const char*>(c.getTransaccionesIds()), sizeof(int) * 100);
        os.write(reinterpret_cast<const char*>(&cantidadTransacciones),
                 sizeof(cantidadTransacciones));

        return static_cast<bool>(os);
    }

    /// Deserializa un Cliente desde stream binario y lo reconstruye via setters.
    static bool readFromStream(std::istream& is, Cliente& c)
    {
        int id = 0;
        char nombre[100] = {0};
        std::int8_t eliminado = 0;
        std::int64_t fechaCreacion = 0;
        std::int64_t fechaModificacion = 0;
        char cedula[20] = {0};
        char telefono[20] = {0};
        char email[100] = {0};
        char direccion[200] = {0};
        int cantidad = 0;
        int historialIds[100] = {0};
        float totalCompras = 0.0f;
        int transaccionesIds[100] = {0};
        int cantidadTransacciones = 0;

        is.read(reinterpret_cast<char*>(&id), sizeof(id));
        is.read(nombre, sizeof(nombre));
        is.read(reinterpret_cast<char*>(&eliminado), sizeof(eliminado));
        is.read(reinterpret_cast<char*>(&fechaCreacion), sizeof(fechaCreacion));
        is.read(reinterpret_cast<char*>(&fechaModificacion), sizeof(fechaModificacion));
        is.read(cedula, sizeof(cedula));
        is.read(telefono, sizeof(telefono));
        is.read(email, sizeof(email));
        is.read(direccion, sizeof(direccion));
        is.read(reinterpret_cast<char*>(&cantidad), sizeof(cantidad));
        is.read(reinterpret_cast<char*>(historialIds), sizeof(historialIds));
        is.read(reinterpret_cast<char*>(&totalCompras), sizeof(totalCompras));
        is.read(reinterpret_cast<char*>(transaccionesIds), sizeof(transaccionesIds));
        is.read(reinterpret_cast<char*>(&cantidadTransacciones), sizeof(cantidadTransacciones));

        if (!is) {
            return false;
        }

        c.setId(id);
        c.setNombre(nombre);
        c.setEliminado(eliminado != 0);
        c.setFechaCreacion(
            std::chrono::system_clock::time_point(std::chrono::seconds(fechaCreacion)));
        c.setFechaUltimaModificacion(
            std::chrono::system_clock::time_point(std::chrono::seconds(fechaModificacion)));
        c.setCedula(cedula);
        c.setTelefono(telefono);
        c.setEmail(email);
        c.setDireccion(direccion);
        c.setCantidad(cantidad);
        for (int i = 0; i < 100; ++i) {
            c.setHistorialIdEnIndice(i, historialIds[i]);
        }
        c.setTotalCompras(totalCompras);
        for (int i = 0; i < 100; ++i) {
            c.setTransaccionIdEnIndice(i, transaccionesIds[i]);
        }
        c.setCantidadTransacciones(cantidadTransacciones);

        return true;
    }
};

/// Adaptador binario para Proveedor.
/// Encapsula conversion entre entidad de dominio y layout binario estable.
template <>
struct EntityTraits<Proveedor> {
    /// Retorna el ID persistente de la entidad.
    static int getId(const Proveedor& p) { return p.getId(); }

    /// Indica si el registro debe tratarse como eliminado logicamente.
    static bool isDeleted(const Proveedor& p) { return p.getEliminado(); }

    /// Marca el estado de borrado logico de la entidad.
    static void setDeleted(Proveedor& p, bool val) { p.setEliminado(val); }

    /// Tamano fijo de un registro de Proveedor en disco.
    static std::streamoff recordSize()
    {
        return sizeof(int) + 100 + sizeof(std::int8_t) + sizeof(std::int64_t) +
               sizeof(std::int64_t) + 20 + (sizeof(int) * 100) + sizeof(int) + 20 + 100 + 200 +
               sizeof(int) + (sizeof(int) * 100);
    }

    /// Serializa Proveedor en orden fijo de campos para escritura binaria.
    static bool writeToStream(std::ostream& os, const Proveedor& p)
    {
        const int id = p.getId();
        const std::int8_t eliminado = p.getEliminado() ? 1 : 0;
        const std::int64_t fechaCreacion = std::chrono::duration_cast<std::chrono::seconds>(
                                               p.getFechaCreacion().time_since_epoch())
                                               .count();
        const std::int64_t fechaModificacion =
            std::chrono::duration_cast<std::chrono::seconds>(
                p.getFechaUltimaModificacion().time_since_epoch())
                .count();

        char nombre[100] = {0};
        std::strncpy(nombre, p.getNombre(), sizeof(nombre) - 1);
        char rif[20] = {0};
        std::strncpy(rif, p.getRif(), sizeof(rif) - 1);
        char telefono[20] = {0};
        std::strncpy(telefono, p.getTelefono(), sizeof(telefono) - 1);
        char email[100] = {0};
        std::strncpy(email, p.getEmail(), sizeof(email) - 1);
        char direccion[200] = {0};
        std::strncpy(direccion, p.getDireccion(), sizeof(direccion) - 1);
        const int cantidadProductos = p.getCantidadProductos();
        const int cantidad = p.getCantidad();

        os.write(reinterpret_cast<const char*>(&id), sizeof(id));
        os.write(nombre, sizeof(nombre));
        os.write(reinterpret_cast<const char*>(&eliminado), sizeof(eliminado));
        os.write(reinterpret_cast<const char*>(&fechaCreacion), sizeof(fechaCreacion));
        os.write(reinterpret_cast<const char*>(&fechaModificacion), sizeof(fechaModificacion));
        os.write(rif, sizeof(rif));
        os.write(reinterpret_cast<const char*>(p.getProductosIds()), sizeof(int) * 100);
        os.write(reinterpret_cast<const char*>(&cantidadProductos), sizeof(cantidadProductos));
        os.write(telefono, sizeof(telefono));
        os.write(email, sizeof(email));
        os.write(direccion, sizeof(direccion));
        os.write(reinterpret_cast<const char*>(&cantidad), sizeof(cantidad));
        os.write(reinterpret_cast<const char*>(p.getHistorialIds()), sizeof(int) * 100);

        return static_cast<bool>(os);
    }

    /// Deserializa un Proveedor desde stream binario y lo reconstruye via setters.
    static bool readFromStream(std::istream& is, Proveedor& p)
    {
        int id = 0;
        char nombre[100] = {0};
        std::int8_t eliminado = 0;
        std::int64_t fechaCreacion = 0;
        std::int64_t fechaModificacion = 0;
        char rif[20] = {0};
        int productosIds[100] = {0};
        int cantidadProductos = 0;
        char telefono[20] = {0};
        char email[100] = {0};
        char direccion[200] = {0};
        int cantidad = 0;
        int historialIds[100] = {0};

        is.read(reinterpret_cast<char*>(&id), sizeof(id));
        is.read(nombre, sizeof(nombre));
        is.read(reinterpret_cast<char*>(&eliminado), sizeof(eliminado));
        is.read(reinterpret_cast<char*>(&fechaCreacion), sizeof(fechaCreacion));
        is.read(reinterpret_cast<char*>(&fechaModificacion), sizeof(fechaModificacion));
        is.read(rif, sizeof(rif));
        is.read(reinterpret_cast<char*>(productosIds), sizeof(productosIds));
        is.read(reinterpret_cast<char*>(&cantidadProductos), sizeof(cantidadProductos));
        is.read(telefono, sizeof(telefono));
        is.read(email, sizeof(email));
        is.read(direccion, sizeof(direccion));
        is.read(reinterpret_cast<char*>(&cantidad), sizeof(cantidad));
        is.read(reinterpret_cast<char*>(historialIds), sizeof(historialIds));

        if (!is) {
            return false;
        }

        p.setId(id);
        p.setNombre(nombre);
        p.setEliminado(eliminado != 0);
        p.setFechaCreacion(
            std::chrono::system_clock::time_point(std::chrono::seconds(fechaCreacion)));
        p.setFechaUltimaModificacion(
            std::chrono::system_clock::time_point(std::chrono::seconds(fechaModificacion)));
        p.setRif(rif);
        for (int i = 0; i < 100; ++i) {
            p.setProductoIdEnIndice(i, productosIds[i]);
        }
        p.setCantidadProductos(cantidadProductos);
        p.setTelefono(telefono);
        p.setEmail(email);
        p.setDireccion(direccion);
        p.setCantidad(cantidad);
        for (int i = 0; i < 100; ++i) {
            p.setHistorialIdEnIndice(i, historialIds[i]);
        }

        return true;
    }
};

/// Adaptador binario para Transaccion.
/// Encapsula conversion entre entidad de dominio y layout binario estable.
template <>
struct EntityTraits<Transaccion> {
    /// Retorna el ID persistente de la entidad.
    static int getId(const Transaccion& t) { return t.getId(); }

    /// Indica si el registro debe tratarse como eliminado logicamente.
    static bool isDeleted(const Transaccion& t) { return t.getEliminado(); }

    /// Marca el estado de borrado logico de la entidad.
    static void setDeleted(Transaccion& t, bool val) { t.setEliminado(val); }

    /// Tamano fijo de un registro de Transaccion en disco.
    static std::streamoff recordSize()
    {
        return sizeof(int) + 100 + sizeof(std::int8_t) + sizeof(std::int64_t) +
               sizeof(std::int64_t) + sizeof(int) + sizeof(int) + sizeof(float) + 200 +
               (sizeof(TransaccionDTO) * 100) + sizeof(int);
    }

    /// Serializa Transaccion en orden fijo de campos para escritura binaria.
    static bool writeToStream(std::ostream& os, const Transaccion& t)
    {
        const int id = t.getId();
        const std::int8_t eliminado = t.getEliminado() ? 1 : 0;
        const std::int64_t fechaCreacion = std::chrono::duration_cast<std::chrono::seconds>(
                                               t.getFechaCreacion().time_since_epoch())
                                               .count();
        const std::int64_t fechaModificacion =
            std::chrono::duration_cast<std::chrono::seconds>(
                t.getFechaUltimaModificacion().time_since_epoch())
                .count();
        int tipo = static_cast<int>(t.getTipoTransaccion());
        const int idRelacionado = t.getIdRelacionado();
        const float total = t.getTotal();

        char nombre[100] = {0};
        std::strncpy(nombre, t.getNombre(), sizeof(nombre) - 1);
        char descripcion[200] = {0};
        std::strncpy(descripcion, t.getDescripcion(), sizeof(descripcion) - 1);

        TransaccionDTO productos[100] = {};
        const int productosTotales = t.getProductosTotales();
        for (int i = 0; i < productosTotales && i < 100; ++i) {
            t.getProductoEnIndice(i, productos[i]);
        }

        os.write(reinterpret_cast<const char*>(&id), sizeof(id));
        os.write(nombre, sizeof(nombre));
        os.write(reinterpret_cast<const char*>(&eliminado), sizeof(eliminado));
        os.write(reinterpret_cast<const char*>(&fechaCreacion), sizeof(fechaCreacion));
        os.write(reinterpret_cast<const char*>(&fechaModificacion), sizeof(fechaModificacion));
        os.write(reinterpret_cast<const char*>(&tipo), sizeof(tipo));
        os.write(reinterpret_cast<const char*>(&idRelacionado), sizeof(idRelacionado));
        os.write(reinterpret_cast<const char*>(&total), sizeof(total));
        os.write(descripcion, sizeof(descripcion));
        os.write(reinterpret_cast<const char*>(productos), sizeof(productos));
        os.write(reinterpret_cast<const char*>(&productosTotales), sizeof(productosTotales));

        return static_cast<bool>(os);
    }

    /// Deserializa una Transaccion desde stream binario y la reconstruye via setters.
    static bool readFromStream(std::istream& is, Transaccion& t)
    {
        int id = 0;
        char nombre[100] = {0};
        std::int8_t eliminado = 0;
        std::int64_t fechaCreacion = 0;
        std::int64_t fechaModificacion = 0;
        int tipo = 0;
        int idRelacionado = 0;
        float total = 0.0f;
        char descripcion[200] = {0};
        TransaccionDTO productos[100] = {};
        int productosTotales = 0;

        is.read(reinterpret_cast<char*>(&id), sizeof(id));
        is.read(nombre, sizeof(nombre));
        is.read(reinterpret_cast<char*>(&eliminado), sizeof(eliminado));
        is.read(reinterpret_cast<char*>(&fechaCreacion), sizeof(fechaCreacion));
        is.read(reinterpret_cast<char*>(&fechaModificacion), sizeof(fechaModificacion));
        is.read(reinterpret_cast<char*>(&tipo), sizeof(tipo));
        is.read(reinterpret_cast<char*>(&idRelacionado), sizeof(idRelacionado));
        is.read(reinterpret_cast<char*>(&total), sizeof(total));
        is.read(descripcion, sizeof(descripcion));
        is.read(reinterpret_cast<char*>(productos), sizeof(productos));
        is.read(reinterpret_cast<char*>(&productosTotales), sizeof(productosTotales));

        if (!is) {
            return false;
        }

        Transaccion loaded;
        loaded.setId(id);
        loaded.setNombre(nombre);
        loaded.setEliminado(eliminado != 0);
        loaded.setFechaCreacion(
            std::chrono::system_clock::time_point(std::chrono::seconds(fechaCreacion)));
        loaded.setFechaUltimaModificacion(
            std::chrono::system_clock::time_point(std::chrono::seconds(fechaModificacion)));
        loaded.setTipoTransaccion(static_cast<TipoDeTransaccion>(tipo));
        loaded.setIdRelacionado(idRelacionado);
        loaded.setTotal(total);
        loaded.setDescripcion(descripcion);

        if (productosTotales < 0) {
            productosTotales = 0;
        }
        if (productosTotales > 100) {
            productosTotales = 100;
        }

        for (int i = 0; i < productosTotales; ++i) {
            loaded.setProducto(productos[i]);
        }

        t = loaded;
        return true;
    }
};

/// Adaptador binario para Tienda.
/// Se usa para persistir el registro unico de resumen global.
template <>
struct EntityTraits<Tienda> {
    /// Tamano fijo de un registro de Tienda en disco.
    static std::streamoff recordSize()
    {
        return sizeof(int) + 100 + sizeof(std::int8_t) + sizeof(std::int64_t) +
               sizeof(std::int64_t) + 20 + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int) +
               sizeof(float) + sizeof(float);
    }

    /// Serializa Tienda en orden fijo de campos para escritura binaria.
    static bool writeToStream(std::ostream& os, const Tienda& t)
    {
        const int id = t.getId();
        const std::int8_t eliminado = t.getEliminado() ? 1 : 0;
        const std::int64_t fechaCreacion = std::chrono::duration_cast<std::chrono::seconds>(
                                               t.getFechaCreacion().time_since_epoch())
                                               .count();
        const std::int64_t fechaModificacion =
            std::chrono::duration_cast<std::chrono::seconds>(
                t.getFechaUltimaModificacion().time_since_epoch())
                .count();

        char nombre[100] = {0};
        std::strncpy(nombre, t.getNombre(), sizeof(nombre) - 1);
        char rif[20] = {0};
        std::strncpy(rif, t.getRif(), sizeof(rif) - 1);
        const int totalProductosActivos = t.getTotalProductosActivos();
        const int totalProveedoresActivos = t.getTotalProveedoresActivos();
        const int totalClientesActivos = t.getTotalClientesActivos();
        const int totalTransaccionesActivas = t.getTotalTransaccionesActivas();
        const float montoTotalVentas = t.getMontoTotalVentas();
        const float montoTotalCompras = t.getMontoTotalCompras();

        os.write(reinterpret_cast<const char*>(&id), sizeof(id));
        os.write(nombre, sizeof(nombre));
        os.write(reinterpret_cast<const char*>(&eliminado), sizeof(eliminado));
        os.write(reinterpret_cast<const char*>(&fechaCreacion), sizeof(fechaCreacion));
        os.write(reinterpret_cast<const char*>(&fechaModificacion), sizeof(fechaModificacion));
        os.write(rif, sizeof(rif));
        os.write(reinterpret_cast<const char*>(&totalProductosActivos),
                 sizeof(totalProductosActivos));
        os.write(reinterpret_cast<const char*>(&totalProveedoresActivos),
                 sizeof(totalProveedoresActivos));
        os.write(reinterpret_cast<const char*>(&totalClientesActivos),
                 sizeof(totalClientesActivos));
        os.write(reinterpret_cast<const char*>(&totalTransaccionesActivas),
                 sizeof(totalTransaccionesActivas));
        os.write(reinterpret_cast<const char*>(&montoTotalVentas), sizeof(montoTotalVentas));
        os.write(reinterpret_cast<const char*>(&montoTotalCompras), sizeof(montoTotalCompras));

        return static_cast<bool>(os);
    }

    /// Deserializa Tienda desde stream binario y reconstruye su estado.
    static bool readFromStream(std::istream& is, Tienda& t)
    {
        int id = 0;
        char nombre[100] = {0};
        std::int8_t eliminado = 0;
        std::int64_t fechaCreacion = 0;
        std::int64_t fechaModificacion = 0;
        char rif[20] = {0};
        int totalProductosActivos = 0;
        int totalProveedoresActivos = 0;
        int totalClientesActivos = 0;
        int totalTransaccionesActivas = 0;
        float montoTotalVentas = 0.0f;
        float montoTotalCompras = 0.0f;

        is.read(reinterpret_cast<char*>(&id), sizeof(id));
        is.read(nombre, sizeof(nombre));
        is.read(reinterpret_cast<char*>(&eliminado), sizeof(eliminado));
        is.read(reinterpret_cast<char*>(&fechaCreacion), sizeof(fechaCreacion));
        is.read(reinterpret_cast<char*>(&fechaModificacion), sizeof(fechaModificacion));
        is.read(rif, sizeof(rif));
        is.read(reinterpret_cast<char*>(&totalProductosActivos), sizeof(totalProductosActivos));
        is.read(reinterpret_cast<char*>(&totalProveedoresActivos), sizeof(totalProveedoresActivos));
        is.read(reinterpret_cast<char*>(&totalClientesActivos), sizeof(totalClientesActivos));
        is.read(reinterpret_cast<char*>(&totalTransaccionesActivas),
                sizeof(totalTransaccionesActivas));
        is.read(reinterpret_cast<char*>(&montoTotalVentas), sizeof(montoTotalVentas));
        is.read(reinterpret_cast<char*>(&montoTotalCompras), sizeof(montoTotalCompras));

        if (!is) {
            return false;
        }

        t.setId(id);
        t.setNombre(nombre);
        t.setEliminado(eliminado != 0);
        t.setFechaCreacion(
            std::chrono::system_clock::time_point(std::chrono::seconds(fechaCreacion)));
        t.setFechaUltimaModificacion(
            std::chrono::system_clock::time_point(std::chrono::seconds(fechaModificacion)));
        t.setRif(rif);
        t.setTotalProductosActivos(totalProductosActivos);
        t.setTotalProveedoresActivos(totalProveedoresActivos);
        t.setTotalClientesActivos(totalClientesActivos);
        t.setTotalTransaccionesActivas(totalTransaccionesActivas);
        t.setMontoTotalVentas(montoTotalVentas);
        t.setMontoTotalCompras(montoTotalCompras);

        return true;
    }
};
