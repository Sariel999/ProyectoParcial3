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
#include "GestorBusquedaMongo.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <queue>
#include <cstring> 

BPlusTreeTitulares::BPlusTreeTitulares(int grado_) : raiz(nullptr), grado(grado_), gestorBusquedaMongo(nullptr) {}

BPlusTreeTitulares::~BPlusTreeTitulares() {
    liberarNodo(raiz);
}

void BPlusTreeTitulares::liberarNodo(NodoBPlus* nodo) {
    if (!nodo) return;
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; ++i) {
            liberarNodo(*(nodo->hijos + i));
        }
    }
    delete nodo;
}

void BPlusTreeTitulares::insertar(const std::string& ci, Titular* titular) {
    if (!raiz) {
        raiz = new NodoBPlus(grado, true);
        *(raiz->claves) = ci;
        *(raiz->datos) = titular;
        raiz->numClaves = 1;
        return;
    }
    NodoBPlus* nuevoHijo = nullptr;
    std::string nuevaClave;
    insertarEnNodo(raiz, ci, titular, nuevoHijo, nuevaClave);
    
    if (nuevoHijo) {
        NodoBPlus* nuevaRaiz = new NodoBPlus(grado, false);
        *(nuevaRaiz->claves) = nuevaClave;
        *(nuevaRaiz->hijos) = raiz;
        *(nuevaRaiz->hijos + 1) = nuevoHijo;
        nuevaRaiz->numClaves = 1;
        raiz = nuevaRaiz;
    }
}

