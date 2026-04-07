#pragma once
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"
#include "infrastructure/datasource/EntityTraits.hpp"

namespace fs = std::filesystem;
using namespace Constants::ASCII_CODES;

enum ListarPorPropiedad { PorId, PorNombre, PorAmbos };

template <typename T>
// remove_reference_t remueve las referencias de T y devuelve T
// Ej. si T es int& -> devuelve int
concept AsignarNum = std::is_arithmetic_v<std::remove_reference_t<T>>;

class CliUtils
{
   public:
    template <typename T>
    static void mostrarDetallesEntidad(const T& entidad);
    template <typename T>
    static void mostrarListaEntidades(const char* titulo, fs::path path,
                                      ListarPorPropiedad tipoProp = PorAmbos);
    static void showSearchCriteriaMenu();
    static int esIdValido(int id);
    static void convertirAMinusculas(char* cadena);

    /*
     * Parse input and assign to outValue
     */
    static bool parsePositiveInt(const std::string& input, int& outValue,
                                 bool zeroInclusive = true);
    static bool parsePositiveFloat(const std::string& input, float& outValue,
                                   bool zeroInclusive = true);

    // lee un id y valida que sea un numero y mayor o igual a 0
    // si el usuario ingresa 'q', retorna -1
    int validarId(const char* msg);

    static bool contieneSubstring(const char* cadena, const char* subcadena);
};
