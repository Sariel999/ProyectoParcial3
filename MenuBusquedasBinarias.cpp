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
#include "GestorBusquedaMongo.h"
#include "NodoDoble.h"
#include <iostream>
#include <cstdlib>
#include <cctype>

using namespace std;

MenuBusquedasBinarias::MenuBusquedasBinarias() : gestorBusquedaMongo(nullptr) {
}

MenuBusquedasBinarias::~MenuBusquedasBinarias() {
}

void MenuBusquedasBinarias::setGestorBusquedaMongo(GestorBusquedaMongo* gestorMongo) {
    gestorBusquedaMongo = gestorMongo;
}

/**
 * @brief Obtiene un titular con datos actualizados desde MongoDB o busqueda local
 * 
 * @param cedula Cedula del titular a buscar
 * @param titulares Lista de titulares local como fallback
 * @return Titular* Puntero al titular encontrado o nullptr
 */
Titular* MenuBusquedasBinarias::obtenerTitularActualizado(const string& cedula, const ListaDobleCircular<Titular*>& titulares) {
    // Intentar obtener datos frescos desde MongoDB
    if (gestorBusquedaMongo) {
        cout << "Obteniendo datos actualizados desde MongoDB..." << endl;
        Titular* titularFresco = gestorBusquedaMongo->obtenerTitularFresco(cedula);
        
        if (titularFresco) {
            cout << "Titular encontrado en base de datos con datos actualizados." << endl;
            return titularFresco;
        } else {
            cout << "Titular no encontrado en MongoDB. Buscando en datos locales..." << endl;
        }
    }
    
    // Fallback: buscar en lista local
    return encontrarTitularPorCI(titulares, cedula);
}

/**
 * @brief Busca el primer deposito mayor o igual a un monto especificado
 * @param titulares Lista de titulares del sistema
 */
void MenuBusquedasBinarias::buscarDepositoMayorIgual(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
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
    system("cls");
    cout << "\n--- Deposito minimo mensual para meta de ahorro ---\n";
    cout << " Esta funcion permite calcular el deposito mensual minimo necesario para alcanzar una meta de ahorro en un plazo maximo de 5 anios (60 meses) inciando con el saldo actual de la cuenta .\n";
    
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
        saldoMeta=val.ingresarMonto("\nSaldo meta: ");
        if (saldoMeta <= saldoInicial) {
            cout << "\nEl saldo meta debe ser mayor al saldo actual de la cuenta. Intente de nuevo.\n";
        }
    } while (saldoMeta <= saldoInicial);
    
    cout << "Meses para alcanzar la meta: "; 
    do
    {
        meses = val.ingresarEntero("\nMeses para alcanzar la meta: ");
        if (meses <= 0) {
            cout << "\nEl numero de meses debe ser un entero positivo. Intente de nuevo.\n";
        }
        if(meses > 60) {
            cout << "\nEl numero de meses no debe ser mayor a 120. Intente de nuevo.\n";
        }
    } while (meses <= 0 || meses > 60);
    
    int deposito = buscador.depositoMinimoParaMeta(saldoInicial, saldoMeta, meses);
    cout << "Deposito mensual minimo necesario: $" << deposito << endl;
    system("pause");
}

/**
 * @brief Busca el primer titular cuyo CI sea mayor o igual al ingresado
 * @param titulares Lista de titulares del sistema
 */
void MenuBusquedasBinarias::buscarTitularPorCI(ListaDobleCircular<Titular*>& titulares) {
    system("cls");
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
    system("cls");
    cout << "\n--- Buscar primer titular por anio de nacimiento (mayor o igual) ---\n";
    int anio;
    do{
        anio = val.ingresarEntero("\nIngrese anio de nacimiento a buscar: ");
        if(anio < 1900 || anio > 2006){
            cout << "\nAnio invalido. Intente de nuevo.\n";
        }
    }while( anio < 1900 || anio > 2025);
    
    Titular* t = buscador.primerTitularAnioNacimientoMayorIgual(titulares, anio);
    if (t) {
        cout << "\nPrimer titular con anio de nacimiento >= " << anio << ":\n";
        t->getPersona().imprimir();
    } else {
        cout << "\nNo se encontro ningun titular con ese anio o mayor.\n";
    }
    system("pause");
}

/**
 * @brief Busca la sucursal mas cercana a las coordenadas especificadas
 * @param listaSucursales Lista de sucursales disponibles
 */
void MenuBusquedasBinarias::buscarSucursalMasCercana(const ListaSucursales& listaSucursales) {
    system("cls");
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
 * @brief Busca el primer deposito mayor o igual a un monto especificado usando MongoDB
 * @param titulares Lista de titulares del sistema (usado como fallback)
 */
void MenuBusquedasBinarias::buscarDepositoMayorIgualMongoDB(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- Buscar primer deposito mayor o igual a un monto (MongoDB) ---\n";
    cout << " Esta funcion permite buscar el primer deposito en una cuenta bancaria que sea mayor o igual a un monto especificado usando datos actualizados desde MongoDB.\n";
    
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular:");
    
    // Obtener titular con datos actualizados desde MongoDB
    Titular* titular = obtenerTitularActualizado(cedula, titulares);
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
        cout << "\n El primer deposito mayor o igual al monto " << monto << " es (datos actualizados):\n";
        mov->imprimir();
    } else {
        cout << "\nNo se encontro ningun deposito mayor o igual a ese monto.";
    }
    system("pause");
}

/**
 * @brief Calcula el deposito mensual minimo para alcanzar una meta de ahorro usando MongoDB
 * @param titulares Lista de titulares del sistema (usado como fallback)
 */
