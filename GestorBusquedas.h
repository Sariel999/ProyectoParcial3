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
#include <vector>
#include "ListaDobleCircular.h"
#include "Titular.h"
#include "Validaciones.h"
#include "ValidacionFecha.h"
#include "GestorBusquedaMongo.h"

class GestorBusquedas {
private:
    Validaciones val;
    
public:
    GestorBusquedas();
    ~GestorBusquedas();
    
    void buscarMovimientosPorFecha(GestorBusquedaMongo& gestorMongo);
    void buscarPorTitular(GestorBusquedaMongo& gestorMongo);
    void buscarPersonalizada(GestorBusquedaMongo& gestorMongo);
    
    // Métodos legacy que mantienen compatibilidad
    void buscarMovimientosPorFecha(const ListaDobleCircular<Titular*>& titulares);
    void buscarPorTitular(const ListaDobleCircular<Titular*>& titulares);
    void buscarPersonalizada(const ListaDobleCircular<Titular*>& titulares);
};

#endif
