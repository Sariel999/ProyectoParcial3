/**
 * @file GestorConexion.h
 * @author MichiBank Team
 * @brief Gestor de conexion TCP y sincronizacion con MongoDB
 * @version 1.0
 * @date 2025-08-10
 * 
 * @copyright Copyright (c) 2025
 */
#ifndef GESTORCONEXION_H
#define GESTORCONEXION_H

#include <string>
#include "ClienteTCP.h"
#include "ListaDobleCircular.h"
#include "Titular.h"
#include "CuentaBancaria.h"
#include "Movimiento.h"
#include <vector>

class GestorConexion {
private:
    ClienteTCP* cliente;
    bool modoServidor;
    bool conectado;

public:
    GestorConexion();
    ~GestorConexion();

    // Configuracion de conexion
    bool conectarComoCliente(const std::string& ip, int puerto);
    void configurarComoServidor();
    void desconectar();
    bool estaConectado() const;
    bool estaModoServidor() const;

    // Sincronizacion con MongoDB
    bool sincronizarTitular(Titular* titular, bool esNuevo = true);
    bool sincronizarCuenta(CuentaBancaria* cuenta, const std::string& cedulaTitular, bool esNueva = true);
    bool sincronizarMovimiento(Movimiento* movimiento, const std::string& idCuenta);
    
    // Validaciones contra base de datos
    bool validarTitularExistente(const std::string& cedula);
    bool validarCuentaExistente(const std::string& idCuenta);
    
    // Busquedas en base de datos
    Titular* buscarTitularEnBD(const std::string& cedula);
    CuentaBancaria* buscarCuentaEnBD(const std::string& idCuenta);
    std::vector<Titular*> obtenerTodosTitulares();
    
    // Parser de JSON
    Titular* parsearTitularDesdeJSON(const std::string& json);
    Titular* parsearTitularCompletoDesdeJSON(const std::string& json);
    std::vector<Titular*> parsearTitularesCompletosDesdeJSON(const std::string& jsonArray);
    CuentaBancaria* parsearCuentaDesdeJSON(const std::string& json);
    Movimiento* parsearMovimientoDesdeJSON(const std::string& json);
    void parsearMovimientosParaCuenta(const std::string& json, CuentaBancaria* cuenta);
    
    // Sincronizacion completa
    bool sincronizarListaTitulares(ListaDobleCircular<Titular*>& listaTitulares);
    bool cargarTitularesDesdeDB(ListaDobleCircular<Titular*>& listaTitulares);
    
    // Chat
    bool enviarMensaje(const std::string& mensaje);
    std::string recibirMensajes();
    
    // Backup y Restore de Base de Datos
    bool exportarBaseDatos(const std::string& nombreArchivo);
    bool importarBaseDatos(const std::string& nombreArchivo);
    
    // TitularCompleto
    bool sincronizarTitularCompleto(const Titular* titular);
    
    // Utilidades
    void mostrarEstadoConexion() const;
    std::string obtenerUltimoError() const;

private:
    std::string ultimoError;
    void establecerError(const std::string& error);
    size_t encontrarFinObjetoJSON(const std::string& json, size_t inicio);
    bool validarJSON(const std::string& json);
};

#endif
