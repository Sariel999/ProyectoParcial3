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
#include <vector>

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
        /* En este bucle no se puede cambiar a for each porque este tiene la funcion de validar una fecha, por ende  
        repite el ingreso de fecha hasta que se cumple la condición fechaValida == true. Es decir, no itera dobre una 
        estructura de datos. Para usar for each se deberia forzar al uso de un arreglo innecesario, lo que romperia el
        principio de claridad clean code.*/
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
    for (Titular* titular : titulares) {
        // Verificar cuenta corriente
        CuentaBancaria* cuentaCorriente = titular->getCuentaCorriente();
        if (cuentaCorriente) {
            ListaDobleCircular<Movimiento*>& movimientos = cuentaCorriente->getMovimientos();              
            for (Movimiento* movimiento : movimientos) {
                Fecha fechaMov = movimiento->getFechaMov();
                
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
                    titular->getPersona().imprimir();
                    cout << "--- CUENTA CORRIENTE ---" << endl;
                    cout << "ID: " << cuentaCorriente->getID() << endl;
                    cout << "--- MOVIMIENTO ---" << endl;
                    movimiento->imprimir();
                }
            }   
        }

        // Verificar cuentas de ahorro
        ListaDobleCircular<CuentaBancaria*>& cuentasAhorro = titular->getCuentasAhorro();
        for (CuentaBancaria* cuenta : cuentasAhorro){
            ListaDobleCircular<Movimiento*>& movimientos = cuenta->getMovimientos();
            for (Movimiento* movimiento : movimientos) {
                Fecha fechaMov = movimiento->getFechaMov();
                
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
                    titular->getPersona().imprimir();
                    cout << "--- CUENTA DE AHORRO ---" << endl;
                    cout << "ID: " << cuenta->getID() << endl;
                    cout << "--- MOVIMIENTO ---" << endl;
                    movimiento->imprimir();
                }                           
            }
        } 
    }
    if (!encontrado) {
        cout << "\nNo se encontraron movimientos en el rango de fechas especificado." << endl;
    }
    
    system("pause");
}

void GestorBusquedas::buscarPorTitular(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n=== BUSQUEDA POR TITULAR ===" << endl;
    
    string criterio = val.ingresarCadena((char*)"Ingrese nombre o apellido del titular:");
    
    if (criterio.empty()) {
        cout << "Error: El criterio de busqueda no puede estar vacio." << endl;
        system("pause");
        return;
    }

    // Lambda para convertir a minusculas
    auto toLower = [](string s) -> string {
        for (char& c : s) {
            if (c >= 'A' && c <= 'Z') {
                c += 32;
            }
        }
        return s;
    };

    string criterioBusqueda = toLower(criterio);
    bool encontrado = false;
    for (Titular* titular : titulares) {
        Persona persona = titular->getPersona();
        string nombre = toLower(persona.getNombre());
        string apellido = toLower(persona.getApellido());

        if (nombre.find(criterioBusqueda) != string::npos || 
            apellido.find(criterioBusqueda) != string::npos) {
            
            encontrado = true;
            cout << "\n--- TITULAR ENCONTRADO ---" << endl;
            persona.imprimir();
            
            // Mostrar cuenta corriente si existe
            if (titular->getCuentaCorriente()) {
                cout << "\n--- CUENTA CORRIENTE ---" << endl;
                titular->getCuentaCorriente()->imprimir();
            }
            
            // Mostrar cuentas de ahorro si existen
            ListaDobleCircular<CuentaBancaria*>& cuentasAhorro = titular->getCuentasAhorro();
            int contador = 1;
            for (CuentaBancaria* cuenta : cuentasAhorro) {
                cout << "\n--- CUENTA DE AHORRO #" << contador++ << " ---" << endl;
                cuenta ->imprimir();
            }
            
            cout << "\n" << string(50, '-') << endl;
        }
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
        for (char& c : s) {
            if (c >= 'A' && c <= 'Z') {
                c += 32;
            }
        }
        return s;
    };

    string criterio = toLower(criterioOriginal);    
    bool encontrado = false;
    for (Titular* titular : titulares){ 
        Persona p = titular->getPersona();
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
            for (Movimiento* m : movs) {
                
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
                
            } 
            
            if (cuentaCoincide || movimientoCoincide) {
                cout << "\n--- DATOS DEL TITULAR ---" << endl;
                p.imprimir();
                cout << "--- DATOS DE LA CUENTA ---" << endl;
                cuenta->imprimir();
                // Mostrar movimientos que coincidan
                
                bool movMostrado = false;
                
                for (Movimiento* m : movs) {
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
                }
                encontrado = true;
            }
            return cuentaCoincide || movimientoCoincide;
        };
        // Si coincide persona, mostrar todo
        if (personaCoincide) {
            cout << "\n--- DATOS DEL TITULAR ---" << endl;
            p.imprimir();
            if (titular->getCuentaCorriente()) {
                cout << "--- CUENTA CORRIENTE ---" << endl;
                titular->getCuentaCorriente()->imprimir();
            }
            int idx = 1;
            for (CuentaBancaria* cuenta : titular->getCuentasAhorro()) {
                cout << "--- CUENTA DE AHORRO #" << idx++ << " ---" << endl;
                cuenta->imprimir();
            }
            encontrado = true;
        } else {
            // Buscar en cuentas
            if (buscarEnCuenta(titular->getCuentaCorriente())) {
                // Ya mostrado dentro de la lambda
            }
            for (CuentaBancaria* cuenta : titular->getCuentasAhorro()) {
                buscarEnCuenta(cuenta);
            }
        }
    }
    
    if (!encontrado) {
        cout << "\nNo se encontraron coincidencias.\n" << endl;
    }
    system("pause");
}

