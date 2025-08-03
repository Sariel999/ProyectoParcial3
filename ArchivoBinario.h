/**
 * @file ArchivoBinario.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ARCHIVOBINARIO_H
#define ARCHIVOBINARIO_H

#include "Titular.h"
#include "NodoDoble.h"
#include <string>

class ArchivoBinario {
public:
    static void guardar(const ListaDobleCircular<Titular*>& titulares, const std::string& nombreArchivo);
    void cargar(ListaDobleCircular<Titular*>& titulares, const std::string& nombreArchivo);
    static bool cargarBackup(ListaDobleCircular<Titular*>& titulares, const std::string& nombreArchivo);
};

#endif