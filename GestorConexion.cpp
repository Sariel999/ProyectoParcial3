/**
 * @file GestorConexion.cpp
 * @author MichiBank Team
 * @brief Implementacion del gestor de conexion TCP y sincronizacion con MongoDB
 * @version 1.0
 * @date 2025-08-10
 * 
 * @copyright Copyright (c) 2025
 */
#include "GestorConexion.h"
#include <iostream>
#include <sstream>
#include <fstream>

GestorConexion::GestorConexion() : cliente(nullptr), modoServidor(false), conectado(false), ultimoError("") {
    cliente = new ClienteTCP();
}

GestorConexion::~GestorConexion() {
    if (cliente) {
        delete cliente;
        cliente = nullptr;
    }
}

bool GestorConexion::conectarComoCliente(const std::string& ip, int puerto) {
    if (!cliente) {
        establecerError("Cliente TCP no inicializado");
        return false;
    }

    if (cliente->conectarServidor(ip, puerto)) {
        // Verificar conexion con MongoDB
        if (cliente->verificarConexionMongo()) {
            conectado = true;
            modoServidor = false;
            std::cout << "Conectado al servidor con acceso a MongoDB" << std::endl;
            return true;
        } else {
            establecerError("Servidor conectado pero MongoDB no disponible");
            cliente->desconectar();
            return false;
        }
    } else {
        establecerError("No se pudo conectar al servidor");
        return false;
    }
}

void GestorConexion::configurarComoServidor() {
    modoServidor = true;
    conectado = false;
    std::cout << "Configurado como servidor local (sin MongoDB)" << std::endl;
}

void GestorConexion::desconectar() {
    if (cliente) {
        cliente->desconectar();
    }
    conectado = false;
    modoServidor = false;
}

bool GestorConexion::estaConectado() const {
    return conectado;
}

bool GestorConexion::estaModoServidor() const {
    return modoServidor;
}

bool GestorConexion::sincronizarTitular(Titular* titular, bool esNuevo) {
    if (!conectado || !cliente) {
        return true; // En modo servidor local no sincronizamos
    }

    try {
        if (esNuevo) {
            // Verificar si ya existe
            if (cliente->existeTitularPorCI(titular->getPersona().getCI())) {
                establecerError("Titular ya existe en la base de datos");
                return false;
            }
            return cliente->insertarTitular(titular);
        } else {
            return cliente->actualizarTitular(titular);
        }
    } catch (...) {
        establecerError("Error durante la sincronizacion del titular");
        return false;
    }
}

bool GestorConexion::sincronizarCuenta(CuentaBancaria* cuenta, const std::string& cedulaTitular, bool esNueva) {
    if (!conectado || !cliente) {
        return true; // En modo servidor local no sincronizamos
    }

    try {
        if (esNueva) {
            // Verificar si ya existe
            if (cliente->existeCuentaPorID(cuenta->getID())) {
                establecerError("Cuenta ya existe en la base de datos");
                return false;
            }
            return cliente->insertarCuenta(cuenta, cedulaTitular);
        } else {
            return cliente->actualizarSaldoCuenta(cuenta->getID(), cuenta->getSaldo());
        }
    } catch (...) {
        establecerError("Error durante la sincronizacion de la cuenta");
        return false;
    }
}

bool GestorConexion::sincronizarMovimiento(Movimiento* movimiento, const std::string& idCuenta) {
    if (!conectado || !cliente) {
        return true; // En modo servidor local no sincronizamos
    }

    try {
        return cliente->insertarMovimiento(movimiento, idCuenta);
    } catch (...) {
        establecerError("Error durante la sincronizacion del movimiento");
        return false;
    }
}

bool GestorConexion::validarTitularExistente(const std::string& cedula) {
    if (!conectado || !cliente) {
        return false; // En modo servidor local no validamos contra BD
    }

    try {
        return cliente->existeTitularPorCI(cedula);
    } catch (...) {
        establecerError("Error al validar titular en base de datos");
        return false;
    }
}

bool GestorConexion::validarCuentaExistente(const std::string& idCuenta) {
    if (!conectado || !cliente) {
        return false; // En modo servidor local no validamos contra BD
    }

    try {
        return cliente->existeCuentaPorID(idCuenta);
    } catch (...) {
        establecerError("Error al validar cuenta en base de datos");
        return false;
    }
}

Titular* GestorConexion::buscarTitularEnBD(const std::string& cedula) {
    if (!conectado || !cliente) {
        establecerError("No hay conexion a la base de datos");
        return nullptr;
    }

    try {
        std::string resultado = cliente->buscarTitularCompleto(cedula);
        if (resultado.empty() || resultado.substr(0, 5) == "ERROR") {
            establecerError("Titular no encontrado en la base de datos");
            return nullptr;
        }
        
        // Aquí deberíamos parsear el JSON para crear el objeto Titular
        // Por ahora, simulamos que se encontró y creamos un titular básico
        // En una implementación real, parsearías el JSON completamente
        
        std::cout << "DEBUG: Resultado de búsqueda: " << resultado << std::endl;
        
        // Parsear JSON manualmente
        Titular* titular = parsearTitularCompletoDesdeJSON(resultado);
        if (titular) {
            std::cout << "Titular encontrado en MongoDB: " << titular->getPersona().getNombre() 
                      << " " << titular->getPersona().getApellido() << std::endl;
            
            // Mostrar información de las cuentas cargadas
            if (titular->getCuentaCorriente() != nullptr) {
                std::cout << "- Cuenta Corriente cargada: " << titular->getCuentaCorriente()->getID() 
                          << " (Saldo: $" << titular->getCuentaCorriente()->getSaldo() << ")" << std::endl;
            }
            
            if (!titular->getCuentasAhorro().vacia()) {
                std::cout << "- Cuentas de Ahorro cargadas exitosamente" << std::endl;
            }
            
            return titular;
        } else {
            establecerError("Error al parsear respuesta de MongoDB");
            return nullptr;
        }
        
    } catch (...) {
        establecerError("Error al buscar titular en base de datos");
        return nullptr;
    }
}

