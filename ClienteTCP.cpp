/**
 * @file ClienteTCP.cpp
 * @author MichiBank Team
 * @brief Implementacion del cliente TCP para comunicacion con servidor Python
 * @version 1.0
 * @date 2025-08-10
 * 
 * @copyright Copyright (c) 2025
 */
#include "ClienteTCP.h"
#include "Hora.h"
#include <iostream>
#include <sstream>
#include <iomanip>

ClienteTCP::ClienteTCP() : clientSocket(INVALID_SOCKET), serverIP(""), serverPort(0), conectado(false) {
    // Inicializar Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error al inicializar Winsock" << std::endl;
    }
}

ClienteTCP::~ClienteTCP() {
    desconectar();
    WSACleanup();
}

bool ClienteTCP::conectarServidor(const std::string& ip, int puerto) {
    serverIP = ip;
    serverPort = puerto;

    // Crear socket
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error al crear socket: " << WSAGetLastError() << std::endl;
        return false;
    }

    // Configurar direccion del servidor
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(puerto);
    
    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Direccion IP invalida" << std::endl;
        closesocket(clientSocket);
        return false;
    }

    // Conectar al servidor
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error al conectar con el servidor: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return false;
    }

    conectado = true;
    std::cout << "Conectado al servidor " << ip << ":" << puerto << std::endl;
    return true;
}

void ClienteTCP::desconectar() {
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }
    conectado = false;
}

bool ClienteTCP::estaConectado() const {
    return conectado;
}

std::string ClienteTCP::enviarComando(const std::string& comando) {
    if (!conectado) {
        return "ERROR: No conectado al servidor";
    }

    // Enviar comando
    int resultado = send(clientSocket, comando.c_str(), comando.length(), 0);
    if (resultado == SOCKET_ERROR) {
        std::cerr << "Error al enviar comando: " << WSAGetLastError() << std::endl;
        return "ERROR: Error de comunicacion";
    }

    // Recibir respuesta
    char buffer[4096];
    resultado = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (resultado > 0) {
        buffer[resultado] = '\0';
        return std::string(buffer);
    } else if (resultado == 0) {
        std::cout << "Conexion cerrada por el servidor" << std::endl;
        conectado = false;
        return "ERROR: Conexion cerrada";
    } else {
        std::cerr << "Error al recibir respuesta: " << WSAGetLastError() << std::endl;
        return "ERROR: Error de comunicacion";
    }
}

bool ClienteTCP::verificarConexionMongo() {
    std::string respuesta = enviarComando("CHECK_MONGO");
    return respuesta == "OK";
}

bool ClienteTCP::existeTitularPorCI(const std::string& cedula) {
    std::string comando = "EXISTS_TITULAR:" + cedula;
    std::string respuesta = enviarComando(comando);
    return respuesta == "TRUE";
}

bool ClienteTCP::existeCuentaPorID(const std::string& idCuenta) {
    std::string comando = "EXISTS_CUENTA:" + idCuenta;
    std::string respuesta = enviarComando(comando);
    return respuesta == "TRUE";
}

bool ClienteTCP::insertarTitular(const Titular* titular) {
    std::string json = titularToJSON(titular);
    std::string comando = "INSERT_TITULAR:" + json;
    std::string respuesta = enviarComando(comando);
    return respuesta == "OK";
}

bool ClienteTCP::actualizarTitular(const Titular* titular) {
    std::string json = titularToJSON(titular);
    std::string comando = "UPDATE_TITULAR:" + json;
    std::string respuesta = enviarComando(comando);
    return respuesta == "OK";
}

std::string ClienteTCP::buscarTitularPorCI(const std::string& cedula) {
    std::string comando = "FIND_TITULAR:" + cedula;
    return enviarComando(comando);
}

bool ClienteTCP::insertarCuenta(const CuentaBancaria* cuenta, const std::string& cedulaTitular) {
    std::string json = cuentaToJSON(cuenta);
    std::string comando = "INSERT_CUENTA:" + cedulaTitular + ":" + json;
    std::string respuesta = enviarComando(comando);
    return respuesta == "OK";
}

bool ClienteTCP::actualizarSaldoCuenta(const std::string& idCuenta, float nuevoSaldo) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << nuevoSaldo;
    std::string comando = "UPDATE_SALDO:" + idCuenta + ":" + oss.str();
    std::string respuesta = enviarComando(comando);
    return respuesta == "OK";
}

std::string ClienteTCP::buscarCuentaPorID(const std::string& idCuenta) {
    std::string comando = "FIND_CUENTA:" + idCuenta;
    return enviarComando(comando);
}

bool ClienteTCP::insertarMovimiento(const Movimiento* movimiento, const std::string& idCuenta) {
    std::string json = movimientoToJSON(movimiento);
    std::string comando = "INSERT_MOVIMIENTO:" + idCuenta + ":" + json;
    std::string respuesta = enviarComando(comando);
    return respuesta == "OK";
}

std::string ClienteTCP::buscarMovimientosPorCuenta(const std::string& idCuenta) {
    std::string comando = "FIND_MOVIMIENTOS_CUENTA:" + idCuenta;
    return enviarComando(comando);
}

