#ifndef CITA_H
#define CITA_H

#include "Sucursal.h"
#include "FechaHora.h"
#include "DiaUtil.h"
#include <iostream>

class Cita {
private:
    Sucursal* sucursal; // Sucursal donde se programa la cita
    FechaHora fechaHora; // Fecha y hora de la cita
    FechaHora fechaGeneracion; // Fecha y hora de generación de la cita

public:
    // Constructor
    Cita(Sucursal* suc, FechaHora fechaActual) : sucursal(suc), fechaGeneracion(fechaActual) {
        generarCita(fechaActual);
    }

    // Generar cita para el siguiente día laborable a la misma hora
    void generarCita(FechaHora fechaActual) {
        // Convertir FechaHora a Fecha para manipulación de fechas
        Fecha fechaCita;
        Anio anioObj;
        anioObj.setAnio(fechaActual.getAnio());
        fechaCita.setAnio(anioObj);
        fechaCita.setMes(fechaActual.getMes());
        fechaCita.setDia(fechaActual.getDia());

        // Mover al siguiente día y ajustar al próximo día laborable
        fechaCita = sumarUnDia(fechaCita);
        fechaCita = ajustarAlDiaHabilMasCercano(fechaCita);

        // Mantener la misma hora y minuto, ajustando al horario laboral (08:00–16:45)
        int hora = fechaActual.getHora();
        int minuto = fechaActual.getMinuto();
        if (hora < 8) {
            hora = 8;
            minuto = 0;
        } else if (hora > 16 || (hora == 16 && minuto > 45)) {
            hora = 16;
            minuto = 45;
        }

        // Configurar la FechaHora de la cita
        fechaHora.setAnio(fechaCita.getAnio().getAnio());
        fechaHora.setMes(fechaCita.getMes());
        fechaHora.setDia(fechaCita.getDia());
        fechaHora.setHora(hora);
        fechaHora.setMinuto(minuto);
        fechaHora.setSegundo(0);
    }

    // Getters
    Sucursal* getSucursal() const { return sucursal; }
    FechaHora getFechaHora() const { return fechaHora; }
    FechaHora getFechaGeneracion() const { return fechaGeneracion; }

    // Método para mostrar la cita
    void mostrar() const {
        std::cout << "\nCita generada exitosamente:\n";
        std::cout << "Sucursal: " << sucursal->getNombre() << "\n";
        std::cout << "Fecha de generacion: " << fechaGeneracion.obtenerFecha() << "\n";
        //std::cout << "Fecha de la cita: " << fechaHora.obtenerFecha() << "\n";
        std::cout << "Hora: " << fechaHora.obtenerHora() << "\n";

        // Calcular e imprimir el día siguiente a la fecha de generación
        Fecha fechaSig;
        Anio anioObj;
        anioObj.setAnio(fechaGeneracion.getAnio());
        fechaSig.setAnio(anioObj);
        fechaSig.setMes(fechaGeneracion.getMes());
        fechaSig.setDia(fechaGeneracion.getDia());
        fechaSig = sumarUnDia(fechaSig);
        std::cout << "Fecha de la cita: " 
                  << fechaSig.getDia() << "/" << fechaSig.getMes() << "/" << fechaSig.getAnio().getAnio() << "\n";
        std::cout << "Hora: " << fechaHora.obtenerHora() << "\n";
    }
};

#endif