CuentaBancaria* GestorConexion::buscarCuentaEnBD(const std::string& idCuenta) {
    if (!conectado || !cliente) {
        return nullptr;
    }

    try {
        std::string resultado = cliente->buscarCuentaPorID(idCuenta);
        if (resultado.empty() || resultado.substr(0, 5) == "ERROR") {
            return nullptr;
        }
        // Aqui normalmente parseriamos el JSON y creariamos el objeto CuentaBancaria
        // Por simplicidad retornamos nullptr
        return nullptr;
    } catch (...) {
        establecerError("Error al buscar cuenta en base de datos");
        return nullptr;
    }
}

bool GestorConexion::sincronizarListaTitulares(ListaDobleCircular<Titular*>& listaTitulares) {
    if (!conectado || !cliente) {
        return true; // En modo servidor local no sincronizamos
    }

    // Sincronizar todos los titulares de la lista local con la base de datos
    NodoDoble<Titular*>* nodo = listaTitulares.getCabeza();
    if (!nodo) return true;

    do {
        if (!sincronizarTitular(nodo->dato, false)) {
            return false;
        }
        nodo = nodo->siguiente;
    } while (nodo != listaTitulares.getCabeza());

    return true;
}

bool GestorConexion::cargarTitularesDesdeDB(ListaDobleCircular<Titular*>& listaTitulares) {
    if (!conectado || !cliente) {
        return false;
    }

    // Esta funcion cargaria todos los titulares desde MongoDB
    // Por simplicidad, solo mostramos el concepto
    std::cout << "Cargando titulares desde base de datos..." << std::endl;
    return true;
}

bool GestorConexion::enviarMensaje(const std::string& mensaje) {
    if (!conectado || !cliente) {
        establecerError("No hay conexion activa para chat");
        return false;
    }

    return cliente->enviarMensajeChat(mensaje);
}

std::string GestorConexion::recibirMensajes() {
    if (!conectado || !cliente) {
        return "";
    }

    return cliente->recibirMensajeChat();
}

