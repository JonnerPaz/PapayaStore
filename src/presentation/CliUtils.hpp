#pragma once
#include <cstring>
#include <filesystem>
#include <limits.h>

namespace fs = std::filesystem;

enum ListarPorPropiedad { PorId, PorNombre, PorAmbos };

template <typename T>
// remove_reference_t remueve las referencias de T y devuelve T
// Ej. si T es int& -> devuelve int
concept AsignarNum = std::is_arithmetic_v<std::remove_reference_t<T>>;

class CliUtils {
  public:
    template <typename T> static void mostrarDetallesEntidad(const T& entidad);
    template <typename T>
    static void mostrarListaEntidades(const char* titulo, fs::path path,
                                      ListarPorPropiedad tipoProp = PorAmbos);
    static void showSearchCriteriaMenu();
    static int esIdValido(int id);
    static void convertirAMinusculas(char* cadena);

    // lee un id y valida que sea un numero y mayor o igual a 0
    // si el usuario ingresa 'q', retorna -1
    int validarId(const char* msg);

    static bool contieneSubstring(const char* cadena, const char* subcadena);

    static void asignarPropiedadNum(const char* msg, AsignarNum auto& prop);

    template <size_t N> static void asignarPropiedadString(const char* msg, char (&prop)[N]);

    template <typename T, size_t N>
    static bool existeDuplicado(const fs::path& path, const char* valorBusqueda);
};
