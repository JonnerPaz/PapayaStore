#pragma once
#include "entidad.entity.hpp"

class Usuario : public Entidad {
  private:
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

  protected:
    virtual char* getTelefono() {
        return this->telefono;
    }

    virtual int setTelefono(char* telefono);

    virtual char* getEmail() {
        return this->email;
    }

    virtual int setEmail(char* email) {
        if (!validarEmail(email)) {
            return 1;
        }

        strcpy(this->email, email);
        return 0;
    }

    virtual char* getDireccion() {
        return this->direccion;
    }

    virtual char* setDireccion(char* direccion);

    virtual char* getFechaRegistro() {
        return this->fechaRegistro;
    }

    virtual int getCantidad() {
        return this->cantidad;
    };

    virtual int* getHistorialIds() {
        return this->historialIds;
    }

  public:
    virtual ~Usuario() = default;
};
