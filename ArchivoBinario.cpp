/**
 * @file ArchivoBinario.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "ArchivoBinario.h"
#include "Persona.h"
#include "CuentaBancaria.h"
#include "Movimiento.h"
#include "Titular.h"
#include "ListaDobleCircular.h"
#include <fstream>
#include <iostream>
#include <direct.h>
#include <vector>
using namespace std;
// No hay como cambiar los for a for-each porque el for se esta ocupando para leer datos de un archivo binario
// --- FUNCIONES AUXILIARES PARA STRINGS ---
void escribirString(ofstream& archivo, const std::string& s) {
    size_t len = s.size();
    archivo.write(reinterpret_cast<char*>(&len), sizeof(size_t));
    archivo.write(s.c_str(), len);
}

void leerString(ifstream& archivo, std::string& s) {
    size_t len = 0;
    archivo.read(reinterpret_cast<char*>(&len), sizeof(size_t));
    char* buffer = new char[len + 1];
    archivo.read(buffer, len);
    buffer[len] = '\0';
    s = buffer;
    delete[] buffer;
}

// --- GUARDAR ---
void ArchivoBinario::guardar(const ListaDobleCircular<Titular*>& titulares, const std::string& nombreArchivo) {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo.\n" << endl;
        return;
    }

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual) {
        do {
            Titular* t = actual->dato;
            Persona p = t->getPersona();
            // Guardar Persona campo por campo
            escribirString(archivo, p.getCI());
            escribirString(archivo, p.getNombre());
            escribirString(archivo, p.getApellido());
            escribirString(archivo, p.getTelefono());
            escribirString(archivo, p.getCorreo());
            Fecha fechaNac = p.getFechaNa();
            archivo.write(reinterpret_cast<char*>(&fechaNac), sizeof(Fecha));

            // Cuenta corriente
            CuentaBancaria* c = t->getCuentaCorriente();
            bool tieneCorriente = c != nullptr;
            archivo.write(reinterpret_cast<char*>(&tieneCorriente), sizeof(bool));
            if (tieneCorriente) {
                escribirString(archivo, c->getID());
                FechaHora fechaCre = c->getFechaCre();
                archivo.write(reinterpret_cast<char*>(&fechaCre), sizeof(FechaHora));
                float saldo = c->getSaldo();
                archivo.write(reinterpret_cast<char*>(&saldo), sizeof(float));
                escribirString(archivo, c->getTipoCuenta());

                // Guardar movimientos de cuenta corriente
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
                        escribirString(archivo, m->getIDMovimiento());
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

            // Guardar cuentas de ahorro
            int totalAhorros = 0;
            NodoDoble<CuentaBancaria*>* nodoA = t->getCuentasAhorro().getCabeza();
            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    totalAhorros++;
                    temp = temp->siguiente;
                } while (temp != nodoA);
            }
            archivo.write(reinterpret_cast<char*>(&totalAhorros), sizeof(int));

            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    CuentaBancaria* ahorro = temp->dato;
                    escribirString(archivo, ahorro->getID());
                    FechaHora fechaCreAhorro = ahorro->getFechaCre();
                    archivo.write(reinterpret_cast<char*>(&fechaCreAhorro), sizeof(Fecha));
                    float saldoAhorro = ahorro->getSaldo();
                    archivo.write(reinterpret_cast<char*>(&saldoAhorro), sizeof(float));
                    escribirString(archivo, ahorro->getTipoCuenta());

                    // Guardar movimientos de cuenta de ahorro
                    ListaDobleCircular<Movimiento*>& movs = ahorro->getMovimientos();
                    int count = 0;
                    NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                    if (nodoMov) {
                        NodoDoble<Movimiento*>* tMov = nodoMov;
                        do {
                            count++;
                            tMov = tMov->siguiente;
                        } while (tMov != nodoMov);
                    }
                    archivo.write(reinterpret_cast<char*>(&count), sizeof(int));
                    if (nodoMov) {
                        NodoDoble<Movimiento*>* tMov = nodoMov;
                        do {
                            Movimiento* m = tMov->dato;
                            escribirString(archivo, m->getIDMovimiento());
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
                            tMov = tMov->siguiente;
                        } while (tMov != nodoMov);
                    }
                    temp = temp->siguiente;
                } while (temp != nodoA);
            }

            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
    archivo.close();
}

/**
 * @brief Guarda titulares desde un vector en archivo binario
 * 
 * @param titulares Vector de titulares obtenido desde MongoDB
 * @param nombreArchivo Nombre del archivo donde guardar
 */
