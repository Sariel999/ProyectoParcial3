/**
 * @file GestorArchivos.h
 * @author your name (you@domain.com)
 * @brief Clase para gestionar archivos TXT, PDF y operaciones de hash MD5
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef GESTORARCHIVOS_H
#define GESTORARCHIVOS_H

#include <string>
#include "ListaDobleCircular.h"
#include "Titular.h"
#include "TablaHash.h"
#include "Validaciones.h"

class GestorArchivos {
private:
    Validaciones val;
    TablaHash& hashes; // Referencia a la tabla hash del sistema
    
public:
    GestorArchivos(TablaHash& hashTable);
    ~GestorArchivos();
    
    void guardarTitularesEnTxt(const ListaDobleCircular<Titular*>& titulares);
    void generarPDFTitulares();
    std::string generarHashMD5(const std::string& nombreArchivo);
    bool compararHashArchivo(const std::string& nombreArchivo);
};

#endif
