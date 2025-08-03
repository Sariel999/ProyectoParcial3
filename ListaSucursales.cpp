#include "ListaSucursales.h"
#include "Sucursal.h"
#include <cmath>
#include <limits>
ListaSucursales::ListaSucursales() : cabeza(nullptr) {}

// Destructor
ListaSucursales::~ListaSucursales() {
    NodoSucursal* actual = cabeza;
    while (actual != nullptr) {
        NodoSucursal* siguiente = actual->siguiente;
        delete actual;  // Libera la memoria del nodo
        actual = siguiente;
    }
}

// Método para agregar una nueva sucursal a la lista
void ListaSucursales::agregarSucursal(const Sucursal& sucursal) {
    NodoSucursal* nuevoNodo = new NodoSucursal(sucursal);
    if (cabeza == nullptr) {
        cabeza = nuevoNodo;  // Si la lista está vacía, la cabeza es el nuevo nodo
    } else {
        NodoSucursal* actual = cabeza;
        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;  // Avanza hasta el último nodo
        }
        actual->siguiente = nuevoNodo;  // Agrega el nuevo nodo al final de la lista
    }
}

// Método para encontrar la sucursal más cercana al usuario
Sucursal* ListaSucursales::encontrarSucursalMasCercana(float latUsuario, float lonUsuario) {
    NodoSucursal* actual = cabeza;
    Sucursal* sucursalMasCercana = nullptr;
    double distanciaMinima = std::numeric_limits<double>::infinity();

    while (actual != nullptr) {
        double distancia = Sucursal::calcularDistancia(latUsuario, lonUsuario, actual->sucursal.getLatitud(), actual->sucursal.getLongitud());
        if (distancia < distanciaMinima) {
            distanciaMinima = distancia;
            sucursalMasCercana = &actual->sucursal;
        }
        actual = actual->siguiente;
    }

    return sucursalMasCercana;  // Retorna la sucursal más cercana
}

// Método para mostrar todas las sucursales
void ListaSucursales::mostrarSucursales() const {
    NodoSucursal* actual = cabeza;
    while (actual != nullptr) {
        actual->sucursal.imprimir();  // Muestra la información de cada sucursal
        actual = actual->siguiente;
    }
}
bool ListaSucursales::existeSucursal(const std::string& idSucursal) const {
    NodoSucursal* actual = cabeza;
    while (actual != nullptr) {
        if (actual->sucursal.getIdSucursal() == idSucursal) {
            return true;
        }
        actual = actual->siguiente;
    }
    return false;
}
NodoSucursal* ListaSucursales::getCabeza() const { return cabeza; }