void GestorConexion::mostrarEstadoConexion() const {
    std::cout << "\n=== Estado de Conexion ===" << std::endl;
    std::cout << "Modo: " << (modoServidor ? "Servidor Local" : "Cliente TCP") << std::endl;
    std::cout << "Conectado: " << (conectado ? "Si" : "No") << std::endl;
    if (!ultimoError.empty()) {
        std::cout << "Ultimo error: " << ultimoError << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

std::string GestorConexion::obtenerUltimoError() const {
    return ultimoError;
}

void GestorConexion::establecerError(const std::string& error) {
    ultimoError = error;
    std::cerr << "Error: " << error << std::endl;
}

bool GestorConexion::sincronizarTitularCompleto(const Titular* titular) {
    if (!conectado || !cliente || modoServidor) {
        establecerError("No conectado como cliente TCP");
        return false;
    }
    
    // Debug: Verificar movimientos antes de sincronizar
    std::cout << "DEBUG: Sincronizando titular completo..." << std::endl;
    if (titular->getCuentaCorriente()) {
        int movCC = 0;
        if (!titular->getCuentaCorriente()->getMovimientos().vacia()) {
            NodoDoble<Movimiento*>* actual = titular->getCuentaCorriente()->getMovimientos().getCabeza();
            if (actual) {
                do {
                    movCC++;
                    actual = actual->siguiente;
                } while (actual != titular->getCuentaCorriente()->getMovimientos().getCabeza());
            }
        }
        std::cout << "  Cuenta Corriente antes de sync: " << movCC << " movimientos" << std::endl;
    }
    
    if (!titular->getCuentasAhorro().vacia()) {
        NodoDoble<CuentaBancaria*>* nodo = titular->getCuentasAhorro().getCabeza();
        int cuentaNum = 1;
        if (nodo) {
            do {
                int movAH = 0;
                if (!nodo->dato->getMovimientos().vacia()) {
                    NodoDoble<Movimiento*>* actual = nodo->dato->getMovimientos().getCabeza();
                    if (actual) {
                        do {
                            movAH++;
                            actual = actual->siguiente;
                        } while (actual != nodo->dato->getMovimientos().getCabeza());
                    }
                }
                std::cout << "  Cuenta Ahorro " << cuentaNum << " antes de sync: " << movAH << " movimientos" << std::endl;
                nodo = nodo->siguiente;
                cuentaNum++;
            } while (nodo != titular->getCuentasAhorro().getCabeza());
        }
    }
    
    // Generar JSON del titular completo
    std::ostringstream json;
    json << "{";
    json << "\"cedula\":\"" << titular->getPersona().getCI() << "\",";
    json << "\"nombre\":\"" << titular->getPersona().getNombre() << "\",";
    json << "\"apellido\":\"" << titular->getPersona().getApellido() << "\",";
    json << "\"telefono\":\"" << titular->getPersona().getTelefono() << "\",";
    json << "\"correo\":\"" << titular->getPersona().getCorreo() << "\",";
    json << "\"fechaNacimiento\":\"" << titular->getPersona().getFechaNa().getDia() << "/"
         << titular->getPersona().getFechaNa().getMes() << "/"
         << titular->getPersona().getFechaNa().getAnio().getAnio() << "\",";
    
    // Agregar cuenta corriente si existe
    json << "\"cuentaCorriente\":";
    if (titular->getCuentaCorriente() != nullptr) {
        json << "{";
        json << "\"id\":\"" << titular->getCuentaCorriente()->getID() << "\",";
        json << "\"saldo\":" << titular->getCuentaCorriente()->getSaldo() << ",";
        json << "\"tipoCuenta\":\"" << titular->getCuentaCorriente()->getTipoCuenta() << "\",";
        json << "\"fechaCreacion\":\"" << titular->getCuentaCorriente()->getFechaCre().getDia() << "/"
             << titular->getCuentaCorriente()->getFechaCre().getMes() << "/"
             << titular->getCuentaCorriente()->getFechaCre().getAnio() << "\",";
        
        // Agregar movimientos de la cuenta corriente
        json << "\"movimientos\":[";
        if (!titular->getCuentaCorriente()->getMovimientos().vacia()) {
            bool primerMovimiento = true;
            NodoDoble<Movimiento*>* actual = titular->getCuentaCorriente()->getMovimientos().getCabeza();
            if (actual) {
                do {
                    if (!primerMovimiento) json << ",";
                    json << "{";
                    json << "\"monto\":" << actual->dato->getMonto() << ",";
                    json << "\"esDeposito\":" << (actual->dato->getTipo() ? "true" : "false") << ",";
                    json << "\"fecha\":\"" << actual->dato->getFechaMov().getDia() << "/"
                         << actual->dato->getFechaMov().getMes() << "/"
                         << actual->dato->getFechaMov().getAnio().getAnio() << "\",";
                    json << "\"hora\":\"" << actual->dato->getHora().getHoras() << ":"
                         << actual->dato->getHora().getMinutos() << ":"
                         << actual->dato->getHora().getSegundos() << "\"";
                    json << "}";
                    primerMovimiento = false;
                    actual = actual->siguiente;
                } while (actual != titular->getCuentaCorriente()->getMovimientos().getCabeza());
            }
        }
        json << "]";
        json << "}";
    } else {
        json << "null";
    }
    
    // Agregar cuentas de ahorro
    json << ",\"cuentasAhorro\":[";
    ListaDobleCircular<CuentaBancaria*>& cuentasAhorro = const_cast<Titular*>(titular)->getCuentasAhorro();
    NodoDoble<CuentaBancaria*>* nodo = cuentasAhorro.getCabeza();
    bool primera = true;
    
    // Debug: Contar cuentas de ahorro antes de serializar
    int contadorCuentasAhorro = 0;
    if (nodo != nullptr) {
        NodoDoble<CuentaBancaria*>* temp = nodo;
        do {
            contadorCuentasAhorro++;
            temp = temp->siguiente;
        } while (temp != nodo);
    }
    std::cout << "DEBUG: Serializando " << contadorCuentasAhorro << " cuentas de ahorro" << std::endl;
    
    if (nodo != nullptr) {
        do {
            if (!primera) json << ",";
            json << "{";
            json << "\"id\":\"" << nodo->dato->getID() << "\",";
            json << "\"saldo\":" << nodo->dato->getSaldo() << ",";
            json << "\"tipoCuenta\":\"" << nodo->dato->getTipoCuenta() << "\",";
            json << "\"fechaCreacion\":\"" << nodo->dato->getFechaCre().getDia() << "/"
                 << nodo->dato->getFechaCre().getMes() << "/"
                 << nodo->dato->getFechaCre().getAnio() << "\",";
            
            // Debug: Info de la cuenta que se está serializando
            std::cout << "DEBUG: Serializando cuenta ahorro ID: " << nodo->dato->getID() 
                      << ", Saldo: " << nodo->dato->getSaldo() << std::endl;
            
            // Agregar movimientos de esta cuenta de ahorro
            json << "\"movimientos\":[";
            if (!nodo->dato->getMovimientos().vacia()) {
                bool primerMovimiento = true;
                NodoDoble<Movimiento*>* actualMov = nodo->dato->getMovimientos().getCabeza();
                if (actualMov) {
                    do {
                        if (!primerMovimiento) json << ",";
                        json << "{";
                        json << "\"monto\":" << actualMov->dato->getMonto() << ",";
                        json << "\"esDeposito\":" << (actualMov->dato->getTipo() ? "true" : "false") << ",";
                        json << "\"fecha\":\"" << actualMov->dato->getFechaMov().getDia() << "/"
                             << actualMov->dato->getFechaMov().getMes() << "/"
                             << actualMov->dato->getFechaMov().getAnio().getAnio() << "\",";
                        json << "\"hora\":\"" << actualMov->dato->getHora().getHoras() << ":"
                             << actualMov->dato->getHora().getMinutos() << ":"
                             << actualMov->dato->getHora().getSegundos() << "\"";
                        json << "}";
                        primerMovimiento = false;
                        actualMov = actualMov->siguiente;
                    } while (actualMov != nodo->dato->getMovimientos().getCabeza());
                }
            }
            json << "]";
            json << "}";
            primera = false;
            nodo = nodo->siguiente;
        } while (nodo != cuentasAhorro.getCabeza());
    }
    json << "]";
    
    json << "}";
    
    // Debug: Mostrar JSON generado
    std::cout << "DEBUG - JSON generado para titular completo:" << std::endl;
    std::cout << json.str() << std::endl;
    
    return cliente->insertarTitularCompleto(json.str());
}

Titular* GestorConexion::parsearTitularDesdeJSON(const std::string& json) {
    try {
        // Parser simple de JSON para extraer campos basicos
        std::string cedula, nombre, apellido, telefono, correo, fechaNacimiento;
        
        // Buscar cedula
        size_t pos = json.find("\"cedula\": \"");
        if (pos != std::string::npos) {
            pos += 11; // Longitud de "cedula": "
            size_t fin = json.find("\"", pos);
            if (fin != std::string::npos) {
                cedula = json.substr(pos, fin - pos);
            }
        }
        
        // Buscar nombre
        pos = json.find("\"nombre\": \"");
        if (pos != std::string::npos) {
            pos += 11; // Longitud de "nombre": "
            size_t fin = json.find("\"", pos);
            if (fin != std::string::npos) {
                nombre = json.substr(pos, fin - pos);
            }
        }
        
        // Buscar apellido
        pos = json.find("\"apellido\": \"");
        if (pos != std::string::npos) {
            pos += 13; // Longitud de "apellido": "
            size_t fin = json.find("\"", pos);
            if (fin != std::string::npos) {
                apellido = json.substr(pos, fin - pos);
            }
        }
        
        // Buscar telefono
        pos = json.find("\"telefono\": \"");
        if (pos != std::string::npos) {
            pos += 13; // Longitud de "telefono": "
            size_t fin = json.find("\"", pos);
            if (fin != std::string::npos) {
                telefono = json.substr(pos, fin - pos);
            }
        }
        
        // Buscar correo
        pos = json.find("\"correo\": \"");
        if (pos != std::string::npos) {
            pos += 11; // Longitud de "correo": "
            size_t fin = json.find("\"", pos);
            if (fin != std::string::npos) {
                correo = json.substr(pos, fin - pos);
            }
        }
        
        // Buscar fecha de nacimiento
        pos = json.find("\"fechaNacimiento\": \"");
        if (pos != std::string::npos) {
            pos += 20; // Longitud de "fechaNacimiento": "
            size_t fin = json.find("\"", pos);
            if (fin != std::string::npos) {
                fechaNacimiento = json.substr(pos, fin - pos);
            }
        }
        
        // Verificar que tengamos los campos minimos
        if (cedula.empty() || nombre.empty() || apellido.empty()) {
            return nullptr;
        }
        
        // Crear objeto Persona
        Persona persona;
        persona.setCI(cedula);
        persona.setNombre(nombre);
        persona.setApellido(apellido);
        persona.setTelefono(telefono);
        persona.setCorreo(correo);
        
        // Parsear fecha de nacimiento (formato: dd/mm/yyyy)
        if (!fechaNacimiento.empty()) {
            size_t pos1 = fechaNacimiento.find("/");
            size_t pos2 = fechaNacimiento.find("/", pos1 + 1);
            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                int dia = std::stoi(fechaNacimiento.substr(0, pos1));
                int mes = std::stoi(fechaNacimiento.substr(pos1 + 1, pos2 - pos1 - 1));
                int anio = std::stoi(fechaNacimiento.substr(pos2 + 1));
                
                Fecha fechaNac;
                fechaNac.setDia(dia);
                fechaNac.setMes(mes);
                Anio anioObj;
                anioObj.setAnio(anio);
                fechaNac.setAnio(anioObj);
                persona.setFechaNa(fechaNac);
            }
        }
        
        // Crear titular con la persona
        Titular* titular = new Titular(persona);
        
        return titular;
        
    } catch (...) {
        return nullptr;
    }
}

Titular* GestorConexion::parsearTitularCompletoDesdeJSON(const std::string& json) {
    try {
        // Primero crear el titular basico usando la funcion existente
        Titular* titular = parsearTitularDesdeJSON(json);
        if (!titular) {
            return nullptr;
        }
        
        // Buscar y parsear cuenta corriente
        size_t corrientePos = json.find("\"cuentaCorriente\": {");
        if (corrientePos != std::string::npos) {
            corrientePos += 19; // Longitud de "cuentaCorriente": 
            size_t inicio = json.find("{", corrientePos);
            if (inicio != std::string::npos) {
                // Encontrar el final del objeto JSON de la cuenta corriente
                int braceCount = 0;
                size_t fin = inicio;
                for (size_t i = inicio; i < json.length(); i++) {
                    if (json[i] == '{') braceCount++;
                    else if (json[i] == '}') {
                        braceCount--;
                        if (braceCount == 0) {
                            fin = i;
                            break;
                        }
                    }
                }
                
                if (fin > inicio) {
                    std::string cuentaJson = json.substr(inicio, fin - inicio + 1);
                    CuentaBancaria* cuentaCorriente = parsearCuentaDesdeJSON(cuentaJson);
                    if (cuentaCorriente) {
                        titular->setCuentaCorriente(cuentaCorriente);
                        // Buscar movimientos para esta cuenta
                        parsearMovimientosParaCuenta(cuentaJson, cuentaCorriente);
                    }
                }
            }
        }
        
        // Buscar y parsear cuentas de ahorro - intentar diferentes formatos
        size_t ahorroPos = json.find("\"cuentasAhorro\":");
        if (ahorroPos != std::string::npos) {
            // Encontrar el inicio del array [
            size_t arrayStart = json.find("[", ahorroPos);
            if (arrayStart != std::string::npos) {
                // Contar brackets y braces para encontrar el final correcto del array
                int bracketCount = 0;
                int braceCount = 0;
                size_t ahorroEnd = arrayStart;
                
                for (size_t i = arrayStart; i < json.length(); i++) {
                    if (json[i] == '[') {
                        bracketCount++;
                    } else if (json[i] == ']') {
                        bracketCount--;
                        if (bracketCount == 0) {
                            ahorroEnd = i;
                            break;
                        }
                    } else if (json[i] == '{') {
                        braceCount++;
                    } else if (json[i] == '}') {
                        braceCount--;
                    }
                }
                
                if (ahorroEnd > arrayStart) {
                    // Extraer el contenido del array (sin los brackets externos)
                    std::string cuentasJson = json.substr(arrayStart + 1, ahorroEnd - arrayStart - 1);
                
                    std::cout << "DEBUG: Parseando cuentas de ahorro. JSON extraído:" << std::endl;
                    std::cout << "'" << cuentasJson << "'" << std::endl;
                    
                    // Parsear cada cuenta de ahorro
                    size_t pos = 0;
                    int cuentasParsedas = 0;
                    while (pos < cuentasJson.length()) {
                        size_t inicio = cuentasJson.find("{", pos);
                        if (inicio == std::string::npos) break;
                        
                        // Encontrar el final del objeto JSON
                        int braceCount = 0;
                        size_t fin = inicio;
                        for (size_t i = inicio; i < cuentasJson.length(); i++) {
                            if (cuentasJson[i] == '{') braceCount++;
                            else if (cuentasJson[i] == '}') {
                                braceCount--;
                                if (braceCount == 0) {
                                    fin = i;
                                    break;
                                }
                            }
                        }
                        
                        if (fin > inicio) {
                            std::string cuentaJson = cuentasJson.substr(inicio, fin - inicio + 1);
                            std::cout << "DEBUG: Parseando cuenta ahorro " << (cuentasParsedas + 1) << ": " << cuentaJson << std::endl;
                            
                            CuentaBancaria* cuenta = parsearCuentaDesdeJSON(cuentaJson);
                            if (cuenta) {
                                std::cout << "DEBUG: Cuenta parseada exitosamente. ID: " << cuenta->getID() 
                                          << ", Saldo: " << cuenta->getSaldo() << std::endl;
                                titular->agregarCuentaAhorro(cuenta);
                                // Buscar movimientos para esta cuenta
                                parsearMovimientosParaCuenta(cuentaJson, cuenta);
                                cuentasParsedas++;
                            } else {
                                std::cout << "DEBUG: Error al parsear cuenta ahorro" << std::endl;
                            }
                        }
                        
                        pos = fin + 1;
                    }
                    
                    std::cout << "DEBUG: Total de cuentas de ahorro parseadas: " << cuentasParsedas << std::endl;
                } else {
                    std::cout << "DEBUG: No se pudo encontrar el final del array de cuentas de ahorro" << std::endl;
                }
            } else {
                std::cout << "DEBUG: No se encontró el inicio del array de cuentas de ahorro" << std::endl;
            }
        } else {
            std::cout << "DEBUG: No se encontró el campo 'cuentasAhorro' en el JSON" << std::endl;
        }
        
        // Debug: Mostrar resumen de movimientos cargados
        std::cout << "DEBUG: Resumen de titular cargado desde MongoDB:" << std::endl;
        if (titular->getCuentaCorriente()) {
            int movCC = 0;
            if (!titular->getCuentaCorriente()->getMovimientos().vacia()) {
                NodoDoble<Movimiento*>* actual = titular->getCuentaCorriente()->getMovimientos().getCabeza();
                if (actual) {
                    do {
                        movCC++;
                        actual = actual->siguiente;
                    } while (actual != titular->getCuentaCorriente()->getMovimientos().getCabeza());
                }
            }
            std::cout << "  Cuenta Corriente: " << movCC << " movimientos" << std::endl;
        }
        
        if (!titular->getCuentasAhorro().vacia()) {
            NodoDoble<CuentaBancaria*>* nodo = titular->getCuentasAhorro().getCabeza();
            int cuentaNum = 1;
            if (nodo) {
                do {
                    int movAH = 0;
                    if (!nodo->dato->getMovimientos().vacia()) {
                        NodoDoble<Movimiento*>* actual = nodo->dato->getMovimientos().getCabeza();
                        if (actual) {
                            do {
                                movAH++;
                                actual = actual->siguiente;
                            } while (actual != nodo->dato->getMovimientos().getCabeza());
                        }
                    }
                    std::cout << "  Cuenta Ahorro " << cuentaNum << ": " << movAH << " movimientos" << std::endl;
                    nodo = nodo->siguiente;
                    cuentaNum++;
                } while (nodo != titular->getCuentasAhorro().getCabeza());
            }
        }
        
        return titular;
        
    } catch (...) {
        return nullptr;
    }
}

CuentaBancaria* GestorConexion::parsearCuentaDesdeJSON(const std::string& json) {
    try {
        std::string id, tipoCuenta, fechaCreacion;
        float saldo = 0.0f;
        
        // Buscar ID
        size_t pos = json.find("\"id\": \"");
        if (pos != std::string::npos) {
            pos += 7; // Longitud de "id": "
            size_t fin = json.find("\"", pos);
            if (fin != std::string::npos) {
                id = json.substr(pos, fin - pos);
            }
        }
        
        // Buscar saldo
        pos = json.find("\"saldo\": ");
        if (pos != std::string::npos) {
            pos += 9; // Longitud de "saldo": 
            size_t fin = json.find(",", pos);
            if (fin == std::string::npos) fin = json.find("}", pos);
            if (fin != std::string::npos) {
                std::string saldoStr = json.substr(pos, fin - pos);
                saldo = std::stof(saldoStr);
            }
        }
        
        // Buscar tipo de cuenta
        pos = json.find("\"tipoCuenta\": \"");
        if (pos != std::string::npos) {
            pos += 15; // Longitud de "tipoCuenta": "
            size_t fin = json.find("\"", pos);
            if (fin != std::string::npos) {
                tipoCuenta = json.substr(pos, fin - pos);
            }
        }
        
        // Crear cuenta bancaria
        CuentaBancaria* cuenta = new CuentaBancaria();
        cuenta->setID(id);
        cuenta->setSaldo(saldo);
        cuenta->setTipoCuenta(tipoCuenta);
        
        return cuenta;
        
    } catch (...) {
        return nullptr;
    }
}

Movimiento* GestorConexion::parsearMovimientoDesdeJSON(const std::string& json) {
    try {
        float monto = 0.0f;
        bool esTipo = true;
        std::string fechaStr, horaStr;
        
        // Buscar monto
        size_t pos = json.find("\"monto\": ");
        if (pos != std::string::npos) {
            pos += 9; // Longitud de "monto": 
            size_t fin = json.find(",", pos);
            if (fin == std::string::npos) fin = json.find("}", pos);
            if (fin != std::string::npos) {
                std::string montoStr = json.substr(pos, fin - pos);
                monto = std::stof(montoStr);
            }
        }
        
        // Buscar tipo (esDeposito)
        pos = json.find("\"esDeposito\": ");
        if (pos != std::string::npos) {
            pos += 14; // Longitud de "esDeposito": 
            size_t fin = json.find(",", pos);
            if (fin == std::string::npos) fin = json.find("}", pos);
            if (fin != std::string::npos) {
                std::string tipoStr = json.substr(pos, fin - pos);
                esTipo = (tipoStr == "true");
            }
        }
        
        // Buscar fecha
        pos = json.find("\"fecha\": \"");
        if (pos != std::string::npos) {
            pos += 10; // Longitud de "fecha": "
            size_t fin = json.find("\"", pos);
            if (fin != std::string::npos) {
                fechaStr = json.substr(pos, fin - pos);
            }
        }
        
        // Buscar hora
        pos = json.find("\"hora\": \"");
        if (pos != std::string::npos) {
            pos += 9; // Longitud de "hora": "
            size_t fin = json.find("\"", pos);
            if (fin != std::string::npos) {
                horaStr = json.substr(pos, fin - pos);
            }
        }
        
        // Crear movimiento
        Movimiento* movimiento = new Movimiento(monto, esTipo, 1);
        
        // Parsear y asignar fecha (formato: dd/mm/yyyy)
        if (!fechaStr.empty()) {
            size_t pos1 = fechaStr.find("/");
            size_t pos2 = fechaStr.find("/", pos1 + 1);
            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                int dia = std::stoi(fechaStr.substr(0, pos1));
                int mes = std::stoi(fechaStr.substr(pos1 + 1, pos2 - pos1 - 1));
                int anio = std::stoi(fechaStr.substr(pos2 + 1));
                
                Fecha fecha;
                fecha.setDia(dia);
                fecha.setMes(mes);
                Anio anioObj;
                anioObj.setAnio(anio);
                fecha.setAnio(anioObj);
                movimiento->setFechaMov(fecha);
            }
        }
        
        // Parsear y asignar hora (formato: hh:mm:ss)
        if (!horaStr.empty()) {
            size_t pos1 = horaStr.find(":");
            size_t pos2 = horaStr.find(":", pos1 + 1);
            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                int horas = std::stoi(horaStr.substr(0, pos1));
                int minutos = std::stoi(horaStr.substr(pos1 + 1, pos2 - pos1 - 1));
                int segundos = std::stoi(horaStr.substr(pos2 + 1));
                
                Hora hora;
                hora.setHoras(horas);
                hora.setMinutos(minutos);
                hora.setSegundos(segundos);
                movimiento->setHora(hora);
            }
        }
        
        return movimiento;
        
    } catch (...) {
        return nullptr;
    }
}