// =============== NUEVAS FUNCIONES QUE USAN MONGODB ===============

void GestorBusquedas::buscarMovimientosPorFecha(GestorBusquedaMongo& gestorMongo) {
    system("cls");
    cout << "\n=== BUSQUEDA DE MOVIMIENTOS POR FECHA (MongoDB) ===" << endl;

    // Obtener todos los titulares desde MongoDB
    std::vector<Titular*> titulares = gestorMongo.obtenerTodosTitularesCompletos();
    
    if (titulares.empty()) {
        cout << "\nNo se pudieron obtener titulares desde MongoDB." << endl;
        system("pause");
        return;
    }

    // Lambda para obtener fecha valida usando ValidacionFecha
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

    // Verificar que fecha inicio sea menor o igual a fecha fin
    ValidacionFecha valFecha;
    int comparacion = valFecha.compararFechas(
        fechaInicio.getDia(), fechaInicio.getMes(), fechaInicio.getAnio().getAnio(),
        fechaFin.getDia(), fechaFin.getMes(), fechaFin.getAnio().getAnio()
    );
    
    if (comparacion > 0) {
        cout << "Error: La fecha de inicio debe ser anterior o igual a la fecha de fin." << endl;
        system("pause");
        // Limpiar memoria de titulares obtenidos
        for (Titular* titular : titulares) {
            delete titular;
        }
        return;
    }

    system("cls");
    cout << "\n=== MOVIMIENTOS ENCONTRADOS (MongoDB) ===" << endl;
    cout << "Rango: " << fechaInicio.getDia() << "/" << fechaInicio.getMes() << "/" 
         << fechaInicio.getAnio().getAnio() << " - " << fechaFin.getDia() << "/" 
         << fechaFin.getMes() << "/" << fechaFin.getAnio().getAnio() << endl;

    bool encontrado = false;
    for (Titular* titular : titulares) {
        // Verificar cuenta corriente
        CuentaBancaria* cuentaCorriente = titular->getCuentaCorriente();
        if (cuentaCorriente) {
            ListaDobleCircular<Movimiento*>& movimientos = cuentaCorriente->getMovimientos();              
            for (Movimiento* movimiento : movimientos) {
                Fecha fechaMov = movimiento->getFechaMov();
                
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
                        cout << "\n--- MOVIMIENTOS EN EL RANGO ---" << endl;
                        encontrado = true;
                    }
                    
                    cout << "\n* Titular: " << titular->getPersona().getNombre() 
                         << " " << titular->getPersona().getApellido() 
                         << " (CI: " << titular->getPersona().getCI() << ")" << endl;
                    cout << "  Cuenta Corriente: " << cuentaCorriente->getID() << endl;
                    movimiento->imprimir();
                }
            }
        }
        
        // Verificar cuentas de ahorro
        ListaDobleCircular<CuentaBancaria*>& cuentasAhorro = titular->getCuentasAhorro();
        for (CuentaBancaria* cuentaAhorro : cuentasAhorro) {
            ListaDobleCircular<Movimiento*>& movimientos = cuentaAhorro->getMovimientos();
            for (Movimiento* movimiento : movimientos) {
                Fecha fechaMov = movimiento->getFechaMov();
                
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
                        cout << "\n--- MOVIMIENTOS EN EL RANGO ---" << endl;
                        encontrado = true;
                    }
                    
                    cout << "\n* Titular: " << titular->getPersona().getNombre() 
                         << " " << titular->getPersona().getApellido() 
                         << " (CI: " << titular->getPersona().getCI() << ")" << endl;
                    cout << "  Cuenta Ahorro: " << cuentaAhorro->getID() << endl;
                    movimiento->imprimir();
                }
            }
        }
    }
    
    if (!encontrado) {
        cout << "\nNo se encontraron movimientos en el rango de fechas especificado." << endl;
    }
    
    // Limpiar memoria de titulares obtenidos
    for (Titular* titular : titulares) {
        delete titular;
    }
    
    system("pause");
}

