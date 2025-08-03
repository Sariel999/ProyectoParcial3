/**
 * @file BPlusTreeTitulares.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "BPlusTreeTitulares.h"
#include <iostream>

BPlusTreeTitulares::BPlusTreeTitulares(int grado_) : raiz(nullptr), grado(grado_) {}

BPlusTreeTitulares::~BPlusTreeTitulares() {
    liberarNodo(raiz);
}

void BPlusTreeTitulares::liberarNodo(NodoBPlus* nodo) {
    if (!nodo) return;
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; ++i)
            liberarNodo(nodo->hijos[i]);
    }
    delete nodo;
}

void BPlusTreeTitulares::insertar(const std::string& ci, Titular* titular) {

    if (!raiz) {
        raiz = new NodoBPlus(grado, true);
        raiz->claves[0] = ci;
        raiz->datos[0] = titular;
        raiz->numClaves = 1;
        return;
    }
    NodoBPlus* nuevoHijo = nullptr;
    std::string nuevaClave;
    insertarEnNodo(raiz, ci, titular, nuevoHijo, nuevaClave);
    if (nuevoHijo) {
        NodoBPlus* nuevaRaiz = new NodoBPlus(grado, false);
        nuevaRaiz->claves[0] = nuevaClave;
        nuevaRaiz->hijos[0] = raiz;
        nuevaRaiz->hijos[1] = nuevoHijo;
        nuevaRaiz->numClaves = 1;
        raiz = nuevaRaiz;
    }
     // Imprimir después de cada inserción para verificar
}

void BPlusTreeTitulares::insertarEnNodo(NodoBPlus* nodo, const std::string& ci, Titular* titular, NodoBPlus*& nuevoHijo, std::string& nuevaClave) {
    int i = nodo->numClaves - 1;
    if (nodo->esHoja) {
        // Insertar en hoja
        while (i >= 0 && ci < nodo->claves[i]) {
            nodo->claves[i + 1] = nodo->claves[i];
            nodo->datos[i + 1] = nodo->datos[i];
            --i;
        }
        nodo->claves[i + 1] = ci;
        nodo->datos[i + 1] = titular;
        nodo->numClaves++;

        // Split si es necesario
        if (nodo->numClaves == grado) {
            int mitad = grado / 2;
            NodoBPlus* nuevo = new NodoBPlus(grado, true);
            nuevo->numClaves = nodo->numClaves - mitad;
            for (int j = 0; j < nuevo->numClaves; ++j) {
                nuevo->claves[j] = nodo->claves[mitad + j];
                nuevo->datos[j] = nodo->datos[mitad + j];
            }
            nodo->numClaves = mitad;
            nuevo->siguiente = nodo->siguiente;
            nodo->siguiente = nuevo;
            nuevaClave = nuevo->claves[0];
            nuevoHijo = nuevo;
        } else {
            nuevoHijo = nullptr;
        }
    } else {
        // Insertar en hijo adecuado
        while (i >= 0 && ci < nodo->claves[i]) --i;
        ++i;
        NodoBPlus* hijoNuevo = nullptr;
        std::string claveNueva;
        insertarEnNodo(nodo->hijos[i], ci, titular, hijoNuevo, claveNueva);
        if (hijoNuevo) {
            // Insertar nueva clave y puntero en este nodo
            for (int j = nodo->numClaves; j > i; --j) {
                nodo->claves[j] = nodo->claves[j - 1];
                nodo->hijos[j + 1] = nodo->hijos[j];
            }
            nodo->claves[i] = claveNueva;
            nodo->hijos[i + 1] = hijoNuevo;
            nodo->numClaves++;
            // Split si es necesario
            if (nodo->numClaves == grado) {
                int mitad = grado / 2;
                NodoBPlus* nuevo = new NodoBPlus(grado, false);
                nuevo->numClaves = nodo->numClaves - mitad - 1;
                for (int j = 0; j < nuevo->numClaves; ++j) {
                    nuevo->claves[j] = nodo->claves[mitad + 1 + j];
                    nuevo->hijos[j] = nodo->hijos[mitad + 1 + j];
                }
                nuevo->hijos[nuevo->numClaves] = nodo->hijos[grado];
                nuevaClave = nodo->claves[mitad];
                nodo->numClaves = mitad;
                nuevoHijo = nuevo;
            } else {
                nuevoHijo = nullptr;
            }
        } else {
            nuevoHijo = nullptr;
        }
    }
}

Titular* BPlusTreeTitulares::buscar(const std::string& ci) const {
    
    if (!raiz) {
        
        return nullptr;
    }
    return buscarEnNodo(raiz, ci);
}

Titular* BPlusTreeTitulares::buscarEnNodo(NodoBPlus* nodo, const std::string& ci) const {
    if (!nodo) {
        
        return nullptr;  // Si el nodo es nulo, no encontramos el CI
    }

    int i = 0;
    while (i < nodo->numClaves && ci > nodo->claves[i]) {
        ++i;
    }


    if (nodo->esHoja) {
        // Si es un nodo hoja, buscamos el titular
        if (i < nodo->numClaves && ci == nodo->claves[i]) {
            return nodo->datos[i];  // Retornamos el Titular correspondiente
        }
        return nullptr;
    }

    // Si no es hoja, seguimos buscando en los hijos
    return buscarEnNodo(nodo->hijos[i], ci);
}

void BPlusTreeTitulares::eliminar(const std::string& ci) {
    if (!raiz) return;

    eliminarEnNodo(raiz, ci, nullptr, -1);

    // Si la raíz queda vacía y tiene un hijo, actualizar la raíz
    if (!raiz->esHoja && raiz->numClaves == 0 && raiz->hijos[0]) {
        NodoBPlus* temp = raiz;
        raiz = raiz->hijos[0];
        delete temp;
    }
}

void BPlusTreeTitulares::eliminarEnNodo(NodoBPlus* nodo, const std::string& ci, NodoBPlus* padre, int indicePadre) {
    int i = 0;
    while (i < nodo->numClaves && ci > nodo->claves[i]) {
        ++i;
    }

    if (nodo->esHoja) {
        // Buscar la clave en la hoja
        if (i < nodo->numClaves && ci == nodo->claves[i]) {
            // Eliminar la clave y el dato
            for (int j = i; j < nodo->numClaves - 1; ++j) {
                nodo->claves[j] = nodo->claves[j + 1];
                nodo->datos[j] = nodo->datos[j + 1];
            }
            nodo->numClaves--;
        } else {
            return; // Clave no encontrada
        }

        // Verificar si el nodo tiene menos claves de las mínimas
        if (nodo->numClaves < (grado + 1) / 2 - 1 && padre) {
            manejarUnderflow(nodo, padre, indicePadre);
        }
    } else {
        // Nodo interno, descender al hijo adecuado
        eliminarEnNodo(nodo->hijos[i], ci, nodo, i);

        // Verificar si el hijo tiene menos claves de las mínimas
        if (i <= nodo->numClaves && nodo->hijos[i]->numClaves < (grado + 1) / 2 - 1) {
            manejarUnderflow(nodo->hijos[i], nodo, i);
        }
    }
}

void BPlusTreeTitulares::manejarUnderflow(NodoBPlus* nodo, NodoBPlus* padre, int indice) {
    // Obtener hermanos
    NodoBPlus* hermanoIzq = (indice > 0) ? padre->hijos[indice - 1] : nullptr;
    NodoBPlus* hermanoDer = (indice < padre->numClaves) ? padre->hijos[indice + 1] : nullptr;

    // Intentar redistribuir con el hermano izquierdo
    if (hermanoIzq && hermanoIzq->numClaves > (grado + 1) / 2 - 1) {
        if (nodo->esHoja) {
            // Mover la última clave del hermano izquierdo al nodo
            for (int j = nodo->numClaves; j > 0; --j) {
                nodo->claves[j] = nodo->claves[j - 1];
                nodo->datos[j] = nodo->datos[j - 1];
            }
            nodo->claves[0] = hermanoIzq->claves[hermanoIzq->numClaves - 1];
            nodo->datos[0] = hermanoIzq->datos[hermanoIzq->numClaves - 1];
            nodo->numClaves++;
            hermanoIzq->numClaves--;
            // Actualizar clave en el padre
            padre->claves[indice - 1] = nodo->claves[0];
        } else {
            // Mover la clave del padre al nodo y la última clave del hermano al padre
            nodo->claves[nodo->numClaves] = padre->claves[indice - 1];
            nodo->hijos[nodo->numClaves + 1] = hermanoIzq->hijos[hermanoIzq->numClaves];
            nodo->numClaves++;
            padre->claves[indice - 1] = hermanoIzq->claves[hermanoIzq->numClaves - 1];
            hermanoIzq->numClaves--;
        }
        return;
    }

    // Intentar redistribuir con el hermano derecho
    if (hermanoDer && hermanoDer->numClaves > (grado + 1) / 2 - 1) {
        if (nodo->esHoja) {
            // Mover la primera clave del hermano derecho al nodo
            nodo->claves[nodo->numClaves] = hermanoDer->claves[0];
            nodo->datos[nodo->numClaves] = hermanoDer->datos[0];
            nodo->numClaves++;
            for (int j = 0; j < hermanoDer->numClaves - 1; ++j) {
                hermanoDer->claves[j] = hermanoDer->claves[j + 1];
                hermanoDer->datos[j] = hermanoDer->datos[j + 1];
            }
            hermanoDer->numClaves--;
            // Actualizar clave en el padre
            padre->claves[indice] = hermanoDer->claves[0];
        } else {
            // Mover la clave del padre al nodo y la primera clave del hermano al padre
            nodo->claves[nodo->numClaves] = padre->claves[indice];
            nodo->hijos[nodo->numClaves + 1] = hermanoDer->hijos[0];
            nodo->numClaves++;
            padre->claves[indice] = hermanoDer->claves[0];
            for (int j = 0; j < hermanoDer->numClaves - 1; ++j) {
                hermanoDer->claves[j] = hermanoDer->claves[j + 1];
                hermanoDer->hijos[j] = hermanoDer->hijos[j + 1];
            }
            hermanoDer->hijos[hermanoDer->numClaves - 1] = hermanoDer->hijos[hermanoDer->numClaves];
            hermanoDer->numClaves--;
        }
        return;
    }

    // Fusionar con el hermano izquierdo o derecho
    if (hermanoIzq) {
        if (nodo->esHoja) {
            // Fusionar nodo con hermano izquierdo
            for (int j = 0; j < nodo->numClaves; ++j) {
                hermanoIzq->claves[hermanoIzq->numClaves + j] = nodo->claves[j];
                hermanoIzq->datos[hermanoIzq->numClaves + j] = nodo->datos[j];
            }
            hermanoIzq->numClaves += nodo->numClaves;
            hermanoIzq->siguiente = nodo->siguiente;
            // Eliminar la clave del padre
            for (int j = indice - 1; j < padre->numClaves - 1; ++j) {
                padre->claves[j] = padre->claves[j + 1];
                padre->hijos[j + 1] = padre->hijos[j + 2];
            }
            padre->numClaves--;
            delete nodo;
        } else {
            // Fusionar nodo con hermano izquierdo
            hermanoIzq->claves[hermanoIzq->numClaves] = padre->claves[indice - 1];
            hermanoIzq->numClaves++;
            for (int j = 0; j < nodo->numClaves; ++j) {
                hermanoIzq->claves[hermanoIzq->numClaves + j] = nodo->claves[j];
                hermanoIzq->hijos[hermanoIzq->numClaves + j] = nodo->hijos[j];
            }
            hermanoIzq->hijos[hermanoIzq->numClaves + nodo->numClaves] = nodo->hijos[nodo->numClaves];
            hermanoIzq->numClaves += nodo->numClaves;
            for (int j = indice - 1; j < padre->numClaves - 1; ++j) {
                padre->claves[j] = padre->claves[j + 1];
                padre->hijos[j + 1] = padre->hijos[j + 2];
            }
            padre->numClaves--;
            delete nodo;
        }
    } else if (hermanoDer) {
        if (nodo->esHoja) {
            // Fusionar nodo con hermano derecho
            for (int j = 0; j < hermanoDer->numClaves; ++j) {
                nodo->claves[nodo->numClaves + j] = hermanoDer->claves[j];
                nodo->datos[nodo->numClaves + j] = hermanoDer->datos[j];
            }
            nodo->numClaves += hermanoDer->numClaves;
            nodo->siguiente = hermanoDer->siguiente;
            for (int j = indice; j < padre->numClaves - 1; ++j) {
                padre->claves[j] = padre->claves[j + 1];
                padre->hijos[j + 1] = padre->hijos[j + 2];
            }
            padre->numClaves--;
            delete hermanoDer;
        } else {
            // Fusionar nodo con hermano derecho
            nodo->claves[nodo->numClaves] = padre->claves[indice];
            nodo->numClaves++;
            for (int j = 0; j < hermanoDer->numClaves; ++j) {
                nodo->claves[nodo->numClaves + j] = hermanoDer->claves[j];
                nodo->hijos[nodo->numClaves + j] = hermanoDer->hijos[j];
            }
            nodo->hijos[nodo->numClaves + hermanoDer->numClaves] = hermanoDer->hijos[hermanoDer->numClaves];
            nodo->numClaves += hermanoDer->numClaves;
            for (int j = indice; j < padre->numClaves - 1; ++j) {
                padre->claves[j] = padre->claves[j + 1];
                padre->hijos[j + 1] = padre->hijos[j + 2];
            }
            padre->numClaves--;
            delete hermanoDer;
        }
    }
}

void BPlusTreeTitulares::imprimir() const {
    imprimirNodo(raiz, 0, true);
}

void BPlusTreeTitulares::imprimirNodo(NodoBPlus* nodo, int nivel,bool esRaiz) const {
    if (!nodo) return;
    if (esRaiz) {
        std::cout << "Raiz: ";
    } else {
        std::cout << std::string(nivel * 2, ' ') << (nodo->esHoja ? "[Hoja] " : "[Interno] ");
    }
    for (int i = 0; i < nodo->numClaves; ++i)
        std::cout << nodo->claves[i] << " ";
    std::cout << std::endl;
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; ++i)
            imprimirNodo(nodo->hijos[i], nivel + 1);
    }
}

void BPlusTreeTitulares::construirDesdeLista(NodoDoble<Titular*>* cabeza) {
    if (!cabeza) return;
    NodoDoble<Titular*>* actual = cabeza;
    do {
        Titular* t = actual->dato;
        insertar(t->getPersona().getCI(), t);
        actual = actual->siguiente;
    } while (actual != cabeza);
}
void BPlusTreeTitulares::imprimirArbolBPlus() const {
    if (!raiz) {
        std::cout << "Arbol B+ vacio" << std::endl;
        return;
    }
    std::cout << "Nivel 0 (Raiz):" << std::endl;
    imprimirNodoArbolBPlus(raiz, 0);
}

void BPlusTreeTitulares::imprimirNodoArbolBPlus(NodoBPlus* nodo, int nivel) const {
    if (!nodo) return;

    // Indentación basada en el nivel
    std::string indent(nivel * 4, ' ');
    std::string connector = (nivel > 0) ? "  |--" : "";

    // Etiqueta de nivel
    std::cout << indent << "Nivel " << nivel << ": " << connector << "[";

    // Mostrar las claves
    for (int i = 0; i < nodo->numClaves; ++i) {
        std::cout << nodo->claves[i];
        if (i < nodo->numClaves - 1) std::cout << ", ";
    }
    std::cout << "]" << (nodo->esHoja ? " (Hoja)" : " (Interno)") << std::endl;

    // Imprimir hijos si no es hoja
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; ++i) {
            // Línea de conexión
            if (i < nodo->numClaves) {
                std::cout << indent << "  |" << std::endl;
            }
            imprimirNodoArbolBPlus(nodo->hijos[i], nivel + 1);
        }
    }
}