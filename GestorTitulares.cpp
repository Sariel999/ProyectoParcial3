/**
 * @file GestorTitulares.cpp
 * @author your name (you@domain.com)
 * @brief Implementacion de la clase GestorTitulares para registro y gestion de cuentas
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "GestorTitulares.h"
#include "Persona.h"
#include "CuentaBancaria.h"
#include "NodoSucursal.h"
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <iomanip>

using namespace std;

GestorTitulares::GestorTitulares(GestorConexion& gestor) : gestorConexion(gestor), gestorBusqueda(gestor) {
}

GestorTitulares::~GestorTitulares() {
}

/**
 * @brief Registra un nuevo titular en el sistema bancario.
 * 
 * @param titulares Lista de titulares del sistema
 * @param arbolTitulares Arbol B+ para busquedas eficientes
 */
void GestorTitulares::registrarTitular(ListaDobleCircular<Titular*>& titulares, BPlusTreeTitulares& arbolTitulares) {
    system("cls");
    cout << "\n--- REGISTRAR TITULAR --- \n" << endl;
    
    // Si estamos conectados, verificar primero en la base de datos
    bool verificarBD = gestorConexion.estaConectado();
    
    string nombre = val.ingresarCadena((char*)"Ingrese nombre:");
    string apellido = val.ingresarCadena((char*)"\nIngrese apellido:");
    for (char& c : nombre) c = toupper(c);
    for (char& c : apellido) c = toupper(c);
    
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula:");
    
    // Verificar localmente
    if (buscarTitularPorCI(titulares, cedula) != nullptr) {
        cout << "\nYa existe un titular registrado con esa cedula (local).\n" << endl;
        system("pause");
        return;
    }
    
    // Verificar en MongoDB si estamos conectados
    if (verificarBD && gestorConexion.validarTitularExistente(cedula)) {
        cout << "\nYa existe un titular con esa cedula en la base de datos.\n" << endl;
        system("pause");
        return;
    }
    
    string telefono = val.ingresarNumeroTelefonico((char*)"\nIngrese telefono:");
    string correo = val.ingresarCorreo((char*)"\nIngrese correo electronico:");

    // Ingreso y validacion de fecha de nacimiento usando ValidacionFecha
    ValidacionFecha valFecha;
    string fechaStr;
    int dia, mes, anio;
    bool fechaValida = false;
    do {
        system("cls");
        cout << "\nIngrese su fecha de nacimiento." << endl;
        valFecha.ingresarFecha(fechaStr, dia, mes, anio);
        if (!valFecha.valoresValidos(dia, mes, anio)) {
            cout << "\nFecha invalida. Intente de nuevo." << endl;
            system("pause");
            continue;
        }
        if (!valFecha.esMayorDeEdad(dia, mes, anio)) {
            cout << "\nNo puede registrarse. Debe ser mayor de edad (18+)." << endl;
            system("pause");
            return;
        }
        fechaValida = true;
    } while (!fechaValida);

    Anio anioObj;
    anioObj.setAnio(anio);
    anioObj.setAnioBisiesto(valFecha.esBisiesto(anio));

    Fecha fechaNacimiento;
    fechaNacimiento.setDia(dia);
    fechaNacimiento.setMes(mes);
    fechaNacimiento.setAnio(anioObj);

    Persona persona(nombre, apellido, cedula, telefono, correo, fechaNacimiento);

    // Crear Titular con la persona
    Titular* nuevo = new Titular(persona);

    // Insertar el nuevo titular en la lista y en el arbol B+
    titulares.insertar(nuevo);
    arbolTitulares.insertar(cedula, nuevo);
    
    // Sincronizar con MongoDB si estamos conectados
    bool exitoMongo = true;
    if (verificarBD) {
        exitoMongo = gestorConexion.sincronizarTitular(nuevo, true);
        if (!exitoMongo) {
            cout << "\nADVERTENCIA: Titular registrado localmente pero no se pudo sincronizar con MongoDB." << endl;
            cout << "Razon: " << gestorConexion.obtenerUltimoError() << endl;
        } else {
            // Sincronizar titular completo
            sincronizarTitularCompleto(nuevo);
        }
    }
    
    arbolTitulares.imprimir();
    std::cout.flush();
    cout << "\nTitular registrado exitosamente." << endl;
    if (verificarBD && exitoMongo) {
        cout << "Sincronizado con base de datos MongoDB." << endl;
    }
    
    Backups backup;
    backup.crearBackup(titulares);
    system("pause");
}

