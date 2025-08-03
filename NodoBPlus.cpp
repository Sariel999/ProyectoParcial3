#include "NodoBPlus.h"

NodoBPlus::NodoBPlus(int grado, bool hoja) {
    esHoja = hoja;
    numClaves = 0;
    claves = new std::string[grado];
    hijos = hoja ? nullptr : new NodoBPlus*[grado + 1];
    datos = hoja ? new Titular*[grado] : nullptr;
    siguiente = nullptr;
}

NodoBPlus::~NodoBPlus() {
    delete[] claves;
    if (hijos) delete[] hijos;
    if (datos) delete[] datos;
}