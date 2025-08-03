#ifndef ORDENAMIENTO_H
#define ORDENAMIENTO_H

#include "ListaDobleCircular.h"

// Ordenamiento genérico usando Merge Sort sobre listas doblemente enlazadas circulares
template <typename T, typename Compare>
class Ordenamiento {
public:
    // Ordena la lista usando Merge Sort
    void mergeSort(ListaDobleCircular<T>& lista, Compare cmp) {
        if (lista.vacia() || lista.getCabeza()->siguiente == lista.getCabeza())
            return; // 0 o 1 elemento

        ListaDobleCircular<T> izquierda, derecha;
        dividir(lista, izquierda, derecha);

        mergeSort(izquierda, cmp);
        mergeSort(derecha, cmp);

        ListaDobleCircular<T> resultado = merge(izquierda, derecha, cmp);

        // Vacía la lista original y copia los elementos ordenados
        while (!lista.vacia()) {
            lista.eliminarPorCabeza();
        }
        NodoDoble<T>* actual = resultado.getCabeza();
        if (actual) {
            do {
                lista.insertar(actual->dato);
                actual = actual->siguiente;
            } while (actual != resultado.getCabeza());
        }
    }

    // Búsqueda binaria en lista doble circular (requiere lista ordenada)
    // key es el valor a buscar, cmp es un comparador que retorna:
    //  0 si son iguales, <0 si a<key, >0 si a>key
    template<typename Key, typename CmpKey>
    NodoDoble<T>* busquedaBinaria(ListaDobleCircular<T>& lista, const Key& key, CmpKey cmpKey) {
        int n = 0;
        NodoDoble<T>* actual = lista.getCabeza();
        if (!actual) return nullptr;
        do {
            n++;
            actual = actual->siguiente;
        } while (actual != lista.getCabeza());

        int left = 0, right = n - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            NodoDoble<T>* midNode = lista.getCabeza();
            for (int i = 0; i < mid; ++i) midNode = midNode->siguiente;

            int res = cmpKey(midNode->dato, key);
            if (res == 0)
                return midNode;
            else if (res < 0)
                left = mid + 1;
            else
                right = mid - 1;
        }
        return nullptr;
    }

private:
    // Divide la lista en dos mitades
    void dividir(ListaDobleCircular<T>& lista, ListaDobleCircular<T>& izquierda, ListaDobleCircular<T>& derecha) {
        NodoDoble<T>* slow = lista.getCabeza();
        NodoDoble<T>* fast = lista.getCabeza();
        if (!slow) return;

        // Encuentra el punto medio usando el método de tortuga y liebre
        while (fast->siguiente != lista.getCabeza() && fast->siguiente->siguiente != lista.getCabeza()) {
            slow = slow->siguiente;
            fast = fast->siguiente->siguiente;
        }

        NodoDoble<T>* mid = slow;
        NodoDoble<T>* actual = lista.getCabeza();
        // Construye la lista izquierda
        do {
            izquierda.insertar(actual->dato);
            if (actual == mid) break;
            actual = actual->siguiente;
        } while (actual != lista.getCabeza());

        // Construye la lista derecha
        actual = mid->siguiente;
        while (actual != lista.getCabeza()) {
            derecha.insertar(actual->dato);
            actual = actual->siguiente;
        }
    }

    // Mezcla dos listas ordenadas en una sola lista ordenada
    ListaDobleCircular<T> merge(ListaDobleCircular<T>& izquierda, ListaDobleCircular<T>& derecha, Compare cmp) {
        ListaDobleCircular<T> resultado;
        NodoDoble<T>* nodoIzq = izquierda.getCabeza();
        NodoDoble<T>* nodoDer = derecha.getCabeza();
        int tamIzq = 0, tamDer = 0;

        // Calcula el tamaño de cada lista
        if (nodoIzq) {
            NodoDoble<T>* temp = nodoIzq;
            do { tamIzq++; temp = temp->siguiente; } while (temp != nodoIzq);
        }
        if (nodoDer) {
            NodoDoble<T>* temp = nodoDer;
            do { tamDer++; temp = temp->siguiente; } while (temp != nodoDer);
        }

        int i = 0, j = 0;
        while (i < tamIzq && j < tamDer) {
            if (cmp(nodoIzq->dato, nodoDer->dato)) {
                resultado.insertar(nodoIzq->dato);
                nodoIzq = nodoIzq->siguiente;
                i++;
            } else {
                resultado.insertar(nodoDer->dato);
                nodoDer = nodoDer->siguiente;
                j++;
            }
        }
        while (i < tamIzq) {
            resultado.insertar(nodoIzq->dato);
            nodoIzq = nodoIzq->siguiente;
            i++;
        }
        while (j < tamDer) {
            resultado.insertar(nodoDer->dato);
            nodoDer = nodoDer->siguiente;
            j++;
        }
        return resultado;
    }
};

#endif