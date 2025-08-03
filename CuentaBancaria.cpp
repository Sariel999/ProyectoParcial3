#include "CuentaBancaria.h"
#include "Pila.h"          // ← NECESARIO
#include "Movimiento.h" 
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

CuentaBancaria::CuentaBancaria() : saldo(0.0f) {
    
}
CuentaBancaria::CuentaBancaria(const std::string& sucursalID) : saldo(0.0f) {
    generarID(sucursalID);
    fechaCre = FechaHora();
}

CuentaBancaria::~CuentaBancaria() {}

std::string CuentaBancaria::getID() const {
    return ID;
}

void CuentaBancaria::setID(const std::string& newID) {
    ID = newID;
}

FechaHora CuentaBancaria::getFechaCre() const {
    return fechaCre;
}

void CuentaBancaria::setFechaCre(const FechaHora& newFechaCre) {
    fechaCre = newFechaCre;
}

float CuentaBancaria::getSaldo() const {
    return saldo;
}

void CuentaBancaria::setSaldo(float newSaldo) {
    saldo = newSaldo;
}

std::string CuentaBancaria::getTipoCuenta() const {
    return tipoCuenta;
}

void CuentaBancaria::setTipoCuenta(const std::string& newTipoCuenta) {
    tipoCuenta = newTipoCuenta;
}

void CuentaBancaria::agregarMovimiento(Movimiento* mov) {
    if (mov == nullptr) return;

    if (mov->getTipo()) {
        // Depósito
        saldo += mov->getMonto();
    } else {
        // Retiro
        if (mov->getMonto() <= saldo) {
            saldo -= mov->getMonto();
        } else {
            std::cout << "Fondos insuficientes para realizar el retiro.\n";
            return;
        }
    }

    movimientos.insertar(mov);
}

ListaDobleCircular<Movimiento*>& CuentaBancaria::getMovimientos() {
    return movimientos;
}
const ListaDobleCircular<Movimiento*>& CuentaBancaria::getMovimientos() const {
    return movimientos;
}

void CuentaBancaria::generarID(const std::string& sucursalID) {
    // Generar el número secuencial de la cuenta (7 dígitos)
    srand(static_cast<unsigned int>(time(0)));  // Semilla para los números aleatorios
    int numeroSecuencial = rand() % 10000000;  // Número aleatorio entre 0 y 9999999

    // Convertir el número secuencial a un string con ceros a la izquierda
    std::ostringstream numeroCuenta;
    numeroCuenta << sucursalID << std::setw(7) << std::setfill('0') << numeroSecuencial;

    // Calcular el dígito verificador
    std::string cuentaSinDigito = numeroCuenta.str();
    char digitoVerificador = calcularDigitoVerificador(cuentaSinDigito);

    // Agregar el dígito verificador al final del número de cuenta
    numeroCuenta << digitoVerificador;

    // Establecer el ID de la cuenta
    ID = numeroCuenta.str();
}

void CuentaBancaria::imprimirMovimientos() {
    if (movimientos.vacia()) {
        std::cout << "  (Sin movimientos)\n";
        return;
    }
    NodoDoble<Movimiento*>* actual = movimientos.getCabeza();
    if (actual) {
        do {
            Movimiento* m = actual->dato;
            if (m) m->imprimir();
            actual = actual->siguiente;
        } while (actual != movimientos.getCabeza());
    }
}

char CuentaBancaria::calcularDigitoVerificador(const std::string& cuenta) {
    int suma = 0;
    int peso = 2;

    for (size_t i = 0; i < cuenta.length(); ++i) {
        if (isdigit(cuenta[i])) {
            suma += (cuenta[i] - '0') * peso;
            peso = (peso == 2) ? 1 : 2;
        }
    }

   int digito =(11-(suma%11))%10;
    return '0'+digito;
}

void CuentaBancaria::imprimir() {
    std::cout << "Tipo de cuenta: " << tipoCuenta << std::endl;
    std::cout << "ID: " << ID << std::endl;
    std::cout << "Saldo: $" << saldo << std::endl;
    std::cout << "Fecha de creacion:" << fechaCre.getDia() << "/"
              << fechaCre.getMes() << "/" << fechaCre.getAnio() << " "
              << fechaCre.getHora() << ":" << fechaCre.getMinuto() << std::endl;
    std::cout << "Movimientos:" << std::endl;
    
    imprimirMovimientos();
    std::cout << "---------------------------" << std::endl;
}