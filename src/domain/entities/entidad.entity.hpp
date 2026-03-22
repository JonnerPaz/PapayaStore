#pragma once
#include <chrono>
#include <cstring>

using std::chrono::system_clock;
using std::chrono::time_point;

class Entidad {
  private:
    int id;           // Identificador único (autoincremental)
    char nombre[100]; // Nombre de la entidad
    bool eliminado;   // Indica si la entidad ha sido eliminada
    time_point<system_clock> fechaCreacion;
    time_point<system_clock> fechaUltimaModificacion;

  protected:
    int setId(int id) {
        this->id = id;
        return id;
    }

    int getId() {
        return this->id;
    }

    char* getNombre() {
        return this->nombre;
    }

    char* setNombre(const char* nombre) {
        strcpy(this->nombre, nombre);
        return this->nombre;
    }

    bool setEliminado(bool eliminado) {
        this->eliminado = eliminado;
        return true;
    }

    time_point<system_clock> setFechaCreacion(time_point<system_clock> fechaCreacion) {
        this->fechaCreacion = fechaCreacion;
        return fechaCreacion;
    }

    time_point<system_clock>
    setFechaUltimaModificacion(time_point<system_clock> fechaUltimaModificacion) {
        this->fechaUltimaModificacion = fechaUltimaModificacion;
        return fechaUltimaModificacion;
    }

    void obtenerFechaActual(char* fecha) {
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
        fecha[10] = '\0'; // Aseguramos el null-termination por si acaso
    }

  public:
    virtual ~Entidad() = default;
};
