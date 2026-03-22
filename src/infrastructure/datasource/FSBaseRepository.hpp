#pragma once
#include "infrastructure/datasource/EntityTraits.hpp"
#include "presentation/HeaderFile.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <variant>

namespace fs = std::filesystem;

template <typename T> class FSBaseRepository {
  protected:
    fs::path filePath;

    FSBaseRepository(fs::path path) : filePath(path) {
    }

    std::variant<T, std::string> leerTemplate(int id) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open())
            return "Error abriendo archivo " + filePath.string();

        HeaderFile header;
        file.read(reinterpret_cast<char*>(&header), sizeof(HeaderFile));

        if (id <= 0 || id >= header.proximoID) {
            return "ID fuera de rango o registro no existe";
        }

        std::streampos offset = sizeof(HeaderFile) + ((id - 1) * sizeof(T));
        file.seekg(offset);
        if (!file)
            return "Error moviendo el puntero de lectura";

        T registro;
        file.read(reinterpret_cast<char*>(&registro), sizeof(T));

        if (EntityTraits<T>::isDeleted(registro)) {
            return "El registro ha sido eliminado";
        }

        return registro;
    }

    std::variant<bool, std::string> actualizarTemplate(int id, const T& entidad) {
        std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open())
            return "Error abriendo archivo para escritura";

        HeaderFile header;
        file.read(reinterpret_cast<char*>(&header), sizeof(HeaderFile));

        if (id <= 0 || id >= header.proximoID) {
            return "ID fuera de rango o registro no existe";
        }

        std::streampos offset = sizeof(HeaderFile) + ((id - 1) * sizeof(T));
        file.seekp(offset);
        file.write(reinterpret_cast<const char*>(&entidad), sizeof(T));

        return true;
    }

    std::variant<bool, std::string> guardarTemplate(const T& entidad) {
        std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            // create if not exist?
            // In initDB we create them. But let's be safe.
            return "Error abriendo archivo para guardar";
        }

        HeaderFile header;
        file.read(reinterpret_cast<char*>(&header), sizeof(HeaderFile));

        // Use the next ID
        int nuevoId = header.proximoID;
        // Check if the entity ID matches? The entity passed should already have
        // its ID set correctly to nuevoId before calling guardarTemplate, or we
        // assume it has. Actually, we just write it at proximoID - 1.

        std::streampos offset = sizeof(HeaderFile) + ((nuevoId - 1) * sizeof(T));
        file.seekp(offset);
        file.write(reinterpret_cast<const char*>(&entidad), sizeof(T));

        header.cantidadRegistros++;
        header.registrosActivos++;
        header.proximoID++;

        file.seekp(0);
        file.write(reinterpret_cast<const char*>(&header), sizeof(HeaderFile));

        return true;
    }

    std::variant<bool, std::string> eliminarLogicamenteTemplate(int id) {
        auto result = leerTemplate(id);
        if (std::holds_alternative<std::string>(result)) {
            return std::get<std::string>(result);
        }

        T registro = std::get<T>(result);

        // We can't do this easily generically without a trait to SET deleted.
        // But let's assume we can just modify the raw struct directly? No.
        // Let's add setDeleted to EntityTraits.
        EntityTraits<T>::setDeleted(registro, true);

        auto updateRes = actualizarTemplate(id, registro);
        if (std::holds_alternative<std::string>(updateRes)) {
            return updateRes;
        }

        // decrement active count
        std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (file.is_open()) {
            HeaderFile header;
            file.read(reinterpret_cast<char*>(&header), sizeof(HeaderFile));
            header.registrosActivos--;
            file.seekp(0);
            file.write(reinterpret_cast<const char*>(&header), sizeof(HeaderFile));
        }

        return true;
    }
};
