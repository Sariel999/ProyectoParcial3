#ifndef NODOS_H
#define NODOS_H

template <typename T>
class NodoS{
    public:
        T dato;
        NodoS<T>* siguiente;
        NodoS();                      // constructor por defecto
   
        NodoS(T valor){
            dato = valor;
            siguiente = nullptr;
        }
};
# endif
