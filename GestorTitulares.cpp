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

using namespace std;

GestorTitulares::GestorTitulares() {
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
    string nombre = val.ingresarCadena((char*)"Ingrese nombre:");
    string apellido = val.ingresarCadena((char*)"\nIngrese apellido:");
    for (char& c : nombre) c = toupper(c);
    for (char& c : apellido) c = toupper(c);
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula:");
    if (buscarTitularPorCI(titulares, cedula) != nullptr) {
        cout << "\nYa existe un titular registrado con esa cedula.\n" << endl;
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
    
    arbolTitulares.imprimir();
    std::cout.flush();
    cout << "\nTitular registrado exitosamente." << endl;
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
    if (titulares.vacia()) {
        return nullptr;
    }
    
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual == nullptr) {
        return nullptr;
    }
    
    do {
        if (actual->dato->getPersona().getCI() == ci) {
            return actual->dato;
        }
        actual = actual->siguiente;
    } while (actual != titulares.getCabeza());
    
    return nullptr;
}

/**
 * @brief Crea una nueva cuenta bancaria para un titular existente.
 * 
 * @param titulares Lista de titulares del sistema
 * @param listaSucursales Lista de sucursales disponibles
 */
void GestorTitulares::crearCuenta(ListaDobleCircular<Titular*>& titulares, ListaSucursales& listaSucursales) {
    system("cls");
    cout << "\n--- CREAR CUENTA ---\n" << endl;
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular: ");
    Titular* titular = buscarTitularPorCI(titulares, cedula);
    if (!titular) {
        cout << "\nTitular no encontrado." << endl;
        system("pause");
        return;
    }
    cout << "\nSeleccione la sucursal para la cuenta:\n";
    listaSucursales.mostrarSucursales();
    string idSucursal = val.ingresarCodigoSucursal((char*)"\nIngrese el ID de la sucursal: ");
    if (!listaSucursales.existeSucursal(idSucursal)) {
        cout << "\nSucursal no encontrada." << endl;
        system("pause");
        return;
    }
    NodoSucursal* nodo = listaSucursales.getCabeza();
    while (nodo != nullptr) {
        if (nodo->sucursal.getIdSucursal() == idSucursal) {
            nodo->sucursal.incrementarContadorCuentas();
            break;
        }
        nodo = nodo->siguiente;
    }
    string tipo = val.ingresarCadena((char*)"\nIngrese tipo de cuenta (Corriente/Ahorro): ");
    CuentaBancaria* nuevaCuenta = new CuentaBancaria(idSucursal);
    for (char& c : tipo) c = toupper(c);
    nuevaCuenta->setTipoCuenta(tipo);

    if (tipo == "CORRIENTE") {
        if (titular->getCuentaCorriente() != nullptr) {
            cout << "\nEste titular ya tiene una cuenta corriente.\n" << endl;
            delete nuevaCuenta;
        } else {
            titular->setCuentaCorriente(nuevaCuenta);
            cout << "\nCuenta corriente creada exitosamente.\n" << endl;
            cout << "--- DATOS DEL TITULAR ---" << endl;
            titular->getPersona().imprimir();
            cout << "--- DATOS DE LA CUENTA ---" << endl;
            nuevaCuenta->imprimir();
        }
    } else if (tipo == "AHORRO") {
        titular->agregarCuentaAhorro(nuevaCuenta);
        cout << "\nCuenta de ahorro creada exitosamente.\n" << endl;
        cout << "--- DATOS DEL TITULAR ---" << endl;
        titular->getPersona().imprimir();
        cout << "--- DATOS DE LA CUENTA ---" << endl;
        nuevaCuenta->imprimir();
    } else {
        cout << "\nTipo de cuenta no valido.\n" << endl;
        delete nuevaCuenta;
    }

    Backups backup;
    backup.crearBackup(titulares);
    system("pause");
}
