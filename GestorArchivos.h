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

// Forward declaration
class GestorBusquedaMongo;

class GestorArchivos {
private:
    Validaciones val;
    TablaHash& hashes; // Referencia a la tabla hash del sistema
    GestorBusquedaMongo* gestorBusquedaMongo; // Puntero al gestor de busqueda MongoDB
    
    // Metodos privados para procesamiento de archivos
    void procesarTitularParaArchivo(std::ofstream& archivo, Titular* titular, int numeroTitular);
    void procesarCuentaCorrienteParaArchivo(std::ofstream& archivo, Titular* titular);
    void procesarCuentasAhorroParaArchivo(std::ofstream& archivo, Titular* titular);
    void procesarMovimientosParaArchivo(std::ofstream& archivo, ListaDobleCircular<Movimiento*>& movimientos);
    void generarYGuardarHashMD5();
    
public:
    GestorArchivos(TablaHash& hashTable, GestorBusquedaMongo* gestorMongo = nullptr);
    ~GestorArchivos();
    
    void setGestorBusquedaMongo(GestorBusquedaMongo* gestorMongo);
    void guardarTitularesEnTxt(const ListaDobleCircular<Titular*>& titulares);
    void generarPDFTitulares();
    std::string generarHashMD5(const std::string& nombreArchivo);
    bool compararHashArchivo(const std::string& nombreArchivo);
};

#endif
