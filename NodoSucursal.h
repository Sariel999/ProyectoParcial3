#ifndef NODOSUCURSAL_H
#define NODOSUCURSAL_H

#include "Sucursal.h"

class NodoSucursal {
public:
    Sucursal sucursal;  // Sucursal almacenada en el nodo
    NodoSucursal* siguiente;  // Puntero al siguiente nodo

    // Constructor
    NodoSucursal(const Sucursal& sucursal) : sucursal(sucursal), siguiente(nullptr) {}
};

#endif
