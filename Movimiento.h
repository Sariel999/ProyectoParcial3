#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H

#include <string>
#include "Fecha.h"
#include "Hora.h"
#include "FechaHora.h"

class Movimiento {
private:
    std::string IDMovimiento;
    float monto;
    Fecha fechaMov;
    Hora time;
    bool tipo; // true = deposito, false = retiro
    int numeroMovimiento; // Número de movimiento asignado por el titular
    void generarID(bool esDeposito); // Construye el ID único usando concatenación

public:
    Movimiento();
    Movimiento(float monto, bool esDeposito, int numeroMovimiento);
    ~Movimiento();

    // Getters y setters
    float getMonto() const;
    void setMonto(float newMonto);

    std::string getIDMovimiento() const;
    void setIDMovimiento(const std::string& newIDMovimiento);

    Fecha getFechaMov() const;
    void setFechaMov(const Fecha& newFechaMov);

    Hora getHora() const;
    void setHora(const Hora& newHora);

    int getNumeroMovimiento() const;
    void setNumeroMovimiento(int num);

    bool getTipo() const;
    void setTipo(bool newTipo);
    
    void imprimir();
};

#endif