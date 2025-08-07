/**
 * @file GestorArchivosBinarios.h
 * @author your name (you@domain.com)
 * @brief Clase para gestionar archivos binarios con cifrado y sin cifrar
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef GESTORARCHIVOS_BINARIOS_H
#define GESTORARCHIVOS_BINARIOS_H

#include <string>
#include "ListaDobleCircular.h"
#include "Titular.h"

class GestorArchivosBinarios {
private:
    
public:
    GestorArchivosBinarios();
    ~GestorArchivosBinarios();
    
    void guardarArchivoBinCifrado(const ListaDobleCircular<Titular*>& titulares);
    void guardarArchivoBinSinCifrar(const ListaDobleCircular<Titular*>& titulares);
    void decifrarArchivoCifrado(const ListaDobleCircular<Titular*>& titulares);
};

#endif
