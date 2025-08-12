/**
 * @file Backups.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "Backups.h"
#include <fstream>  
#include <iostream>
#include <ctime>
#include "ArchivoBinario.h"
#include "BPlusTreeTitulares.h"
/**
 * @brief Crea un backup de los titulares y sus cuentas bancarias en un archivo binario.
 * 
 * @param titulares 
 */
void Backups :: crearBackup (const ListaDobleCircular<Titular*>& titulares) {
    if (titulares.vacia()) {
        std::cout << "\nNo hay titulares registrados para respaldar.\n" << std::endl;
        system("pause");
        return;
    }

    time_t now = time(0);
    tm* ltm = localtime(&now);
    char nombreArchivo[100];
    sprintf(nombreArchivo, "%04d%02d%02d_%02d%02d%02d.bak",
        1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
        ltm->tm_hour, ltm->tm_min, ltm->tm_sec);

    std::ofstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo) {
        std::cout << "\nNo se pudo crear el archivo de respaldo.\n" << std::endl;
        system("pause");
        return;
    }

    // Función auxiliar para escribir strings
    auto escribirString = [&](const std::string& s) {
        size_t len = s.size();
        archivo.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
        archivo.write(s.c_str(), len);
    };

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual) {
        do {
            Titular* t = actual->dato;
            Persona p = t->getPersona();
            escribirString(p.getCI());
            escribirString(p.getNombre());
            escribirString(p.getApellido());
            escribirString(p.getTelefono());
            escribirString(p.getCorreo());
            Fecha fechaNac = p.getFechaNa();
            archivo.write(reinterpret_cast<const char*>(&fechaNac), sizeof(Fecha));
            // Cuenta corriente
            CuentaBancaria* c = t->getCuentaCorriente();
            bool tieneCorriente = c != nullptr;
            archivo.write(reinterpret_cast<char*>(&tieneCorriente), sizeof(bool));
            if (tieneCorriente) {
                escribirString(c->getID());
                FechaHora fechaCre = c->getFechaCre();
                archivo.write(reinterpret_cast<char*>(&fechaCre), sizeof(FechaHora));
                float saldo = c->getSaldo();
                archivo.write(reinterpret_cast<char*>(&saldo), sizeof(float));
                escribirString(c->getTipoCuenta());

                // Movimientos
                ListaDobleCircular<Movimiento*>& movs = c->getMovimientos();
                int count = 0;
                NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                if (nodoMov) {
                    NodoDoble<Movimiento*>* temp = nodoMov;
                    do {
                        count++;
                        temp = temp->siguiente;
                    } while (temp != nodoMov);
                }
                archivo.write(reinterpret_cast<char*>(&count), sizeof(int));
                if (nodoMov) {
                    NodoDoble<Movimiento*>* temp = nodoMov;
                    do {
                        Movimiento* m = temp->dato;
                        escribirString(m->getIDMovimiento());
                        Fecha fechaMov = m->getFechaMov();
                        archivo.write(reinterpret_cast<char*>(&fechaMov), sizeof(Fecha));
                        Hora hora = m->getHora();
                        archivo.write(reinterpret_cast<char*>(&hora), sizeof(Hora));
                        float monto = m->getMonto();
                        archivo.write(reinterpret_cast<char*>(&monto), sizeof(float));
                        bool tipo = m->getTipo();
                        archivo.write(reinterpret_cast<char*>(&tipo), sizeof(bool));
                        int numMov = m->getNumeroMovimiento();
                        archivo.write(reinterpret_cast<char*>(&numMov), sizeof(int));
                        temp = temp->siguiente;
                    } while (temp != nodoMov);
                }
            }

            // Cuentas de ahorro
            int totalAhorros = 0;
            NodoDoble<CuentaBancaria*>* nodoA = t->getCuentasAhorro().getCabeza();
            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do { totalAhorros++; temp = temp->siguiente; } while (temp != nodoA);
            }
            archivo.write(reinterpret_cast<char*>(&totalAhorros), sizeof(int));
            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    CuentaBancaria* ahorro = temp->dato;
                    escribirString(ahorro->getID());
                    FechaHora fechaCreAhorro = ahorro->getFechaCre();
                    archivo.write(reinterpret_cast<char*>(&fechaCreAhorro), sizeof(FechaHora));
                    float saldoAhorro = ahorro->getSaldo();
                    archivo.write(reinterpret_cast<char*>(&saldoAhorro), sizeof(float));
                    escribirString(ahorro->getTipoCuenta());

                    // Movimientos
                    ListaDobleCircular<Movimiento*>& movs = ahorro->getMovimientos();
                    int count = 0;
                    NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                    if (nodoMov) {
                        NodoDoble<Movimiento*>* tmp = nodoMov;
                        do {
                            count++;
                            tmp = tmp->siguiente;
                        } while (tmp != nodoMov);
                    }
                    archivo.write(reinterpret_cast<char*>(&count), sizeof(int));
                    if (nodoMov) {
                        NodoDoble<Movimiento*>* tmp = nodoMov;
                        do {
                            Movimiento* m = tmp->dato;
                            escribirString(m->getIDMovimiento());
                            Fecha fechaMov = m->getFechaMov();
                            archivo.write(reinterpret_cast<char*>(&fechaMov), sizeof(Fecha));
                            Hora hora = m->getHora();
                            archivo.write(reinterpret_cast<char*>(&hora), sizeof(Hora));
                            float monto = m->getMonto();
                            archivo.write(reinterpret_cast<char*>(&monto), sizeof(float));
                            bool tipo = m->getTipo();
                            archivo.write(reinterpret_cast<char*>(&tipo), sizeof(bool));
                            int numMov = m->getNumeroMovimiento();
                            archivo.write(reinterpret_cast<char*>(&numMov), sizeof(int));
                            tmp = tmp->siguiente;
                        } while (tmp != nodoMov);
                    }
                    temp = temp->siguiente;
                } while (temp != nodoA);
            }
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
    archivo.close();
    cout << "\nBackup creado exitosamente: '" << nombreArchivo << "'.\n" << endl;
    cout << "Ubicacion: " << nombreArchivo << endl;
    system("pause");
}

