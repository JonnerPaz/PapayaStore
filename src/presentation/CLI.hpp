#pragma once
#include <filesystem>

namespace fs = std::filesystem;

enum ListarPorPropiedad { PorId, PorNombre, PorAmbos };

class CLI {
  private:
  public:
    template <typename T> static void mostrarDetallesEntidad(const T& entidad);
    template <typename T>
    static void mostrarListaEntidades(const char* titulo, fs::path path,
                                      ListarPorPropiedad tipoProp = PorAmbos);
    static void showSearchCriteriaMenu();
    static int esIdValido(int id);
};