void GestorConexion::parsearMovimientosParaCuenta(const std::string& json, CuentaBancaria* cuenta) {
    if (!cuenta) return;
    
    try {
        std::cout << "DEBUG: JSON recibido para parsear movimientos:" << std::endl;
        std::cout << json << std::endl;
        std::cout << "DEBUG: ================================" << std::endl;
        
        // Buscar la seccion de movimientos dentro del JSON de la cuenta
        // El formato puede ser: "movimientos":[...] o "movimientos": [...]
        size_t movPos = json.find("\"movimientos\":");
        if (movPos != std::string::npos) {
            std::cout << "DEBUG: Encontró sección movimientos en posición: " << movPos << std::endl;
            
            // Buscar el inicio del array [
            size_t arrayStart = json.find("[", movPos);
            if (arrayStart != std::string::npos) {
                size_t movEnd = json.find("]", arrayStart);
                if (movEnd != std::string::npos) {
                    std::string movimientosJson = json.substr(arrayStart + 1, movEnd - arrayStart - 1);
                    std::cout << "DEBUG: JSON de movimientos extraído: " << movimientosJson << std::endl;
                
                // Parsear cada movimiento
                size_t pos = 0;
                int contador = 0;
                while (pos < movimientosJson.length()) {
                    size_t inicio = movimientosJson.find("{", pos);
                    if (inicio == std::string::npos) break;
                    
                    // Encontrar el final del objeto JSON
                    int braceCount = 0;
                    size_t fin = inicio;
                    for (size_t i = inicio; i < movimientosJson.length(); i++) {
                        if (movimientosJson[i] == '{') braceCount++;
                        else if (movimientosJson[i] == '}') {
                            braceCount--;
                            if (braceCount == 0) {
                                fin = i;
                                break;
                            }
                        }
                    }
                    
                    if (fin > inicio) {
                        std::string movJson = movimientosJson.substr(inicio, fin - inicio + 1);
                        std::cout << "DEBUG: Parseando movimiento " << (contador + 1) << ": " << movJson << std::endl;
                        Movimiento* movimiento = parsearMovimientoDesdeJSON(movJson);
                        if (movimiento) {
                            cuenta->cargarMovimientoSinRecalcular(movimiento);  // ✅ Usar método que NO recalcula
                            contador++;
                            std::cout << "DEBUG: Movimiento cargado exitosamente - Monto: $" << movimiento->getMonto() 
                                      << " Tipo: " << (movimiento->getTipo() ? "Deposito" : "Retiro") << std::endl;
                        } else {
                            std::cout << "DEBUG: Error al parsear movimiento" << std::endl;
                        }
                    }
                    
                    pos = fin + 1;
                }
                std::cout << "DEBUG: Total de movimientos procesados: " << contador << std::endl;
            } else {
                std::cout << "DEBUG: No se encontró el final del array de movimientos" << std::endl;
            }
        } else {
            std::cout << "DEBUG: No se encontró el inicio del array [" << std::endl;
        }
        } else {
            std::cout << "DEBUG: No se encontró la sección 'movimientos' en el JSON" << std::endl;
        }
        
    } catch (...) {
        std::cout << "DEBUG: Error al parsear movimientos" << std::endl;
        // Error silencioso, no detener el procesamiento
    }
}

