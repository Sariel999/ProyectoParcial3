/**
 * @file Backups.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef BACKUPS_H
#define BACKUPS_H
#include <iostream>
#include "ListaDobleCircular.h"
#include "Titular.h"
#include "BPlusTreeTitulares.h"


class Backups {
public:
    static void crearBackup(const ListaDobleCircular<Titular*>& titulares);
    bool restaurarBackup(ListaDobleCircular<Titular*>& titulares, BPlusTreeTitulares& arbolTitulares,const std::string& archivo) ;
};
#endif
