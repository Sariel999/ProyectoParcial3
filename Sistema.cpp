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
#include "Validaciones.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <iostream>
using namespace std;

Sistema::Sistema(): arbolTitulares(3), gestorArchivos(hashes), gestorTitulares(gestorConexion), operacionesBancarias(gestorConexion) {
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
        "Generar PDF de titulares",
        "Chat",
        "Salir"
    };
    Menu menu;
    int opcion;
    do {
        opcion = menu.ingresarMenu("SISTEMA BANCARIO", opciones, 18);
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
            case 12: verificarIntegridadArchivo(); break;
            case 13: mostrarTablaHash();  break;
            case 14: mostrarAyuda(); break;
            case 15: generadorQR.generarQRPDF(titulares); break;
            case 16: generarPDFTitulares(); break;
            case 17: menuChat(); break;
            case 18: {
                Backups backup;
                backup.crearBackup(titulares);
                cout << "\nSaliendo...\n" << endl; break;}
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 18);
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
                    system("cls");
                    std::cout << "\n" << std::endl;
                    std::cout << "\n--- BUSCAR EN ARBOL B+ ---" << std::endl;
                    std::string ci = val.ingresarCedula((char*)"\nIngrese CI del titular a buscar: ");
                    Titular* titularEncontrado = arbolTitulares.buscar(ci); // arbolTitulares is your B+ tree
                    if (!titularEncontrado) {
                        std::cout << "\nTitular no encontrado." << std::endl;
                        break;
                    }
                    // Mostrar nombre y apellido
                    std::cout << "\nTitular encontrado:\n";
                    std::cout << "\nNombre: " << titularEncontrado->getPersona().getNombre() << " "
                            << titularEncontrado->getPersona().getApellido() << std::endl;
                    // Mostrar cuentas
                    if (titularEncontrado->getCuentaCorriente()) {
                        std::cout << "\nCuenta Corriente: " << titularEncontrado->getCuentaCorriente()->getID() << std::endl;
                    }
                    if (!titularEncontrado->getCuentasAhorro().vacia()) {
                        std::cout << "\nCuentas de Ahorro:\n";
                        NodoDoble<CuentaBancaria*>* actual = titularEncontrado->getCuentasAhorro().getCabeza();
                        if (actual) {
                            do {
                                std::cout << "  - " << actual->dato->getID() << std::endl;
                                actual = actual->siguiente;
                            } while (actual != titularEncontrado->getCuentasAhorro().getCabeza());
                        }
                    } else {
                        std::cout << "No tiene cuentas de ahorro." << std::endl;
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

void Sistema::verificarIntegridadArchivo() {
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
}

void Sistema::mostrarTablaHash() {
    system("cls");
    cout << "\n--- TABLA HASH ---" << endl;
    hashes.mostrarContenido();
    system("pause");
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
    gestorTitulares.crearCuenta(titulares, listaSucursales, arbolTitulares);
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
    Menu menu;
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
                menuBusquedasBinarias.buscarDepositoMayorIgual(titulares);
                break;
            }
            case 2: {
                menuBusquedasBinarias.calcularDepositoMinimoMeta(titulares);
                break;
            }
            case 3: {
                menuBusquedasBinarias.buscarTitularPorCI(titulares);
                break;
            }
            case 4: {
                menuBusquedasBinarias.buscarTitularPorAnioNacimiento(titulares);
                break;
            }
            case 5: {
                menuBusquedasBinarias.buscarSucursalMasCercana(listaSucursales);
                break;
            }
            case 6:
                cout << "\nRegresando al menu principal...\n";
                break;
            default:
                cout << "\nOpcion invalida.\n"; system("pause"); break;
        }
    } while(opcion != 6);
}
    /**
         * @brief Guarda los titulares y sus cuentas bancarias en un archivo de texto.
         * 
         */
void Sistema::guardarTitularesEnTxt() {
    gestorArchivos.guardarTitularesEnTxt(titulares);
}

// Nuevos metodos para gestion de conexion
void Sistema::configurarModoServidor() {
    gestorConexion.configurarComoServidor();
    cout << "Sistema configurado como servidor local." << endl;
    cout << "Funcionara sin sincronizacion con MongoDB." << endl;
    system("pause");
}

void Sistema::configurarModoCliente() {
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
        cout << "Sistema sincronizado con MongoDB." << endl;
    } else {
        cout << "Error al conectar con el servidor." << endl;
        cout << "Ultima razon: " << gestorConexion.obtenerUltimoError() << endl;
    }
    system("pause");
}

void Sistema::menuChat() {
    if (!gestorConexion.estaConectado()) {
        cout << "Debe conectarse al servidor primero (opcion 2)." << endl;
        system("pause");
        return;
    }
    
    cout << "\n=== CHAT MICHIBANK ===" << endl;
    cout << "Escriba 'salir' para regresar al menu principal" << endl;
    cout << "Escriba 'mensajes' para ver mensajes recientes" << endl;
    cout << "========================" << endl;
    
    string mensaje;
    while (true) {
        cout << "\nMensaje: ";
        getline(cin, mensaje);
        
        if (mensaje == "salir") {
            break;
        } else if (mensaje == "mensajes") {
            string mensajesRecientes = gestorConexion.recibirMensajes();
            cout << "\n--- Mensajes Recientes ---" << endl;
            cout << mensajesRecientes << endl;
            cout << "-------------------------" << endl;
        } else if (!mensaje.empty()) {
            if (gestorConexion.enviarMensaje(mensaje)) {
                cout << "Mensaje enviado" << endl;
            } else {
                cout << "Error al enviar mensaje" << endl;
            }
        }
    }
}

std::string Sistema::generarJSONTitularCompleto(const Titular* titular) {
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
             << titular->getCuentaCorriente()->getFechaCre().getAnio() << "\"";
        json << "}";
    } else {
        json << "null";
    }
    json << "}";
    
    return json.str();
}

void Sistema::sincronizarTitularCompleto(const Titular* titular) {
    if (gestorConexion.estaConectado() && !gestorConexion.estaModoServidor()) {
        gestorConexion.sincronizarTitularCompleto(titular);
    }
}