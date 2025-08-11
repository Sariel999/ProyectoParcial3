/**
 * @file GeneradorQR.h
 * @author your name (you@domain.com)
 * @brief Clase para generar codigos QR y PDFs con informacion de titulares
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef GENERADORQR_H
#define GENERADORQR_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Titular.h"
#include "ListaDobleCircular.h"
#include "FechaHora.h"
#include "Validaciones.h"
// Para QR real con libqrencode
#include <qrencode.h>

// Forward declaration
class GestorBusquedaMongo;

class GeneradorQR {
private:
    Validaciones val;
    GestorBusquedaMongo* gestorBusquedaMongo; // Puntero al gestor de busqueda MongoDB

public:
    // Constructor y destructor
    GeneradorQR();
    ~GeneradorQR();

    // Configuracion del gestor MongoDB
    void setGestorBusquedaMongo(GestorBusquedaMongo* gestorMongo);

    // Funciones principales para generar QR
    void generarQRPDF(const ListaDobleCircular<Titular*>& titulares);
    void generarPDFConQR(const std::string& nombre, const std::string& cedula, 
                         const std::string& cuentasInfo, const std::string& qrData);
    
    // Funciones auxiliares para QR
    void generarQRSimple(const std::string& data, bool qrMatrix[21][21]);
    
    // Funcion auxiliar para buscar titular
    Titular* buscarTitularPorCI(const ListaDobleCircular<Titular*>& titulares, const std::string& ci);

private:
    // Metodo para obtener titular con datos frescos o locales
    Titular* obtenerTitularActualizado(const std::string& cedula, const ListaDobleCircular<Titular*>& titulares);
};

#endif
