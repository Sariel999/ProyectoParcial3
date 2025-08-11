/**
 * @file OperacionesBancarias.cpp
 * @author your name (you@domain.com)
 * @brief Implementacion de la clase OperacionesBancarias para depositos y retiros
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "OperacionesBancarias.h"
#include "NodoDoble.h"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <iomanip>

using namespace std;

OperacionesBancarias::OperacionesBancarias(GestorConexion& gestor) : gestorConexion(gestor), gestorBusqueda(gestor) {
}

OperacionesBancarias::~OperacionesBancarias() {
}

/**
 * @brief Busca un titular por su cedula de identidad (CI).
 * 
 * @param titulares Lista de titulares donde buscar
 * @param ci Cedula de identidad a buscar
 * @return Titular* Puntero al titular encontrado o nullptr si no existe
 */
Titular* OperacionesBancarias::buscarTitularPorCI(const ListaDobleCircular<Titular*>& titulares, const std::string& ci) {
    // Usar el gestor de busqueda para busqueda solo local
    return gestorBusqueda.buscarTitularLocal(titulares, ci);
}

/**
 * @brief Permite al usuario seleccionar una cuenta del titular mediante numero de cuenta
 * 
 * @param titular Titular propietario de las cuentas
 * @return CuentaBancaria* Cuenta seleccionada o nullptr si no tiene cuentas
 */
CuentaBancaria* OperacionesBancarias::seleccionarCuenta(Titular* titular) {
    cout << "\nCuentas disponibles:" << endl;
    
    bool tieneCuentaCorriente = titular->getCuentaCorriente() != nullptr;
    bool tieneCuentasAhorro = !titular->getCuentasAhorro().vacia();
    
    if (!tieneCuentaCorriente && !tieneCuentasAhorro) {
        cout << "Este titular no tiene cuentas registradas." << endl;
        return nullptr;
    }
    
    // Mostrar cuenta corriente si existe
    if (tieneCuentaCorriente) {
        cout << "- Cuenta Corriente - ID: " << titular->getCuentaCorriente()->getID() << endl;
    }
    
    // Mostrar cuentas de ahorro si existen
    if (tieneCuentasAhorro) {
        for (CuentaBancaria* cuenta : titular->getCuentasAhorro()) {
            cout << "- Cuenta de Ahorro - ID: " << cuenta->getID() << endl;
        }
    }
    
    // Solicitar numero de cuenta con validacion
    string numeroCuenta;
    bool cuentaValida = false;
    /* No se puede cambia a for-each porque no recorre una estructura de datos
    solo esta repitiendo un proceso para validar el numero de cuenta*/
    do {
        numeroCuenta = val.ingresarNumeros((char*)"Ingrese numero de cuenta: ");
        
        // Validar que no este vacio
        if (numeroCuenta.empty()) {
            cout << "\nEl numero de cuenta no puede estar vacio. Intente nuevamente." << endl;
            continue;
        }
        
        // Validar longitud minima (debe tener al menos algunos digitos)
        if (numeroCuenta.length() < 8) {
            cout << "\nEl numero de cuenta debe tener al menos 8 digitos. Intente nuevamente." << endl;
            continue;
        }
        
        cuentaValida = true;
    } while (!cuentaValida);
    
    // Buscar en cuenta corriente
    if (tieneCuentaCorriente && titular->getCuentaCorriente()->getID() == numeroCuenta) {
        return titular->getCuentaCorriente();
    }
    
    // Buscar en cuentas de ahorro
    if (tieneCuentasAhorro) {
        for (CuentaBancaria* cuenta : titular->getCuentasAhorro()) {
            if (cuenta->getID() == numeroCuenta) {
                return cuenta;
            }
        }
    }
    
    cout << "\nNumero de cuenta no encontrado." << endl;
    return nullptr;
}

/**
 * @brief Realiza un deposito en una cuenta bancaria de un titular.
 * 
 * @param titulares Lista de titulares del sistema
 */