size_t GestorConexion::encontrarFinObjetoJSON(const std::string& json, size_t inicio) {
    if (inicio >= json.length() || json[inicio] != '{') {
        return std::string::npos;
    }
    
    int nivelLlaves = 0;
    bool dentroString = false;
    bool escape = false;
    
    for (size_t i = inicio; i < json.length(); i++) {
        char c = json[i];
        
        if (escape) {
            escape = false;
            continue;
        }
        
        if (c == '\\') {
            escape = true;
            continue;
        }
        
        if (c == '"') {
            dentroString = !dentroString;
            continue;
        }
        
        if (!dentroString) {
            if (c == '{') {
                nivelLlaves++;
            } else if (c == '}') {
                nivelLlaves--;
                if (nivelLlaves == 0) {
                    return i;
                }
            }
        }
    }
    
    return std::string::npos; // No se encontró el final
}

bool GestorConexion::validarJSON(const std::string& json) {
    int nivelLlaves = 0;
    int nivelCorchetes = 0;
    bool dentroString = false;
    bool escape = false;
    
    for (size_t i = 0; i < json.length(); i++) {
        char c = json[i];
        
        if (escape) {
            escape = false;
            continue;
        }
        
        if (c == '\\') {
            escape = true;
            continue;
        }
        
        if (c == '"') {
            dentroString = !dentroString;
            continue;
        }
        
        if (!dentroString) {
            if (c == '{') {
                nivelLlaves++;
            } else if (c == '}') {
                nivelLlaves--;
                if (nivelLlaves < 0) return false;
            } else if (c == '[') {
                nivelCorchetes++;
            } else if (c == ']') {
                nivelCorchetes--;
                if (nivelCorchetes < 0) return false;
            }
        }
    }
    
    return (nivelLlaves == 0 && nivelCorchetes == 0 && !dentroString);
}

