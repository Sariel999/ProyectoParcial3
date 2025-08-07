/**
 * @file GestorBusquedas.cpp
 * @author your name (you@domain.com)
 * @brief Implementacion de la clase GestorBusquedas
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "GestorBusquedas.h"
#include <iostream>
#include <string>
#include <cctype>

using namespace std;

GestorBusquedas::GestorBusquedas() {
}

GestorBusquedas::~GestorBusquedas() {
}

void GestorBusquedas::buscarMovimientosPorFecha(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n=== BUSQUEDA DE MOVIMIENTOS POR FECHA ===" << endl;

    // Lambda para obtener fecha valida usando ValidacionFecha (igual que registrarTitular)
    auto obtenerFecha = [&](const string& mensaje) -> Fecha {
        ValidacionFecha valFecha;
        string fechaStr;
        int dia, mes, anio;
        bool fechaValida = false;
        
        cout << mensaje << endl;
        cout << "Formato: dd/mm/yyyy (ejemplo: 15/12/2023)" << endl;
        
        do {
            valFecha.ingresarFecha(fechaStr, dia, mes, anio);
            if (!valFecha.valoresValidos(dia, mes, anio)) {
                cout << "\nFecha invalida. Intente de nuevo." << endl;
                system("pause");
                continue;
            }
            fechaValida = true;
        } while (!fechaValida);

        Anio anioObj;
        anioObj.setAnio(anio);
        anioObj.setAnioBisiesto(valFecha.esBisiesto(anio));
        
        Fecha fecha;
        fecha.setDia(dia);
        fecha.setMes(mes);
        fecha.setAnio(anioObj);
        
        return fecha;
    };

    Fecha fechaInicio = obtenerFecha("Ingrese fecha de inicio");
    Fecha fechaFin = obtenerFecha("Ingrese fecha de fin");

    // Verificar que fecha inicio sea menor o igual a fecha fin usando ValidacionFecha
    ValidacionFecha valFecha;
    int comparacion = valFecha.compararFechas(
        fechaInicio.getDia(), fechaInicio.getMes(), fechaInicio.getAnio().getAnio(),
        fechaFin.getDia(), fechaFin.getMes(), fechaFin.getAnio().getAnio()
    );
    
    if (comparacion > 0) {
        cout << "Error: La fecha de inicio debe ser anterior o igual a la fecha de fin." << endl;
        system("pause");
        return;
    }

    system("cls");
    cout << "\n=== MOVIMIENTOS ENCONTRADOS ===" << endl;
    cout << "Rango: " << fechaInicio.getDia() << "/" << fechaInicio.getMes() << "/" 
         << fechaInicio.getAnio().getAnio() << " - " << fechaFin.getDia() << "/" 
         << fechaFin.getMes() << "/" << fechaFin.getAnio().getAnio() << endl;

    bool encontrado = false;
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual) {
        do {
            // Verificar cuenta corriente
            CuentaBancaria* cuentaCorriente = actual->dato->getCuentaCorriente();
            if (cuentaCorriente) {
                ListaDobleCircular<Movimiento*>& movimientos = cuentaCorriente->getMovimientos();
                NodoDoble<Movimiento*>* nodoMov = movimientos.getCabeza();
                if (nodoMov) {
                    do {
                        Fecha fechaMov = nodoMov->dato->getFechaMov();
                        
                        // Comparar usando ValidacionFecha
                        int compInicio = valFecha.compararFechas(
                            fechaMov.getDia(), fechaMov.getMes(), fechaMov.getAnio().getAnio(),
                            fechaInicio.getDia(), fechaInicio.getMes(), fechaInicio.getAnio().getAnio()
                        );
                        int compFin = valFecha.compararFechas(
                            fechaMov.getDia(), fechaMov.getMes(), fechaMov.getAnio().getAnio(),
                            fechaFin.getDia(), fechaFin.getMes(), fechaFin.getAnio().getAnio()
                        );
                        
                        if (compInicio >= 0 && compFin <= 0) {
                            if (!encontrado) {
                                encontrado = true;
                            }
                            cout << "\n--- TITULAR ---" << endl;
                            actual->dato->getPersona().imprimir();
                            cout << "--- CUENTA CORRIENTE ---" << endl;
                            cout << "ID: " << cuentaCorriente->getID() << endl;
                            cout << "--- MOVIMIENTO ---" << endl;
                            nodoMov->dato->imprimir();
                        }
                        nodoMov = nodoMov->siguiente;
                    } while (nodoMov != movimientos.getCabeza());
                }
            }

            // Verificar cuentas de ahorro
            ListaDobleCircular<CuentaBancaria*>& cuentasAhorro = actual->dato->getCuentasAhorro();
            NodoDoble<CuentaBancaria*>* nodoAhorro = cuentasAhorro.getCabeza();
            if (nodoAhorro) {
                do {
                    ListaDobleCircular<Movimiento*>& movimientos = nodoAhorro->dato->getMovimientos();
                    NodoDoble<Movimiento*>* nodoMov = movimientos.getCabeza();
                    if (nodoMov) {
                        do {
                            Fecha fechaMov = nodoMov->dato->getFechaMov();
                            
                            // Comparar usando ValidacionFecha
                            int compInicio = valFecha.compararFechas(
                                fechaMov.getDia(), fechaMov.getMes(), fechaMov.getAnio().getAnio(),
                                fechaInicio.getDia(), fechaInicio.getMes(), fechaInicio.getAnio().getAnio()
                            );
                            int compFin = valFecha.compararFechas(
                                fechaMov.getDia(), fechaMov.getMes(), fechaMov.getAnio().getAnio(),
                                fechaFin.getDia(), fechaFin.getMes(), fechaFin.getAnio().getAnio()
                            );
                            
                            if (compInicio >= 0 && compFin <= 0) {
                                if (!encontrado) {
                                    encontrado = true;
                                }
                                cout << "\n--- TITULAR ---" << endl;
                                actual->dato->getPersona().imprimir();
                                cout << "--- CUENTA DE AHORRO ---" << endl;
                                cout << "ID: " << nodoAhorro->dato->getID() << endl;
                                cout << "--- MOVIMIENTO ---" << endl;
                                nodoMov->dato->imprimir();
                            }
                            nodoMov = nodoMov->siguiente;
                        } while (nodoMov != movimientos.getCabeza());
                    }
                    nodoAhorro = nodoAhorro->siguiente;
                } while (nodoAhorro != cuentasAhorro.getCabeza());
            }

            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }

    if (!encontrado) {
        cout << "\nNo se encontraron movimientos en el rango de fechas especificado." << endl;
    }
    
    system("pause");
}

void GestorBusquedas::buscarPorTitular(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n=== BUSQUEDA POR TITULAR ===" << endl;
    
    string criterio;
    cout << "Ingrese nombre o apellido del titular: ";
    getline(cin, criterio);
    
    if (criterio.empty()) {
        cout << "Error: El criterio de busqueda no puede estar vacio." << endl;
        system("pause");
        return;
    }

    // Lambda para convertir a minusculas
    auto toLower = [](string s) -> string {
        for (size_t i = 0; i < s.length(); i++) {
            if (s[i] >= 'A' && s[i] <= 'Z') {
                s[i] += 32;
            }
        }
        return s;
    };

    string criterioBusqueda = toLower(criterio);
    bool encontrado = false;

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual) {
        do {
            Persona persona = actual->dato->getPersona();
            string nombre = toLower(persona.getNombre());
            string apellido = toLower(persona.getApellido());

            if (nombre.find(criterioBusqueda) != string::npos || 
                apellido.find(criterioBusqueda) != string::npos) {
                
                encontrado = true;
                cout << "\n--- TITULAR ENCONTRADO ---" << endl;
                persona.imprimir();
                
                // Mostrar cuenta corriente si existe
                if (actual->dato->getCuentaCorriente()) {
                    cout << "\n--- CUENTA CORRIENTE ---" << endl;
                    actual->dato->getCuentaCorriente()->imprimir();
                }
                
                // Mostrar cuentas de ahorro si existen
                ListaDobleCircular<CuentaBancaria*>& cuentasAhorro = actual->dato->getCuentasAhorro();
                NodoDoble<CuentaBancaria*>* nodoAhorro = cuentasAhorro.getCabeza();
                if (nodoAhorro) {
                    int contador = 1;
                    do {
                        cout << "\n--- CUENTA DE AHORRO #" << contador++ << " ---" << endl;
                        nodoAhorro->dato->imprimir();
                        nodoAhorro = nodoAhorro->siguiente;
                    } while (nodoAhorro != cuentasAhorro.getCabeza());
                }
                cout << "\n" << string(50, '-') << endl;
            }
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }

    if (!encontrado) {
        cout << "\nNo se encontraron titulares con el criterio especificado." << endl;
    }
    
    system("pause");
}

void GestorBusquedas::buscarPersonalizada(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n=== BUSQUEDA PERSONALIZADA ===" << endl;
    cout << "Esta busqueda buscara en todos los campos del sistema:" << endl;
    cout << "- Datos del titular (CI, nombre, apellido, telefono, correo)" << endl;
    cout << "- Datos de cuentas (ID, tipo, saldo)" << endl;
    cout << "- Datos de movimientos (monto, numero, tipo, fecha)" << endl;
    
    string criterioOriginal;
    cout << "\nIngrese el criterio de busqueda: ";
    getline(cin, criterioOriginal);
    
    if (criterioOriginal.empty()) {
        cout << "Error: El criterio de busqueda no puede estar vacio." << endl;
        system("pause");
        return;
    }

    // Lambda para convertir a minusculas
    auto toLower = [](string s) -> string {
        for (size_t i = 0; i < s.length(); i++) {
            if (s[i] >= 'A' && s[i] <= 'Z') {
                s[i] += 32;
            }
        }
        return s;
    };

    string criterio = toLower(criterioOriginal);

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    bool encontrado = false;
    if (actual) {
        do {
            Persona p = actual->dato->getPersona();
            // Buscar en Persona
            string ci = toLower(p.getCI());
            string nombre = toLower(p.getNombre());
            string apellido = toLower(p.getApellido());
            string telefono = toLower(p.getTelefono());
            string correo = toLower(p.getCorreo());

            bool personaCoincide = ci.find(criterio) != string::npos ||
                                   nombre.find(criterio) != string::npos ||
                                   apellido.find(criterio) != string::npos ||
                                   telefono.find(criterio) != string::npos ||
                                   correo.find(criterio) != string::npos;

            // Buscar en cuentas corriente y ahorro
            auto buscarEnCuenta = [&](CuentaBancaria* cuenta) {
                if (!cuenta) return false;
                string idCuenta = toLower(cuenta->getID());
                string tipoCuenta = toLower(cuenta->getTipoCuenta());
                string saldo = toLower(to_string(cuenta->getSaldo()));

                bool cuentaCoincide = idCuenta.find(criterio) != string::npos ||
                                      tipoCuenta.find(criterio) != string::npos ||
                                      saldo.find(criterio) != string::npos;

                // Buscar en movimientos (lista doble circular)
                bool movimientoCoincide = false;
                ListaDobleCircular<Movimiento*>& movs = cuenta->getMovimientos();
                NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                if (nodoMov) {
                    do {
                        Movimiento* m = nodoMov->dato;
                        string monto = toLower(to_string(m->getMonto()));
                        string numMov = toLower(to_string(m->getNumeroMovimiento()));
                        string tipoMov = m->getTipo() ? "deposito" : "retiro";
                        tipoMov = toLower(tipoMov);

                        Fecha f = m->getFechaMov();
                        string fecha = to_string(f.getDia()) + "/" + to_string(f.getMes()) + "/" + to_string(f.getAnio().getAnio());

                        if (monto.find(criterio) != string::npos ||
                            numMov.find(criterio) != string::npos ||
                            tipoMov.find(criterio) != string::npos ||
                            fecha.find(criterio) != string::npos) {
                            movimientoCoincide = true;
                            break;
                        }
                        nodoMov = nodoMov->siguiente;
                    } while (nodoMov != movs.getCabeza());
                }

                if (cuentaCoincide || movimientoCoincide) {
                    cout << "\n--- DATOS DEL TITULAR ---" << endl;
                    p.imprimir();
                    cout << "--- DATOS DE LA CUENTA ---" << endl;
                    cuenta->imprimir();
                    // Mostrar movimientos que coincidan
                    NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                    bool movMostrado = false;
                    if (nodoMov) {
                        do {
                            Movimiento* m = nodoMov->dato;
                            string monto = toLower(to_string(m->getMonto()));
                            string numMov = toLower(to_string(m->getNumeroMovimiento()));
                            string tipoMov = m->getTipo() ? "deposito" : "retiro";
                            tipoMov = toLower(tipoMov);
                            Fecha f = m->getFechaMov();
                            string fecha = to_string(f.getDia()) + "/" + to_string(f.getMes()) + "/" + to_string(f.getAnio().getAnio());

                            if (monto.find(criterio) != string::npos ||
                                numMov.find(criterio) != string::npos ||
                                tipoMov.find(criterio) != string::npos ||
                                fecha.find(criterio) != string::npos) {
                                if (!movMostrado) {
                                    cout << "--- MOVIMIENTOS COINCIDENTES ---" << endl;
                                    movMostrado = true;
                                }
                                m->imprimir();
                            }
                            nodoMov = nodoMov->siguiente;
                        } while (nodoMov != movs.getCabeza());
                    }
                    encontrado = true;
                }
                return cuentaCoincide || movimientoCoincide;
            };

            // Si coincide persona, mostrar todo
            if (personaCoincide) {
                cout << "\n--- DATOS DEL TITULAR ---" << endl;
                p.imprimir();
                if (actual->dato->getCuentaCorriente()) {
                    cout << "--- CUENTA CORRIENTE ---" << endl;
                    actual->dato->getCuentaCorriente()->imprimir();
                }
                NodoDoble<CuentaBancaria*>* nodoA = actual->dato->getCuentasAhorro().getCabeza();
                if (nodoA) {
                    int idx = 1;
                    NodoDoble<CuentaBancaria*>* temp = nodoA;
                    do {
                        cout << "--- CUENTA DE AHORRO #" << idx++ << " ---" << endl;
                        temp->dato->imprimir();
                        temp = temp->siguiente;
                    } while (temp != nodoA);
                }
                encontrado = true;
            } else {
                // Buscar en cuentas
                if (buscarEnCuenta(actual->dato->getCuentaCorriente())) {
                    // Ya mostrado dentro de la lambda
                }
                NodoDoble<CuentaBancaria*>* nodoA = actual->dato->getCuentasAhorro().getCabeza();
                if (nodoA) {
                    NodoDoble<CuentaBancaria*>* temp = nodoA;
                    do {
                        buscarEnCuenta(temp->dato);
                        temp = temp->siguiente;
                    } while (temp != nodoA);
                }
            }
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }

    if (!encontrado) {
        cout << "\nNo se encontraron coincidencias.\n" << endl;
    }
    system("pause");
}
