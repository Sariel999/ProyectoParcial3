/**
 * @file MenuBusquedasBinarias.cpp
 * @author your name (you@domain.com)
 * @brief Implementacion de la clase MenuBusquedasBinarias
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "MenuBusquedasBinarias.h"
#include "NodoDoble.h"
#include <iostream>
#include <cstdlib>
#include <cctype>

using namespace std;

MenuBusquedasBinarias::MenuBusquedasBinarias() {
}

MenuBusquedasBinarias::~MenuBusquedasBinarias() {
}

/**
 * @brief Busca el primer deposito mayor o igual a un monto especificado
 * @param titulares Lista de titulares del sistema
 */
void MenuBusquedasBinarias::buscarDepositoMayorIgual(const ListaDobleCircular<Titular*>& titulares) {
    cout << "\n--- Buscar primer deposito mayor o igual a un monto ---\n";
    cout << " Esta funcion permite buscar el primer deposito en una cuenta bancaria que sea mayor o igual a un monto especificado.\n";
    
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular:");
    Titular* titular = encontrarTitularPorCI(titulares, cedula);
    if (!titular) {
        cout << "Titular no encontrado.\n"; 
        system("pause"); 
        return;
    }
    
    string tipo = val.ingresarCadena((char*)"\nTipo de cuenta (Corriente/Ahorro):");
    for (char& c : tipo) c = toupper(c);
    
    string idCuenta = val.ingresarNumeros((char*)"\nIngrese ID de la cuenta:");
    CuentaBancaria* cuenta = encontrarCuentaPorTipo(titular, tipo, idCuenta);
    if (!cuenta) {
        cout << "Cuenta no encontrada o ID incorrecto.\n"; 
        system("pause"); 
        return;
    }
    
    float monto = val.ingresarMonto((char*)"\nIngrese monto minimo a buscar:");
    Movimiento* mov = buscador.primerDepositoMayorIgual(cuenta->getMovimientos(), monto);
    if (mov) {
        cout << "\n El primer deposito mayor o igual al monto " << monto << " es :\n";
        mov->imprimir();
    } else {
        cout << "\nNo se encontro ningun deposito mayor o igual a ese monto.";
    }
    system("pause");
}

/**
 * @brief Calcula el deposito mensual minimo para alcanzar una meta de ahorro
 * @param titulares Lista de titulares del sistema
 */
void MenuBusquedasBinarias::calcularDepositoMinimoMeta(const ListaDobleCircular<Titular*>& titulares) {
    cout << "\n--- Deposito minimo mensual para meta de ahorro ---\n";
    cout << " Esta funcion permite calcular el deposito mensual minimo necesario para alcanzar una meta de ahorro en un plazo determinado inciando con el saldo actual de la cuenta .\n";
    
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular:");
    Titular* titular = encontrarTitularPorCI(titulares, cedula);
    if (!titular) {
        cout << "Titular no encontrado.\n"; 
        system("pause"); 
        return;
    }
    
    if (titular->getCuentasAhorro().vacia()) {
        cout << "El titular no tiene cuentas de ahorro.\n"; 
        system("pause"); 
        return;
    }
    
    string idCuenta = val.ingresarNumeros((char*)"\nIngrese ID de la cuenta de ahorro:");
    CuentaBancaria* cuentaAhorro = nullptr;
    NodoDoble<CuentaBancaria*>* actual = titular->getCuentasAhorro().getCabeza();
    if (actual) {
        do {
            if (actual->dato->getID() == idCuenta) {
                cuentaAhorro = actual->dato;
                break;
            }
            actual = actual->siguiente;
        } while (actual != titular->getCuentasAhorro().getCabeza());
    }
    
    if (!cuentaAhorro) {
        cout << "Cuenta de ahorro no encontrada.\n"; 
        system("pause"); 
        return;
    }

    float saldoInicial = cuentaAhorro->getSaldo();
    cout << "Saldo inicial de la cuenta: $" << saldoInicial << endl;
    
    float saldoMeta;
    int meses;
    do {
        cout << "Saldo meta: "; 
        cin >> saldoMeta;
        if (saldoMeta <= saldoInicial) {
            cout << "El saldo meta debe ser mayor al saldo actual de la cuenta. Intente de nuevo.\n";
        }
    } while (saldoMeta <= saldoInicial);
    
    cout << "Meses para alcanzar la meta: "; 
    cin >> meses;
    
    int deposito = buscador.depositoMinimoParaMeta(saldoInicial, saldoMeta, meses);
    cout << "Deposito mensual minimo necesario: $" << deposito << endl;
    system("pause");
}

/**
 * @brief Busca el primer titular cuyo CI sea mayor o igual al ingresado
 * @param titulares Lista de titulares del sistema
 */