std::vector<Titular*> GestorConexion::parsearTitularesCompletosDesdeJSON(const std::string& jsonArray) {
    std::vector<Titular*> titulares;
    
    try {
        std::cout << "DEBUG: Iniciando parseo de array de titulares" << std::endl;
        std::cout << "DEBUG: Longitud del JSON: " << jsonArray.length() << std::endl;
        
        // Verificar si el JSON está vacío o es un array vacío
        if (jsonArray.empty() || jsonArray == "[]") {
            std::cout << "DEBUG: Array JSON vacío recibido" << std::endl;
            return titulares;
        }
        
        // Buscar el inicio y fin del array
        size_t inicioArray = jsonArray.find('[');
        size_t finArray = jsonArray.rfind(']');
        
        if (inicioArray == std::string::npos || finArray == std::string::npos || finArray <= inicioArray) {
            std::cout << "DEBUG: No se encontraron corchetes válidos del array JSON" << std::endl;
            return titulares;
        }
        
        std::cout << "DEBUG: Array encontrado desde posición " << inicioArray << " hasta " << finArray << std::endl;
        
        // Extraer el contenido del array
        std::string contenidoArray = jsonArray.substr(inicioArray + 1, finArray - inicioArray - 1);
        
        // Eliminar espacios en blanco del inicio y final
        size_t inicio = contenidoArray.find_first_not_of(" \t\n\r");
        if (inicio == std::string::npos) {
            std::cout << "DEBUG: Array JSON contiene solo espacios en blanco" << std::endl;
            return titulares;
        }
        contenidoArray = contenidoArray.substr(inicio);
        
        std::cout << "DEBUG: Contenido del array (primeros 200 chars): " 
                  << contenidoArray.substr(0, 200) << std::endl;
        
        // Intentar método alternativo de parseo si detectamos múltiples objetos
        std::cout << "DEBUG: Intentando método alternativo de parseo..." << std::endl;
        
        // Método alternativo: buscar patrones "},{" para separar objetos
        std::vector<std::string> objetosJSON;
        size_t inicioActual = 0;
        
        // Buscar todas las posiciones donde un objeto termina y otro comienza
        while (inicioActual < contenidoArray.length()) {
            // Saltar espacios al inicio
            while (inicioActual < contenidoArray.length() && 
                   (contenidoArray[inicioActual] == ' ' || contenidoArray[inicioActual] == '\t' || 
                    contenidoArray[inicioActual] == '\n' || contenidoArray[inicioActual] == '\r' || 
                    contenidoArray[inicioActual] == ',')) {
                inicioActual++;
            }
            
            if (inicioActual >= contenidoArray.length()) break;
            
            // Buscar el final de este objeto
            size_t finActual = encontrarFinObjetoJSON(contenidoArray, inicioActual);
            
            if (finActual == std::string::npos || finActual <= inicioActual) {
                std::cout << "DEBUG: No se pudo encontrar fin del objeto en posición " << inicioActual << std::endl;
                break;
            }
            
            std::string objetoJSON = contenidoArray.substr(inicioActual, finActual - inicioActual + 1);
            objetosJSON.push_back(objetoJSON);
            
            std::cout << "DEBUG: Objeto " << objetosJSON.size() << " extraído (longitud: " 
                      << objetoJSON.length() << " chars)" << std::endl;
            
            inicioActual = finActual + 1;
        }
        
        std::cout << "DEBUG: Total de objetos JSON separados: " << objetosJSON.size() << std::endl;
        
        // Procesar cada objeto JSON separado
        for (size_t i = 0; i < objetosJSON.size(); i++) {
            const std::string& titularJSON = objetosJSON[i];
            
            std::cout << "DEBUG: Parseando titular " << (i + 1) << " de " << objetosJSON.size() 
                      << " (JSON de " << titularJSON.length() << " caracteres)" << std::endl;
            
            // Validar el JSON individual
            if (!validarJSON(titularJSON)) {
                std::cout << "DEBUG: ¡ADVERTENCIA! JSON del titular " << (i + 1) << " no está bien formado" << std::endl;
                std::cout << "DEBUG: JSON problemático: " << titularJSON.substr(0, 200) << "..." << std::endl;
                continue;
            }
            
            std::cout << "DEBUG: JSON del titular (primeros 150 chars): " 
                      << titularJSON.substr(0, 150) << std::endl;
            
            // Parsear el titular usando el método existente
            Titular* titular = parsearTitularCompletoDesdeJSON(titularJSON);
            
            if (titular) {
                titulares.push_back(titular);
                std::cout << "DEBUG: ✓ Titular " << (i + 1) << " parseado exitosamente: " 
                          << titular->getPersona().getNombre() << " " 
                          << titular->getPersona().getApellido() 
                          << " (CI: " << titular->getPersona().getCI() << ")" << std::endl;
            } else {
                std::cout << "DEBUG: ✗ Error al parsear titular " << (i + 1) << std::endl;
                
                // Guardar el JSON problemático en un archivo separado
                try {
                    std::ofstream problemFile("debug_problema_titular_" + std::to_string(i + 1) + ".txt");
                    problemFile << titularJSON;
                    problemFile.close();
                    std::cout << "DEBUG: JSON problemático guardado en 'debug_problema_titular_" 
                              << (i + 1) << ".txt'" << std::endl;
                } catch (...) {
                    std::cout << "DEBUG: No se pudo guardar archivo de problema" << std::endl;
                }
            }
        }
        
        std::cout << "DEBUG: Parseo alternativo completado. Total titulares obtenidos: " << titulares.size() << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "DEBUG: Excepción durante el parseo: " << e.what() << std::endl;
        // Limpiar memoria en caso de error
        for (Titular* titular : titulares) {
            delete titular;
        }
        titulares.clear();
    } catch (...) {
        std::cout << "DEBUG: Error desconocido durante el parseo de titulares" << std::endl;
        // Limpiar memoria en caso de error
        for (Titular* titular : titulares) {
            delete titular;
        }
        titulares.clear();
    }
    
    return titulares;
}

