#pragma once
#include "domain/entities/cliente/Cliente.entity.hpp"
#include "domain/entities/producto/producto.entity.hpp"
#include "domain/entities/proveedor/Proveedor.entity.hpp"
#include "domain/entities/transaccion/transaccion.entity.hpp"

template <typename T> struct EntityTraits;

template <> struct EntityTraits<Producto> {
    static int getId(const Producto& p) {
        return p.getId();
    }

    static bool isDeleted(const Producto& p) {
        return p.isEliminado();
    }

    static void setDeleted(Producto& p, bool val) {
        p.setEliminado(val);
    }
};

template <> struct EntityTraits<Cliente> {
    static int getId(const Cliente& c) {
        return c.id;
    }

    static bool isDeleted(const Cliente& c) {
        return c.eliminado;
    }

    static void setDeleted(Cliente& c, bool val) {
        c.eliminado = val;
    }
};

template <> struct EntityTraits<Proveedor> {
    static int getId(const Proveedor& p) {
        return p.id;
    }

    static bool isDeleted(const Proveedor& p) {
        return p.eliminado;
    }

    static void setDeleted(Proveedor& p, bool val) {
        p.eliminado = val;
    }
};

template <> struct EntityTraits<Transaccion> {
    static int getId(const Transaccion& t) {
        return t.id;
    }

    static bool isDeleted(const Transaccion& t) {
        return t.eliminado;
    }

    static void setDeleted(Transaccion& t, bool val) {
        t.eliminado = val;
    }
};