bool Backups::restaurarBackup(ListaDobleCircular<Titular*>& titulares, 
                            BPlusTreeTitulares& arbolTitulares,
                            const std::string& archivo) {
    // Limpiar titulares existentes con manejo seguro de memoria
    while (!titulares.vacia()) {
        Titular* t = titulares.eliminarPorCabeza();
        if (t) {
            // Limpiar memoria de las cuentas bancarias
            if (t->getCuentaCorriente()) {
                delete t->getCuentaCorriente();
            }
            
            // Limpiar cuentas de ahorro
            ListaDobleCircular<CuentaBancaria*>& cuentasAhorro = t->getCuentasAhorro();
            while (!cuentasAhorro.vacia()) {
                CuentaBancaria* cuenta = cuentasAhorro.eliminarPorCabeza();
                if (cuenta) {
                    delete cuenta;
                }
            }
            
            delete t;
            t = nullptr;
        }
    }

    // Reinicializar el árbol B+
    arbolTitulares = BPlusTreeTitulares(3);
    
    // Cargar desde el archivo de backup
    bool exito = ArchivoBinario::cargarBackup(titulares, archivo);
    
    if (exito) {
        // Reconstruir el árbol B+ con los nuevos datos
        if (titulares.getCabeza()) {
            arbolTitulares.construirDesdeLista(titulares.getCabeza());
            std::cout << "\nBackup restaurado correctamente desde '" << archivo << "'.\n" << std::endl;
            std::cout << "\nARBOL B+ DESPUES DE LA RESTAURACION \n"; 
            arbolTitulares.imprimir();
        }
    } else {
        std::cout << "\nNo se pudo restaurar el backup desde '" << archivo << "'.\n" << std::endl;
    }
    
    system("pause");
    return exito;
}