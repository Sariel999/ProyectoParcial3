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

    // std::cout << "DEBUG CuentaBancaria: Agregando movimiento..." << std::endl;
    // std::cout << "  - Saldo actual: $" << saldo << std::endl;
    // std::cout << "  - Monto: $" << mov->getMonto() << std::endl;
    // std::cout << "  - Tipo: " << (mov->getTipo() ? "Depósito" : "Retiro") << std::endl;

    if (mov->getTipo()) {
        // Depósito
        saldo += mov->getMonto();
        // std::cout << "  - Nuevo saldo después de depósito: $" << saldo << std::endl;
    } else {
        // Retiro
        if (mov->getMonto() <= saldo) {
            saldo -= mov->getMonto();
            // std::cout << "  - Nuevo saldo después de retiro: $" << saldo << std::endl;
        } else {
            std::cout << "ERROR: Fondos insuficientes para realizar el retiro." << std::endl;
            std::cout << "  - Saldo disponible: $" << saldo << std::endl;
            std::cout << "  - Monto solicitado: $" << mov->getMonto() << std::endl;
            return;
        }
    }

    movimientos.insertar(mov);
    // std::cout << "DEBUG CuentaBancaria: Movimiento agregado exitosamente" << std::endl;
}

void CuentaBancaria::cargarMovimientoSinRecalcular(Movimiento* mov) {
    if (mov == nullptr) return;

    // std::cout << "DEBUG CuentaBancaria: Cargando movimiento desde BD (sin recalcular saldo)..." << std::endl;
    // std::cout << "  - Monto: $" << mov->getMonto() << std::endl;
    // std::cout << "  - Tipo: " << (mov->getTipo() ? "Depósito" : "Retiro") << std::endl;
    // std::cout << "  - Saldo permanece: $" << saldo << " (no se recalcula)" << std::endl;

    // Solo agregamos el movimiento a la lista, SIN modificar el saldo
    // porque el saldo ya viene correcto de MongoDB
    movimientos.insertar(mov);
    // std::cout << "DEBUG CuentaBancaria: Movimiento cargado exitosamente sin recalcular" << std::endl;
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
    
    for (Movimiento* m : movimientos) {
        if (m) m->imprimir();
    }
}

char CuentaBancaria::calcularDigitoVerificador(const std::string& cuenta) {
    int suma = 0;
    int peso = 2;

    for (char c : cuenta) {
        if (isdigit(c)) {
            suma += (c - '0') * peso;
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