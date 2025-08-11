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
#include <vector>
#include "ListaDobleCircular.h"
#include "Titular.h"

// Forward declaration
class GestorBusquedaMongo;

class GestorArchivosBinarios {
private:
    GestorBusquedaMongo* gestorBusquedaMongo; // Puntero al gestor de busqueda MongoDB
    
public:
    GestorArchivosBinarios();
    ~GestorArchivosBinarios();
    
    void setGestorBusquedaMongo(GestorBusquedaMongo* gestorMongo);
    void guardarArchivoBinCifrado(const ListaDobleCircular<Titular*>& titulares);
    void guardarArchivoBinSinCifrar(const ListaDobleCircular<Titular*>& titulares);
    void decifrarArchivoCifrado(const ListaDobleCircular<Titular*>& titulares);

private:
    // Metodos auxiliares para obtener datos
    std::vector<Titular*> obtenerDatosActualizados(const ListaDobleCircular<Titular*>& titulares);
};

#endif