std::vector<Titular*> GestorConexion::obtenerTodosTitulares() {
    std::vector<Titular*> titulares;
    
    if (!conectado || !cliente) {
        establecerError("No hay conexion a la base de datos");
        return titulares; // Vector vacío
    }

    try {
        std::string resultado = cliente->obtenerTodosTitulares();
        if (resultado.empty() || resultado.substr(0, 5) == "ERROR") {
            establecerError("Error al obtener titulares desde la base de datos");
            return titulares; // Vector vacío
        }
        
        std::cout << "DEBUG: Respuesta completa del servidor (primeros 500 chars): " 
                  << resultado.substr(0, 500) << std::endl;
        
        // Validar JSON antes de procesarlo
        if (!validarJSON(resultado)) {
            std::cout << "DEBUG: ¡ADVERTENCIA! JSON recibido no está bien formado" << std::endl;
        } else {
            std::cout << "DEBUG: JSON recibido parece estar bien formado" << std::endl;
        }
        
        // Guardar el JSON completo en un archivo para debugging
        try {
            std::ofstream debugFile("debug_json_response.txt");
            debugFile << resultado;
            debugFile.close();
            std::cout << "DEBUG: JSON completo guardado en 'debug_json_response.txt'" << std::endl;
        } catch (...) {
            std::cout << "DEBUG: No se pudo guardar el archivo de debug" << std::endl;
        }
        
        // Usar el nuevo método especializado para parsear arrays
        titulares = parsearTitularesCompletosDesdeJSON(resultado);
        
        std::cout << "Total de titulares parseados exitosamente: " << titulares.size() << std::endl;
        
    } catch (...) {
        establecerError("Error al obtener titulares desde la base de datos");
        // Limpiar titulares ya creados en caso de error
        for (Titular* titular : titulares) {
            delete titular;
        }
        titulares.clear();
    }
    
    return titulares;
}

