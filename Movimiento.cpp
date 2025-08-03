#include "Movimiento.h"
#include <cstdlib>
#include <iostream>
#include <ctime>

Movimiento::Movimiento() : monto(0.0), numeroMovimiento(0) {}


Movimiento::Movimiento(float monto, bool esDeposito, int numeroMovimiento)
    : monto(monto), tipo(esDeposito), numeroMovimiento(numeroMovimiento) {
    FechaHora fh;
    Anio anio;
    anio.setAnio(fh.getAnio());
    anio.setAnioBisiesto((fh.getAnio() % 4 == 0 && fh.getAnio() % 100 != 0) || (fh.getAnio() % 400 == 0));

    fechaMov.setDia(fh.getDia());
    fechaMov.setMes(fh.getMes());
    fechaMov.setAnio(anio);

    time.setHoras(fh.getHora());
    time.setMinutos(fh.getMinuto());
    time.setSegundos(fh.getSegundo());

    generarID(esDeposito);
}

Movimiento::~Movimiento() {}

float Movimiento::getMonto() const {
    return monto;
}

void Movimiento::setMonto(float newMonto) {
    monto = newMonto;
}

std::string Movimiento::getIDMovimiento() const {
    return IDMovimiento;
}

void Movimiento::setIDMovimiento(const std::string& newIDMovimiento) {
    IDMovimiento = newIDMovimiento;
}

Fecha Movimiento::getFechaMov() const {
    return fechaMov;
}

void Movimiento::setFechaMov(const Fecha& newFechaMov) {
    fechaMov = newFechaMov;
}

Hora Movimiento::getHora() const {
    return time;
}

void Movimiento::setHora(const Hora& newHora) {
    time = newHora;
}

int Movimiento::getNumeroMovimiento() const {
    return numeroMovimiento;
}

void Movimiento::setNumeroMovimiento(int num) {
    numeroMovimiento = num;
}

bool Movimiento::getTipo() const {
    return tipo;
}
void Movimiento::setTipo(bool newTipo) {
    tipo = newTipo;
}

void Movimiento::generarID(bool esDeposito) {
    IDMovimiento = "";
    for (int i = 0; i < 10; ++i) {
        IDMovimiento += std::to_string(rand() % 10);
    }
}

void Movimiento::imprimir(){
    std::cout << "ID: " << IDMovimiento << "\n";
    std::cout << "Monto: $" << monto << "\n";
    std::cout << "Fecha: " << fechaMov.getDia() << "/"
              << fechaMov.getMes() << "/"
              << fechaMov.getAnio().getAnio() << "\n";
    std::cout << "Hora: " << time.getHoras() << ":"
              << time.getMinutos() << ":"
              << time.getSegundos() << "\n";
    std::cout << "---------------------------\n";
}