void GestorBusquedas::buscarPorTitular(GestorBusquedaMongo& gestorMongo) {
    system("cls");
    cout << "\n=== BUSQUEDA POR TITULAR (MongoDB) ===" << endl;
    
    // Obtener todos los titulares desde MongoDB
    std::vector<Titular*> titulares = gestorMongo.obtenerTodosTitularesCompletos();
    
    if (titulares.empty()) {
        cout << "\nNo se pudieron obtener titulares desde MongoDB." << endl;
        system("pause");
        return;
    }
    
    string criterio = val.ingresarCadena((char*)"Ingrese nombre o apellido del titular:");
    
    if (criterio.empty()) {
        cout << "Error: El criterio de busqueda no puede estar vacio." << endl;
        system("pause");
        // Limpiar memoria
        for (Titular* titular : titulares) {
            delete titular;
        }
        return;
    }

    // Lambda para convertir a minusculas
    auto toLower = [](string s) -> string {
        for (char& c : s) {
            if (c >= 'A' && c <= 'Z') {
                c += 32;
            }
        }
        return s;
    };

    string criterioBusqueda = toLower(criterio);
    bool encontrado = false;
    
    for (Titular* titular : titulares) {
        Persona persona = titular->getPersona();
        string nombre = toLower(persona.getNombre());
        string apellido = toLower(persona.getApellido());

        if (nombre.find(criterioBusqueda) != string::npos || 
            apellido.find(criterioBusqueda) != string::npos) {
            
            encontrado = true;
            cout << "\n--- TITULAR ENCONTRADO (MongoDB) ---" << endl;
            persona.imprimir();
            
            // Mostrar cuenta corriente si existe
            if (titular->getCuentaCorriente()) {
                cout << "\n--- CUENTA CORRIENTE ---" << endl;
                titular->getCuentaCorriente()->imprimir();
            }
            
            // Mostrar cuentas de ahorro si existen
            ListaDobleCircular<CuentaBancaria*>& cuentasAhorro = titular->getCuentasAhorro();
            int contador = 1;
            for (CuentaBancaria* cuenta : cuentasAhorro) {
                cout << "\n--- CUENTA DE AHORRO #" << contador++ << " ---" << endl;
                cuenta->imprimir();
            }
            cout << "\n" << string(50, '-') << endl;
        }
    }
    
    if (!encontrado) {
        cout << "\nNo se encontraron titulares que coincidan con el criterio." << endl;
    }
    
    // Limpiar memoria de titulares obtenidos
    for (Titular* titular : titulares) {
        delete titular;
    }
    
    system("pause");
}

