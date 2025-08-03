#ifndef CIFRADOCESAR_H
#define CIFRADOCESAR_H

#include <string>
using std::string;

void cifrarCesarArchivoBinario(const string& archivoEntrada, const string& archivoSalida, int desplazamiento);
void descifrarCesarArchivoBinario(const string& archivoEntrada, const string& archivoSalida, int desplazamiento);


#endif