void MenuBusquedasBinarias::calcularDepositoMinimoMetaMongoDB(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- Deposito minimo mensual para meta de ahorro (MongoDB) ---\n";
    cout << " Esta funcion permite calcular el deposito mensual minimo necesario para alcanzar una meta de ahorro usando datos actualizados desde MongoDB.\n";
    
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular:");
    
    // Obtener titular con datos actualizados desde MongoDB
    Titular* titular = obtenerTitularActualizado(cedula, titulares);
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
    cout << "Saldo inicial de la cuenta (datos actualizados): $" << saldoInicial << endl;
    
    float saldoMeta;
    int meses;
    do {
        saldoMeta=val.ingresarMonto("\nSaldo meta: ");
        if (saldoMeta <= saldoInicial) {
            cout << "\nEl saldo meta debe ser mayor al saldo actual de la cuenta. Intente de nuevo.\n";
        }
    } while (saldoMeta <= saldoInicial);
    
    cout << "Meses para alcanzar la meta: "; 
    do
    {
        meses = val.ingresarEntero("\nMeses para alcanzar la meta: ");
        if (meses <= 0) {
            cout << "\nEl numero de meses debe ser un entero positivo. Intente de nuevo.\n";
        }
        if(meses > 60) {
            cout << "\nEl numero de meses no debe ser mayor a 60. Intente de nuevo.\n";
        }
    } while (meses <= 0 || meses > 60);
    
    int deposito = buscador.depositoMinimoParaMeta(saldoInicial, saldoMeta, meses);
    cout << "Deposito mensual minimo necesario (con datos actualizados): $" << deposito << endl;
    system("pause");
}

/**
 * @brief Busca el primer titular cuyo CI sea mayor o igual al ingresado usando MongoDB
 * @param titulares Lista de titulares del sistema (usado como fallback)
 */
void MenuBusquedasBinarias::buscarTitularPorCIMongoDB(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- Buscar primer titular por CI (MongoDB) ---\n";
    cout << " Esta funcion permite buscar el primer titular cuyo CI sea mayor o igual al ingresado usando datos actualizados desde MongoDB.\n";
    
    string ci = val.ingresarCedula((char*)"Ingrese CI a buscar:");
    
    // Intentar obtener titular directamente desde MongoDB
    if (gestorBusquedaMongo) {
        cout << "Buscando titular en MongoDB..." << endl;
        Titular* titular = gestorBusquedaMongo->obtenerTitularFresco(ci);
        
        if (titular) {
            cout << "Titular encontrado con CI exacto: " << ci << " (datos actualizados):\n";
            titular->getPersona().imprimir();
            system("pause");
            return;
        } else {
            cout << "No se encontro titular con CI exacto en MongoDB. Buscando en datos locales..." << endl;
        }
    }
    
    // Fallback: buscar en lista local
    Titular* t = buscador.primerTitularCIMayorIgual(const_cast<ListaDobleCircular<Titular*>&>(titulares), ci);
    if (t) {
        cout << "Primer titular con CI >= " << ci << " (datos locales):\n";
        t->getPersona().imprimir();
    } else {
        cout << "No se encontro ningun titular con ese CI o mayor.\n";
    }
    system("pause");
}

/**
 * @brief Busca el primer titular por anio de nacimiento mayor o igual al especificado usando MongoDB
 * @param titulares Lista de titulares del sistema (usado como fallback)
 */
void MenuBusquedasBinarias::buscarTitularPorAnioNacimientoMongoDB(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- Buscar primer titular por anio de nacimiento (MongoDB) ---\n";
    cout << " Esta funcion busca titulares por anio de nacimiento usando todos los datos completos desde MongoDB.\n";
    
    int anio;
    do{
        anio = val.ingresarEntero("\nIngrese anio de nacimiento a buscar: ");
        if(anio < 1900 || anio > 2006){
            cout << "\nAnio invalido. Intente de nuevo.\n";
        }
    }while( anio < 1900 || anio > 2025);
    
    // Obtener todos los titulares desde MongoDB
    if (gestorBusquedaMongo) {
        cout << "Obteniendo todos los titulares desde MongoDB..." << endl;
        std::vector<Titular*> titularesCompletos = gestorBusquedaMongo->obtenerTodosTitularesCompletos();
        
        if (!titularesCompletos.empty()) {
            cout << "Buscando en " << titularesCompletos.size() << " titulares desde MongoDB..." << endl;
            
            // Buscar el primer titular con anio mayor o igual
            Titular* encontrado = nullptr;
            for (Titular* titular : titularesCompletos) {
                int anioTitular = titular->getPersona().getFechaNa().getAnio().getAnio();
                if (anioTitular >= anio) {
                    encontrado = titular;
                    break;
                }
            }
            
            if (encontrado) {
                cout << "\nPrimer titular con anio de nacimiento >= " << anio << " (datos actualizados desde MongoDB):\n";
                encontrado->getPersona().imprimir();
            } else {
                cout << "\nNo se encontro ningun titular con ese anio o mayor en MongoDB.\n";
            }
            system("pause");
            return;
        } else {
            cout << "No se pudieron obtener datos desde MongoDB. Buscando en datos locales..." << endl;
        }
    }
    
    // Fallback: buscar en lista local
    Titular* t = buscador.primerTitularAnioNacimientoMayorIgual(const_cast<ListaDobleCircular<Titular*>&>(titulares), anio);
    if (t) {
        cout << "\nPrimer titular con anio de nacimiento >= " << anio << " (datos locales):\n";
        t->getPersona().imprimir();
    } else {
        cout << "\nNo se encontro ningun titular con ese anio o mayor.\n";
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
