#ifndef PILA_H
#define PILA_H

#include "NodoS.h"
#include <iostream>
#include <stdexcept>

template <typename T>
class Pila {
private:
    NodoS<T>* tope;

public:
    Pila();
    ~Pila();

    void push(T movimiento);
    void pop();
    T cima() const;
    bool vacia() const;
    void imprimir() const;
};

template <typename T>
Pila<T>::Pila() {
    tope = nullptr;
}

template <typename T>
Pila<T>::~Pila() {
    while (tope != nullptr) {
        NodoS<T>* temp = tope;
        tope = tope->siguiente;
        delete temp;
    }
}

template <typename T>
void Pila<T>::push(T movimiento) {
    NodoS<T>* nuevo = new NodoS<T>(movimiento);
    nuevo->siguiente = tope;
    tope = nuevo;
}

template <typename T>
void Pila<T>::pop() {
    if (tope != nullptr) {
        NodoS<T>* temp = tope;
        tope = tope->siguiente;
        delete temp;
    }
}

template <typename T>
T Pila<T>::cima() const {
    if (tope != nullptr) {
        return tope->dato;
    }
    throw std::runtime_error("La pila esta vacia");
}

template <typename T>
bool Pila<T>::vacia() const {
    return tope == nullptr;
}

template <typename T>
void Pila<T>::imprimir() const {
    NodoS<T>* actual = tope;
    while (actual != nullptr) {
        std::cout << actual->dato << " ";
        actual = actual->siguiente;
    }
    std::cout << std::endl;
}

#endif