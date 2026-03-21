#pragma once
#include "domain/datasource/DB.datasource.hpp"

class FSDB : public DB
{
   public:
    FSDB();
    ~FSDB();
};
