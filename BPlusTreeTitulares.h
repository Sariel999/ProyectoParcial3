/**
 * @file BPlusTreeTitulares.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef BPLUSTREETITULARES_H
#define BPLUSTREETITULARES_H

#include "NodoBPlus.h"
#include "Titular.h"
#include "ListaDobleCircular.h"
#include <SFML/Graphics.hpp>

class BPlusTreeTitulares {
private:
    NodoBPlus* raiz;
    int grado;

    void liberarNodo(NodoBPlus* nodo);
    void insertarEnNodo(NodoBPlus* nodo, const std::string& ci, Titular* titular, 
                       NodoBPlus*& nuevoHijo, std::string& nuevaClave);
    Titular* buscarEnNodo(NodoBPlus* nodo, const std::string& ci) const;
    void eliminarEnNodo(NodoBPlus* nodo, const std::string& ci, NodoBPlus* padre, int indicePadre);
    void manejarUnderflow(NodoBPlus* nodo, NodoBPlus* padre, int indice);
    void imprimirNodo(NodoBPlus* nodo, int nivel, bool esRaiz = false) const;
    void imprimirNodoArbolBPlus(NodoBPlus* nodo, int nivel) const;

public:
    BPlusTreeTitulares(int grado_);
    BPlusTreeTitulares();
    ~BPlusTreeTitulares();

    void insertar(const std::string& ci, Titular* titular);
    Titular* buscar(const std::string& ci) const;
    void eliminar(const std::string& ci);
    void imprimir() const;
    void imprimirArbolBPlus() const;
    void construirDesdeLista(NodoDoble<Titular*>* cabeza);
    void graficarArbol() const;
};

#endif