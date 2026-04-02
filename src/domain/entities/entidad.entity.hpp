#pragma once
#include <chrono>
#include <cstring>

using std::chrono::system_clock;
using std::chrono::time_point;

class EntidadBase
{
   protected:
    int m_id;            // Identificador único (autoincremental)
    char m_nombre[100];  // Nombre de la entidad
    bool m_eliminado;    // Indica si la entidad ha sido eliminada
    time_point<system_clock> m_fechaCreacion;
    time_point<system_clock> m_fechaUltimaModificacion;

   public:
    EntidadBase(int id, const char* nombre, bool eliminado, time_point<system_clock> fechaCreacion,
                time_point<system_clock> fechaUltimaModificacion);

    time_point<system_clock> setFechaCreacion(time_point<system_clock> fechaCreacion)
    {
        this->m_fechaCreacion = fechaCreacion;
        return fechaCreacion;
    }

    int getId() const { return m_id; }

    void setId(int id) { this->m_id = id; }

    char* getNombre() { return m_nombre; }

    const char* getNombre() const { return m_nombre; }

    void setNombre(const char* nombre) { strcpy(this->m_nombre, nombre); }

    bool getEliminado() const { return m_eliminado; }

    void setEliminado(bool eliminado) { this->m_eliminado = eliminado; }

    time_point<system_clock> setFechaUltimaModificacion(
        time_point<system_clock> fechaUltimaModificacion)
    {
        this->m_fechaUltimaModificacion = fechaUltimaModificacion;
        return fechaUltimaModificacion;
    }

    void obtenerFechaActual(char* fecha)
    {
        auto ahora = std::chrono::system_clock::now();
        // Convertimos el tiempo a formato de días (sys_days)
        auto sys_days_now = std::chrono::time_point_cast<std::chrono::days>(ahora);
        // Extraemos la fecha del calendario civil (year_month_day)
        std::chrono::year_month_day ymd{sys_days_now};

        // Convertimos las partes a enteros
        int anio = static_cast<int>(ymd.year());
        unsigned mes = static_cast<unsigned>(ymd.month());
        unsigned dia = static_cast<unsigned>(ymd.day());

        // Formateamos como "YYYY-MM-DD" y lo guardamos en el buffer
        // (máximo 11 caracteres)
        snprintf(fecha, 11, "%04d-%02u-%02u", anio, mes, dia);
        fecha[10] = '\0';  // Aseguramos el null-termination por si acaso
    }

    virtual ~EntidadBase() = 0;
};

using Entidad = EntidadBase;
