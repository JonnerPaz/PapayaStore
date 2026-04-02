#pragma once
#include <chrono>
#include <cstring>

using std::chrono::system_clock;
using std::chrono::time_point;

class EntidadBase
{
   private:
    int m_id{0};              // Identificador único (autoincremental)
    char m_nombre[100]{0};    // Nombre de la entidad
    bool m_eliminado{false};  // Indica si la entidad ha sido eliminada
    time_point<system_clock> m_fechaCreacion{system_clock::now()};
    time_point<system_clock> m_fechaUltimaModificacion{system_clock::now()};

   protected:
    void copiarCadenaSeguro(char* destino, size_t capacidad, const char* fuente);

   public:
    EntidadBase(int id, const char* nombre, bool eliminado, time_point<system_clock> fechaCreacion,
                time_point<system_clock> fechaUltimaModificacion);

    int getId() const { return m_id; }

    bool setId(int id);

    const char* getNombre() const { return m_nombre; }

    bool setNombre(const char* nombre);

    bool getEliminado() const { return m_eliminado; }

    bool setEliminado(bool eliminado);

    bool setFechaUltimaModificacion(time_point<system_clock> fechaUltimaModificacion);

    virtual ~EntidadBase() = 0;
};

using Entidad = EntidadBase;
