#pragma once
#include "domain/entities/entidad.entity.hpp"
#include <chrono>

class Proveedor : public EntidadBase {
  private:
    char rif[20]; // RIF o identificación fiscal
    int productosIds[100];
    int cantidadProductos;
    char telefono[20];
    char email[100];
    char direccion[200];
    char fechaRegistro[11];
    int cantidad;          // cantidad de transacciones / productos
    int historialIds[100]; // Identificadores de transacciones / productos

    bool validarEmail(const char* email) {
        int PosicionAt = -1;            // verificar y darle valor al "@"
        bool TienePuntoDespues = false; // para comprobar que tenga "." despues del "@"
        int longitud = strlen(email);   // para que el ciclo for sepa hasta donde buscar

        // la funcion "for" para recorrer todo el email
        for (int i = 0; i < longitud; i++) {
            if (email[i] == '@') {
                // verificacion si ya habia un @ y hay dos @
                if (PosicionAt != -1)
                    return false;
                PosicionAt = i;
            }
            // verificacion de si hay un '.' despues de ya tener el '@'
            if (PosicionAt != -1 && email[i] == '.') {
                if (i > PosicionAt + 1) {
                    TienePuntoDespues = true;
                }
            }
        }

        // verificacion de los errores
        if (PosicionAt > 0 && PosicionAt < longitud - 1 && TienePuntoDespues) {
            return true;
        }

        return false;
    }

    bool validarFecha(const char* fecha) {
        using namespace std;
        using namespace std::chrono;

        // extraemos los datos de YYYY-MM-DD
        stringstream ss(fecha);
        int y, m, d;
        char dash1, dash2;

        // verificar si es correcto el formato
        if (!(ss >> y >> dash1 >> m >> dash2 >> d)) {
            return false;
        }

        // para ver que tenga los guiones donde van
        if (dash1 != '-' || dash2 != '-') {
            return false;
        }
        // year_month_day representa una fecha en el calendario civil
        year_month_day ymd{year{y}, month{(unsigned)m}, day{(unsigned)d}};
        return ymd.ok();
    }

    char* getTelefono() {
        return this->telefono;
    }

    int setTelefono(char* telefono) {
        strcpy(this->telefono, telefono);
        return 0;
    }

    char* getEmail() {
        return this->email;
    }

    int setEmail(char* email) {
        if (!validarEmail(email)) {
            return 1;
        }

        strcpy(this->email, email);
        return 0;
    }

    char* getDireccion() {
        return this->direccion;
    }

    char* setDireccion(char* direccion) {
        strcpy(this->direccion, direccion);
        return this->direccion;
    }

    char* getFechaRegistro() {
        return this->fechaRegistro;
    }

    int getCantidad() {
        return this->cantidad;
    };

    int* getHistorialIds() {
        return this->historialIds;
    }

  public:
    Proveedor(char* telefono, char* email, char* direccion, char* fechaRegistro, int id,
              char* nombre, bool eliminado, time_point<system_clock> fechaCreacion,
              time_point<system_clock> fechaUltimaModificacion);

    std::chrono::time_point<std::chrono::system_clock> fechaCreacion;
    std::chrono::time_point<std::chrono::system_clock> fechaUltimaModificacion;
};
