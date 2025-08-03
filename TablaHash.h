/**
 * @file TablaHash.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef TABLAHASH_H
#define TABLAHASH_H

#include <string>
#include <list>
#include <vector>

class TablaHash {
private:
    static const int TAMANO = 10; // Tamaño de la tabla (ajustable)
    std::vector<std::list<std::pair<std::string, std::string>>> tabla;

    // Función hash simple
    unsigned int hash(const std::string& clave) {
        unsigned int valor = 0;
        for (char c : clave) {
            valor += (unsigned char)c;
        }
        return valor % TAMANO;
    }

public:
    TablaHash() : tabla(TAMANO) {}

    void insertar(const std::string& clave, const std::string& valor) {
        unsigned int indice = hash(clave);
        for (auto& par : tabla[indice]) {
            if (par.first == clave) {
                par.second = valor; // Actualizar si la clave ya existe
                return;
            }
        }
        tabla[indice].emplace_back(clave, valor);
    }

    bool buscar(const std::string& clave, std::string& valor) {
        unsigned int indice = hash(clave);
        for (const auto& par : tabla[indice]) {
            if (par.first == clave) {
                valor = par.second;
                return true;
            }
        }
        return false;
    }

    void eliminar(const std::string& clave) {
        unsigned int indice = hash(clave);
        tabla[indice].remove_if([&clave](const auto& par) { return par.first == clave; });
    }

    // Función para mostrar el contenido de la tabla hash
    void mostrarContenido() const;
};

#endif