void BPlusTreeTitulares::insertarEnNodo(NodoBPlus* nodo, const std::string& ci, 
    Titular* titular, NodoBPlus*& nuevoHijo, std::string& nuevaClave) {
    
    int i = nodo->numClaves - 1;
    
    if (nodo->esHoja) {
        while (i >= 0 && ci < *(nodo->claves + i)) {
            *(nodo->claves + i + 1) = *(nodo->claves + i);
            *(nodo->datos + i + 1) = *(nodo->datos + i);
            --i;
        }
        
        *(nodo->claves + i + 1) = ci;
        *(nodo->datos + i + 1) = titular;
        nodo->numClaves++;

        if (nodo->numClaves == grado) {
            int mitad = grado / 2;
            NodoBPlus* nuevo = new NodoBPlus(grado, true);
            nuevo->numClaves = nodo->numClaves - mitad;
            
            for (int j = 0; j < nuevo->numClaves; ++j) {
                *(nuevo->claves + j) = *(nodo->claves + mitad + j);
                *(nuevo->datos + j) = *(nodo->datos + mitad + j);
            }
            
            nodo->numClaves = mitad;
            nuevo->siguiente = nodo->siguiente;
            nodo->siguiente = nuevo;
            nuevaClave = *nuevo->claves;
            nuevoHijo = nuevo;
        } else {
            nuevoHijo = nullptr;
        }
    } else {
        while (i >= 0 && ci < *(nodo->claves + i)) --i;
        ++i;
        
        NodoBPlus* hijoNuevo = nullptr;
        std::string claveNueva;
        insertarEnNodo(*(nodo->hijos + i), ci, titular, hijoNuevo, claveNueva);
        
        if (hijoNuevo) {
            for (int j = nodo->numClaves; j > i; --j) {
                *(nodo->claves + j) = *(nodo->claves + j - 1);
                *(nodo->hijos + j + 1) = *(nodo->hijos + j);
            }
            
            *(nodo->claves + i) = claveNueva;
            *(nodo->hijos + i + 1) = hijoNuevo;
            nodo->numClaves++;
            
            if (nodo->numClaves == grado) {
                int mitad = grado / 2;
                NodoBPlus* nuevo = new NodoBPlus(grado, false);
                nuevo->numClaves = nodo->numClaves - mitad - 1;
                
                for (int j = 0; j < nuevo->numClaves; ++j) {
                    *(nuevo->claves + j) = *(nodo->claves + mitad + 1 + j);
                    *(nuevo->hijos + j) = *(nodo->hijos + mitad + 1 + j);
                }
                
                *(nuevo->hijos + nuevo->numClaves) = *(nodo->hijos + grado);
                nuevaClave = *(nodo->claves + mitad);
                nodo->numClaves = mitad;
                nuevoHijo = nuevo;
            } else {
                nuevoHijo = nullptr;
            }
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
    if (!nodo) return nullptr;

    int i = 0;
    while (i < nodo->numClaves && ci > *(nodo->claves + i)) {
        ++i;
    }

    if (nodo->esHoja) {
        if (i < nodo->numClaves && ci == *(nodo->claves + i)) {
            return *(nodo->datos + i);
        }
        return nullptr;
    }

    return buscarEnNodo(*(nodo->hijos + i), ci);
}

void BPlusTreeTitulares::eliminar(const std::string& ci) {
    if (!raiz) return;

    eliminarEnNodo(raiz, ci, nullptr, -1);

    // Si la raíz queda vacía y tiene un hijo, actualizar la raíz
    if (!raiz->esHoja && raiz->numClaves == 0 && *(raiz->hijos)) {
        NodoBPlus* temp = raiz;
        raiz = *(raiz->hijos);
        delete temp;
    }
}

void BPlusTreeTitulares::eliminarEnNodo(NodoBPlus* nodo, const std::string& ci, 
    NodoBPlus* padre, int indicePadre) {
    
    int i = 0;
    while (i < nodo->numClaves && ci > *(nodo->claves + i)) {
        ++i;
    }

    if (nodo->esHoja) {
        if (i < nodo->numClaves && ci == *(nodo->claves + i)) {
            for (int j = i; j < nodo->numClaves - 1; ++j) {
                *(nodo->claves + j) = *(nodo->claves + j + 1);
                *(nodo->datos + j) = *(nodo->datos + j + 1);
            }
            nodo->numClaves--;
        } else {
            return;
        }

        if (nodo->numClaves < (grado + 1) / 2 - 1 && padre) {
            manejarUnderflow(nodo, padre, indicePadre);
        }
    } else {
        eliminarEnNodo(*(nodo->hijos + i), ci, nodo, i);

        if (i <= nodo->numClaves && (*(nodo->hijos + i))->numClaves < (grado + 1) / 2 - 1) {
            manejarUnderflow(*(nodo->hijos + i), nodo, i);
        }
    }
}

void BPlusTreeTitulares::manejarUnderflow(NodoBPlus* nodo, NodoBPlus* padre, int indice) {
    // Obtener hermanos usando aritmética de punteros
    NodoBPlus* hermanoIzq = (indice > 0) ? *(padre->hijos + indice - 1) : nullptr;
    NodoBPlus* hermanoDer = (indice < padre->numClaves) ? *(padre->hijos + indice + 1) : nullptr;

    // Intentar redistribuir con el hermano izquierdo
    if (hermanoIzq && hermanoIzq->numClaves > (grado + 1) / 2 - 1) {
        if (nodo->esHoja) {
            // Mover la última clave del hermano izquierdo al nodo
            for (int j = nodo->numClaves; j > 0; --j) {
                *(nodo->claves + j) = *(nodo->claves + j - 1);
                *(nodo->datos + j) = *(nodo->datos + j - 1);
            }
            *(nodo->claves) = *(hermanoIzq->claves + hermanoIzq->numClaves - 1);
            *(nodo->datos) = *(hermanoIzq->datos + hermanoIzq->numClaves - 1);
            nodo->numClaves++;
            hermanoIzq->numClaves--;
            // Actualizar clave en el padre
            *(padre->claves + indice - 1) = *(nodo->claves);
        } else {
            // Mover la clave del padre al nodo y la última clave del hermano al padre
            *(nodo->claves + nodo->numClaves) = *(padre->claves + indice - 1);
            *(nodo->hijos + nodo->numClaves + 1) = *(hermanoIzq->hijos + hermanoIzq->numClaves);
            nodo->numClaves++;
            *(padre->claves + indice - 1) = *(hermanoIzq->claves + hermanoIzq->numClaves - 1);
            hermanoIzq->numClaves--;
        }
        return;
    }

    // Intentar redistribuir con el hermano derecho
    if (hermanoDer && hermanoDer->numClaves > (grado + 1) / 2 - 1) {
        if (nodo->esHoja) {
            // Mover la primera clave del hermano derecho al nodo
            *(nodo->claves + nodo->numClaves) = *(hermanoDer->claves);
            *(nodo->datos + nodo->numClaves) = *(hermanoDer->datos);
            nodo->numClaves++;
            for (int j = 0; j < hermanoDer->numClaves - 1; ++j) {
                *(hermanoDer->claves + j) = *(hermanoDer->claves + j + 1);
                *(hermanoDer->datos + j) = *(hermanoDer->datos + j + 1);
            }
            hermanoDer->numClaves--;
            // Actualizar clave en el padre
            *(padre->claves + indice) = *(hermanoDer->claves);
        } else {
            // Mover la clave del padre al nodo y la primera clave del hermano al padre
            *(nodo->claves + nodo->numClaves) = *(padre->claves + indice);
            *(nodo->hijos + nodo->numClaves + 1) = *(hermanoDer->hijos);
            nodo->numClaves++;
            *(padre->claves + indice) = *(hermanoDer->claves);
            for (int j = 0; j < hermanoDer->numClaves - 1; ++j) {
                *(hermanoDer->claves + j) = *(hermanoDer->claves + j + 1);
                *(hermanoDer->hijos + j) = *(hermanoDer->hijos + j + 1);
            }
            *(hermanoDer->hijos + hermanoDer->numClaves - 1) = *(hermanoDer->hijos + hermanoDer->numClaves);
            hermanoDer->numClaves--;
        }
        return;
    }

    // Fusionar con el hermano izquierdo o derecho
    if (hermanoIzq) {
        if (nodo->esHoja) {
            // Fusionar nodo con hermano izquierdo
            for (int j = 0; j < nodo->numClaves; ++j) {
                *(hermanoIzq->claves + hermanoIzq->numClaves + j) = *(nodo->claves + j);
                *(hermanoIzq->datos + hermanoIzq->numClaves + j) = *(nodo->datos + j);
            }
            hermanoIzq->numClaves += nodo->numClaves;
            hermanoIzq->siguiente = nodo->siguiente;
            // Eliminar la clave del padre
            for (int j = indice - 1; j < padre->numClaves - 1; ++j) {
                *(padre->claves + j) = *(padre->claves + j + 1);
                *(padre->hijos + j + 1) = *(padre->hijos + j + 2);
            }
            padre->numClaves--;
            delete nodo;
        } else {
            // Fusionar nodo con hermano izquierdo
            *(hermanoIzq->claves + hermanoIzq->numClaves) = *(padre->claves + indice - 1);
            hermanoIzq->numClaves++;
            for (int j = 0; j < nodo->numClaves; ++j) {
                *(hermanoIzq->claves + hermanoIzq->numClaves + j) = *(nodo->claves + j);
                *(hermanoIzq->hijos + hermanoIzq->numClaves + j) = *(nodo->hijos + j);
            }
            *(hermanoIzq->hijos + hermanoIzq->numClaves + nodo->numClaves) = *(nodo->hijos + nodo->numClaves);
            hermanoIzq->numClaves += nodo->numClaves;
            for (int j = indice - 1; j < padre->numClaves - 1; ++j) {
                *(padre->claves + j) = *(padre->claves + j + 1);
                *(padre->hijos + j + 1) = *(padre->hijos + j + 2);
            }
            padre->numClaves--;
            delete nodo;
        }
    } else if (hermanoDer) {
        if (nodo->esHoja) {
            // Fusionar nodo con hermano derecho
            for (int j = 0; j < hermanoDer->numClaves; ++j) {
                *(nodo->claves + nodo->numClaves + j) = *(hermanoDer->claves + j);
                *(nodo->datos + nodo->numClaves + j) = *(hermanoDer->datos + j);
            }
            nodo->numClaves += hermanoDer->numClaves;
            nodo->siguiente = hermanoDer->siguiente;
            for (int j = indice; j < padre->numClaves - 1; ++j) {
                *(padre->claves + j) = *(padre->claves + j + 1);
                *(padre->hijos + j + 1) = *(padre->hijos + j + 2);
            }
            padre->numClaves--;
            delete hermanoDer;
        } else {
            // Fusionar nodo con hermano derecho
            *(nodo->claves + nodo->numClaves) = *(padre->claves + indice);
            nodo->numClaves++;
            for (int j = 0; j < hermanoDer->numClaves; ++j) {
                *(nodo->claves + nodo->numClaves + j) = *(hermanoDer->claves + j);
                *(nodo->hijos + nodo->numClaves + j) = *(hermanoDer->hijos + j);
            }
            *(nodo->hijos + nodo->numClaves + hermanoDer->numClaves) = *(hermanoDer->hijos + hermanoDer->numClaves);
            nodo->numClaves += hermanoDer->numClaves;
            for (int j = indice; j < padre->numClaves - 1; ++j) {
                *(padre->claves + j) = *(padre->claves + j + 1);
                *(padre->hijos + j + 1) = *(padre->hijos + j + 2);
            }
            padre->numClaves--;
            delete hermanoDer;
        }
    }
}

void BPlusTreeTitulares::imprimir() const {
    imprimirNodo(raiz, 0, true);
}

void BPlusTreeTitulares::imprimirNodo(NodoBPlus* nodo, int nivel, bool esRaiz) const {
    if (!nodo) return;
    
    if (esRaiz) {
        std::cout << "Raiz: ";
    } else {
        std::cout << std::string(nivel * 2, ' ') 
                 << (nodo->esHoja ? "[Hoja] " : "[Interno] ");
    }
    
    for (int i = 0; i < nodo->numClaves; ++i) {
        std::cout << *(nodo->claves + i) << " ";
    }
    std::cout << std::endl;
    
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; ++i) {
            imprimirNodo(*(nodo->hijos + i), nivel + 1);
        }
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

    std::string indent(nivel * 4, ' ');
    std::string connector = (nivel > 0) ? "  |--" : "";

    std::cout << indent << "Nivel " << nivel << ": " << connector << "[";

    // Mostrar las claves usando aritmética de punteros
    for (int i = 0; i < nodo->numClaves; ++i) {
        std::cout << *(nodo->claves + i);
        if (i < nodo->numClaves - 1) std::cout << ", ";
    }
    std::cout << "]" << (nodo->esHoja ? " (Hoja)" : " (Interno)") << std::endl;

    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; ++i) {
            if (i < nodo->numClaves) {
                std::cout << indent << "  |" << std::endl;
            }
            imprimirNodoArbolBPlus(*(nodo->hijos + i), nivel + 1);
        }
    }
}

