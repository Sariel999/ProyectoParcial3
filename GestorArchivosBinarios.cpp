/**
 * @file GestorArchivosBinarios.cpp
 * @author your name (you@domain.com)
 * @brief Implementacion de la clase GestorArchivosBinarios para manejo de archivos binarios
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "GestorArchivosBinarios.h"
#include "GestorBusquedaMongo.h"
#include "ArchivoBinario.h"
#include "CifradoCesar.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

using namespace std;

GestorArchivosBinarios::GestorArchivosBinarios() : gestorBusquedaMongo(nullptr) {
}

GestorArchivosBinarios::~GestorArchivosBinarios() {
}

void GestorArchivosBinarios::setGestorBusquedaMongo(GestorBusquedaMongo* gestorMongo) {
    gestorBusquedaMongo = gestorMongo;
}

/**
 * @brief Obtiene datos actualizados desde MongoDB o usa datos locales como fallback
 * 
 * @param titulares Lista de titulares local como fallback
 * @return std::vector<Titular*> Vector con datos actualizados
 */
std::vector<Titular*> GestorArchivosBinarios::obtenerDatosActualizados(const ListaDobleCircular<Titular*>& titulares) {
    std::vector<Titular*> datosActualizados;
    
    // Intentar obtener datos desde MongoDB
    if (gestorBusquedaMongo) {
        cout << "Obteniendo datos completos desde MongoDB..." << endl;
        datosActualizados = gestorBusquedaMongo->obtenerTodosTitularesCompletos();
        
        if (!datosActualizados.empty()) {
            cout << "Se obtuvieron " << datosActualizados.size() << " titulares desde MongoDB." << endl;
            return datosActualizados;
        } else {
            cout << "No se pudieron obtener datos desde MongoDB. Usando datos locales." << endl;
        }
    }
    
    // Fallback: convertir lista local a vector
    if (!titulares.vacia()) {
        NodoDoble<Titular*>* actual = titulares.getCabeza();
        if (actual) {
            do {
                datosActualizados.push_back(actual->dato);
                actual = actual->siguiente;
            } while (actual != titulares.getCabeza());
        }
    }
    
    return datosActualizados;
}

/**
 * @brief Guarda los titulares en un archivo binario cifrado usando cifrado Cesar
 * Utiliza datos actualizados desde MongoDB si hay conexion disponible.
 * 
 * @param titulares Lista de titulares del sistema (usado como fallback)
 */
void GestorArchivosBinarios::guardarArchivoBinCifrado(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- GUARDAR CUENTAS EN ARCHIVO BINARIO CIFRADO ---" << endl;
    
    // Obtener datos actualizados desde MongoDB o locales
    std::vector<Titular*> datosActualizados = obtenerDatosActualizados(titulares);
    
    if (datosActualizados.empty()) {
        cout << "\nNo hay titulares registrados para guardar.\n" << endl;
        system("pause");
        return;
    }
    
    // Usar la version que trabaja con vector
    ArchivoBinario::guardarDesdeVector(datosActualizados, "cuentas_temp.bin");

    int desplazamiento = 3; // Puedes elegir el desplazamiento que desees
    
    cifrarCesarArchivoBinario(std::string("cuentas_temp.bin"), std::string("cuentasCifrado.bin"), desplazamiento);

    remove("cuentas_temp.bin");

    cout << "\nCuentas guardadas y cifradas en 'cuentasCifrado.bin'.\n" << endl;
    system("pause");
}

/**
 * @brief Guarda los titulares en un archivo binario sin cifrar
 * Utiliza datos actualizados desde MongoDB si hay conexion disponible.
 * 
 * @param titulares Lista de titulares del sistema (usado como fallback)
 */
void GestorArchivosBinarios::guardarArchivoBinSinCifrar(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- GUARDAR CUENTAS EN ARCHIVO BINARIO SIN CIFRAR ---" << endl;
    
    // Obtener datos actualizados desde MongoDB o locales
    std::vector<Titular*> datosActualizados = obtenerDatosActualizados(titulares);
    
    if (datosActualizados.empty()) {
        cout << "\nNo hay titulares registrados para guardar.\n" << endl;
        system("pause");
        return;
    }
    
    // Usar la version que trabaja con vector
    ArchivoBinario::guardarDesdeVector(datosActualizados, "cuentasSinCifrar.bin");

    cout << "\nCuentas guardadas en 'cuentasSinCifrar.bin'.\n" << endl;
    system("pause");
}

/**
 * @brief Decifra un archivo binario previamente cifrado con cifrado Cesar
 * 
 * @param titulares Lista de titulares del sistema (no utilizada, se mantiene por compatibilidad)
 */
void GestorArchivosBinarios::decifrarArchivoCifrado(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- DECIFRAR ARCHIVO CIFRADO ---" << endl;
    ifstream archivo("cuentasCifrado.bin", ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo cifrado 'cuentasCifrado.bin'.\n" << endl;
        system("pause");
        return;
    }
    archivo.close();

    int desplazamiento = 3; // Mismo desplazamiento usado para cifrar
    cout << "\nDescifrando archivo con desplazamiento: " << desplazamiento << endl;
    
    // Usar la funcion de descifrado que toma el desplazamiento positivo
    descifrarCesarArchivoBinario(std::string("cuentasCifrado.bin"), std::string("cuentasDecifradas.bin"), desplazamiento);
    
    // Verificar que se creo el archivo descifrado
    ifstream verificacion("cuentasDecifradas.bin", ios::binary);
    if (verificacion) {
        cout << "\nArchivo decifrado exitosamente en 'cuentasDecifradas.bin'.\n" << endl;
        verificacion.close();
    } else {
        cout << "\nError: No se pudo crear el archivo descifrado.\n" << endl;
    }
    
    system("pause");
}
