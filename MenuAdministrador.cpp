/**
 * @file MenuAdministrador.cpp
 * @author MichiBank Team
 * @brief Implementacion del menu de administrador
 * @version 1.0
 * @date 2025-08-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "MenuAdministrador.h"
#include "Menu.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <fstream>
#include <cstdlib>

using namespace std;

MenuAdministrador::MenuAdministrador() : gestorConexion(nullptr) {
}

MenuAdministrador::MenuAdministrador(GestorConexion& gestor) : gestorConexion(&gestor) {
}

MenuAdministrador::~MenuAdministrador() {
}

bool MenuAdministrador::validarContrasenaAdmin() {
    const string CONTRASENA_CORRECTA = "Sariel123";
    string contrasenaIngresada;
    
    cout << "\n=== ACCESO DE ADMINISTRADOR ===" << endl;
    contrasenaIngresada = val.ingresarContrasena("Ingrese la contrasena de administrador: ");
    
    if (contrasenaIngresada == CONTRASENA_CORRECTA) {
        cout << "\nAcceso concedido. Bienvenido, Administrador." << endl;
        system("pause");
        return true;
    } else {
        cout << "\nERROR: Contrasena invalida." << endl;
        system("pause");
        return false;
    }
}

void MenuAdministrador::mostrarMenuAdministrador(ListaDobleCircular<Titular*>& titulares, BPlusTreeTitulares& arbolTitulares) {
    Menu menu;
    const char* opciones[] = {
        "Hacer backup base de datos",
        "Reestablecer Backup Base de datos", 
        "Hacer Backup en hora especifica",
        "Regresar al menu principal"
    };
    
    int opcion;
    do {
        opcion = menu.ingresarMenu("MENU ADMINISTRADOR", opciones, 4);
        
        switch(opcion) {
            case 1:
                hacerBackupBaseDatos();
                break;
                
            case 2:
                reestablecerBackupBaseDatos();
                break;
                
            case 3:
                programarBackupBaseDatos();
                break;
                
            case 4:
                cout << "\nRegresando al menu principal..." << endl;
                break;
                
            default:
                cout << "\nOpcion invalida." << endl;
                system("pause");
                break;
        }
        
    } while(opcion != 4);
}

std::string MenuAdministrador::crearNombreBackupDB() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char nombreArchivo[25];
    sprintf(nombreArchivo, "%04d%02d%02d_%02d%02d%02d_db.json",
        1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
        ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return string(nombreArchivo);
}

bool MenuAdministrador::exportarBaseDatosCompleta(const std::string& nombreArchivo) {
    if (!gestorConexion || !gestorConexion->estaConectado()) {
        cout << "Error: No hay conexion con el servidor MongoDB." << endl;
        return false;
    }
    
    cout << "Solicitando backup completo de la base de datos..." << endl;
    
    return gestorConexion->exportarBaseDatos(nombreArchivo);
}

bool MenuAdministrador::importarBaseDatosCompleta(const std::string& nombreArchivo) {
    if (!gestorConexion || !gestorConexion->estaConectado()) {
        cout << "Error: No hay conexion con el servidor MongoDB." << endl;
        return false;
    }
    
    cout << "Solicitando restauracion completa de la base de datos..." << endl;
    
    return gestorConexion->importarBaseDatos(nombreArchivo);
}

void MenuAdministrador::hacerBackupBaseDatos() {
    system("cls");
    cout << "\n=== CREAR BACKUP DE BASE DE DATOS ===" << endl;
    
    string nombreArchivo = crearNombreBackupDB();
    cout << "Creando backup con nombre: " << nombreArchivo << endl;
    
    if (exportarBaseDatosCompleta(nombreArchivo)) {
        cout << "\nBackup de base de datos creado exitosamente." << endl;
        cout << "Archivo: " << nombreArchivo << endl;
        cout << "Formato: JSON completo de todas las colecciones" << endl;
    } else {
        cout << "\nError al crear backup de base de datos." << endl;
    }
    
    system("pause");
}

void MenuAdministrador::reestablecerBackupBaseDatos() {
    system("cls");
    cout << "\n=== REESTABLECER BACKUP DE BASE DE DATOS ===" << endl;
    cout << "ATENCION: Esta operacion reemplazara TODA la base de datos actual." << endl;
    cout << "Asegurese de que el archivo de backup existe y es valido." << endl;
    
    string archivo = val.ingresarNombreBackupDBConCancelacion("Ingrese el nombre del archivo de backup (ejemplo: 20240812_115030_db.json): ");
    
    // Si el usuario canceló o se agotaron los intentos
    if (archivo.empty()) {
        return; // Regresar al menú de administrador
    }
    
    char confirmacion;
    cout << "\n¿Esta seguro de que desea reemplazar toda la base de datos? (S/N): ";
    cin >> confirmacion;
    cin.ignore();
    
    if (confirmacion == 'S' || confirmacion == 's') {
        if (importarBaseDatosCompleta(archivo)) {
            cout << "\nBase de datos restaurada exitosamente." << endl;
        } else {
            cout << "\nError al restaurar la base de datos." << endl;
        }
    } else {
        cout << "\nOperacion cancelada." << endl;
    }
    
    system("pause");
}

void MenuAdministrador::programarBackupBaseDatos() {
    system("cls");
    cout << "\n=== PROGRAMAR BACKUP DE BASE DE DATOS EN HORA ESPECIFICA ===" << endl;
    
    int hora = val.ingresarEntero("Ingrese la hora (0-23): ");
    while (hora < 0 || hora > 23) {
        cout << "Hora invalida. Debe estar entre 0 y 23." << endl;
        hora = val.ingresarEntero("Ingrese la hora (0-23): ");
    }
    
    int minuto = val.ingresarEntero("Ingrese los minutos (0-59): ");
    while (minuto < 0 || minuto > 59) {
        cout << "Minutos invalidos. Deben estar entre 0 y 59." << endl;
        minuto = val.ingresarEntero("Ingrese los minutos (0-59): ");
    }
    
    int segundo = val.ingresarEntero("Ingrese los segundos (0-59): ");
    while (segundo < 0 || segundo > 59) {
        cout << "Segundos invalidos. Deben estar entre 0 y 59." << endl;
        segundo = val.ingresarEntero("Ingrese los segundos (0-59): ");
    }
    
    cout << "\nBackup de base de datos programado para las " << hora << ":" << minuto << ":" << segundo << endl;
    cout << "Esperando hasta la hora programada..." << endl;
    
    // Esperar hasta la hora especificada
    esperarHoraEspecifica(hora, minuto, segundo);
    
    // Crear el backup de base de datos
    cout << "\nCreando backup programado de base de datos..." << endl;
    string nombreArchivo = crearNombreBackupDB();
    
    if (exportarBaseDatosCompleta(nombreArchivo)) {
        cout << "\nBackup programado de base de datos completado exitosamente." << endl;
        cout << "Archivo: " << nombreArchivo << endl;
    } else {
        cout << "\nError al crear backup programado." << endl;
    }
    
    system("pause");
}

void MenuAdministrador::esperarHoraEspecifica(int hora, int minuto, int segundo) {
    while (true) {
        // Obtener tiempo actual
        time_t now = time(0);
        tm* ltm = localtime(&now);
        
        int horaActual = ltm->tm_hour;
        int minutoActual = ltm->tm_min;
        int segundoActual = ltm->tm_sec;
        
        // Verificar si ya es la hora
        if (horaActual == hora && minutoActual == minuto && segundoActual >= segundo) {
            break;
        }
        
        // Mostrar cuenta regresiva cada segundo
        cout << "\rHora actual: " << horaActual << ":" << minutoActual << ":" << segundoActual 
             << " | Objetivo: " << hora << ":" << minuto << ":" << segundo << "    ";
        cout.flush();
        
        // Esperar 1 segundo
        this_thread::sleep_for(chrono::seconds(1));
    }
    cout << endl;
}
