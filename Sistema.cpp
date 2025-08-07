#include "Sistema.h"
#include "Menu.h"
#include "ValidacionFecha.h"
#include "FechaHora.h"
#include "ArchivoBinario.h"
#include "CifradoCesar.h"
#include "Backups.h"
#include "BPlusTreeTitulares.h"
#include "BusquedasBinarias.h"
#include "TablaHash.h"
#include "Cita.h"
#include "GestorArchivosBinarios.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <iostream>
using namespace std;

Sistema::Sistema(): arbolTitulares(3), gestorArchivos(hashes) {
    listaSucursales.agregarSucursal(Sucursal("Sucursal Central", -34.6037, -58.3816, "123"));
    listaSucursales.agregarSucursal(Sucursal("Sucursal Norte", -34.7000, -58.3000, "456"));
    listaSucursales.agregarSucursal(Sucursal("Sucursal Sur", -34.8000, -58.4000, "789"));
    actualizarContadoresSucursales();
}
/**
 * @brief Destroy the Sistema:: Sistema object
 * 
 */
Sistema::~Sistema() {
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual != nullptr) {
        do {
            delete actual->dato;
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
}
/**
 * @brief   Genera un archivo PDF con los titulares registrados.
 * 
 */
void Sistema::generarPDFTitulares() {
    gestorArchivos.generarPDFTitulares();
}
/**
 * @brief Muestra el menú principal del sistema bancario y permite al usuario seleccionar opciones.
 * 
 */
void Sistema::menuPrincipal() {
    const char* opciones[] = {
        "Registrar titular",
        "Crear cuenta",
        "Realizar deposito",
        "Realizar retiro",
        "Buscar movimientos por fecha",
        "Buscar titular por datos",
        "Busqueda personalizada",
        "Busquedas Binarias",
        "Guardar Archivo Binario",
        "Arbol B+",
        "Guardar titulares en archivo TXT",
        "Verificar integridad de archivo TXT",
        "Mostrar tabla hash",
        "Mostrar ayuda",
        "Generar codigo QR en PDF",
        "Generar PDF de titulares", // Nueva opción
        "Salir"
    };
    Menu menu;
    int opcion;
    do {
        opcion = menu.ingresarMenu("SISTEMA BANCARIO", opciones, 17); // Update to 17 options
        switch(opcion) {
            case 1: registrarTitular(); break;
            case 2: crearCuenta(); break;
            case 3: realizarDeposito(); break;
            case 4: realizarRetiro(); break;
            case 5: buscarMovimientosPorFecha(); break;
            case 6: buscarPorTitular(); break;
            case 7: buscarPersonalizada(); break;
            case 8: menuBB(); break;
            case 9: menuSecundario(); break;
            case 10: menuArbol(); break;
            case 11: guardarTitularesEnTxt(); break;
            case 12: {
                system("cls");
                cout << "\n--- VERIFICAR INTEGRIDAD DE ARCHIVO TXT ---" << endl;
                cout << "Esto compara el hash MD5 actual del archivo con el hash almacenado en la tabla hash.\n";
                string nombreArchivo = val.ingresarNombreArchivo((char*)"Ingrese el nombre del archivo TXT (ejemplo: titulares.txt): ");
                if (gestorArchivos.compararHashArchivo(nombreArchivo)) {
                    cout << "\nEl archivo no ha sido modificado (los hashes coinciden).\n" << endl;
                } else {
                    cout << "\nEl archivo ha sido modificado o no se encontro el hash en la tabla hash.\n" << endl;
                }
                system("pause");
                break;
            }
            case 13: {
                system("cls");
                cout << "\n--- MOSTRAR TABLA HASH ---" << endl;
                hashes.mostrarContenido();
                system("pause");
                break;
            }
            case 14: mostrarAyuda(); break;
            case 15: generadorQR.generarQRPDF(titulares); break;
            case 16: generarPDFTitulares(); break; // Nueva opcion
            case 17: {Backups backup;
                backup.crearBackup(titulares);
                cout << "\nSaliendo...\n" << endl; break;}
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 17);
}
/**
 * @brief Muestra el menú del árbol B+ y permite al usuario realizar operaciones sobre él.
 * 
 */
void Sistema::menuArbol(){
    const char* opciones[] = {
        "Buscar en Arbol B+",
        "Eliminar en Arbol B+",
        "Graficar Arbol B+",
        "Regresar al menu principal"
    };
    Menu menu;
    int opcion;
    do {
        opcion = menu.ingresarMenu("Arbol B+", opciones, 4);
        switch(opcion) {
            case 1:{
                string ciBuscar = val.ingresarCedula((char*)"\nIngrese el CI del titular para buscar: ");
                std::cout << "\nBuscando en el Arbol B+...\n";
                Titular* titularEncontrado = arbolTitulares.buscar(ciBuscar);
                std::cout << "\nResultado de la busqueda:\n";
                if (titularEncontrado) {
                    std::cout << "\nTitular encontrado:\n";
                    std::cout << "Nombre: " << titularEncontrado->getPersona().getNombre() << " " 
                              << titularEncontrado->getPersona().getApellido() << std::endl;
                    std::cout << "Numero de cuenta corriente: "
                              << (titularEncontrado->getCuentaCorriente() ? titularEncontrado->getCuentaCorriente()->getID() : "No tiene cuenta corriente") << std::endl;
                    std::cout << "Cuentas de ahorro: ";
                    if (titularEncontrado->getCuentasAhorro().vacia()) {
                        std::cout << "No tiene cuentas de ahorro" << std::endl;
                    } else {
                        std::cout << std::endl;
                        titularEncontrado->mostrarCuentasAhorro();
                    }
                } else {
                    std::cout << "\nTitular no encontrado." << std::endl;
                }
                std::cout.flush(); // Forzar la salida
                system("pause");
                break;
            }  
            case 2: {
                string ciEliminar = val.ingresarCedula((char*)"\nIngrese el CI del titular para eliminar: ");
                std::cout << "\nBuscando titular para eliminar...\n";
                Titular* titularEncontrado = arbolTitulares.buscar(ciEliminar);
                if (titularEncontrado) {
                    // Eliminar del árbol B+
                    arbolTitulares.eliminar(ciEliminar);
                    // Eliminar de la lista de titulares
                    NodoDoble<Titular*>* actual = titulares.getCabeza();
                    if (actual) {
                        do {
                            if (actual->dato->getPersona().getCI() == ciEliminar) {
                                titulares.eliminar(actual);
                                delete actual->dato; // Liberar memoria del titular
                                break;
                            }
                            actual = actual->siguiente;
                        } while (actual != titulares.getCabeza());
                    }
                    std::cout << "\nTitular eliminado exitosamente.\n";
                    // Actualizar contadores de sucursales
                    actualizarContadoresSucursales();
                    // Crear backup
                    Backups backup;
                    backup.crearBackup(titulares);
                    // Imprimir árbol para verificar
                    std::cout << "\nArbol despues de la eliminacion:\n";
                    arbolTitulares.imprimir();
                } else {
                    std::cout << "\nTitular no encontrado." << std::endl;
                }
                std::cout.flush();
                system("pause");
                break;
            }
            case 3: {
                system("cls");
                std::cout << "\n--- GRAFICAR ARBOL B+ (REPRESENTACION TEXTUAL) ---" << std::endl;
                std::cout << "\nMostrando la estructura del Arbol B+ con niveles:\n";
                arbolTitulares.imprimirArbolBPlus();
                arbolTitulares.graficarArbol();
                
                system("pause");
                break;
            }
            case 4: cout << "\nRegresando al menu principal...\n" << endl; break;
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 4);
}
/**
 * @brief Muestra el menú secundario del sistema bancario y permite al usuario seleccionar opciones relacionadas con archivos binarios.
 * 
 */
void Sistema::menuSecundario(){
    const char* opciones[] = {
        "Guardar archivo sin cifrar",
        "Guardar archivo cifrado",
        "Decifrar archivo cifrado",
        "Restaurar backup",
        "Regresar al menu principal"
    };
    Menu menu;
    int opcion;
    do {
        opcion = menu.ingresarMenu("Archivos Binarios", opciones, 5);
        switch(opcion) {
            case 1: gestorArchivosBinarios.guardarArchivoBinSinCifrar(titulares); break;
            case 2: gestorArchivosBinarios.guardarArchivoBinCifrado(titulares); break;
            case 3: gestorArchivosBinarios.decifrarArchivoCifrado(titulares); break;
            case 4: { // Restaurar backup
                system("cls");
                cout << "/n--- RESTAURAR BACKUP ---/n" << endl;
                string archivo = val.ingresarCodigoBak((char*)"Ingrese el nombre del archivo de backup (ejemplo: 20240601_153000.bak):");
                Backups backup;
                
                if (backup.restaurarBackup(titulares, arbolTitulares,archivo)) {
                    actualizarContadoresSucursales();
                }
                break;
            }
            case 5: cout << "\nRegresando al menu principal...\n" << endl; break;
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 5);
}
/**
 * @brief Registra un nuevo titular en el sistema bancario.
 * 
 */
void Sistema::registrarTitular() {
    gestorTitulares.registrarTitular(titulares, arbolTitulares);
}
/**
 * @brief Busca un titular por su cedula de identidad (CI).
 * 
 * @param ci 
 * @return Titular* 
 */
Titular* Sistema::buscarTitularPorCI(const std::string& ci) {
    return gestorTitulares.buscarTitularPorCI(titulares, ci);
}
/**
 * @brief Crea una nueva cuenta bancaria para un titular existente.
 * 
 */
void Sistema::crearCuenta() {
    gestorTitulares.crearCuenta(titulares, listaSucursales);
}
/**
 * @brief Realiza un deposito en una cuenta bancaria de un titular.
 * 
 */
void Sistema::realizarDeposito() {
    operacionesBancarias.realizarDeposito(titulares);
}

/**
 * @brief Realiza un retiro de una cuenta bancaria de un titular.
 * 
 */
void Sistema::realizarRetiro() {
    operacionesBancarias.realizarRetiro(titulares);
}

void Sistema::buscarMovimientosPorFecha() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    gestorBusquedas.buscarMovimientosPorFecha(titulares);
}
/**
 * @brief Busca titulares por nombre o apellido.
 * 
 */
void Sistema::buscarPorTitular() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    gestorBusquedas.buscarPorTitular(titulares);
}
/**
 * @brief Busca titulares y cuentas bancarias utilizando un criterio de búsqueda personalizado.
 * 
 */
void Sistema::buscarPersonalizada() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    gestorBusquedas.buscarPersonalizada(titulares);
}
/**
 * @brief Muestra la ayuda del sistema abriendo un archivo CHM.
 * 
 */
void Sistema::mostrarAyuda() {
    system("cls");
    cout << "\n--- AYUDA DEL SISTEMA ---\n" << endl;
    cout << "Abriendo el archivo de ayuda...\n" << endl;
    // Abre el archivo CHM con la aplicación predeterminada en Windows
    system("start Ayuda-CuentasBancarias.chm");
    system("pause");
}
/**
 * @brief Actualiza los contadores de cuentas en cada sucursal.
 * 
 * Recorre la lista de titulares y sus cuentas, actualizando el contador de cuentas en cada sucursal.
 */
// Supón que tienes: titulares (ListaDobleCircular<Titular*>&) y listaSucursales (ListaSucursales&)
void Sistema::actualizarContadoresSucursales() {
    // Reinicia todos los contadores
    NodoSucursal* nodoSuc = listaSucursales.getCabeza();
    while (nodoSuc != nullptr) {
        nodoSuc->sucursal.setContadorCuentas(0); // Debes tener este setter
        nodoSuc = nodoSuc->siguiente;
    }

    // Recorre titulares y cuentas
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual) {
        do {
            Titular* t = actual->dato;
            // Cuenta corriente
            CuentaBancaria* c = t->getCuentaCorriente();
            if (c) {
                std::string idSucursal = c->getID().substr(0, 3); // Ajusta si tu ID es diferente
                NodoSucursal* nodo = listaSucursales.getCabeza();
                while (nodo != nullptr) {
                    if (nodo->sucursal.getIdSucursal() == idSucursal) {
                        nodo->sucursal.incrementarContadorCuentas();
                        break;
                    }
                    nodo = nodo->siguiente;
                }
            }
            // Cuentas de ahorro
            NodoDoble<CuentaBancaria*>* nodoA = t->getCuentasAhorro().getCabeza();
            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    CuentaBancaria* ahorro = temp->dato;
                    std::string idSucursal = ahorro->getID().substr(0, 3);
                    NodoSucursal* nodo = listaSucursales.getCabeza();
                    while (nodo != nullptr) {
                        if (nodo->sucursal.getIdSucursal() == idSucursal) {
                            nodo->sucursal.incrementarContadorCuentas();
                            break;
                        }
                        nodo = nodo->siguiente;
                    }
                    temp = temp->siguiente;
                } while (temp != nodoA);
            }
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
}

