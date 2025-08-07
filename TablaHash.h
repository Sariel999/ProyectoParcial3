/**
 * @file TablaHash.h
 * @author your name (you@domain.com)
 * @brief Tabla Hash implementada con memoria dinamica pura
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef TABLAHASH_H
#define TABLAHASH_H

#include <string>

/**
 * @brief Nodo para almacenar pares clave-valor en la tabla hash
 */
struct NodoHash {
    std::string clave;
    std::string valor;
    NodoHash* siguiente;
    
    NodoHash(const std::string& c, const std::string& v) 
        : clave(c), valor(v), siguiente(nullptr) {}
};

class TablaHash {
private:
    static const int TAMANO = 10; // Tamano de la tabla
    NodoHash** tabla; // Puntero a punteros para memoria dinamica
    
    // Funcion hash simple
    unsigned int hash(const std::string& clave) const {
        unsigned int valor = 0;
        const char* ptr = clave.c_str();
        while (*ptr) {
            valor += (unsigned char)*ptr;
            ptr++;
        }
        return valor % TAMANO;
    }
    
    // Liberar memoria de una lista enlazada
    void liberarLista(NodoHash* cabeza);
    
    // Obtener puntero a posicion especifica usando aritmetica de punteros
    NodoHash** obtenerPosicion(int indice) const {
        return tabla + indice;
    }

public:
    TablaHash();
    ~TablaHash();
    
    // Constructor de copia y operador de asignacion (Regla de los 3)
    TablaHash(const TablaHash& otra);
    TablaHash& operator=(const TablaHash& otra);
    
    void insertar(const std::string& clave, const std::string& valor);
    bool buscar(const std::string& clave, std::string& valor) const;
    void eliminar(const std::string& clave);
    void mostrarContenido() const;
    bool estaVacia() const;
    int contarElementos() const;
};

#endif