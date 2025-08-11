/**
 * @file ClienteTCP.h
 * @author MichiBank Team
 * @brief Cliente TCP para comunicacion con servidor Python
 * @version 1.0
 * @date 2025-08-10
 * 
 * @copyright Copyright (c) 2025
 */
#ifndef CLIENTETCP_H
#define CLIENTETCP_H

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "Titular.h"
#include "CuentaBancaria.h"
#include "Movimiento.h"

#pragma comment(lib, "ws2_32.lib")

// Resolve byte ambiguity between std::byte and Windows byte
#ifdef byte
#undef byte
#endif

class ClienteTCP {
private:
    SOCKET clientSocket;
    std::string serverIP;
    int serverPort;
    bool conectado;

public:
    ClienteTCP();
    ~ClienteTCP();

    // Conexion y desconexion
    bool conectarServidor(const std::string& ip, int puerto);
    void desconectar();
    bool estaConectado() const;

    // Operaciones con MongoDB a traves del servidor
    bool verificarConexionMongo();
    bool existeTitularPorCI(const std::string& cedula);
    bool existeCuentaPorID(const std::string& idCuenta);
    
    // CRUD Titulares
    bool insertarTitular(const Titular* titular);
    bool actualizarTitular(const Titular* titular);
    std::string buscarTitularPorCI(const std::string& cedula);
    
    // CRUD Cuentas
    bool insertarCuenta(const CuentaBancaria* cuenta, const std::string& cedulaTitular);
    bool actualizarSaldoCuenta(const std::string& idCuenta, float nuevoSaldo);
    std::string buscarCuentaPorID(const std::string& idCuenta);
    
    // CRUD Movimientos
    bool insertarMovimiento(const Movimiento* movimiento, const std::string& idCuenta);
    std::string buscarMovimientosPorCuenta(const std::string& idCuenta);
    std::string buscarMovimientosPorFecha(const std::string& fechaInicio, const std::string& fechaFin);

    // Chat
    bool enviarMensajeChat(const std::string& mensaje);
    std::string recibirMensajeChat();

    // TitularCompleto
    bool insertarTitularCompleto(const std::string& jsonData);
    bool actualizarTitularCompleto(const std::string& cedula, const std::string& jsonData);
    std::string buscarTitularCompleto(const std::string& cedula);

private:
    // Utilidades
    std::string enviarComando(const std::string& comando);
    std::string titularToJSON(const Titular* titular);
    std::string cuentaToJSON(const CuentaBancaria* cuenta);
    std::string movimientoToJSON(const Movimiento* movimiento);
    Titular* JSONToTitular(const std::string& json);
    CuentaBancaria* JSONToCuenta(const std::string& json);
    std::string formatearFecha(const Fecha& fecha);
    std::string formatearFechaHora(const FechaHora& fechaHora);
};

#endif