/**
 * @brief Muestra el menú de busquedas binarias.
 * 
 */
// BUSQUEDAS BINARIAS
void Sistema::busquedasBinarias() {
    menuBB();
}

void Sistema::menuBB() {
    system("cls");
    BusquedasBinarias buscador;
    Menu menu;
    ListaSucursales sucursales;
    const char* opciones[] = {
        "Buscar deposito mayor o igual a un monto",
        "Deposito minimo mensual para meta de ahorro",
        "Buscar titular por CI",
        "Buscar titular por anio de nacimiento ",
        "Buscar sucursal mas cercana",
        "Regresar al menu principal"
    };
    int opcion;
    do {
        opcion = menu.ingresarMenu("BUSQUEDAS BINARIAS", opciones, 6);
        switch(opcion) {
            case 1: {
                // Buscar primer deposito mayor o igual a un monto
                cout << "\n--- Buscar primer deposito mayor o igual a un monto ---\n";
                cout << " Esta funcion permite buscar el primer deposito en una cuenta bancaria que sea mayor o igual a un monto especificado.\n";
                string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular:");
                Titular* titular = buscarTitularPorCI(cedula);
                if (!titular) {
                    cout << "Titular no encontrado.\n"; system("pause"); break;
                }
                string tipo = val.ingresarCadena((char*)"\nTipo de cuenta (Corriente/Ahorro):");
                for (char& c : tipo) c = toupper(c);
                CuentaBancaria* cuenta = nullptr;
                string idCuenta = val.ingresarNumeros((char*)"\nIngrese ID de la cuenta:");
                if (tipo == "CORRIENTE") {
                    cuenta = titular->getCuentaCorriente();
                    if (!cuenta || cuenta->getID() != idCuenta) {
                        cout << "Cuenta corriente no encontrada o ID incorrecto.\n"; system("pause"); break;
                    }
                } else if (tipo == "AHORRO") {
                    NodoDoble<CuentaBancaria*>* actual = titular->getCuentasAhorro().getCabeza();
                    if (actual) {
                        do {
                            if (actual->dato->getID() == idCuenta) {
                                cuenta = actual->dato;
                                break;
                            }
                            actual = actual->siguiente;
                        } while (actual != titular->getCuentasAhorro().getCabeza());
                    }
                    if (!cuenta) {
                        cout << "Cuenta de ahorro no encontrada.\n"; system("pause"); break;
                    }
                } else {
                    cout << "Tipo de cuenta no valido.\n"; system("pause"); break;
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
                break;
            }
            case 2: {
                cout << "\n--- Deposito minimo mensual para meta de ahorro ---\n";
                cout << " Esta funcion permite calcular el deposito mensual minimo necesario para alcanzar una meta de ahorro en un plazo determinado inciando con el saldo actual de la cuenta .\n";
                string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular:");
                Titular* titular = buscarTitularPorCI(cedula);
                if (!titular) {
                    cout << "Titular no encontrado.\n"; system("pause"); break;
                }
                if (titular->getCuentasAhorro().vacia()) {
                    cout << "El titular no tiene cuentas de ahorro.\n"; system("pause"); break;
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
                    cout << "Cuenta de ahorro no encontrada.\n"; system("pause"); break;
                }

                float saldoInicial = cuentaAhorro->getSaldo();
                cout << "Saldo inicial de la cuenta: $" << saldoInicial << endl;
                float saldoMeta;
                int meses;
                do {
                    cout << "Saldo meta: "; cin >> saldoMeta;
                    if (saldoMeta <= saldoInicial) {
                        cout << "El saldo meta debe ser mayor al saldo actual de la cuenta. Intente de nuevo.\n";
                    }
                } while (saldoMeta <= saldoInicial);
                cout << "Meses para alcanzar la meta: "; 
                cin >> meses;
                int deposito = buscador.depositoMinimoParaMeta(saldoInicial, saldoMeta, meses);
                cout << "Deposito mensual minimo necesario: $" << deposito << endl;
                system("pause");
                break;
            }
            case 3: {
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
                break;
            }
            case 4: {
                // Buscar primer titular por año de nacimiento
                cout << "\n--- Buscar primer titular por anio de nacimiento (mayor o igual) ---\n";
                int anio;
                cout << "Ingrese anio de nacimiento a buscar: "; cin >> anio;
                Titular* t = buscador.primerTitularAnioNacimientoMayorIgual(titulares, anio);
                if (t) {
                    cout << "Primer titular con anio de nacimiento >= " << anio << ":\n";
                    t->getPersona().imprimir();
                } else {
                    cout << "No se encontro ningun titular con ese anio o mayor.\n";
                }
                system("pause");
                break;
            }case 5: {cout << "\n--- Buscar sucursal mas cercana ---\n";
    cout << " Esta funcion encuentra la sucursal mas cercana a las coordenadas geograficas ingresadas.\n";
    float latUsuario = val.ingresarCoordenada((char*)"\nIngrese latitud:", true);
    float lonUsuario = val.ingresarCoordenada((char*)"\nIngrese longitud:", false);
    Sucursal* sucursal = buscador.sucursalMasCercana(listaSucursales.getCabeza(), latUsuario, lonUsuario);
    if (sucursal) {
        cout << "\nSucursal mas cercana:\n";
        sucursal->imprimir();
        
        // Crear cita para el siguiente día laborable
        FechaHora fechaActual;
        fechaActual.actualizarFechaHora(); // Obtener fecha y hora actual
        Cita cita(sucursal, fechaActual);
        cita.mostrar();
        
    } else {
        cout << "No se encontraron sucursales.\n";
    }
    system("pause");
    break;
                }  
            /*case 6: {
                cout << "\n--- Calcular intervalo maximo entre citas ---\n";
                            cout << " Esta funcion calcula el maximo intervalo de tiempo (en minutos) entre citas consecutivas para programar un numero dado de clientes.\n";
                            int nClientes;
                            cout << "Ingrese numero de clientes a programar: ";
                            cin >> nClientes;
                            if (cin.fail() || nClientes < 2 || nClientes > 100000) {
                                cin.clear();
                                cin.ignore(10000, '\n');
                                cout << "Numero de clientes invalido. Debe estar entre 2 y 100000.\n";
                                system("pause");
                                break;
                            }
                            int duracionCita;
                            cout << "Ingrese duracion de cada cita (en minutos, minimo 1): ";
                            cin >> duracionCita;
                            if (cin.fail() || duracionCita < 1) {
                                cin.clear();
                                cin.ignore(10000, '\n');
                                cout << "Duracion de cita invalida. Debe ser al menos 1 minuto.\n";
                                system("pause");
                                break;
                            }
                            if (!listaSucursales.getCabeza()) {
                                cout << "No hay sucursales disponibles.\n";
                                system("pause");
                                break;
                            }
                            int maxD = buscador.maximoIntervaloCitas(listaSucursales.getCabeza(), nClientes, duracionCita);
                            if (maxD == 0) {
                                cout << "No es posible programar las citas con los datos proporcionados.\n";
                            } else {
                                cout << "El maximo intervalo entre citas es: " << maxD << " minutos.\n";
                            }
                            system("pause");
                            break;
}
           */ case 6:
                cout << "\nRegresando al menu principal...\n";
                break;
            default:
                cout << "\nOpcion invalida.\n"; system("pause"); break;
        }
        }while(opcion != 6);
    }
    /**
         * @brief Guarda los titulares y sus cuentas bancarias en un archivo de texto.
         * 
         */
void Sistema::guardarTitularesEnTxt() {
    gestorArchivos.guardarTitularesEnTxt(titulares);
}