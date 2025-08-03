#ifndef NODOBPLUS_H
#define NODOBPLUS_H

#include <string>
#include "Titular.h"

class NodoBPlus {
public:

    bool esHoja;
    int numClaves;
    std::string* claves;
    NodoBPlus** hijos;
    Titular** datos; // Solo para hojas
    NodoBPlus* siguiente; // Solo para hojas

    NodoBPlus(int grado, bool hoja);
    ~NodoBPlus();
};

#endif