std::string ClienteTCP::buscarMovimientosPorFecha(const std::string& fechaInicio, const std::string& fechaFin) {
    std::string comando = "FIND_MOVIMIENTOS_FECHA:" + fechaInicio + ":" + fechaFin;
    return enviarComando(comando);
}

bool ClienteTCP::enviarMensajeChat(const std::string& mensaje) {
    std::string comando = "CHAT_SEND:" + mensaje;
    std::string respuesta = enviarComando(comando);
    return respuesta == "OK";
}

std::string ClienteTCP::recibirMensajeChat() {
    std::string comando = "CHAT_RECEIVE";
    return enviarComando(comando);
}

bool ClienteTCP::insertarTitularCompleto(const std::string& jsonData) {
    std::string comando = "INSERT_TITULAR_COMPLETO:" + jsonData;
    std::string respuesta = enviarComando(comando);
    return respuesta == "OK";
}

bool ClienteTCP::actualizarTitularCompleto(const std::string& cedula, const std::string& jsonData) {
    std::string comando = "UPDATE_TITULAR_COMPLETO:" + cedula + ":" + jsonData;
    std::string respuesta = enviarComando(comando);
    return respuesta == "OK";
}

std::string ClienteTCP::buscarTitularCompleto(const std::string& cedula) {
    std::string comando = "FIND_TITULAR_COMPLETO:" + cedula;
    return enviarComando(comando);
}

std::string ClienteTCP::formatearFecha(const Fecha& fecha) {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << fecha.getDia() << "/"
        << std::setfill('0') << std::setw(2) << fecha.getMes() << "/"
        << fecha.getAnio().getAnio();
    return oss.str();
}

std::string ClienteTCP::formatearFechaHora(const FechaHora& fechaHora) {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << fechaHora.getDia() << "/"
        << std::setfill('0') << std::setw(2) << fechaHora.getMes() << "/"
        << fechaHora.getAnio() << " "
        << std::setfill('0') << std::setw(2) << fechaHora.getHora() << ":"
        << std::setfill('0') << std::setw(2) << fechaHora.getMinuto() << ":"
        << std::setfill('0') << std::setw(2) << fechaHora.getSegundo();
    return oss.str();
}

std::string ClienteTCP::titularToJSON(const Titular* titular) {
    std::ostringstream json;
    
    // Como getPersona() retorna const, necesitamos hacer cast para acceder a getFechaNa()
    Titular* titularNoConst = const_cast<Titular*>(titular);
    const Persona& persona = titular->getPersona();
    Persona& personaNoConst = const_cast<Persona&>(persona);
    
    json << "{";
    json << "\"cedula\":\"" << persona.getCI() << "\",";
    json << "\"nombre\":\"" << persona.getNombre() << "\",";
    json << "\"apellido\":\"" << persona.getApellido() << "\",";
    json << "\"telefono\":\"" << persona.getTelefono() << "\",";
    json << "\"correo\":\"" << persona.getCorreo() << "\",";
    
    // Obtener fecha de nacimiento y formatearla
    Fecha fechaNac = personaNoConst.getFechaNa();
    json << "\"fechaNacimiento\":\"" << formatearFecha(fechaNac) << "\"";
    json << "}";
    
    return json.str();
}

std::string ClienteTCP::cuentaToJSON(const CuentaBancaria* cuenta) {
    std::ostringstream json;
    
    json << "{";
    json << "\"id\":\"" << cuenta->getID() << "\",";
    json << "\"saldo\":" << std::fixed << std::setprecision(2) << cuenta->getSaldo() << ",";
    json << "\"tipoCuenta\":\"" << cuenta->getTipoCuenta() << "\",";
    json << "\"fechaCreacion\":\"" << formatearFechaHora(cuenta->getFechaCre()) << "\"";
    json << "}";
    
    return json.str();
}

std::string ClienteTCP::movimientoToJSON(const Movimiento* movimiento) {
    std::ostringstream json;
    
    // Crear FechaHora combinando fecha y hora del movimiento
    FechaHora fh;
    Fecha fecha = movimiento->getFechaMov();
    Hora hora = movimiento->getHora();
    
    fh.setAnio(fecha.getAnio().getAnio());
    fh.setMes(fecha.getMes());
    fh.setDia(fecha.getDia());
    fh.setHora(hora.getHoras());
    fh.setMinuto(hora.getMinutos());
    fh.setSegundo(hora.getSegundos());
    
    json << "{";
    json << "\"numeroMovimiento\":" << movimiento->getNumeroMovimiento() << ",";
    json << "\"monto\":" << std::fixed << std::setprecision(2) << movimiento->getMonto() << ",";
    json << "\"tipo\":\"" << (movimiento->getTipo() ? "deposito" : "retiro") << "\",";
    json << "\"fechaHora\":\"" << formatearFechaHora(fh) << "\"";
    json << "}";
    
    return json.str();
}

// Estas funciones las implementaremos cuando sea necesario parsear respuestas JSON
Titular* ClienteTCP::JSONToTitular(const std::string& json) {
    // Implementacion simplificada - en un proyecto real usaria una libreria JSON
    return nullptr;
}

CuentaBancaria* ClienteTCP::JSONToCuenta(const std::string& json) {
    // Implementacion simplificada - en un proyecto real usaria una libreria JSON
    return nullptr;
}
