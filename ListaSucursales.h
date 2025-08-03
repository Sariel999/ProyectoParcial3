#ifndef LISTASUCURSAL_H
#define LISTASUCURSAL_H

#include "NodoSucursal.h"
#include <string>

class ListaSucursales {
private:
    NodoSucursal* cabeza;
public:
    ListaSucursales();
    ~ListaSucursales();
    void agregarSucursal(const Sucursal& sucursal);
    Sucursal* encontrarSucursalMasCercana(float latUsuario, float lonUsuario);
    void mostrarSucursales() const;
    bool existeSucursal(const std::string& idSucursal) const;
    NodoSucursal* getCabeza() const;
};
#endif