/**
 * @brief Busca un titular por su cedula de identidad (CI).
 * 
 * @param titulares Lista de titulares donde buscar
 * @param ci Cedula de identidad a buscar
 * @return Titular* Puntero al titular encontrado o nullptr si no existe
 */
Titular* GestorTitulares::buscarTitularPorCI(const ListaDobleCircular<Titular*>& titulares, const std::string& ci) {
    // Usar el gestor de busqueda para busqueda solo local
    return gestorBusqueda.buscarTitularLocal(titulares, ci);
}

/**
 * @brief Crea una nueva cuenta bancaria para un titular existente.
 * 
 * @param titulares Lista de titulares del sistema
 * @param listaSucursales Lista de sucursales disponibles
 * @param arbolTitulares Arbol B+ para busquedas eficientes
 */
void GestorTitulares::crearCuenta(ListaDobleCircular<Titular*>& titulares, ListaSucursales& listaSucursales, BPlusTreeTitulares& arbolTitulares) {
    system("cls");
    cout << "\n--- CREAR CUENTA ---\n" << endl;
    
    bool verificarBD = gestorConexion.estaConectado();
    
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular: ");
    Titular* titular = gestorBusqueda.buscarTitularConCarga(titulares, cedula);
    
    if (!titular) {
        cout << "Titular no encontrado en la base de datos." << endl;
        cout << "Razon: " << gestorConexion.obtenerUltimoError() << endl;
        
        cout << "\nOpciones:" << endl;
        cout << "1. Verificar que el titular exista en MongoDB" << endl;
        cout << "2. Verificar la conexion a la base de datos" << endl;
        cout << "3. Registrar el titular primero" << endl;
        
        system("pause");
        return;
    }
    
    // También actualizar en el árbol B+ si se cargó desde MongoDB
    arbolTitulares.insertar(cedula, titular);
    
    cout << "\nSeleccione la sucursal para la cuenta:\n";
    listaSucursales.mostrarSucursales();
    string idSucursal = val.ingresarCodigoSucursal((char*)"\nIngrese el ID de la sucursal: ");
    if (!listaSucursales.existeSucursal(idSucursal)) {
        cout << "\nSucursal no encontrada." << endl;
        system("pause");
        return;
    }
    
    // Actualizar contador de sucursal
    NodoSucursal* nodo = listaSucursales.getCabeza();
    while (nodo != nullptr) {
        if (nodo->sucursal.getIdSucursal() == idSucursal) {
            nodo->sucursal.incrementarContadorCuentas();
            break;
        }
        nodo = nodo->siguiente;
    }
    
    string tipo = val.ingresarCadena((char*)"\nIngrese tipo de cuenta (Corriente/Ahorro): ");
    for (char& c : tipo) c = toupper(c);
    
    CuentaBancaria* nuevaCuenta = new CuentaBancaria(idSucursal);
    nuevaCuenta->setTipoCuenta(tipo);
    
    // Verificar si la cuenta ya existe en MongoDB
    if (verificarBD && gestorConexion.validarCuentaExistente(nuevaCuenta->getID())) {
        cout << "\nEsta cuenta ya existe en la base de datos." << endl;
        delete nuevaCuenta;
        system("pause");
        return;
    }

    bool cuentaCreada = false;
    if (tipo == "CORRIENTE") {
        if (titular->getCuentaCorriente() != nullptr) {
            cout << "\nEste titular ya tiene una cuenta corriente.\n" << endl;
            delete nuevaCuenta;
        } else {
            titular->setCuentaCorriente(nuevaCuenta);
            cuentaCreada = true;
            cout << "\nCuenta corriente creada exitosamente.\n" << endl;
        }
    } else if (tipo == "AHORRO") {
        titular->agregarCuentaAhorro(nuevaCuenta);
        cuentaCreada = true;
        cout << "\nCuenta de ahorro creada exitosamente.\n" << endl;
    } else {
        cout << "\nTipo de cuenta no valido.\n" << endl;
        delete nuevaCuenta;
    }
    
    if (cuentaCreada) {
        // Sincronizar con MongoDB
        bool exitoMongo = true;
        if (verificarBD) {
            exitoMongo = gestorConexion.sincronizarCuenta(nuevaCuenta, cedula, true);
            if (!exitoMongo) {
                cout << "\nADVERTENCIA: Cuenta creada localmente pero no se pudo sincronizar con MongoDB." << endl;
                cout << "Razon: " << gestorConexion.obtenerUltimoError() << endl;
            } else {
                // Sincronizar titular completo con la nueva cuenta
                sincronizarTitularCompleto(titular);
            }
        }
        
        cout << "--- DATOS DEL TITULAR ---" << endl;
        titular->getPersona().imprimir();
        cout << "--- DATOS DE LA CUENTA ---" << endl;
        nuevaCuenta->imprimir();
        
        if (verificarBD && exitoMongo) {
            cout << "\nCuenta sincronizada con base de datos MongoDB." << endl;
        }
    }

    Backups backup;
    backup.crearBackup(titulares);
    system("pause");
}

