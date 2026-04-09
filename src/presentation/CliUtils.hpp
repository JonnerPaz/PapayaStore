#pragma once

#include <string>
#include <type_traits>

#include "domain/constants.hpp"

using namespace Constants::ASCII_CODES;
using namespace Constants::PATHS;

enum ListarPorPropiedad { PorId, PorNombre, PorAmbos };

class CliUtils
{
   public:
    /**
     * Lee un id y valida que sea un numero y mayor o igual a 0 si el usuario ingresa 'q', retorna 1
     */
    static std::string readLine(const char* prompt);
    static int readValidId(const char* msg);
    static bool readValidText(const char* prompt, std::string& outValue);

    static void toLowerCase(char* cadena);

    template <typename T>
    static bool readValidNumber(const char* prompt, T& outValue, const char* errorMsg,
                                bool zeroInclusive = true)
    {
        const std::string input = CliUtils::readLine(prompt);
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            return (CliUtils::parsePositiveNumber(input, outValue, zeroInclusive)) ? true : false;
        }
        return false;
    }

    /*
     * Parse input and assign to outValue
     */
    template <typename T>
    static bool parsePositiveNumber(const std::string& input, T& outValue,
                                    bool zeroInclusive = true)
    {
        if (input.empty()) return false;
        try {
            int parsed;
            if constexpr (std::is_integral_v<T>) {
                parsed = std::stoi(input);
            } else if constexpr (std::is_floating_point_v<T>) {
                parsed = std::stof(input);
            }

            bool cond = zeroInclusive ? parsed < 0 : parsed <= 0;
            if (cond) return false;

            outValue = parsed;
            return true;
        } catch (...) {
            return false;
        }
    }
};
