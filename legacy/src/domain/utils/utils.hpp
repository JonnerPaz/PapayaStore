#pragma once

#include <string>

class DomainUtils
{
   private:
    static std::string trim(const std::string& value);

   public:
    static std::string sanitizeName(const std::string& value);
    static std::string normalizeName(const std::string& value);
};
