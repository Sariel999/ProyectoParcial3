/**
 * @file MenuBusquedasBinarias.h
 * @author your name (you@domain.com)
 * @brief Clase para gestionar las operaciones del menu de busquedas binarias
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MENUBUSQUEDASBINARIAS_H
#define MENUBUSQUEDASBINARIAS_H

#include "ListaDobleCircular.h"
#include "Titular.h"
#include "CuentaBancaria.h"
#include "BusquedasBinarias.h"
#include "Validaciones.h"
#include "ListaSucursales.h"
#include "Sucursal.h"
#include "FechaHora.h"
#include "Cita.h"
#include <string>
#include <vector>

// Forward declaration
class GestorBusquedaMongo;

class MenuBusquedasBinarias {
private:
    Validaciones val;
    BusquedasBinarias buscador;
    GestorBusquedaMongo* gestorBusquedaMongo; // Puntero al gestor de busqueda MongoDB

public:
    MenuBusquedasBinarias();
    ~MenuBusquedasBinarias();
    
    // Configuracion del gestor MongoDB
    void setGestorBusquedaMongo(GestorBusquedaMongo* gestorMongo);
    
    // Funciones originales (usan lista local)
    void buscarDepositoMayorIgual(const ListaDobleCircular<Titular*>& titulares);
    void calcularDepositoMinimoMeta(const ListaDobleCircular<Titular*>& titulares);
    void buscarTitularPorCI(ListaDobleCircular<Titular*>& titulares);
    void buscarTitularPorAnioNacimiento(ListaDobleCircular<Titular*>& titulares);
    void buscarSucursalMasCercana(const ListaSucursales& listaSucursales);
    
    // Nuevas funciones que usan MongoDB
    void buscarDepositoMayorIgualMongoDB(const ListaDobleCircular<Titular*>& titulares);
    void calcularDepositoMinimoMetaMongoDB(const ListaDobleCircular<Titular*>& titulares);
    void buscarTitularPorCIMongoDB(const ListaDobleCircular<Titular*>& titulares);
    void buscarTitularPorAnioNacimientoMongoDB(const ListaDobleCircular<Titular*>& titulares);

private:
    // Metodos auxiliares
    Titular* encontrarTitularPorCI(const ListaDobleCircular<Titular*>& titulares, const std::string& ci);
    CuentaBancaria* encontrarCuentaPorTipo(Titular* titular, const std::string& tipo, const std::string& idCuenta);
    
    // Metodos auxiliares para MongoDB
    Titular* obtenerTitularActualizado(const std::string& cedula, const ListaDobleCircular<Titular*>& titulares);
};

#endif
