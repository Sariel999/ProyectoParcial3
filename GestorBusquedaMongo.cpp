/**
 * @file GestorBusquedaMongo.cpp
 * @author your name (you@domain.com)
 * @brief Implementacion de la clase GestorBusquedaMongo
 * @version 0.1
 * @date 2025-08-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "GestorBusquedaMongo.h"
#include "NodoDoble.h"
#include <iostream>
#include <vector>

using namespace std;

GestorBusquedaMongo::GestorBusquedaMongo(GestorConexion& gestor) : gestorConexion(gestor) {
}

GestorBusquedaMongo::~GestorBusquedaMongo() {
}

Titular* GestorBusquedaMongo::obtenerTitularFresco(const std::string& ci) {
    // Siempre obtener datos frescos directamente desde MongoDB
    if (!gestorConexion.estaConectado()) {
        cout << "No hay conexion a MongoDB." << endl;
        return nullptr;
    }
    
    cout << "Obteniendo datos frescos desde MongoDB..." << endl;
    
    // Buscar titular en MongoDB
    Titular* titularFresco = gestorConexion.buscarTitularEnBD(ci);
    
    if (titularFresco) {
        cout << "Titular encontrado con datos actualizados." << endl;
        return titularFresco;
    } else {
        cout << "Titular no encontrado en base de datos." << endl;
        return nullptr;
    }
}

Titular* GestorBusquedaMongo::buscarTitularConCarga(ListaDobleCircular<Titular*>& titulares, const std::string& ci) {
    // Primero buscar en memoria local
    Titular* titular = buscarTitularLocal(titulares, ci);
    
    if (titular) {
        // cout << "DEBUG: Titular encontrado en lista local" << endl;
        return titular;
    }
    
    // Si no se encuentra localmente, buscar en MongoDB
    cout << "\nTitular no encontrado localmente." << endl;
    
    if (gestorConexion.estaConectado()) {
        cout << "Buscando en base de datos..." << endl;
        
        // Buscar titular en MongoDB
        Titular* titularDesdeDB = gestorConexion.buscarTitularEnBD(ci);
        
        if (titularDesdeDB) {
            cout << "Titular encontrado en base de datos. Cargando..." << endl;
            
            // Reemplazar o agregar el titular en la lista local
            if (reemplazarOAgregarTitular(titulares, titularDesdeDB, ci)) {
                return titularDesdeDB;
            } else {
                // Si fallo el reemplazo/agregado, limpiar memoria
                delete titularDesdeDB;
                return nullptr;
            }
        } else {
            cout << "\nTitular no encontrado en base de datos." << endl;
            return nullptr;
        }
    } else {
        cout << "\nNo hay conexion a MongoDB." << endl;
        return nullptr;
    }
}

Titular* GestorBusquedaMongo::buscarTitularLocal(const ListaDobleCircular<Titular*>& titulares, const std::string& ci) {
    if (titulares.vacia()) {
        // cout << "DEBUG: Lista de titulares está vacía" << endl;
        return nullptr;
    }
    
    // cout << "DEBUG: Buscando titular con CI: " << ci << endl;
    
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual == nullptr) {
        // cout << "DEBUG: getCabeza() retornó nullptr" << endl;
        return nullptr;
    }
    
    for (Titular* titular : titulares) {
        if (titular->getPersona().getCI() == ci) {
            return titular;
        }
    }

    // cout << "DEBUG: Titular no encontrado en lista local" << endl;
    return nullptr;
}

bool GestorBusquedaMongo::reemplazarOAgregarTitular(ListaDobleCircular<Titular*>& titulares, Titular* titular, const std::string& ci) {
    if (!titular) return false;
    
    // Buscar si ya existe un titular con la misma cedula para reemplazarlo
    NodoDoble<Titular*>* nodo = titulares.getCabeza();
    bool encontrado = false;
    
    if (nodo) {
        do {
            if (nodo->dato->getPersona().getCI() == ci) {
                // Eliminar el titular anterior y reemplazar
                // cout << "DEBUG: Reemplazando titular existente con CI: " << ci << endl;
                delete nodo->dato;
                nodo->dato = titular;
                encontrado = true;
                break;
            }
            nodo = nodo->siguiente;
        } while (nodo != titulares.getCabeza());
    }
    
    if (!encontrado) {
        // Si no existía, agregarlo a la lista
        // cout << "DEBUG: Agregando titular nuevo desde DB con CI: " << ci << endl;
        titulares.insertar(titular);
    }
    
    return true;
}

std::vector<Titular*> GestorBusquedaMongo::obtenerTodosTitularesCompletos() {
    std::vector<Titular*> titularesCompletos;
    
    if (!gestorConexion.estaConectado()) {
        cout << "No hay conexion a MongoDB para obtener titulares completos." << endl;
        return titularesCompletos; // Vector vacío
    }
    
    cout << "Obteniendo todos los titulares completos desde MongoDB..." << endl;
    
    // Obtener todos los titulares desde MongoDB usando el gestor de conexión
    titularesCompletos = gestorConexion.obtenerTodosTitulares();
    
    if (titularesCompletos.empty()) {
        cout << "No se encontraron titulares en la base de datos." << endl;
    } else {
        cout << "Se obtuvieron " << titularesCompletos.size() << " titulares desde MongoDB." << endl;
    }
    
    return titularesCompletos;
}