bool GestorConexion::exportarBaseDatos(const std::string& nombreArchivo) {
    if (!conectado || !cliente) {
        establecerError("No hay conexion activa para exportar base de datos");
        return false;
    }

    std::cout << "Solicitando exportacion completa de la base de datos..." << std::endl;
    
    // Enviar comando al servidor para exportar toda la base de datos
    std::string respuesta = cliente->enviarComando("EXPORT_DATABASE:" + nombreArchivo);
    
    if (respuesta.find("SUCCESS") != std::string::npos) {
        std::cout << "Base de datos exportada exitosamente: " << nombreArchivo << std::endl;
        return true;
    } else {
        establecerError("Error al exportar base de datos: " + respuesta);
        return false;
    }
}

bool GestorConexion::importarBaseDatos(const std::string& nombreArchivo) {
    if (!conectado || !cliente) {
        establecerError("No hay conexion activa para importar base de datos");
        return false;
    }

    std::cout << "Solicitando importacion completa de la base de datos..." << std::endl;
    
    // Enviar comando al servidor para importar toda la base de datos
    std::string respuesta = cliente->enviarComando("IMPORT_DATABASE:" + nombreArchivo);
    
    if (respuesta.find("SUCCESS") != std::string::npos) {
        std::cout << "Base de datos importada exitosamente desde: " << nombreArchivo << std::endl;
        return true;
    } else {
        establecerError("Error al importar base de datos: " + respuesta);
        return false;
    }
}