void BPlusTreeTitulares::graficarArbol() const {
    // Crear ventana SFML
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(1200, 800), "B+ Tree Visualization");
    window->setFramerateLimit(60);

    // Configuración de la fuente
    sf::Font* font = new sf::Font();
    if (!font->loadFromFile("arial.ttf")) {
        std::cerr << "Error: No se pudo cargar la fuente arial.ttf" << std::endl;
        delete window;
        delete font;
        return;
    }

    // Validar si el árbol está vacío
    if (!raiz) {
        sf::Text* mensaje = new sf::Text("Arbol B+ vacio", *font, 24);
        mensaje->setFillColor(sf::Color::Black);
        sf::FloatRect bounds = mensaje->getLocalBounds();
        mensaje->setPosition(window->getSize().x / 2.0f - bounds.width / 2.0f,
                           window->getSize().y / 2.0f - bounds.height / 2.0f);

        window->clear(sf::Color::White);
        window->draw(*mensaje);
        window->display();

        sf::Event evt;
        while (window->waitEvent(evt)) {
            if (evt.type == sf::Event::Closed) {
                window->close();
                break;
            }
        }

        delete mensaje;
        delete font;
        delete window;
        std::cout << "\nVentana de visualizacion cerrada. Regresando al menu...\n" << std::endl;
        return;
    }

    // Parámetros de visualización
    const float baseNodeWidth = 160.0f;
    const float nodeHeight = 60.0f;
    const float verticalSpacing = 120.0f;
    const float horizontalSpacing = 30.0f;
    const float textOffsetY = 15.0f;

    // Estructura para información de nodos
    struct NodeInfo {
        NodoBPlus* nodo;
        float x, y;
        float nodeWidth;
        int nivel;
        NodeInfo* siguiente;
        NodeInfo(NodoBPlus* n, float x_, float y_, float w, int lvl)
            : nodo(n), x(x_), y(y_), nodeWidth(w), nivel(lvl), siguiente(nullptr) {}
    };

    // Estructura para niveles del árbol
    struct NivelArbol {
        NodeInfo* primero;
        NivelArbol* siguiente;
        NivelArbol() : primero(nullptr), siguiente(nullptr) {}
        ~NivelArbol() {
            NodeInfo* actual = primero;
            while (actual) {
                NodeInfo* temp = actual;
                actual = actual->siguiente;
                delete temp;
            }
        }
    };

    // Estructura para cola dinámica
    struct Cola {
        struct NodoCola {
            NodeInfo* info;
            NodoCola* siguiente;
            NodoCola(NodeInfo* i) : info(i), siguiente(nullptr) {}
        };
        NodoCola* frente;
        NodoCola* final;
        Cola() : frente(nullptr), final(nullptr) {}
        void push(NodeInfo* info) {
            NodoCola* nuevo = new NodoCola(info);
            if (!frente) frente = final = nuevo;
            else {
                final->siguiente = nuevo;
                final = nuevo;
            }
        }
        NodeInfo* pop() {
            if (!frente) return nullptr;
            NodoCola* temp = frente;
            NodeInfo* info = temp->info;
            frente = frente->siguiente;
            if (!frente) final = nullptr;
            delete temp;
            return info;
        }
        bool empty() const { return frente == nullptr; }
        ~Cola() {
            while (frente) {
                NodoCola* temp = frente;
                frente = frente->siguiente;
                delete temp;
            }
        }
    };

    Cola* cola = new Cola();
    NivelArbol* nivelesInicio = nullptr;
    NivelArbol* nivelesFin = nullptr;

    // Procesar raíz
    if (raiz) {
        std::string* rootText = new std::string();
        for (int i = 0; i < raiz->numClaves; ++i) {
            *rootText += *(raiz->claves + i);
            if (i < raiz->numClaves - 1) *rootText += ", ";
        }
        sf::Text* tempText = new sf::Text(*rootText, *font, 16);
        float rootWidth = tempText->getLocalBounds().width + 20.0f;
        float startX = window->getSize().x / 2.0f;
        cola->push(new NodeInfo(raiz, startX, 50.0f, (rootWidth > baseNodeWidth ? rootWidth : baseNodeWidth), 0));
        delete rootText;
        delete tempText;
    }

    // Calcular posiciones de los nodos
    while (!cola->empty()) {
        NodeInfo* current = cola->pop();
        if (!nivelesInicio || nivelesFin->primero->nivel < current->nivel) {
            NivelArbol* nuevoNivel = new NivelArbol();
            if (!nivelesInicio) nivelesInicio = nuevoNivel;
            else nivelesFin->siguiente = nuevoNivel;
            nivelesFin = nuevoNivel;
        }
        NodeInfo* temp = nivelesFin->primero;
        if (temp) {
            while (temp->siguiente) temp = temp->siguiente;
            temp->siguiente = current;
        } else {
            nivelesFin->primero = current;
        }

        if (!current->nodo->esHoja) {
            float totalWidth = (current->nodo->numClaves + 1) * baseNodeWidth + current->nodo->numClaves * horizontalSpacing;
            float startX = current->x - totalWidth / 2.0f + baseNodeWidth / 2.0f;
            for (int i = 0; i <= current->nodo->numClaves; ++i) {
                NodoBPlus* hijo = *(current->nodo->hijos + i);
                if (hijo) {
                    std::string* childText = new std::string();
                    for (int j = 0; j < hijo->numClaves; ++j) {
                        *childText += *(hijo->claves + j);
                        if (j < hijo->numClaves - 1) *childText += ", ";
                    }
                    sf::Text* tempText = new sf::Text(*childText, *font, 16);
                    float childWidth = tempText->getLocalBounds().width + 20.0f;
                    float childX = startX + i * (baseNodeWidth + horizontalSpacing);
                    cola->push(new NodeInfo(hijo, childX, current->y + verticalSpacing,
                                          (childWidth > baseNodeWidth ? childWidth : baseNodeWidth), current->nivel + 1));
                    delete childText;
                    delete tempText;
                }
            }
        }
    }

    // Ajustar posiciones para centrar niveles
    NivelArbol* nivel = nivelesInicio;
    while (nivel) {
        NodeInfo* nodo = nivel->primero;
        float totalWidth = 0.0f;
        int count = 0;
        while (nodo) {
            totalWidth += nodo->nodeWidth;
            count++;
            nodo = nodo->siguiente;
        }
        totalWidth += (count - 1) * horizontalSpacing;
        float levelStartX = (window->getSize().x - totalWidth) / 2.0f;
        float currentX = levelStartX;
        nodo = nivel->primero;
        while (nodo) {
            nodo->x = currentX + nodo->nodeWidth / 2.0f;
            currentX += nodo->nodeWidth + horizontalSpacing;
            nodo = nodo->siguiente;
        }
        nivel = nivel->siguiente;
    }

    // Bucle principal
    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window->close();
            }
        }

        window->clear(sf::Color::White);

        // Dibujar conexiones entre nodos
        nivel = nivelesInicio;
        while (nivel) {
            NodeInfo* nodo = nivel->primero;
            while (nodo) {
                if (!nodo->nodo->esHoja) {
                    float parentX = nodo->x;
                    float parentY = nodo->y + nodeHeight;
                    for (int i = 0; i <= nodo->nodo->numClaves; ++i) {
                        NodoBPlus* hijo = *(nodo->nodo->hijos + i);
                        if (hijo) {
                            NivelArbol* tmpNivel = nivel->siguiente;
                            while (tmpNivel) {
                                NodeInfo* tmpNodo = tmpNivel->primero;
                                while (tmpNodo) {
                                    if (tmpNodo->nodo == hijo) {
                                        sf::Vertex* line = new sf::Vertex[2];
                                        line[0] = sf::Vertex(sf::Vector2f(parentX, parentY), sf::Color::Black);
                                        line[1] = sf::Vertex(sf::Vector2f(tmpNodo->x, tmpNodo->y), sf::Color::Black);
                                        window->draw(line, 2, sf::Lines);
                                        delete[] line;
                                    }
                                    tmpNodo = tmpNodo->siguiente;
                                }
                                tmpNivel = tmpNivel->siguiente;
                            }
                        }
                    }
                }
                nodo = nodo->siguiente;
            }
            nivel = nivel->siguiente;
        }

        // Dibujar nodos
        nivel = nivelesInicio;
        while (nivel) {
            NodeInfo* nodo = nivel->primero;
            while (nodo) {
                float x = nodo->x - nodo->nodeWidth / 2.0f;
                float y = nodo->y;

                // Dibujar rectángulo
                sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(nodo->nodeWidth, nodeHeight));
                rect->setPosition(x, y);
                rect->setFillColor(nodo->nodo->esHoja ? sf::Color(144, 238, 144) : sf::Color(173, 216, 230));
                rect->setOutlineColor(sf::Color::Black);
                rect->setOutlineThickness(2);
                window->draw(*rect);
                delete rect;

                // Dibujar texto
                std::string* texto = new std::string();
                for (int i = 0; i < nodo->nodo->numClaves; ++i) {
                    *texto += *(nodo->nodo->claves + i);
                    if (i < nodo->nodo->numClaves - 1) *texto += ", ";
                }
                sf::Text* textoNodo = new sf::Text(*texto, *font, 16);
                textoNodo->setFillColor(sf::Color::Black);
                float textX = x + (nodo->nodeWidth - textoNodo->getLocalBounds().width) / 2.0f;
                textoNodo->setPosition(textX, y + textOffsetY);
                window->draw(*textoNodo);
                delete texto;
                delete textoNodo;

                // Dibujar flecha a siguiente hoja
                if (nodo->nodo->esHoja && nodo->nodo->siguiente) {
                    NodeInfo* nextNode = nullptr;
                    NivelArbol* tmpNivel = nivel;
                    while (tmpNivel) {
                        NodeInfo* tmpNodo = tmpNivel->primero;
                        while (tmpNodo) {
                            if (tmpNodo->nodo == nodo->nodo->siguiente) {
                                nextNode = tmpNodo;
                                break;
                            }
                            tmpNodo = tmpNodo->siguiente;
                        }
                        if (nextNode) break;
                        tmpNivel = tmpNivel->siguiente;
                    }
                    if (nextNode) {
                        sf::Vector2f inicio(x + nodo->nodeWidth, y + nodeHeight / 2);
                        sf::Vector2f fin(nextNode->x - nextNode->nodeWidth / 2, y + nodeHeight / 2);

                        // Dibujar línea
                        sf::Vertex* line = new sf::Vertex[2];
                        line[0] = sf::Vertex(inicio, sf::Color::Red);
                        line[1] = sf::Vertex(fin, sf::Color::Red);
                        window->draw(line, 2, sf::Lines);
                        delete[] line;

                        // Calcular dirección y punto medio para la flecha
                        sf::Vector2f direction(fin.x - inicio.x, fin.y - inicio.y);
                        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                        if (length > 0) {
                            sf::Vector2f unitDirection(direction.x / length, direction.y / length);
                            sf::Vector2f medio((inicio.x + fin.x) / 2.0f, (inicio.y + fin.y) / 2.0f);

                            // Ajustar tamaño de la flecha según la distancia
                            float arrowSize = std::min(20.0f, length / 4.0f); // Tamaño proporcional
                            sf::ConvexShape* arrow = new sf::ConvexShape();
                            arrow->setPointCount(3);
                            arrow->setPoint(0, sf::Vector2f(0, 0));
                            arrow->setPoint(1, sf::Vector2f(-arrowSize, -arrowSize / 2));
                            arrow->setPoint(2, sf::Vector2f(-arrowSize, arrowSize / 2));
                            arrow->setFillColor(sf::Color::Red);
                            arrow->setPosition(medio.x - arrowSize / 2 * unitDirection.x, medio.y - arrowSize / 2 * unitDirection.y);
                            float angle = std::atan2(unitDirection.y, unitDirection.x) * 180.0f / 3.14159f;
                            arrow->setRotation(angle);
                            window->draw(*arrow);
                            delete arrow;
                        }
                    }
                }
                nodo = nodo->siguiente;
            }
            nivel = nivel->siguiente;
        }

        window->display();
    }

    // Liberar memoria
    delete cola;
    while (nivelesInicio) {
        NivelArbol* tempNivel = nivelesInicio;
        nivelesInicio = nivelesInicio->siguiente;
        delete tempNivel;
    }
    delete font;
    delete window;

    std::cout << "\nVentana de visualizacion cerrada. Regresando al menu...\n" << std::endl;
}

