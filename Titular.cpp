/**
 * @file Titular.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "Titular.h"

Titular::Titular() {
    cuentaCorriente = nullptr;
}

Titular::Titular(const Persona& p) : persona(p) {
    cuentaCorriente = nullptr;
}

Titular::~Titular() {
    delete cuentaCorriente;
    // Las cuentas de ahorro deben ser eliminadas si fueron creadas dinámicamente.
    NodoDoble<CuentaBancaria*>* actual = cuentasAhorro.getCabeza();
    if (actual != nullptr) {
        do {
            delete actual->dato;
            actual = actual->siguiente;
        } while (actual != cuentasAhorro.getCabeza());
    }
}

Persona Titular::getPersona() const {
    return persona;
}

void Titular::setPersona(const Persona& p) {
    persona = p;
}

CuentaBancaria* Titular::getCuentaCorriente() const {
    return cuentaCorriente;
}

void Titular::setCuentaCorriente(CuentaBancaria* cuenta) {
    cuentaCorriente = cuenta;
}

void Titular::agregarCuentaAhorro(CuentaBancaria* cuenta) {
    cuentasAhorro.insertar(cuenta);
}

void Titular::mostrarCuentasAhorro() const {
    cuentasAhorro.imprimirAdelante();
}

ListaDobleCircular<CuentaBancaria*>& Titular::getCuentasAhorro() {
    return cuentasAhorro;
}