void ArchivoBinario::guardarDesdeVector(const std::vector<Titular*>& titulares, const std::string& nombreArchivo) {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo.\n" << endl;
        return;
    }

    for (Titular* t : titulares) {
        if (!t) continue; // Validacion de seguridad
        
        Persona p = t->getPersona();
        // Guardar Persona campo por campo
        escribirString(archivo, p.getCI());
        escribirString(archivo, p.getNombre());
        escribirString(archivo, p.getApellido());
        escribirString(archivo, p.getTelefono());
        escribirString(archivo, p.getCorreo());
        Fecha fechaNac = p.getFechaNa();
        archivo.write(reinterpret_cast<char*>(&fechaNac), sizeof(Fecha));

        // Cuenta corriente
        CuentaBancaria* c = t->getCuentaCorriente();
        bool tieneCorriente = c != nullptr;
        archivo.write(reinterpret_cast<char*>(&tieneCorriente), sizeof(bool));
        if (tieneCorriente) {
            escribirString(archivo, c->getID());
            FechaHora fechaCre = c->getFechaCre();
            archivo.write(reinterpret_cast<char*>(&fechaCre), sizeof(FechaHora));
            float saldo = c->getSaldo();
            archivo.write(reinterpret_cast<char*>(&saldo), sizeof(float));
            escribirString(archivo, c->getTipoCuenta());

            // Guardar movimientos de cuenta corriente
            procesarMovimientosParaArchivoBinario(archivo, c->getMovimientos());
        }

        // Guardar cuentas de ahorro
        int totalAhorros = 0;
        NodoDoble<CuentaBancaria*>* nodoA = t->getCuentasAhorro().getCabeza();
        if (nodoA) {
            NodoDoble<CuentaBancaria*>* temp = nodoA;
            do {
                totalAhorros++;
                temp = temp->siguiente;
            } while (temp != nodoA);
        }
        archivo.write(reinterpret_cast<char*>(&totalAhorros), sizeof(int));

        if (nodoA) {
            NodoDoble<CuentaBancaria*>* temp = nodoA;
            do {
                CuentaBancaria* ahorro = temp->dato;
                escribirString(archivo, ahorro->getID());
                FechaHora fechaCreAhorro = ahorro->getFechaCre();
                archivo.write(reinterpret_cast<char*>(&fechaCreAhorro), sizeof(FechaHora));
                float saldoAhorro = ahorro->getSaldo();
                archivo.write(reinterpret_cast<char*>(&saldoAhorro), sizeof(float));
                escribirString(archivo, ahorro->getTipoCuenta());

                // Guardar movimientos de cuenta de ahorro
                procesarMovimientosParaArchivoBinario(archivo, ahorro->getMovimientos());
                
                temp = temp->siguiente;
            } while (temp != nodoA);
        }
    }
    
    archivo.close();
}

/**
 * @brief Procesa y guarda movimientos de una cuenta en archivo binario
 * 
 * @param archivo Stream del archivo binario
 * @param movimientos Lista de movimientos a procesar
 */