/**
 * @brief Configura el gestor de busqueda MongoDB
 * @param gestorMongo Puntero al gestor de busqueda MongoDB
 */
void BPlusTreeTitulares::setGestorBusquedaMongo(GestorBusquedaMongo* gestorMongo) {
    gestorBusquedaMongo = gestorMongo;
}

/**
 * @brief Busca un titular en el arbol B+ con datos actualizados desde MongoDB
 * @param ci Cedula de identidad del titular a buscar
 * @return Titular* Puntero al titular encontrado o nullptr
 */
Titular* BPlusTreeTitulares::buscarDB(const std::string& ci) const {
    // Primero intentar obtener datos frescos desde MongoDB
    if (gestorBusquedaMongo) {
        std::cout << "Buscando titular en MongoDB..." << std::endl;
        Titular* titularFresco = gestorBusquedaMongo->obtenerTitularFresco(ci);
        
        if (titularFresco) {
            std::cout << "Titular encontrado con datos actualizados desde MongoDB." << std::endl;
            return titularFresco;
        } else {
            std::cout << "Titular no encontrado en MongoDB. Buscando en arbol local..." << std::endl;
        }
    }
    
    // Fallback: buscar en el arbol local
    Titular* titularLocal = buscar(ci);
    if (titularLocal) {
        std::cout << "Titular encontrado en arbol local." << std::endl;
    } else {
        std::cout << "Titular no encontrado." << std::endl;
    }
    
    return titularLocal;
}

