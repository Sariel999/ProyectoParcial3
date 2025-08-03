#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include "CifradoCesar.h"
using namespace std;

void cifrarCesarArchivoBinario(const string& archivoEntrada, const string& archivoSalida, int desplazamiento) {
    ifstream in(archivoEntrada, ios::binary);
    ofstream out(archivoSalida, ios::binary);

    if (!in || !out) {
        cerr << "No se pudo abrir el archivo.\n";
        return;
    }

    char byte;
    while (in.get(byte)) {
        unsigned char cifrado = static_cast<unsigned char>(byte) + desplazamiento;
        out.put(static_cast<char>(cifrado));
    }

    in.close();
    out.close();
}

void descifrarCesarArchivoBinario(const string& archivoEntrada, const string& archivoSalida, int desplazamiento) {
    ifstream in(archivoEntrada, ios::binary);
    ofstream out(archivoSalida, ios::binary);

    if (!in || !out) {
        cerr << "No se pudo abrir el archivo.\n";
        return;
    }

    char byte;
    while (in.get(byte)) {
        unsigned char descifrado = static_cast<unsigned char>(byte) - desplazamiento;
        out.put(static_cast<char>(descifrado));
    }

    in.close();
    out.close();
}