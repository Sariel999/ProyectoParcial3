/**
 * @file GestorBusquedas.h
 * @author your name (you@domain.com)
 * @brief Clase para gestionar busquedas avanzadas de titulares y movimientos
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef GESTORBUSQUEDAS_H
#define GESTORBUSQUEDAS_H

#include <string>
#include "ListaDobleCircular.h"
#include "Titular.h"
#include "Validaciones.h"
#include "ValidacionFecha.h"

class GestorBusquedas {
private:
    Validaciones val;
    
public:
    GestorBusquedas();
    ~GestorBusquedas();
    
    void buscarMovimientosPorFecha(const ListaDobleCircular<Titular*>& titulares);
    void buscarPorTitular(const ListaDobleCircular<Titular*>& titulares);
    void buscarPersonalizada(const ListaDobleCircular<Titular*>& titulares);
};

#endif