/**
 * @brief Elimina un titular del arbol B+ con verificacion MongoDB
 * @param ci Cedula de identidad del titular a eliminar
 */
void BPlusTreeTitulares::eliminarDB(const std::string& ci) {
    // Verificar si el titular existe en MongoDB primero
    if (gestorBusquedaMongo) {
        std::cout << "Verificando existencia del titular en MongoDB..." << std::endl;
        Titular* titularVerificacion = gestorBusquedaMongo->obtenerTitularFresco(ci);
        
        if (titularVerificacion) {
            std::cout << "Titular encontrado en MongoDB. Procediendo con eliminacion local..." << std::endl;
            // Eliminar del arbol local
            eliminar(ci);
            std::cout << "Titular eliminado del arbol local." << std::endl;
            std::cout << "NOTA: Para eliminar completamente, use las funciones de gestion de base de datos." << std::endl;
            return;
        } else {
            std::cout << "Titular no encontrado en MongoDB." << std::endl;
        }
    }
    
    // Verificar en arbol local como fallback
    Titular* titularLocal = buscar(ci);
    if (titularLocal) {
        std::cout << "Titular encontrado en arbol local. Eliminando..." << std::endl;
        eliminar(ci);
        std::cout << "Titular eliminado del arbol local." << std::endl;
    } else {
        std::cout << "Titular no encontrado en el sistema." << std::endl;
    }
}

