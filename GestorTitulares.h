/**
 * @file GestorTitulares.h
 * @author your name (you@domain.com)
 * @brief Clase para gestionar registro de titulares y creacion de cuentas bancarias
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef GESTORTITULARES_H
#define GESTORTITULARES_H

#include <string>
#include "Titular.h"
#include "ListaDobleCircular.h"
#include "BPlusTreeTitulares.h"
#include "ListaSucursales.h"
#include "Validaciones.h"
#include "ValidacionFecha.h"
#include "Backups.h"

class GestorTitulares {
private:
    Validaciones val;
    
public:
    GestorTitulares();
    ~GestorTitulares();
    
    // REGISTRO PRINCIPAL DE TITULARES
    void registrarTitular(ListaDobleCircular<Titular*>& titulares, BPlusTreeTitulares& arbolTitulares);
    Titular* buscarTitularPorCI(const ListaDobleCircular<Titular*>& titulares, const std::string& ci);
    
    // GESTION DE CUENTAS BANCARIAS
    void crearCuenta(ListaDobleCircular<Titular*>& titulares, ListaSucursales& listaSucursales);
};

#endif
