#pragma once
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>

#include "domain/HeaderFile.hpp"
#include "domain/constants.hpp"

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

    // lee un id y valida que sea un numero y mayor o igual a 0
    // si el usuario ingresa 'q', retorna -1
    int validarId(const char* msg);

    static bool contieneSubstring(const char* cadena, const char* subcadena);

    static void asignarPropiedadNum(const char* msg, AsignarNum auto& prop);

    template <size_t N, typename Setter>
    static void asignarPropiedadString(const char* msg, Setter setter)
    {
        char prop[N];

        std::cout << COLOR_YELLOW << msg << " (q para salir): " << COLOR_RESET;
        if (std::cin.peek() == '\n') {
            std::cin.ignore();
        }

        std::cin.getline(prop, N);

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        setter(prop);
    }

    template <typename T, typename Getter>
    static bool existeDuplicado(const fs::path& path, const char* valorBusqueda,
                                Getter getPropiedad)
    {
        if (valorBusqueda == nullptr) return false;

        std::ifstream archivo(path, std::ios::binary);
        if (!archivo.is_open()) return false;

        HeaderFile header;
        archivo.read(reinterpret_cast<char*>(&header), sizeof(HeaderFile));
        if (!archivo) return false;

        T entidad;
        while (archivo.read(reinterpret_cast<char*>(&entidad), sizeof(T))) {
            if constexpr (requires(const T& e) { e.getEliminado(); }) {
                if (entidad.getEliminado()) continue;
            }

            const char* valor = getPropiedad(entidad);
            if (valor != nullptr && std::strcmp(valor, valorBusqueda) == 0) {
                return true;
            }
        }
        return false;
    }
};
