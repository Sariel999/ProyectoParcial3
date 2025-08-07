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
#include "ArchivoBinario.h"
#include "CifradoCesar.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

GestorArchivosBinarios::GestorArchivosBinarios() {
}

GestorArchivosBinarios::~GestorArchivosBinarios() {
}

/**
 * @brief Guarda los titulares en un archivo binario cifrado usando cifrado Cesar
 * 
 * @param titulares Lista de titulares del sistema
 */
void GestorArchivosBinarios::guardarArchivoBinCifrado(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- GUARDAR CUENTAS EN ARCHIVO BINARIO ---" << endl;
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados para guardar.\n" << endl;
        system("pause");
        return;
    }
    ArchivoBinario::guardar(titulares, "cuentas_temp.bin");

    int desplazamiento = 3; // Puedes elegir el desplazamiento que desees
    
    cifrarCesarArchivoBinario(std::string("cuentas_temp.bin"), std::string("cuentasCifrado.bin"), desplazamiento);

    remove("cuentas_temp.bin");

    cout << "\nCuentas guardadas y cifradas en 'cuentasCifrado.bin'.\n" << endl;
    system("pause");
}

/**
 * @brief Guarda los titulares en un archivo binario sin cifrar
 * 
 * @param titulares Lista de titulares del sistema
 */
void GestorArchivosBinarios::guardarArchivoBinSinCifrar(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- GUARDAR CUENTAS EN ARCHIVO BINARIO ---" << endl;
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados para guardar.\n" << endl;
        system("pause");
        return;
    }
    ArchivoBinario::guardar(titulares, "cuentasSinCifrar.bin");

    cout << "\nCuentas guardadas 'cuentasSinCifrar.bin'.\n" << endl;
    system("pause");
}

/**
 * @brief Decifra un archivo binario previamente cifrado con cifrado Cesar
 * 
 * @param titulares Lista de titulares del sistema (para guardar el resultado)
 */
void GestorArchivosBinarios::decifrarArchivoCifrado(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- DECIFRAR ARCHIVO CIFRADO ---" << endl;
    ifstream archivo("cuentasCifrado.bin", ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo cifrado.\n" << endl;
        system("pause");
        return;
    }

    int desplazamiento = 3; 
    cifrarCesarArchivoBinario(std::string("cuentasCifrado.bin"), std::string("cuentasDecifradas.bin"), -desplazamiento);
    ArchivoBinario::guardar(titulares, "cuentasDecifradas.bin");
    cout << "\nArchivo decifrado y cargado exitosamente.\n" << endl;
    system("pause");
}