void ArchivoBinario::procesarMovimientosParaArchivoBinario(ofstream& archivo, ListaDobleCircular<Movimiento*>& movimientos) {
    int count = 0;
    NodoDoble<Movimiento*>* nodoMov = movimientos.getCabeza();
    
    // Contar movimientos
    if (nodoMov) {
        NodoDoble<Movimiento*>* temp = nodoMov;
        do {
            count++;
            temp = temp->siguiente;
        } while (temp != nodoMov);
    }
    
    archivo.write(reinterpret_cast<char*>(&count), sizeof(int));
    
    // Guardar movimientos
    if (nodoMov) {
        NodoDoble<Movimiento*>* temp = nodoMov;
        do {
            Movimiento* m = temp->dato;
            escribirString(archivo, m->getIDMovimiento());
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

// --- CARGAR ---
void ArchivoBinario::cargar(ListaDobleCircular<Titular*>& titulares, const std::string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo para lectura.\n" << endl;
        return;
    }

    while (archivo.peek() != EOF) {
        // Leer Persona
        std::string ci, nombre, apellido, telefono, correo;
        leerString(archivo, ci);
        leerString(archivo, nombre);
        leerString(archivo, apellido);
        leerString(archivo, telefono);
        leerString(archivo, correo);
        Persona p;
        p.setCI(ci);
        p.setNombre(nombre);
        p.setApellido(apellido);
        p.setTelefono(telefono);
        p.setCorreo(correo);
        Titular* t = new Titular(p);

        // Leer cuenta corriente
        bool tieneCorriente = false;
        archivo.read(reinterpret_cast<char*>(&tieneCorriente), sizeof(bool));
        if (tieneCorriente) {
            std::string id, tipoCuenta;
            FechaHora fechaCre;
            float saldo;
            leerString(archivo, id);
            archivo.read(reinterpret_cast<char*>(&fechaCre), sizeof(FechaHora));
            archivo.read(reinterpret_cast<char*>(&saldo), sizeof(float));
            leerString(archivo, tipoCuenta);

            CuentaBancaria* cc = new CuentaBancaria();
            cc->setID(id);
            cc->setFechaCre(fechaCre);
            cc->setSaldo(saldo);
            cc->setTipoCuenta(tipoCuenta);

            // Leer movimientos de cuenta corriente
            int count = 0;
            archivo.read(reinterpret_cast<char*>(&count), sizeof(int));
            for (int i = 0; i < count; ++i) {
                std::string idMov;
                Fecha fechaMov;
                Hora hora;
                float monto;
                bool tipo;
                int numero;
                leerString(archivo, idMov);
                archivo.read(reinterpret_cast<char*>(&fechaMov), sizeof(Fecha));
                archivo.read(reinterpret_cast<char*>(&hora), sizeof(Hora));
                archivo.read(reinterpret_cast<char*>(&monto), sizeof(float));
                archivo.read(reinterpret_cast<char*>(&tipo), sizeof(bool));
                archivo.read(reinterpret_cast<char*>(&numero), sizeof(int));
                Movimiento* m = new Movimiento(monto, tipo, numero);
                m->setIDMovimiento(idMov);
                m->setFechaMov(fechaMov);
                m->setHora(hora);
                cc->getMovimientos().insertar(m); 
            }
            t->setCuentaCorriente(cc);
        }

        // Leer cuentas de ahorro
        int totalAhorros = 0;
        archivo.read(reinterpret_cast<char*>(&totalAhorros), sizeof(int));
        for (int i = 0; i < totalAhorros; ++i) {
            std::string id, tipoCuenta;
            FechaHora fechaCre;
            float saldo;
            leerString(archivo, id);
            archivo.read(reinterpret_cast<char*>(&fechaCre), sizeof(FechaHora));
            archivo.read(reinterpret_cast<char*>(&saldo), sizeof(float));
            leerString(archivo, tipoCuenta);

            CuentaBancaria* ahorro = new CuentaBancaria();
            ahorro->setID(id);
            ahorro->setFechaCre(fechaCre);
            ahorro->setSaldo(saldo);
            ahorro->setTipoCuenta(tipoCuenta);

            // Leer movimientos de cuenta de ahorro
            int count = 0;
            archivo.read(reinterpret_cast<char*>(&count), sizeof(int));
            for (int j = 0; j < count; ++j) {
                std::string idMov;
                Fecha fechaMov;
                Hora hora;
                float monto;
                bool tipo;
                int numero;
                leerString(archivo, idMov);
                archivo.read(reinterpret_cast<char*>(&fechaMov), sizeof(Fecha));
                archivo.read(reinterpret_cast<char*>(&hora), sizeof(Hora));
                archivo.read(reinterpret_cast<char*>(&monto), sizeof(float));
                archivo.read(reinterpret_cast<char*>(&tipo), sizeof(bool));
                archivo.read(reinterpret_cast<char*>(&numero), sizeof(int));
                Movimiento* m = new Movimiento(monto, tipo, numero);
                m->setIDMovimiento(idMov);
                m->setFechaMov(fechaMov);
                m->setHora(hora);
                ahorro->getMovimientos().insertar(m);
            }
            t->agregarCuentaAhorro(ahorro);
        }

        titulares.insertar(t);
    }

    archivo.close();
}
bool ArchivoBinario::cargarBackup(ListaDobleCircular<Titular*>& titulares, const std::string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo para lectura.\n" << endl;
        return false;
    }

    // Función auxiliar para leer strings
    auto leerString = [](ifstream& archivo, std::string& s) {
        size_t len = 0;
        archivo.read(reinterpret_cast<char*>(&len), sizeof(size_t));
        char* buffer = new char[len + 1];
        archivo.read(buffer, len);
        buffer[len] = '\0';
        s = buffer;
        delete[] buffer;
    };

    while (archivo.peek() != EOF) {
        // Leer Persona (en el mismo orden en que se guardó en el backup)
        std::string ci, nombre, apellido, telefono, correo;
        leerString(archivo, ci);
        leerString(archivo, nombre);
        leerString(archivo, apellido);
        leerString(archivo, telefono);
        leerString(archivo, correo);
        Fecha fechaNac;
        archivo.read(reinterpret_cast<char*>(&fechaNac), sizeof(Fecha));
        Persona p;
        p.setCI(ci);
        p.setNombre(nombre);
        p.setApellido(apellido);
        p.setTelefono(telefono);
        p.setCorreo(correo);
        p.setFechaNa(fechaNac);
        Titular* t = new Titular(p);

        // Leer cuenta corriente
        bool tieneCorriente = false;
        archivo.read(reinterpret_cast<char*>(&tieneCorriente), sizeof(bool));
        if (tieneCorriente) {
            std::string id, tipoCuenta;
            FechaHora fechaCre;
            float saldo;
            leerString(archivo, id);
            archivo.read(reinterpret_cast<char*>(&fechaCre), sizeof(FechaHora));
            archivo.read(reinterpret_cast<char*>(&saldo), sizeof(float));
            leerString(archivo, tipoCuenta);

            CuentaBancaria* cc = new CuentaBancaria();
            cc->setID(id);
            cc->setFechaCre(fechaCre);
            cc->setSaldo(saldo);
            cc->setTipoCuenta(tipoCuenta);

            // Leer movimientos de cuenta corriente
            int count = 0;
            archivo.read(reinterpret_cast<char*>(&count), sizeof(int));
            for (int i = 0; i < count; ++i) {
                std::string idMov;
                Fecha fechaMov;
                Hora hora;
                float monto;
                bool tipo;
                int numero;
                leerString(archivo, idMov);
                archivo.read(reinterpret_cast<char*>(&fechaMov), sizeof(Fecha));
                archivo.read(reinterpret_cast<char*>(&hora), sizeof(Hora));
                archivo.read(reinterpret_cast<char*>(&monto), sizeof(float));
                archivo.read(reinterpret_cast<char*>(&tipo), sizeof(bool));
                archivo.read(reinterpret_cast<char*>(&numero), sizeof(int));
                Movimiento* m = new Movimiento(monto, tipo, numero);
                m->setIDMovimiento(idMov);
                m->setFechaMov(fechaMov);
                m->setHora(hora);
                cc->getMovimientos().insertar(m);
            }
            t->setCuentaCorriente(cc);
        }

        // Leer cuentas de ahorro
        int totalAhorros = 0;
        archivo.read(reinterpret_cast<char*>(&totalAhorros), sizeof(int));
        for (int i = 0; i < totalAhorros; ++i) {
            std::string id, tipoCuenta;
            FechaHora fechaCre;
            float saldo;
            leerString(archivo, id);
            archivo.read(reinterpret_cast<char*>(&fechaCre), sizeof(FechaHora));
            archivo.read(reinterpret_cast<char*>(&saldo), sizeof(float));
            leerString(archivo, tipoCuenta);

            CuentaBancaria* ahorro = new CuentaBancaria();
            ahorro->setID(id);
            ahorro->setFechaCre(fechaCre);
            ahorro->setSaldo(saldo);
            ahorro->setTipoCuenta(tipoCuenta);

            // Leer movimientos de cuenta de ahorro
            int count = 0;
            archivo.read(reinterpret_cast<char*>(&count), sizeof(int));
            for (int j = 0; j < count; ++j) {
                std::string idMov;
                Fecha fechaMov;
                Hora hora;
                float monto;
                bool tipo;
                int numero;
                leerString(archivo, idMov);
                archivo.read(reinterpret_cast<char*>(&fechaMov), sizeof(Fecha));
                archivo.read(reinterpret_cast<char*>(&hora), sizeof(Hora));
                archivo.read(reinterpret_cast<char*>(&monto), sizeof(float));
                archivo.read(reinterpret_cast<char*>(&tipo), sizeof(bool));
                archivo.read(reinterpret_cast<char*>(&numero), sizeof(int));
                Movimiento* m = new Movimiento(monto, tipo, numero);
                m->setIDMovimiento(idMov);
                m->setFechaMov(fechaMov);
                m->setHora(hora);
                ahorro->getMovimientos().insertar(m);
            }
            t->agregarCuentaAhorro(ahorro);
        }

        titulares.insertar(t);
    }

    archivo.close();
    return true;

}