void OperacionesBancarias::realizarDeposito(ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- REALIZAR DEPOSITO ---" << endl;
    
    // Verificar conexion con MongoDB
    bool verificarBD = gestorConexion.estaConectado();
    if (!verificarBD) {
        cout << "\nNo hay conexion con MongoDB. No se pueden realizar operaciones." << endl;
        system("pause");
        return;
    }
    
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular: ");
    
    // SIEMPRE obtener datos frescos desde MongoDB
    Titular* titular = gestorBusqueda.obtenerTitularFresco(cedula);
    if (!titular) {
        cout << "\nTitular no encontrado." << endl;
        system("pause");
        return;
    }
    
    // Debug: Verificar movimientos del titular antes de deposito
    cout << "DEBUG: Movimientos antes de deposito:" << endl;
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
        cout << "  Cuenta Corriente: " << movCC << " movimientos" << endl;
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
                cout << "  Cuenta Ahorro " << cuentaNum << ": " << movAH << " movimientos" << endl;
                nodo = nodo->siguiente;
                cuentaNum++;
            } while (nodo != titular->getCuentasAhorro().getCabeza());
        }
    }
    
    CuentaBancaria* cuenta = seleccionarCuenta(titular);
    if (!cuenta) {
        cout << "\nNo se pudo seleccionar una cuenta." << endl;
        system("pause");
        delete titular;
        return;
    }
    
    float monto = val.ingresarMonto((char*)"\nIngrese el monto a depositar: ");
    if (monto <= 0) {
        cout << "\nEl monto debe ser mayor a 0." << endl;
        system("pause");
        delete titular;
        return;
    }
    
    if (monto < 10.0) {
        cout << "\nEl monto minimo de deposito es $10." << endl;
        system("pause");
        delete titular;
        return;
    }
    if (monto > 10000.0) {
        cout << "\nEl monto maximo de deposito es $10,000." << endl;
        system("pause");
        delete titular;
        return;
    }
    
    // Obtener el numero de movimiento
    int numMov = 1;
    if (!cuenta->getMovimientos().vacia()) {
        numMov = cuenta->getMovimientos().getCabeza()->anterior->dato->getNumeroMovimiento() + 1;
    }
    
    // Guardar saldo anterior para mostrar información
    float saldoAnterior = cuenta->getSaldo();
    
    Movimiento* mov = new Movimiento(monto, true, numMov);
    cuenta->agregarMovimiento(mov);  // Ya actualiza el saldo
    
    // Sincronizar con MongoDB
    bool exitoMongo = true;
    if (verificarBD) {
        exitoMongo = gestorConexion.sincronizarCuenta(cuenta, cedula, false);
        if (exitoMongo) {
            exitoMongo = gestorConexion.sincronizarMovimiento(mov, cuenta->getID());
        }
        
        if (!exitoMongo) {
            cout << "\nADVERTENCIA: Operacion realizada localmente pero no se pudo sincronizar con MongoDB." << endl;
            cout << "Razon: " << gestorConexion.obtenerUltimoError() << endl;
        } else {
            // Sincronizar titular completo actualizado
            sincronizarTitularCompleto(titular);
        }
    }
    
    cout << "\nDeposito realizado exitosamente." << endl;
    cout << "Saldo anterior: $" << fixed << setprecision(2) << saldoAnterior << endl;
    cout << "Monto depositado: $" << fixed << setprecision(2) << monto << endl;
    cout << "Nuevo saldo: $" << fixed << setprecision(2) << cuenta->getSaldo() << endl;
    
    if (verificarBD && exitoMongo) {
        cout << "Transaccion sincronizada con base de datos MongoDB." << endl;
    }
    
    Backups backup;
    backup.crearBackup(titulares);
    
    // Limpiar memoria del titular fresco
    delete titular;
    
    system("pause");
}

/**
 * @brief Realiza un retiro de una cuenta bancaria de un titular.
 * 
 * @param titulares Lista de titulares del sistema
 */
