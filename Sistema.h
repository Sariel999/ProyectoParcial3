/**
 * @file Sistema.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef SISTEMA_H
#define SISTEMA_H

#include "Titular.h"
#include "Validaciones.h"
#include "ListaDobleCircular.h"
#include "ListaSucursales.h"
#include "BPlusTreeTitulares.h"
#include "TablaHash.h"
#include "GeneradorQR.h"
#include "GestorTitulares.h"
#include "OperacionesBancarias.h"
#include "GestorArchivos.h"
#include "GestorArchivosBinarios.h"
#include "GestorBusquedas.h"
#include "MenuBusquedasBinarias.h"
#include "GestorConexion.h"
#include "GestorBusquedaMongo.h"
#include <vector>
#include <string>

class Sistema {
private:
    ListaDobleCircular<Titular*> titulares;
    Validaciones val;
    BPlusTreeTitulares arbolTitulares;
    ListaSucursales listaSucursales;
    TablaHash hashes;
    GeneradorQR generadorQR;
    GestorConexion gestorConexion;
    GestorBusquedaMongo gestorBusquedaMongo;
    GestorTitulares gestorTitulares;
    OperacionesBancarias operacionesBancarias;
    GestorArchivos gestorArchivos;
    GestorArchivosBinarios gestorArchivosBinarios;
    GestorBusquedas gestorBusquedas;
    MenuBusquedasBinarias menuBusquedasBinarias;

    Titular* buscarTitularPorCI(const std::string& cedula);
    void actualizarContadoresSucursales();

public:
    Sistema();
    ~Sistema();

    void menuPrincipal();
    void menuConexion();
    void configurarModoServidor();
    void configurarModoCliente();
    void menuChat();
    void registrarTitular();
    void crearCuenta();
    void realizarDeposito();
    void realizarRetiro();
    void buscarMovimientosPorFecha();
    void buscarPorTitular();
    void buscarPersonalizada();
    void generarPDFTitulares();
    void crearBackup();
    void mostrarAyuda();
    void menuSecundario();
    void menuArbol();
    void busquedasBinarias();
    void menuBB();
    void guardarTitularesEnTxt();
    void verificarIntegridadArchivo();
    void mostrarTablaHash();

private:
    std::string generarJSONTitularCompleto(const Titular* titular);
    void sincronizarTitularCompleto(const Titular* titular);

};

#endif