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

using namespace std;

OperacionesBancarias::OperacionesBancarias() {
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
    if (titulares.vacia()) {
        return nullptr;
    }
    
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual == nullptr) {
        return nullptr;
    }
    
    for (Titular* titular : titulares) {
        if (titular->getPersona().getCI() == ci) {
            return titular;
        }
    }
    
    return nullptr;
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
    solo esta repitiendo un proceso para validar el numero de cuenta*/
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
    if (titulares.vacia()) {
        system("cls");
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    system("cls");
    cout << "\n--- REALIZAR DEPOSITO ---" << endl;
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular: ");
    Titular* titular = buscarTitularPorCI(titulares, cedula);
    if (!titular) {
        cout << "\nTitular no encontrado." << endl;
        system("pause");
        return;
    }
    
    CuentaBancaria* cuenta = seleccionarCuenta(titular);
    if (!cuenta) {
        cout << "\nNo se pudo seleccionar una cuenta." << endl;
        system("pause");
        return;
    }
    
    float monto = val.ingresarMonto((char*)"\nIngrese el monto a depositar: ");
    if (monto <= 0) {
        cout << "\nEl monto debe ser mayor a 0." << endl;
        system("pause");
        return;
    }
    
    if (monto < 10.0) {
        cout << "\nEl monto minimo de deposito es $10." << endl;
        system("pause");
        return;
    }
    if (monto > 10000.0) {
        cout << "\nEl monto maximo de deposito es $10,000." << endl;
        system("pause");
        return;
    }
    
    // Obtener el numero de movimiento
    int numMov = 1;
    if (!cuenta->getMovimientos().vacia()) {
        numMov = cuenta->getMovimientos().getCabeza()->anterior->dato->getNumeroMovimiento() + 1;
    }
    
    Movimiento* mov = new Movimiento(monto, true, numMov);
    cuenta->agregarMovimiento(mov);  // Ya actualiza el saldo
    
    cout << "\nDeposito realizado exitosamente.\n" << endl;
    cout << "Nuevo saldo: $" << cuenta->getSaldo() << endl;
    
    Backups backup;
    backup.crearBackup(titulares);
    system("pause");
}

/**
 * @brief Realiza un retiro de una cuenta bancaria de un titular.
 * 
 * @param titulares Lista de titulares del sistema
 */
void OperacionesBancarias::realizarRetiro(ListaDobleCircular<Titular*>& titulares) {
    if (titulares.vacia()) {
        system("cls");
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    system("cls");
    cout << "\n--- REALIZAR RETIRO ---" << endl;
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular: ");
    Titular* titular = buscarTitularPorCI(titulares, cedula);
    if (!titular) {
        cout << "\nTitular no encontrado." << endl;
        system("pause");
        return;
    }
    
    CuentaBancaria* cuenta = seleccionarCuenta(titular);
    if (!cuenta) {
        cout << "\nNo se pudo seleccionar una cuenta." << endl;
        system("pause");
        return;
    }
    
    cout << "\nSaldo actual: $" << cuenta->getSaldo() << endl;
    float monto = val.ingresarMonto((char*)"\nIngrese el monto a retirar: ");
    
    if (monto <= 0) {
        cout << "\nEl monto debe ser mayor a 0." << endl;
        system("pause");
        return;
    }
    
    if (monto < 10.0) {
        cout << "\nEl monto minimo de retiro es $10." << endl;
        system("pause");
        return;
    }
    
    if (cuenta->getSaldo() < monto) {
        cout << "\nSaldo insuficiente para realizar el retiro." << endl;
        system("pause");
        return;
    }
    
    // Obtener el numero de movimiento
    int numMov = 1;
    if (!cuenta->getMovimientos().vacia()) {
        numMov = cuenta->getMovimientos().getCabeza()->anterior->dato->getNumeroMovimiento() + 1;
    }
    
    Movimiento* mov = new Movimiento(monto, false, numMov);
    cuenta->agregarMovimiento(mov);  // Ya actualiza el saldo
    
    cout << "\nRetiro realizado exitosamente.\n" << endl;
    cout << "Nuevo saldo: $" << cuenta->getSaldo() << endl;
    
    Backups backup;
    backup.crearBackup(titulares);
    system("pause");
}