void OperacionesBancarias::realizarRetiro(ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- REALIZAR RETIRO ---" << endl;
    
    // Verificar conexion con MongoDB
    bool verificarBD = gestorConexion.estaConectado();
    if (!verificarBD) {
        cout << "\nNo hay conexion con MongoDB. No se pueden realizar operaciones." << endl;
        system("pause");
        return;
    }
    
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular: ");
    
    // SIEMPRE obtener datos frescos desde MongoDB
    Titular* titular = gestorBusqueda.obtenerTitularFresco(cedula);
    if (!titular) {
        cout << "\nTitular no encontrado." << endl;
        system("pause");
        return;
    }
    
    // Debug: Verificar movimientos del titular antes de retiro
    cout << "DEBUG: Movimientos antes de retiro:" << endl;
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
        cout << "  Cuenta Corriente: " << movCC << " movimientos" << endl;
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
                cout << "  Cuenta Ahorro " << cuentaNum << ": " << movAH << " movimientos" << endl;
                nodo = nodo->siguiente;
                cuentaNum++;
            } while (nodo != titular->getCuentasAhorro().getCabeza());
        }
    }
    
    CuentaBancaria* cuenta = seleccionarCuenta(titular);
    if (!cuenta) {
        cout << "\nNo se pudo seleccionar una cuenta." << endl;
        system("pause");
        delete titular;
        return;
    }
    
    cout << "\nSaldo actual: $" << cuenta->getSaldo() << endl;
    float monto = val.ingresarMonto((char*)"\nIngrese el monto a retirar: ");
    
    if (monto <= 0) {
        cout << "\nEl monto debe ser mayor a 0." << endl;
        system("pause");
        delete titular;
        return;
    }
    
    if (monto < 10.0) {
        cout << "\nEl monto minimo de retiro es $10." << endl;
        system("pause");
        delete titular;
        return;
    }
    
    // Guardar saldo anterior para verificación y mostrar información
    float saldoAnterior = cuenta->getSaldo();
    
    cout << "DEBUG: Saldo antes del retiro: $" << saldoAnterior << endl;
    cout << "DEBUG: Monto a retirar: $" << monto << endl;
    
    if (saldoAnterior < monto) {
        cout << "\nSaldo insuficiente para realizar el retiro." << endl;
        system("pause");
        delete titular;
        return;
    }
    
    // Obtener el numero de movimiento
    int numMov = 1;
    if (!cuenta->getMovimientos().vacia()) {
        numMov = cuenta->getMovimientos().getCabeza()->anterior->dato->getNumeroMovimiento() + 1;
    }
    
    cout << "DEBUG: Creando movimiento de retiro..." << endl;
    Movimiento* mov = new Movimiento(monto, false, numMov);
    
    cout << "DEBUG: Saldo antes de agregarMovimiento: $" << cuenta->getSaldo() << endl;
    cuenta->agregarMovimiento(mov);  // Ya actualiza el saldo
    cout << "DEBUG: Saldo después de agregarMovimiento: $" << cuenta->getSaldo() << endl;
    
    // Sincronizar con MongoDB
    bool exitoMongo = true;
    if (verificarBD) {
        exitoMongo = gestorConexion.sincronizarCuenta(cuenta, cedula, false);
        if (exitoMongo) {
            exitoMongo = gestorConexion.sincronizarMovimiento(mov, cuenta->getID());
        }
        
        if (!exitoMongo) {
            cout << "\nADVERTENCIA: Operacion realizada localmente pero no se pudo sincronizar con MongoDB." << endl;
            cout << "Razon: " << gestorConexion.obtenerUltimoError() << endl;
        } else {
            // Sincronizar titular completo actualizado
            sincronizarTitularCompleto(titular);
        }
    }
    
    cout << "\nRetiro realizado exitosamente." << endl;
    cout << "Saldo anterior: $" << fixed << setprecision(2) << saldoAnterior << endl;
    cout << "Monto retirado: $" << fixed << setprecision(2) << monto << endl;
    cout << "Nuevo saldo: $" << fixed << setprecision(2) << cuenta->getSaldo() << endl;
    
    if (verificarBD && exitoMongo) {
        cout << "Transaccion sincronizada con base de datos MongoDB." << endl;
    }
    
    Backups backup;
    backup.crearBackup(titulares);
    
    // Limpiar memoria del titular fresco
    delete titular;
    
    system("pause");
}

/**
 * @brief Sincroniza un titular completo con MongoDB
 * @param titular Puntero al titular a sincronizar
 */
void OperacionesBancarias::sincronizarTitularCompleto(const Titular* titular) {
    if (gestorConexion.estaConectado() && !gestorConexion.estaModoServidor()) {
        gestorConexion.sincronizarTitularCompleto(titular);
    }
}