#include "TablaHash.h"
#include <iostream>

using namespace std;

/**
 * @brief Constructor por defecto - inicializa la tabla con memoria dinamica
 */
TablaHash::TablaHash() {
    tabla = new NodoHash*[TAMANO];
    // Inicializar usando aritmetica de punteros
    NodoHash** ptr = tabla;
    for (int i = 0; i < TAMANO; i++) {
        *ptr = nullptr;
        ptr++;
    }
}

/**
 * @brief Destructor - libera toda la memoria dinamica
 */
TablaHash::~TablaHash() {
    NodoHash** ptr = tabla;
    for (int i = 0; i < TAMANO; i++) {
        liberarLista(*ptr);
        ptr++;
    }
    delete[] tabla;
}

/**
 * @brief Constructor de copia
 */
TablaHash::TablaHash(const TablaHash& otra) {
    tabla = new NodoHash*[TAMANO];
    // Inicializar usando aritmetica de punteros
    NodoHash** ptr = tabla;
    for (int i = 0; i < TAMANO; i++) {
        *ptr = nullptr;
        ptr++;
    }
    
    // Copiar todos los elementos de la otra tabla
    NodoHash** otraPtr = otra.tabla;
    for (int i = 0; i < TAMANO; i++) {
        NodoHash* actual = *otraPtr;
        while (actual != nullptr) {
            insertar(actual->clave, actual->valor);
            actual = actual->siguiente;
        }
        otraPtr++;
    }
}

/**
 * @brief Operador de asignacion
 */
TablaHash& TablaHash::operator=(const TablaHash& otra) {
    if (this != &otra) {
        // Liberar memoria existente usando aritmetica de punteros
        NodoHash** ptr = tabla;
        for (int i = 0; i < TAMANO; i++) {
            liberarLista(*ptr);
            *ptr = nullptr;
            ptr++;
        }
        
        // Copiar elementos de la otra tabla
        NodoHash** otraPtr = otra.tabla;
        for (int i = 0; i < TAMANO; i++) {
            NodoHash* actual = *otraPtr;
            while (actual != nullptr) {
                insertar(actual->clave, actual->valor);
                actual = actual->siguiente;
            }
            otraPtr++;
        }
    }
    return *this;
}

/**
 * @brief Libera la memoria de una lista enlazada
 */
void TablaHash::liberarLista(NodoHash* cabeza) {
    while (cabeza != nullptr) {
        NodoHash* temp = cabeza;
        cabeza = cabeza->siguiente;
        delete temp;
    }
}

/**
 * @brief Inserta un par clave-valor en la tabla hash
 */
void TablaHash::insertar(const std::string& clave, const std::string& valor) {
    unsigned int indice = hash(clave);
    NodoHash** posicion = obtenerPosicion(indice);
    
    // Buscar si la clave ya existe
    NodoHash* actual = *posicion;
    while (actual != nullptr) {
        if (actual->clave == clave) {
            actual->valor = valor; // Actualizar valor existente
            return;
        }
        actual = actual->siguiente;
    }
    
    // Crear nuevo nodo e insertarlo al inicio de la lista
    NodoHash* nuevoNodo = new NodoHash(clave, valor);
    nuevoNodo->siguiente = *posicion;
    *posicion = nuevoNodo;
}

/**
 * @brief Busca un valor por su clave
 */
bool TablaHash::buscar(const std::string& clave, std::string& valor) const {
    unsigned int indice = hash(clave);
    NodoHash** posicion = obtenerPosicion(indice);
    
    NodoHash* actual = *posicion;
    while (actual != nullptr) {
        if (actual->clave == clave) {
            valor = actual->valor;
            return true;
        }
        actual = actual->siguiente;
    }
    return false;
}

/**
 * @brief Elimina un elemento por su clave
 */
void TablaHash::eliminar(const std::string& clave) {
    unsigned int indice = hash(clave);
    NodoHash** posicion = obtenerPosicion(indice);
    
    NodoHash* actual = *posicion;
    NodoHash* anterior = nullptr;
    
    while (actual != nullptr) {
        if (actual->clave == clave) {
            if (anterior == nullptr) {
                // Eliminar el primer nodo
                *posicion = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            delete actual;
            return;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
}

/**
 * @brief Muestra el contenido completo de la tabla hash
 */
void TablaHash::mostrarContenido() const {
    cout << "Contenido de la Tabla Hash:" << endl;
    NodoHash** ptr = tabla;
    for (int i = 0; i < TAMANO; i++) {
        cout << "Posicion " << i << ": ";
        NodoHash* actual = *ptr;
        if (actual == nullptr) {
            cout << "vacia" << endl;
        } else {
            while (actual != nullptr) {
                cout << "[" << actual->clave << ":" << actual->valor << "]";
                if (actual->siguiente != nullptr) {
                    cout << " -> ";
                }
                actual = actual->siguiente;
            }
            cout << endl;
        }
        ptr++;
    }
}

/**
 * @brief Verifica si la tabla esta vacia
 */
bool TablaHash::estaVacia() const {
    NodoHash** ptr = tabla;
    for (int i = 0; i < TAMANO; i++) {
        if (*ptr != nullptr) {
            return false;
        }
        ptr++;
    }
    return true;
}

/**
 * @brief Cuenta el numero total de elementos en la tabla
 */
int TablaHash::contarElementos() const {
    int contador = 0;
    NodoHash** ptr = tabla;
    for (int i = 0; i < TAMANO; i++) {
        NodoHash* actual = *ptr;
        while (actual != nullptr) {
            contador++;
            actual = actual->siguiente;
        }
        ptr++;
    }
    return contador;
}
