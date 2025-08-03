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

class BPlusTreeTitulares {
    NodoBPlus* raiz;
    int grado; // Grado del árbol (máximo de claves por nodo)
public:
    BPlusTreeTitulares(int grado = 3);
    ~BPlusTreeTitulares();

    void insertar(const std::string& ci, Titular* titular);
    Titular* buscar(const std::string& ci) const;
    void imprimir() const;
    void construirDesdeLista(NodoDoble<Titular*>* cabeza);
    void manejarUnderflow(NodoBPlus* nodo, NodoBPlus* padre, int indice);
    void eliminarEnNodo(NodoBPlus* nodo, const std::string& ci, NodoBPlus* padre, int indicePadre);
    void eliminar(const std::string& ci);
    NodoBPlus* getRaiz() const { return raiz; }
    void imprimirArbolBPlus() const;
    void imprimirNodoArbolBPlus(NodoBPlus* nodo, int nivel) const;

private:
    void insertarEnNodo(NodoBPlus* nodo, const std::string& ci, Titular* titular, NodoBPlus*& nuevoHijo, std::string& nuevaClave);
    Titular* buscarEnNodo(NodoBPlus* nodo, const std::string& ci) const;
    void imprimirNodo(NodoBPlus* nodo, int nivel,bool esRaiz = false) const;
    void liberarNodo(NodoBPlus* nodo);
};

#endif