void MenuBusquedasBinarias::buscarTitularPorCI(ListaDobleCircular<Titular*>& titulares) {
    cout << "\n--- Buscar primer titular por CI ---\n";
    cout << " Esta funcion permite buscar el primer titular cuyo CI sea mayor o igual al ingresado.\n";
    
    string ci = val.ingresarCedula((char*)"Ingrese CI a buscar:");
    Titular* t = buscador.primerTitularCIMayorIgual(titulares, ci);
    if (t) {
        cout << "Primer titular con CI >= " << ci << ":\n";
        t->getPersona().imprimir();
    } else {
        cout << "No se encontro ningun titular con ese CI o mayor.\n";
    }
    system("pause");
}

/**
 * @brief Busca el primer titular por anio de nacimiento mayor o igual al especificado
 * @param titulares Lista de titulares del sistema
 */
void MenuBusquedasBinarias::buscarTitularPorAnioNacimiento(ListaDobleCircular<Titular*>& titulares) {
    cout << "\n--- Buscar primer titular por anio de nacimiento (mayor o igual) ---\n";
    int anio;
    cout << "Ingrese anio de nacimiento a buscar: "; 
    cin >> anio;
    
    Titular* t = buscador.primerTitularAnioNacimientoMayorIgual(titulares, anio);
    if (t) {
        cout << "Primer titular con anio de nacimiento >= " << anio << ":\n";
        t->getPersona().imprimir();
    } else {
        cout << "No se encontro ningun titular con ese anio o mayor.\n";
    }
    system("pause");
}

/**
 * @brief Busca la sucursal mas cercana a las coordenadas especificadas
 * @param listaSucursales Lista de sucursales disponibles
 */
void MenuBusquedasBinarias::buscarSucursalMasCercana(const ListaSucursales& listaSucursales) {
    cout << "\n--- Buscar sucursal mas cercana ---\n";
    cout << " Esta funcion encuentra la sucursal mas cercana a las coordenadas geograficas ingresadas.\n";
    
    float latUsuario = val.ingresarCoordenada((char*)"\nIngrese latitud:", true);
    float lonUsuario = val.ingresarCoordenada((char*)"\nIngrese longitud:", false);
    
    Sucursal* sucursal = buscador.sucursalMasCercana(listaSucursales.getCabeza(), latUsuario, lonUsuario);
    if (sucursal) {
        cout << "\nSucursal mas cercana:\n";
        sucursal->imprimir();
        
        // Crear cita para el siguiente dia laborable
        FechaHora fechaActual;
        fechaActual.actualizarFechaHora(); // Obtener fecha y hora actual
        Cita cita(sucursal, fechaActual);
        cita.mostrar();
        
    } else {
        cout << "No se encontraron sucursales.\n";
    }
    system("pause");
}

/**
 * @brief Metodo auxiliar para encontrar un titular por CI
 * @param titulares Lista de titulares
 * @param ci Cedula de identidad a buscar
 * @return Titular* Puntero al titular encontrado o nullptr
 */
Titular* MenuBusquedasBinarias::encontrarTitularPorCI(const ListaDobleCircular<Titular*>& titulares, const std::string& ci) {
    if (titulares.vacia()) return nullptr;
    
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    do {
        if (actual->dato->getPersona().getCI() == ci) {
            return actual->dato;
        }
        actual = actual->siguiente;
    } while (actual != titulares.getCabeza());
    
    return nullptr;
}

/**
 * @brief Metodo auxiliar para encontrar una cuenta bancaria por tipo e ID
 * @param titular Titular propietario de las cuentas
 * @param tipo Tipo de cuenta (CORRIENTE/AHORRO)
 * @param idCuenta ID de la cuenta a buscar
 * @return CuentaBancaria* Puntero a la cuenta encontrada o nullptr
 */
CuentaBancaria* MenuBusquedasBinarias::encontrarCuentaPorTipo(Titular* titular, const std::string& tipo, const std::string& idCuenta) {
    if (tipo == "CORRIENTE") {
        CuentaBancaria* cuenta = titular->getCuentaCorriente();
        if (!cuenta || cuenta->getID() != idCuenta) {
            return nullptr;
        }
        return cuenta;
    } else if (tipo == "AHORRO") {
        NodoDoble<CuentaBancaria*>* actual = titular->getCuentasAhorro().getCabeza();
        if (actual) {
            do {
                if (actual->dato->getID() == idCuenta) {
                    return actual->dato;
                }
                actual = actual->siguiente;
            } while (actual != titular->getCuentasAhorro().getCabeza());
        }
        return nullptr;
    }
    return nullptr;
}
