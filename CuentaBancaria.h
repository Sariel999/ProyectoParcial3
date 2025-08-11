#ifndef CUENTABANCARIA_H
#define CUENTABANCARIA_H

#include <string>
#include "FechaHora.h"
#include "Movimiento.h"
#include "ListaDobleCircular.h"
  // Clase individual de movimiento

class CuentaBancaria {
private:
    std::string ID;
    FechaHora fechaCre;
    float saldo;
    std::string tipoCuenta;  // Ahorros o Corriente
    ListaDobleCircular<Movimiento*> movimientos;

public:
    CuentaBancaria(const std::string& sucursalID);
    CuentaBancaria();
    ~CuentaBancaria();

    // Getters y setters
    std::string getID() const;
    void setID(const std::string& newID);

    FechaHora getFechaCre() const;
    void setFechaCre(const FechaHora& newFechaCre);

    float getSaldo() const;
    void setSaldo(float newSaldo);

    std::string getTipoCuenta() const;
    void setTipoCuenta(const std::string& newTipoCuenta);

    void agregarMovimiento(Movimiento* mov);
    void cargarMovimientoSinRecalcular(Movimiento* mov);  // Nuevo método para cargar desde BD
    ListaDobleCircular<Movimiento*>& getMovimientos();
    const ListaDobleCircular<Movimiento*>& getMovimientos() const;

    void generarID(const std::string& sucursalID);  // Método para generar el ID de la cuenta

    // Método para calcular el dígito verificador
    char calcularDigitoVerificador(const std::string& cuenta);
    void imprimirMovimientos();
    void imprimir();
};

#endif