// NUEVOS METODOS PARA GESTION DE CONEXION MONGODB

/**
 * @brief Configura el gestor para trabajar en modo servidor local sin MongoDB
 */
void GestorTitulares::configurarModoServidor() {
    gestorConexion.configurarComoServidor();
    cout << "GestorTitulares configurado como servidor local." << endl;
    cout << "Funcionara sin sincronizacion con MongoDB." << endl;
    system("pause");
}

/**
 * @brief Configura el gestor para conectarse como cliente a MongoDB
 */
void GestorTitulares::configurarModoCliente() {
    string ip;
    int puerto;
    
    cout << "Ingrese la IP del servidor (localhost): ";
    getline(cin, ip);
    if (ip.empty()) ip = "localhost";
    
    cout << "Ingrese el puerto del servidor (8888): ";
    string puertoStr;
    getline(cin, puertoStr);
    puerto = puertoStr.empty() ? 8888 : stoi(puertoStr);
    
    cout << "Conectando a " << ip << ":" << puerto << "..." << endl;
    
    if (gestorConexion.conectarComoCliente(ip, puerto)) {
        cout << "Conexion establecida exitosamente!" << endl;
        cout << "GestorTitulares sincronizado con MongoDB." << endl;
    } else {
        cout << "Error al conectar con el servidor." << endl;
        cout << "Ultima razon: " << gestorConexion.obtenerUltimoError() << endl;
    }
    system("pause");
}

/**
 * @brief Verifica si el gestor esta conectado a MongoDB
 * @return true si esta conectado, false en caso contrario
 */
bool GestorTitulares::estaConectadoMongoDB() const {
    return gestorConexion.estaConectado();
}

/**
 * @brief Genera un JSON completo del titular con todas sus cuentas y movimientos
 * @param titular Puntero al titular a serializar
 * @return string JSON del titular completo
 */
std::string GestorTitulares::generarJSONTitularCompleto(const Titular* titular) {
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
    
    // Agregar cuentas de ahorro si existen
    json << ",\"cuentasAhorro\":[";
    if (!titular->getCuentasAhorro().vacia()) {
        bool primeraCuenta = true;
        NodoDoble<CuentaBancaria*>* actualCuenta = titular->getCuentasAhorro().getCabeza();
        if (actualCuenta) {
            do {
                if (!primeraCuenta) json << ",";
                json << "{";
                json << "\"id\":\"" << actualCuenta->dato->getID() << "\",";
                json << "\"saldo\":" << actualCuenta->dato->getSaldo() << ",";
                json << "\"fechaCreacion\":\"" << actualCuenta->dato->getFechaCre().getDia() << "/"
                     << actualCuenta->dato->getFechaCre().getMes() << "/"
                     << actualCuenta->dato->getFechaCre().getAnio() << "\",";
                
                // Agregar movimientos de esta cuenta de ahorro
                json << "\"movimientos\":[";
                if (!actualCuenta->dato->getMovimientos().vacia()) {
                    bool primerMovimiento = true;
                    NodoDoble<Movimiento*>* actualMov = actualCuenta->dato->getMovimientos().getCabeza();
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
                        } while (actualMov != actualCuenta->dato->getMovimientos().getCabeza());
                    }
                }
                json << "]";
                json << "}";
                primeraCuenta = false;
                actualCuenta = actualCuenta->siguiente;
            } while (actualCuenta != titular->getCuentasAhorro().getCabeza());
        }
    }
    json << "]";
    json << "}";
    
    return json.str();
}

/**
 * @brief Sincroniza un titular completo con MongoDB
 * @param titular Puntero al titular a sincronizar
 */
void GestorTitulares::sincronizarTitularCompleto(const Titular* titular) {
    if (gestorConexion.estaConectado() && !gestorConexion.estaModoServidor()) {
        gestorConexion.sincronizarTitularCompleto(titular);
    }
}