/**
 * @brief Grafica el arbol B+ construido con todos los titulares desde MongoDB
 */
void BPlusTreeTitulares::graficarArbolDB() const {
    if (gestorBusquedaMongo) {
        std::cout << "Obteniendo todos los titulares desde MongoDB para graficar el arbol..." << std::endl;
        std::vector<Titular*> titularesCompletos = gestorBusquedaMongo->obtenerTodosTitularesCompletos();
        
        if (!titularesCompletos.empty()) {
            std::cout << "Construyendo arbol B+ temporal con " << titularesCompletos.size() << " titulares desde MongoDB..." << std::endl;
            
            // Crear un arbol temporal con los datos de MongoDB
            BPlusTreeTitulares arbolTemporal(grado);
            
            // Insertar todos los titulares de MongoDB en el arbol temporal
            for (Titular* titular : titularesCompletos) {
                arbolTemporal.insertar(titular->getPersona().getCI(), titular);
            }
            
            std::cout << "Graficando arbol B+ con datos actualizados desde MongoDB..." << std::endl;
            arbolTemporal.graficarArbol();
            
            std::cout << "Visualizacion completada." << std::endl;
            return;
        } else {
            std::cout << "No se encontraron titulares en MongoDB. Graficando arbol local..." << std::endl;
        }
    } else {
        std::cout << "No hay conexion a MongoDB. Graficando arbol local..." << std::endl;
    }
    
    // Fallback: graficar el arbol local
    graficarArbol();
}