void GestorBusquedas::buscarPersonalizada(GestorBusquedaMongo& gestorMongo) {
    system("cls");
    cout << "\n=== BUSQUEDA PERSONALIZADA (MongoDB) ===" << endl;
    
    // Obtener todos los titulares desde MongoDB
    std::vector<Titular*> titulares = gestorMongo.obtenerTodosTitularesCompletos();
    
    if (titulares.empty()) {
        cout << "\nNo se pudieron obtener titulares desde MongoDB." << endl;
        system("pause");
        return;
    }
    
    string criterio = val.ingresarTextoLibre((char*)"Ingrese criterio de busqueda (nombre, apellido, CI, telefono, correo, ID cuenta, monto, etc.):");
    
    if (criterio.empty()) {
        cout << "Error: El criterio de busqueda no puede estar vacio." << endl;
        system("pause");
        // Limpiar memoria
        for (Titular* titular : titulares) {
            delete titular;
        }
        return;
    }

    // Lambda para convertir a minusculas
    auto toLower = [](string s) -> string {
        for (char& c : s) {
            if (c >= 'A' && c <= 'Z') {
                c += 32;
            }
        }
        return s;
    };

    criterio = toLower(criterio);
    bool encontrado = false;

    for (Titular* titular : titulares) {
        Persona p = titular->getPersona();
        
        // Buscar en datos de persona
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

        // Lambda para buscar en cuenta
        auto buscarEnCuenta = [&](CuentaBancaria* cuenta) {
            if (!cuenta) return false;
            string idCuenta = toLower(cuenta->getID());
            string tipoCuenta = toLower(cuenta->getTipoCuenta());
            string saldo = toLower(to_string(cuenta->getSaldo()));

            bool cuentaCoincide = idCuenta.find(criterio) != string::npos ||
                                    tipoCuenta.find(criterio) != string::npos ||
                                    saldo.find(criterio) != string::npos;

            // Buscar en movimientos
            bool movimientoCoincide = false;
            ListaDobleCircular<Movimiento*>& movs = cuenta->getMovimientos();                
            for (Movimiento* m : movs) {
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
            } 
            
            if (cuentaCoincide || movimientoCoincide) {
                cout << "\n--- DATOS DEL TITULAR (MongoDB) ---" << endl;
                p.imprimir();
                cout << "--- DATOS DE LA CUENTA ---" << endl;
                cuenta->imprimir();
                
                // Mostrar movimientos que coincidan
                bool movMostrado = false;
                for (Movimiento* m : movs) {
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
                }
                encontrado = true;
            }
            return cuentaCoincide || movimientoCoincide;
        };
        
        // Si coincide persona, mostrar todo
        if (personaCoincide) {
            cout << "\n--- DATOS DEL TITULAR (MongoDB) ---" << endl;
            p.imprimir();
            if (titular->getCuentaCorriente()) {
                cout << "--- CUENTA CORRIENTE ---" << endl;
                titular->getCuentaCorriente()->imprimir();
            }
            int idx = 1;
            for (CuentaBancaria* cuenta : titular->getCuentasAhorro()) {
                cout << "--- CUENTA DE AHORRO #" << idx++ << " ---" << endl;
                cuenta->imprimir();
            }
            encontrado = true;
        } else {
            // Buscar en cuentas
            if (buscarEnCuenta(titular->getCuentaCorriente())) {
                // Ya mostrado dentro de la lambda
            }
            for (CuentaBancaria* cuenta : titular->getCuentasAhorro()) {
                buscarEnCuenta(cuenta);
            }
        }
    }
    
    if (!encontrado) {
        cout << "\nNo se encontraron coincidencias en MongoDB.\n" << endl;
    }
    
    // Limpiar memoria de titulares obtenidos
    for (Titular* titular : titulares) {
        delete titular;
    }
    
    system("pause");
}
