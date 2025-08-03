#ifndef NODODOBLE_H
#define NODODOBLE_H

template <typename T>
class NodoDoble {
public:
    T dato;
    NodoDoble<T>* siguiente;
    NodoDoble<T>* anterior;

    NodoDoble(const T& valor) : dato(valor), siguiente(nullptr), anterior(nullptr) {}
};

#endif