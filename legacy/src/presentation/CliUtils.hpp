#pragma once

#include <cctype>
#include <cmath>
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

        // lambda
        auto hasOnlyTrailingSpaces = [](const std::string& text, std::size_t pos) {
            for (std::size_t i = pos; i < text.size(); ++i) {
                if (!std::isspace(static_cast<unsigned char>(text[i]))) {
                    return false;
                }
            }

            return true;
        };

        try {
            if constexpr (std::is_integral_v<T>) {
                std::size_t pos = 0;
                const int parsed = std::stoi(input, &pos, 10);
                if (!hasOnlyTrailingSpaces(input, pos)) {
                    return false;
                }

                const bool cond = zeroInclusive ? parsed < 0 : parsed <= 0;
                if (cond) return false;

                outValue = static_cast<T>(parsed);
                return true;
            } else if constexpr (std::is_floating_point_v<T>) {
                std::size_t pos = 0;
                const float parsed = std::stof(input, &pos);
                if (!hasOnlyTrailingSpaces(input, pos)) {
                    return false;
                }

                if (!std::isfinite(parsed)) {
                    return false;
                }

                const bool cond = zeroInclusive ? parsed < 0.0f : parsed <= 0.0f;
                if (cond) return false;

                outValue = static_cast<T>(parsed);
                return true;
            }

            return false;
        } catch (...) {
            return false;
        }
    }
};
