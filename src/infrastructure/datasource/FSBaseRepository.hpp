#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <variant>

#include "domain/HeaderFile.hpp"
#include "infrastructure/datasource/EntityTraits.hpp"

namespace fs = std::filesystem;

template <typename T>
class FSBaseRepository
{
   private:
    fs::path filePath;

    /// Lee el HeaderFile del archivo asociado y posiciona el cursor al inicio.
    std::variant<HeaderFile, std::string> readHeader(std::fstream& file)
    {
        HeaderFile header = {};
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(&header), sizeof(HeaderFile));
        if (!file) {
            return "No se pudo leer el encabezado del archivo";
        }

        return header;
    }

    /// Escribe el HeaderFile en el inicio del archivo asociado.
    std::variant<bool, std::string> writeHeader(std::fstream& file, const HeaderFile& header)
    {
        file.seekp(0, std::ios::beg);
        file.write(reinterpret_cast<const char*>(&header), sizeof(HeaderFile));
        if (!file) {
            return "No se pudo escribir el encabezado del archivo";
        }

        return true;
    }

    /// Calcula el offset binario de un registro por ID usando tamano fijo.
    std::streampos getRecordOffset(int id) const
    {
        return static_cast<std::streampos>(sizeof(HeaderFile)) +
               static_cast<std::streampos>(id - 1) * EntityTraits<T>::recordSize();
    }

   public:
    explicit FSBaseRepository(fs::path path) : filePath(std::move(path)) {}

    /// Retorna estadisticas del archivo (HeaderFile) para la entidad T.
    std::variant<HeaderFile, std::string> obtenerEstadisticasTemplate()
    {
        std::fstream file(filePath, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            return "Error abriendo archivo para obtener estadísticas: " + filePath.string();
        }

        auto headerResult = readHeader(file);
        if (std::holds_alternative<std::string>(headerResult)) {
            return std::get<std::string>(headerResult);
        }

        return std::get<HeaderFile>(headerResult);
    }

    /// Lee un registro activo por ID usando acceso aleatorio y EntityTraits<T>.
    std::variant<T, std::string> leerTemplate(int id)
    {
        std::fstream file(filePath, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            return "Error abriendo archivo para lectura: " + filePath.string();
        }

        auto headerResult = readHeader(file);
        if (std::holds_alternative<std::string>(headerResult)) {
            return std::get<std::string>(headerResult);
        }

        const HeaderFile header = std::get<HeaderFile>(headerResult);
        if (id <= 0 || id >= header.proximoID) {
            return "ID fuera de rango o registro no existe";
        }

        file.seekg(getRecordOffset(id), std::ios::beg);
        if (!file) {
            return "Error moviendo el puntero de lectura";
        }

        T registro;
        if (!EntityTraits<T>::readFromStream(file, registro)) {
            return "Error leyendo registro desde archivo";
        }

        if (EntityTraits<T>::isDeleted(registro)) {
            return "El registro ha sido eliminado";
        }

        return registro;
    }

    /// Sobrescribe un registro existente por ID usando serializacion deterministica.
    std::variant<bool, std::string> actualizarTemplate(int id, const T& entidad)
    {
        std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            return "Error abriendo archivo para escritura";
        }

        auto headerResult = readHeader(file);
        if (std::holds_alternative<std::string>(headerResult)) {
            return std::get<std::string>(headerResult);
        }

        const HeaderFile header = std::get<HeaderFile>(headerResult);
        if (id <= 0 || id >= header.proximoID) {
            return "ID fuera de rango o registro no existe";
        }

        file.seekp(getRecordOffset(id), std::ios::beg);
        if (!file) {
            return "Error moviendo el puntero de escritura";
        }

        if (!EntityTraits<T>::writeToStream(file, entidad)) {
            return "Error escribiendo registro en archivo";
        }

        return true;
    }

    /// Guarda un nuevo registro al final logico del archivo y actualiza el header.
    std::variant<bool, std::string> guardarTemplate(const T& entidad)
    {
        std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            return "Error abriendo archivo para guardar";
        }

        auto headerResult = readHeader(file);
        if (std::holds_alternative<std::string>(headerResult)) {
            return std::get<std::string>(headerResult);
        }

        HeaderFile header = std::get<HeaderFile>(headerResult);
        const int nuevoId = header.proximoID;
        const int entidadId = EntityTraits<T>::getId(entidad);
        if (entidadId != nuevoId) {
            return "El ID de la entidad no coincide con proximoID";
        }

        file.seekp(getRecordOffset(nuevoId), std::ios::beg);
        if (!file) {
            return "Error moviendo el puntero de escritura";
        }

        if (!EntityTraits<T>::writeToStream(file, entidad)) {
            return "Error escribiendo registro en archivo";
        }

        header.cantidadRegistros += 1;
        header.registrosActivos += 1;
        header.proximoID += 1;

        auto headerWriteResult = writeHeader(file, header);
        if (std::holds_alternative<std::string>(headerWriteResult)) {
            return std::get<std::string>(headerWriteResult);
        }

        return true;
    }

    /// Marca un registro como eliminado y decrementa registros activos en header.
    std::variant<bool, std::string> eliminarLogicamenteTemplate(int id)
    {
        auto result = leerTemplate(id);
        if (std::holds_alternative<std::string>(result)) {
            return std::get<std::string>(result);
        }

        T registro = std::get<T>(result);
        EntityTraits<T>::setDeleted(registro, true);

        auto updateResult = actualizarTemplate(id, registro);
        if (std::holds_alternative<std::string>(updateResult)) {
            return std::get<std::string>(updateResult);
        }

        std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            return "Registro eliminado, pero no se pudo abrir archivo para actualizar header";
        }

        auto headerResult = readHeader(file);
        if (std::holds_alternative<std::string>(headerResult)) {
            return std::get<std::string>(headerResult);
        }

        HeaderFile header = std::get<HeaderFile>(headerResult);
        if (header.registrosActivos > 0) {
            header.registrosActivos -= 1;
        }

        auto headerWriteResult = writeHeader(file, header);
        if (std::holds_alternative<std::string>(headerWriteResult)) {
            return std::get<std::string>(headerWriteResult);
        }

        return true;
    }
};
